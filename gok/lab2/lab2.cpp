#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

int main()
{
    LLVMContext context;
    Module module("lab2", context);
    IRBuilder<> builder(context);

    FunctionType *mainType = FunctionType::get(builder.getInt32Ty(), false);

    Function *mainFunction = Function::Create(
        mainType,
        Function::ExternalLinkage,
        "main",
        module
    );

    BasicBlock *entryBlock = BasicBlock::Create(
        context,
        "entry",
        mainFunction
    );

    builder.SetInsertPoint(entryBlock);

    Value *left = ConstantInt::get(builder.getInt32Ty(), 353);
    Value *right = ConstantInt::get(builder.getInt32Ty(), 48);

    Value *sum = builder.CreateAdd(left, right, "sum");

    builder.CreateRet(sum);

    module.print(outs(), nullptr);

    return 0;
}