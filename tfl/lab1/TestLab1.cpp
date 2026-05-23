#include <bits/stdc++.h>
using namespace std;

struct Rule {
    string lhs;
    string rhs;
};

const int MAX_RULES = 20;   // ограничение на число правил в T'
const int MAX_LEN   = 20;   // максимум длины слова в фазз-тесте

static const string alphabet = "abcs"; 

// порядок a > b > c > s
int charRank(char ch) {
    switch (ch) {
        case 's': return 0;
        case 'c': return 1;
        case 'b': return 2;
        case 'a': return 3;
    }
    return -1;
}

// w1 ? w2 
bool wordGreater(const string &w1, const string &w2) {
    if (w1.size() != w2.size()) {
        return w1.size() > w2.size();
    }
    for (size_t i = 0; i < w1.size(); ++i) {
        int r1 = charRank(w1[i]);
        int r2 = charRank(w2[i]);
        if (r1 != r2) return r1 > r2;
    }
    return false; // равны
}

// шаг переписывания
bool rewriteOnce(const vector<Rule> &rules, string &w) {
    for (size_t i = 0; i < w.size(); ++i) {
        for (const auto &rule : rules) {
            const string &lhs = rule.lhs;
            if (lhs.empty()) continue;
            if (i + lhs.size() <= w.size() &&
                w.compare(i, lhs.size(), lhs) == 0) {
                string res;
                res.reserve(w.size() - lhs.size() + rule.rhs.size());
                res.append(w.begin(), w.begin() + (long)i);
                res.append(rule.rhs);
                res.append(w.begin() + (long)i + (long)lhs.size(), w.end());
                w.swap(res);
                return true;
            }
        }
    }
    return false;
}

// нормальная форма
string normalForm(const vector<Rule> &rules, string w) {
    int safe = 2000;
    while (safe-- > 0 && rewriteOnce(rules, w)) {}
    return w;
}

// Tn
vector<Rule> buildTn() {
    vector<Rule> Tn;
    // bb ~ s  -> bb -> s
    Tn.push_back({"bb", "s"});
    // bb ~ c  -> bb -> c
    Tn.push_back({"bb", "c"});
    // cc ~ cssc -> cssc -> cc
    Tn.push_back({"cssc", "cc"});
    // sss ~ ac -> sss -> ac
    Tn.push_back({"sss", "ac"});
    // aa ~ bc -> aa -> bc (a > b)
    Tn.push_back({"aa", "bc"});
    // bc ~ abba -> abba -> bc
    Tn.push_back({"abba", "bc"});
    return Tn;
}

// добавить правило, если его ещё нет
bool addRuleIfNew(vector<Rule> &rules, const Rule &r) {
    for (const auto &e : rules) {
        if (e.lhs == r.lhs && e.rhs == r.rhs) return false;
    }
    rules.push_back(r);
    return true;
}

// ориентация уравнения u ~ v в правило по  порядку
bool orientEquationAndAdd(vector<Rule> &rules,
                          const string &u, const string &v) {
    if (u == v) return false;
    Rule r;
    if (wordGreater(u, v)) {
        r.lhs = u; r.rhs = v;
    } else if (wordGreater(v, u)) {
        r.lhs = v; r.rhs = u;
    } else {
        return false;
    }
    return addRuleIfNew(rules, r);
}

// построение критических пар
void generateCriticalPairs(const vector<Rule> &rules,
                           vector<pair<string,string>> &newEquations) {
    int n = (int)rules.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            const string &l1 = rules[i].lhs;
            const string &r1 = rules[i].rhs;
            const string &l2 = rules[j].lhs;
            const string &r2 = rules[j].rhs;

            if (l1.size() < 2 || l2.size() < 2) continue;

            int maxOverlap = min((int)l1.size() - 1, (int)l2.size() - 1);
            for (int k = 1; k <= maxOverlap; ++k) {
                if (l1.compare((int)l1.size() - k, k, l2, 0, k) == 0) {
                    string w = l1 + l2.substr(k);

                    string w1 = w;
                    if (w1.compare(0, l1.size(), l1) == 0) {
                        w1.replace(0, l1.size(), r1);
                    }
                    w1 = normalForm(rules, w1);

                    string w2 = w;
                    int pos2 = (int)l1.size() - k;
                    if (w2.compare(pos2, l2.size(), l2) == 0) {
                        w2.replace(pos2, l2.size(), r2);
                    }
                    w2 = normalForm(rules, w2);

                    if (w1 != w2) {
                        newEquations.emplace_back(w1, w2);
                    }
                }
            }
        }
    }
}

// пополнение по Кнуту–Бендиксу
vector<Rule> knuthBendixCompletion(const vector<Rule> &Tn) {
    vector<Rule> rules = Tn;

    bool changed = true;
    while (changed && (int)rules.size() < MAX_RULES) {
        changed = false;

        // нормализуем правые части существующих правил
        for (auto &r : rules) {
            r.rhs = normalForm(rules, r.rhs);
        }

        // генерируем критические пары
        vector<pair<string,string>> newEqs;
        generateCriticalPairs(rules, newEqs);

        // нормализуем и ориентируем критические пары
        for (auto &cp : newEqs) {
            if ((int)rules.size() >= MAX_RULES) break;
            string u_nf = normalForm(rules, cp.first);
            string v_nf = normalForm(rules, cp.second);
            if (u_nf == v_nf) continue;
            if (orientEquationAndAdd(rules, u_nf, v_nf)) {
                changed = true;
            }
            if ((int)rules.size() >= MAX_RULES) break;
        }
    }

    return rules;
}

// случайное слово
string randomWord(mt19937 &gen, int minLen, int maxLen) {
    uniform_int_distribution<int> lenDist(minLen, maxLen);
    uniform_int_distribution<int> chDist(0, (int)alphabet.size() - 1);
    int len = lenDist(gen);
    string w;
    w.reserve(len);
    for (int i = 0; i < len; ++i) {
        w.push_back(alphabet[chDist(gen)]);
    }
    return w;
}

// случайный шаг переписывания по Tn
bool applyRandomStep(const vector<Rule> &rules, string &w, mt19937 &gen) {
    vector<pair<int,int>> matches;
    for (int i = 0; i < (int)w.size(); ++i) {
        for (int r = 0; r < (int)rules.size(); ++r) {
            const string &lhs = rules[r].lhs;
            if (lhs.empty()) continue;
            if (i + (int)lhs.size() <= (int)w.size() &&
                w.compare(i, lhs.size(), lhs) == 0) {
                matches.emplace_back(i, r);
            }
        }
    }
    if (matches.empty()) return false;
    uniform_int_distribution<int> pick(0, (int)matches.size() - 1);
    auto [pos, idx] = matches[pick(gen)];
    const Rule &rule = rules[idx];

    string res;
    res.reserve(w.size() - rule.lhs.size() + rule.rhs.size());
    res.append(w.begin(), w.begin() + pos);
    res.append(rule.rhs);
    res.append(w.begin() + pos + rule.lhs.size(), w.end());

    if ((int)res.size() > MAX_LEN) {
        return false;
    }
    w.swap(res);
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Rule> Tn = buildTn();

    cout << "Tn:\n";
    for (auto &r : Tn) {
        cout << r.lhs << " -> " << r.rhs << "\n";
    }
    cout << "\n";

    vector<Rule> Tprime = knuthBendixCompletion(Tn);

    cout << "T':\n";
    for (auto &r : Tprime) {
        cout << r.lhs << " -> " << r.rhs << "\n";
    }
    cout << "\n";

    // сравнение Tn и T' по нормальным формам T'
    mt19937 gen((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());

    int tests = 10000;
    int minLen = 1;
    int maxLen = 10;
    int maxSteps = 30;

    for (int t = 0; t < tests; ++t) {
        string w = randomWord(gen, minLen, maxLen);
        string wPrime = w;

        uniform_int_distribution<int> stepsDist(0, maxSteps);
        int steps = stepsDist(gen);
        for (int i = 0; i < steps; ++i) {
            if (!applyRandomStep(Tn, wPrime, gen)) break;
        }

        string nf_w  = normalForm(Tprime, w);
        string nf_wp = normalForm(Tprime, wPrime);

        if (nf_w != nf_wp) {
            cout << "Найден контрпример эквивалентности Tn и T'\n";
            cout << "w  = " << w << "\n";
            cout << "w' = " << wPrime << "\n";
            cout << "NF_T'(w)  = " << nf_w  << "\n";
            cout << "NF_T'(w') = " << nf_wp << "\n";
            cout << "Итерация: " << t << "\n";
            return 0;
        }
    }

    cout << "Фазз-тест завершён, контрпримеров не найдено в " << tests << " испытаниях.\n";
    return 0;
}
