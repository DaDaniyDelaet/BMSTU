#include <bits/stdc++.h>
using namespace std;

static const string ALPHABET = "abc";

int symIndex(char c) {
    if (c == 'a') return 0;
    if (c == 'b') return 1;
    if (c == 'c') return 2;
    return -1;
}

struct QPart {
    static const int K = 3;
    static const int N = 5;

    int start = 0;
    bool isFinal[N];
    vector<int> delta[N][K];

    QPart() {
        for (int q = 0; q < N; ++q) {
            isFinal[q] = false;
            for (int s = 0; s < K; ++s) delta[q][s].clear();
        }
        isFinal[0] = true;

        auto add = [&](int from, char ch, int to) {
            delta[from][symIndex(ch)].push_back(to);
        };

        add(0,'a',1);
        add(1,'b',0);

        add(0,'b',2);
        add(2,'c',0);

        add(2,'b',3);
        add(3,'b',0);
        add(3,'b',4);
        add(4,'b',0);
    }

    bitset<N> step(const bitset<N> &cur, char ch) const {
        int s = symIndex(ch);
        bitset<N> nxt; nxt.reset();
        for (int q = 0; q < N; ++q) if (cur.test(q)) {
            for (int to : delta[q][s]) nxt.set(to);
        }
        return nxt;
    }
};

struct KPart {
    static const int K = 3;
    static const int N = 12;

    int start = 0;
    bool isFinal[N];
    vector<int> delta[N][K];

    KPart() {
        for (int q = 0; q < N; ++q) {
            isFinal[q] = false;
            for (int s = 0; s < K; ++s) delta[q][s].clear();
        }

        auto add = [&](int from, char ch, int to) {
            delta[from][symIndex(ch)].push_back(to);
        };

        for (char ch : ALPHABET) add(0, ch, 1);
        for (char ch : ALPHABET) add(1, ch, 2);
        for (char ch : ALPHABET) add(2, ch, 3);
        for (char ch : ALPHABET) add(3, ch, 4);

        isFinal[4] = true;

        add(4,'c',5);
        add(5,'a',6);
        add(6,'c',4);

        add(4,'a',8);
        add(8,'c',9);
        add(9,'c',10);
        add(10,'c',4);

        add(4,'b',7);
        add(7,'c',11);
        add(11,'a',4);
    }

    bool acceptsFrom(const string &w, int pos) const {
        bitset<N> cur, nxt;
        cur.reset();
        cur.set(start);

        for (int i = pos; i < (int)w.size(); ++i) {
            int s = symIndex(w[i]);
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

struct PPart {
    static const int K = 3;
    static const int N = 5;

    int start = 0;
    bool isFinal[N];
    vector<int> delta[N][K];

    PPart() {
        for (int q = 0; q < N; ++q) {
            isFinal[q] = false;
            for (int s = 0; s < K; ++s) delta[q][s].clear();
        }

        auto add = [&](int from, char ch, int to) {
            delta[from][symIndex(ch)].push_back(to);
        };

        add(0,'b',1);
        add(1,'b',2);
        add(2,'a',3); add(2,'b',3);
        add(3,'a',4); add(3,'b',4);

        isFinal[4] = true;
        for (char ch : ALPHABET) add(4, ch, 4);
    }

    bool acceptsFrom(const string &w, int pos) const {
        bitset<N> cur, nxt;
        cur.reset();
        cur.set(start);

        for (int i = pos; i < (int)w.size(); ++i) {
            int s = symIndex(w[i]);
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

struct PKA {
    QPart qpart;
    KPart kpart;
    PPart ppart;

    bool accepts(const string &w) const {
        bitset<QPart::N> cur;
        cur.reset();
        cur.set(qpart.start);

        for (int i = 0; i <= (int)w.size(); ++i) {
            if (cur.test(0)) {
                if (kpart.acceptsFrom(w, i) && ppart.acceptsFrom(w, i))
                    return true;
            }

            if (i == (int)w.size()) break;

            int s = symIndex(w[i]);
            if (s < 0) return false;

            cur = qpart.step(cur, w[i]);
            if (cur.none()) return false;
        }
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
    PKA pka;

    mt19937 rng(123456);
    int TESTS = 100000;
    int MAX_LEN = 20;

    for (int t = 0; t < TESTS; ++t) {
        string w = randomWord(rng, MAX_LEN);

        bool r1 = acceptsByRegex(w);
        bool r2 = pka.accepts(w);

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
