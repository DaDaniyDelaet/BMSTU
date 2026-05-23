#include <bits/stdc++.h>
using namespace std;

static const string ALPHABET = "abc";

int symIndex(char c) {
    if (c == 'a') return 0;
    if (c == 'b') return 1;
    if (c == 'c') return 2;
    return -1;
}

struct DFA {
    static const int N = 45;
    static const int K = 3;
    int start = 1;
    bool isFinal[N];
    int delta[N][K];

    DFA() {
        for (int q = 0; q < N; ++q) {
            for (int s = 0; s < K; ++s) delta[q][s] = 0;
            isFinal[q] = false;
        }

        int finals[] = {2,3,4,5,6,7,8,9,16,17,18,19,20,21,22,24,31};
        for (int f : finals) isFinal[f] = true;

        delta[1][0] = 26; delta[1][1] = 27;
        delta[2][0] = 32; delta[2][1] = 33; delta[2][2] = 34;
        delta[3][0] = 6;  delta[3][1] = 7;  delta[3][2] = 34;
        delta[4][0] = 8;  delta[4][1] = 9;  delta[4][2] = 38;
        delta[5][0] = 32; delta[5][1] = 35; delta[5][2] = 34;
        delta[6][0] = 32; delta[6][1] = 33; delta[6][2] = 10;
        delta[7][0] = 36; delta[7][1] = 11; delta[7][2] = 12;
        delta[8][0] = 32; delta[8][1] = 35; delta[8][2] = 10;
        delta[9][0] = 13; delta[9][1] = 14; delta[9][2] = 15;
        delta[10][0] = 41; delta[10][1] = 41;
        delta[11][1] = 28; delta[11][2] = 43;
        delta[12][0] = 16; delta[12][1] = 16;
        delta[13][0] = 2;  delta[13][1] = 17; delta[13][2] = 39;
        delta[14][0] = 3;  delta[14][1] = 18; delta[14][2] = 43;
        delta[15][0] = 19; delta[15][1] = 27;
        delta[16][0] = 32; delta[16][1] = 33; delta[16][2] = 20;
        delta[17][0] = 36; delta[17][1] = 11; delta[17][2] = 34;
        delta[18][0] = 21; delta[18][1] = 22; delta[18][2] = 38;
        delta[19][0] = 32; delta[19][1] = 35; delta[19][2] = 20;
        delta[20][0] = 23; delta[20][1] = 33; delta[20][2] = 34;
        delta[21][0] = 6;  delta[21][1] = 7;  delta[21][2] = 10;
        delta[22][0] = 21; delta[22][1] = 22; delta[22][2] = 15;
        delta[23][2] = 24;
        delta[24][0] = 32; delta[24][1] = 33; delta[24][2] = 25;
        delta[25][0] = 41; delta[25][2] = 2;
        delta[26][1] = 1;
        delta[27][1] = 28; delta[27][2] = 1;
        delta[28][0] = 29; delta[28][1] = 30;
        delta[29][1] = 2;
        delta[30][0] = 5;  delta[30][1] = 31;
        delta[31][0] = 36; delta[31][1] = 37; delta[31][2] = 38;
        delta[32][2] = 39;
        delta[33][2] = 40;
        delta[34][0] = 41;
        delta[35][0] = 26; delta[35][1] = 27; delta[35][2] = 40;
        delta[36][1] = 1;  delta[36][2] = 39;
        delta[37][0] = 29; delta[37][1] = 42; delta[37][2] = 43;
        delta[38][0] = 44; delta[38][1] = 27;
        delta[39][2] = 41;
        delta[40][0] = 2;
        delta[41][2] = 2;
        delta[42][0] = 3;  delta[42][1] = 4;
        delta[43][0] = 5;  delta[43][1] = 27;
        delta[44][1] = 1;  delta[44][2] = 2;
    }

    bool accepts(const string &w) const {
        int q = start;
        for (char c : w) {
            int s = symIndex(c);
            if (s < 0) return false;
            q = delta[q][s];
        }
        return isFinal[q];
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
    uniform_int_distribution<int> chDist(0, ALPHABET.size() - 1);
    int L = lenDist(rng);
    string w;
    w.reserve(L);
    for (int i = 0; i < L; ++i)
        w.push_back(ALPHABET[chDist(rng)]);
    return w;
}

int main() {
    DFA dfa;


    mt19937 rng(123456);
    int TESTS   = 100000;
    int MAX_LEN = 15;

    for (int t = 0; t < TESTS; ++t) {
        string w = randomWord(rng, MAX_LEN);
        bool r1 = acceptsByRegex(w);
        bool r2 = dfa.accepts(w);

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
