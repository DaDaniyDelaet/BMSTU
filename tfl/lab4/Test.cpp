#include <bits/stdc++.h>
using namespace std;

static inline bool check_lookahead(const string& s) {
    int n = (int)s.size();
    int i = 0;
    if (i >= n || s[i] != 'b') return false;
    while (i < n && s[i] == 'b') i++;
    if (i >= n || s[i] != 'a') return false;
    i++;

    while (i < n) {
        if (s[i] == 'a') {
            i++;
            continue;
        }
        if (i + 3 <= n && s.compare(i, 3, "bba") == 0) {
            i += 3;
            continue;
        }
        if (i + 4 <= n && s.compare(i, 4, "bbba") == 0) {
            i += 4;
            continue;
        }
        break;
    }

    while (i < n) {
        if (s[i] != 'b') return false;
        i++;
    }
    return true;
}

static bool go_naive(int pos, bool used_babb, bool had_bb_before_babb, const string& s) {
    int n = (int)s.size();
    if (pos == n) return used_babb && had_bb_before_babb;

    if (pos + 2 <= n && s.compare(pos, 2, "bb") == 0) {
        bool had2 = had_bb_before_babb || (!used_babb);
        if (go_naive(pos + 2, used_babb, had2, s)) return true;
    }

    if (!used_babb && had_bb_before_babb && pos + 4 <= n && s.compare(pos, 4, "babb") == 0) {
        if (go_naive(pos + 4, true, had_bb_before_babb, s)) return true;
    }

    return false;
}

static bool naive_matches(const string& s) {
    if (!check_lookahead(s)) return false;
    return go_naive(0, false, false, s);
}

static bool optimized_matches(const string& s) {
    if (!check_lookahead(s)) return false;

    int n = (int)s.size();
    int posA = -1, cntA = 0;
    for (int i = 0; i < n; i++) {
        if (s[i] == 'a') { cntA++; posA = i; }
        else if (s[i] != 'b') return false;
    }
    if (cntA != 1) return false;

    int leftLen = posA;
    int rightLen = n - posA - 1;

    if (leftLen < 3 || (leftLen % 2) == 0) return false;
    if (rightLen < 2 || (rightLen % 2) != 0) return false;

    for (int i = 0; i < posA; i++) if (s[i] != 'b') return false;
    for (int i = posA + 1; i < n; i++) if (s[i] != 'b') return false;

    return true;
}

struct Totals {
    long long slow_in = 0, fast_in = 0, slow_out = 0, fast_out = 0;
    long long cnt_in = 0, cnt_out = 0;
};

static void gen_and_test(int target_len, string& cur, Totals& tot) {
    if ((int)cur.size() == target_len) {
        using clk = chrono::high_resolution_clock;

        auto t0 = clk::now();
        bool slow = naive_matches(cur);
        auto t1 = clk::now();
        bool fast = optimized_matches(cur);
        auto t2 = clk::now();

        long long slow_ns = chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count();
        long long fast_ns = chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();

        if (slow != fast) {
            cout << "MISMATCH," << target_len << "," << cur << "," << slow << "," << fast << "\n";
            exit(0);
        }

        if (slow) {
            tot.cnt_in++;
            tot.slow_in += slow_ns;
            tot.fast_in += fast_ns;
        } else {
            tot.cnt_out++;
            tot.slow_out += slow_ns;
            tot.fast_out += fast_ns;
        }
        return;
    }

    cur.push_back('a');
    gen_and_test(target_len, cur, tot);
    cur.pop_back();

    cur.push_back('b');
    gen_and_test(target_len, cur, tot);
    cur.pop_back();
}

static inline double div_safe(long long a, long long b) {
    if (b == 0) return 0.0;
    return (double)a / (double)b;
}

int main() {
    const int MIN_LEN = 1;
    const int MAX_LEN = 20;

    cout << "L,cnt_in,slow_in_ns,fast_in_ns,speedup_in,cnt_out,slow_out_ns,fast_out_ns,speedup_out\n";

    for (int l = MIN_LEN; l <= MAX_LEN; l++) {
        Totals tot;
        string cur;
        cur.reserve(l);
        gen_and_test(l, cur, tot);

        double speedup_in = div_safe(tot.slow_in, tot.fast_in);
        double speedup_out = div_safe(tot.slow_out, tot.fast_out);

        cout << l << ","
             << tot.cnt_in << ","
             << tot.slow_in << ","
             << tot.fast_in << ","
             << fixed << setprecision(6) << speedup_in << ","
             << tot.cnt_out << ","
             << tot.slow_out << ","
             << tot.fast_out << ","
             << fixed << setprecision(6) << speedup_out
             << "\n";
    }

    return 0;
}
