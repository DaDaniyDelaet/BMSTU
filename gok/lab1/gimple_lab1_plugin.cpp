
#include "gcc-plugin.h"
#include "plugin-version.h"

#include "coretypes.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-pass.h"
#include "context.h"
#include "basic-block.h"
#include "function.h"
#include "cfg.h"
#include "cgraph.h"
#include "gimple-pretty-print.h"

#include <cstdio>
#include <cstring>

int plugin_is_GPL_compatible;

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ПЕЧАТИ tree

static void print_indent(int n) {
    for (int i = 0; i < n; ++i) {
        std::printf(" ");
    }
}

static void print_tree_simple(tree t);

static void print_decl_name(tree t) {
    if (!t) {
        std::printf("<null>");
        return;
    }

    tree name = DECL_NAME(t);
    if (name && TREE_CODE(name) == IDENTIFIER_NODE) {
        std::printf("%s", IDENTIFIER_POINTER(name));
    } else {
        std::printf("<unnamed>");
    }
}

static void print_integer_cst(tree t) {
    if (!t) {
        std::printf("<null-int>");
        return;
    }

    if (tree_fits_shwi_p(t)) {
        std::printf("%lld", (long long) tree_to_shwi(t));
    } else if (tree_fits_uhwi_p(t)) {
        std::printf("%llu", (unsigned long long) tree_to_uhwi(t));
    } else {
        std::printf("<big-int>");
    }
}

static void print_real_cst(tree) {
    std::printf("<real>");
}

static void print_ssa_name(tree t) {
    if (!t) {
        std::printf("<null-ssa>");
        return;
    }

    tree var = SSA_NAME_VAR(t);
    if (var && DECL_NAME(var) && TREE_CODE(DECL_NAME(var)) == IDENTIFIER_NODE) {
        std::printf("%s_%u",
                    IDENTIFIER_POINTER(DECL_NAME(var)),
                    SSA_NAME_VERSION(t));
    } else {
        std::printf("ssa_%u", SSA_NAME_VERSION(t));
    }
}

static const char* expr_code_to_symbol(enum tree_code code) {
    switch (code) {
        case PLUS_EXPR: return "+";
        case MINUS_EXPR: return "-";
        case MULT_EXPR: return "*";
        case TRUNC_DIV_EXPR: return "/";
        case EXACT_DIV_EXPR: return "/";
        case RDIV_EXPR: return "/";
        case LT_EXPR: return "<";
        case LE_EXPR: return "<=";
        case GT_EXPR: return ">";
        case GE_EXPR: return ">=";
        case EQ_EXPR: return "==";
        case NE_EXPR: return "!=";
        case BIT_AND_EXPR: return "&";
        case BIT_IOR_EXPR: return "|";
        case BIT_XOR_EXPR: return "^";
        case LSHIFT_EXPR: return "<<";
        case RSHIFT_EXPR: return ">>";
        case TRUTH_AND_EXPR: return "&&";
        case TRUTH_OR_EXPR: return "||";
        default: return nullptr;
    }
}

static void print_tree_simple(tree t) {
    if (!t) {
        std::printf("<null>");
        return;
    }

    enum tree_code code = TREE_CODE(t);

    switch (code) {
        case SSA_NAME:
            print_ssa_name(t);
            break;

        case VAR_DECL:
        case PARM_DECL:
        case RESULT_DECL:
        case FUNCTION_DECL:
        case LABEL_DECL:
        case FIELD_DECL:
            print_decl_name(t);
            break;

        case INTEGER_CST:
            print_integer_cst(t);
            break;

        case REAL_CST:
            print_real_cst(t);
            break;

        case STRING_CST:
            std::printf("\"%s\"", TREE_STRING_POINTER(t));
            break;

        case ARRAY_REF: //массив
            std::printf("ARRAY_REF(");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(", ");
            print_tree_simple(TREE_OPERAND(t, 1));
            std::printf(")");
            break;

        case MEM_REF: //память по адресу
            std::printf("MEM_REF(");
            print_tree_simple(TREE_OPERAND(t, 0));
            if (TREE_OPERAND(t, 1)) {
                std::printf(", ");
                print_tree_simple(TREE_OPERAND(t, 1));
            }
            std::printf(")");
            break;

        case COMPONENT_REF: //поле структуры
            std::printf("COMPONENT_REF(");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(", ");
            print_tree_simple(TREE_OPERAND(t, 1));
            std::printf(")");
            break;

        case INDIRECT_REF: //разыменование указателя
            std::printf("*(");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(")");
            break;

        case ADDR_EXPR: //взятие адреса
            std::printf("&(");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(")");
            break;

        case PLUS_EXPR:
        case MINUS_EXPR:
        case MULT_EXPR:
        case TRUNC_DIV_EXPR:
        case EXACT_DIV_EXPR:
        case RDIV_EXPR:
        case LT_EXPR:
        case LE_EXPR:
        case GT_EXPR:
        case GE_EXPR:
        case EQ_EXPR:
        case NE_EXPR:
        case BIT_AND_EXPR:
        case BIT_IOR_EXPR:
        case BIT_XOR_EXPR:
        case LSHIFT_EXPR:
        case RSHIFT_EXPR:
        case TRUTH_AND_EXPR:
        case TRUTH_OR_EXPR: {
            const char* op = expr_code_to_symbol(code);
            std::printf("(");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(" %s ", op ? op : get_tree_code_name(code));
            print_tree_simple(TREE_OPERAND(t, 1));
            std::printf(")");
            break;
        }

        case NEGATE_EXPR:
            std::printf("(-");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(")");
            break;

        case NOP_EXPR:
        case CONVERT_EXPR:
            std::printf("%s(",
                        code == NOP_EXPR ? "NOP_EXPR" : "CONVERT_EXPR");
            print_tree_simple(TREE_OPERAND(t, 0));
            std::printf(")");
            break;

        default:
            std::printf("%s", get_tree_code_name(code));
            break;
    }
}

static bool is_memory_ref_like(tree t) {
    if (!t) return false;

    enum tree_code code = TREE_CODE(t);
    return code == ARRAY_REF
        || code == MEM_REF
        || code == COMPONENT_REF
        || code == INDIRECT_REF
        || code == TARGET_MEM_REF;
}


//   ПЕЧАТЬ PHI

static void print_phi_node(gphi* phi, int indent) {
    print_indent(indent);

    tree lhs = gimple_phi_result(phi);

    std::printf("PHI: ");
    print_tree_simple(lhs);
    std::printf(" = PHI(");

    int nargs = gimple_phi_num_args(phi);
    for (int i = 0; i < nargs; ++i) {
        if (i > 0) {
            std::printf(", ");
        }

        tree arg = gimple_phi_arg_def(phi, i);
        edge e = gimple_phi_arg_edge(phi, i);

        std::printf("[");
        print_tree_simple(arg);
        if (e && e->src) {
            std::printf(", from BB%d", e->src->index);
        } else {
            std::printf(", from BB?");
        }
        std::printf("]");
    }

    std::printf(")\n");
}


//   ПЕЧАТЬ ОТДЕЛЬНЫХ GIMPLE ИНСТРУКЦИЙ

static void print_assign_stmt(gassign* stmt, int indent) {
    print_indent(indent);

    tree lhs = gimple_assign_lhs(stmt);
    tree rhs1 = gimple_assign_rhs1(stmt);
    tree rhs2 = nullptr;
    tree rhs3 = nullptr;

    enum tree_code rhs_code = gimple_assign_rhs_code(stmt);

    std::printf("ASSIGN: ");
    print_tree_simple(lhs);
    std::printf(" = ");

    int nrhs = gimple_num_ops(stmt);

    if (nrhs == 1) {
        print_tree_simple(rhs1);
    } else if (nrhs == 2) {
        rhs2 = gimple_assign_rhs2(stmt);
        const char* op = expr_code_to_symbol(rhs_code);

        if (op) {
            print_tree_simple(rhs1);
            std::printf(" %s ", op);
            print_tree_simple(rhs2);
        } else {
            std::printf("%s(",
                        rhs_code < LAST_AND_UNUSED_TREE_CODE
                            ? get_tree_code_name(rhs_code)
                            : "<rhs-op>");
            print_tree_simple(rhs1);
            std::printf(", ");
            print_tree_simple(rhs2);
            std::printf(")");
        }
    } else if (nrhs == 3) {
        rhs2 = gimple_assign_rhs2(stmt);
        rhs3 = gimple_assign_rhs3(stmt);

        std::printf("%s(",
                    rhs_code < LAST_AND_UNUSED_TREE_CODE
                        ? get_tree_code_name(rhs_code)
                        : "<rhs-op>");
        print_tree_simple(rhs1);
        std::printf(", ");
        print_tree_simple(rhs2);
        std::printf(", ");
        print_tree_simple(rhs3);
        std::printf(")");
    } else {
        std::printf("<unsupported-rhs>");
    }

    if (is_memory_ref_like(lhs) ||
        is_memory_ref_like(rhs1) ||
        is_memory_ref_like(rhs2) ||
        is_memory_ref_like(rhs3)) {
        std::printf("    ; memory-access");
    }

    std::printf("\n");
}

static void print_cond_stmt(gcond* stmt, int indent) {
    print_indent(indent);

    tree lhs = gimple_cond_lhs(stmt);
    tree rhs = gimple_cond_rhs(stmt);
    enum tree_code code = gimple_cond_code(stmt);

    std::printf("COND: if (");
    print_tree_simple(lhs);
    std::printf(" %s ", expr_code_to_symbol(code) ? expr_code_to_symbol(code) : "?");
    print_tree_simple(rhs);
    std::printf(")\n");
}

static void print_call_stmt(gcall* stmt, int indent) {
    print_indent(indent);

    tree lhs = gimple_call_lhs(stmt);
    tree fn = gimple_call_fn(stmt);

    std::printf("CALL: ");

    if (lhs) {
        print_tree_simple(lhs);
        std::printf(" = ");
    }

    print_tree_simple(fn);
    std::printf("(");

    unsigned nargs = gimple_call_num_args(stmt);
    for (unsigned i = 0; i < nargs; ++i) {
        if (i > 0) {
            std::printf(", ");
        }
        print_tree_simple(gimple_call_arg(stmt, i));
    }

    std::printf(")\n");
}

static void print_return_stmt(greturn* stmt, int indent) {
    print_indent(indent);
    std::printf("RETURN");

    tree retval = gimple_return_retval(stmt);
    if (retval) {
        std::printf(" ");
        print_tree_simple(retval);
    }

    std::printf("\n");
}

static void print_goto_stmt(ggoto* stmt, int indent) {
    print_indent(indent);
    std::printf("GOTO ");

    tree dest = gimple_goto_dest(stmt);
    print_tree_simple(dest);

    std::printf("\n");
}

static void print_switch_stmt(gswitch* stmt, int indent) {
    print_indent(indent);

    std::printf("SWITCH: index=");
    print_tree_simple(gimple_switch_index(stmt));
    std::printf(", labels=%u\n", gimple_switch_num_labels(stmt));
}

static void print_stmt(gimple* stmt, int indent) {
    switch (gimple_code(stmt)) {
        case GIMPLE_ASSIGN:
            print_assign_stmt(as_a<gassign*>(stmt), indent);
            break;

        case GIMPLE_COND:
            print_cond_stmt(as_a<gcond*>(stmt), indent);
            break;

        case GIMPLE_CALL:
            print_call_stmt(as_a<gcall*>(stmt), indent);
            break;

        case GIMPLE_RETURN:
            print_return_stmt(as_a<greturn*>(stmt), indent);
            break;

        case GIMPLE_GOTO:
            print_goto_stmt(as_a<ggoto*>(stmt), indent);
            break;

        case GIMPLE_SWITCH:
            print_switch_stmt(as_a<gswitch*>(stmt), indent);
            break;

        default:
            print_indent(indent);
            std::printf("STMT: %s\n", gimple_code_name[gimple_code(stmt)]);
            break;
    }
}


//   ОБХОД ФУНКЦИИ И ЕЁ CFG

static unsigned int execute_gimple_printer(function* fun) {
    const char* fname = "<unknown-function>";

    if (fun && fun->decl && DECL_NAME(fun->decl)) {
        fname = IDENTIFIER_POINTER(DECL_NAME(fun->decl));
    }

    std::printf("Function: %s\n", fname);

    basic_block bb;
    FOR_ALL_BB_FN(bb, fun) {
        std::printf("\nBB%d\n", bb->index);

        std::printf("  preds: ");
        {
            edge e;
            edge_iterator ei;
            bool first = true;

            FOR_EACH_EDGE(e, ei, bb->preds) {
                if (!first) std::printf(", ");
                std::printf("BB%d", e->src->index);
                first = false;
            }

            if (first) std::printf("(none)");
            std::printf("\n");
        }

        std::printf("  succs: ");
        {
            edge e;
            edge_iterator ei;
            bool first = true;

            FOR_EACH_EDGE(e, ei, bb->succs) {
                if (!first) std::printf(", ");
                std::printf("BB%d", e->dest->index);
                first = false;
            }

            if (first) std::printf("(none)");
            std::printf("\n");
        }

        std::printf("  phi nodes:\n");
        for (gphi_iterator pi = gsi_start_phis(bb); !gsi_end_p(pi); gsi_next(&pi)) {
            gphi* phi = pi.phi ();
            print_phi_node(phi, 4);
        }

        std::printf("  statements:\n");
        for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
            gimple* stmt = gsi_stmt(gsi);
            print_stmt(stmt, 4);
        }
    }

    return 0;
}


//   PASS

namespace {

const pass_data my_pass_data = {
    GIMPLE_PASS,
    "my-gimple-printer",
    OPTGROUP_NONE,
    TV_NONE,
    PROP_cfg | PROP_ssa,
    0,
    0,
    0,
    0
};

class my_gimple_pass : public gimple_opt_pass {
public:
    my_gimple_pass(gcc::context* ctxt)
        : gimple_opt_pass(my_pass_data, ctxt) {}

    unsigned int execute(function* fun) override {
        return execute_gimple_printer(fun);
    }

    bool gate(function*) override {
        return true;
    }
};

} 


//   ИНИЦИАЛИЗАЦИЯ ПЛАГИНА

int plugin_init(struct plugin_name_args* plugin_info,
                struct plugin_gcc_version* version) {
    if (!plugin_default_version_check(version, &gcc_version)) {
        std::fprintf(stderr, "Plugin/GCC version mismatch\n");
        return 1;
    }

    register_pass_info pass_info;

    pass_info.pass = new my_gimple_pass(g);
    pass_info.reference_pass_name = "ssa";
    pass_info.ref_pass_instance_number = 1;
    pass_info.pos_op = PASS_POS_INSERT_AFTER;

    register_callback(plugin_info->base_name,
                      PLUGIN_PASS_MANAGER_SETUP,
                      nullptr,
                      &pass_info);

    return 0;
}