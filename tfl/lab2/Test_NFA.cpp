#include <bits/stdc++.h>
using namespace std;

static const string ALPHABET = "abc";

int symIndex(char c) {
    if (c == 'a') return 0;
    if (c == 'b') return 1;
    if (c == 'c') return 2;
    return -1;
}

struct NFA {
    static const int K = 3;
    static const int N = 14;

    int start = 0;
    bool isFinal[N];
    vector<int> delta[N][K];

    NFA() {
        for (int q = 0; q < N; ++q) {
            isFinal[q] = false;
            for (int s = 0; s < K; ++s) delta[q][s].clear();
        }

        isFinal[6] = true;

        auto add = [&](int from, char ch, int to) {
            int s = symIndex(ch);
            delta[from][s].push_back(to);
        };

        add(0,'a',1); add(1,'b',0);
        add(0,'b',2); add(2,'c',0);

        add(2,'b',3);
        add(3,'b',0);
        add(3,'b',5);
        add(5,'b',0);

        add(3,'a',4);
        add(3,'b',5);

        add(4,'a',6); add(4,'b',6);
        add(5,'a',6); add(5,'b',6);

        add(6,'c',7);
        add(7,'a',8);
        add(8,'c',6);

        add(6,'a',9);
        add(9,'c',10);
        add(10,'c',11);
        add(11,'c',6);

        add(6,'b',12);
        add(12,'c',13);
        add(13,'a',6);
    }

    bool accepts(const string &w) const {
        bitset<N> cur, nxt;
        cur.reset();
        cur.set(start);

        for (char ch : w) {
            int s = symIndex(ch);
            if (s < 0) return false;

            nxt.reset();
            for (int q = 0; q < N; ++q) if (cur.test(q)) {
                for (int to : delta[q][s]) nxt.set(to);
            }
            cur = nxt;
            if (cur.none()) return false;
        }

        for (int q = 0; q < N; ++q)
            if (cur.test(q) && isFinal[q]) return true;
        return false;
    }
};

const regex REGEX_LANG(
    R"(^(ab|bc|bbbb|bbb)*bb(a|b)(a|b)(cac|accc|bca)*$)"
);

bool acceptsByRegex(const string &w) {
    return regex_match(w, REGEX_LANG);
}

string randomWord(mt19937 &rng, int maxLen) {
    uniform_int_distribution<int> lenDist(0, maxLen);
    uniform_int_distribution<int> chDist(0, (int)ALPHABET.size() - 1);
    int L = lenDist(rng);
    string w;
    w.reserve(L);
    for (int i = 0; i < L; ++i) w.push_back(ALPHABET[chDist(rng)]);
    return w;
}

int main() {
    NFA nfa;

    mt19937 rng(123456);
    int TESTS = 100000;
    int MAX_LEN = 15;

    for (int t = 0; t < TESTS; ++t) {
        string w = randomWord(rng, MAX_LEN);
        bool r1 = acceptsByRegex(w);
        bool r2 = nfa.accepts(w);

        if (r1 != r2) {
            cout << "Mismatch\n";
            cout << w << "\n";
            cout << (r1 ? 1 : 0) << "\n";
            cout << (r2 ? 1 : 0) << "\n";
            return 0;
        }
    }

    cout << "Equivalent on " << TESTS << " random tests\n";
    return 0;
}
