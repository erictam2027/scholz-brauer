// Minimal addition-chain solver (Appendix A.2) and Hansen l0-chain
// validator (Appendix A.3), C++17.
//
// min_chain_length(n) : exact l(n) via iterative-deepening DFS between
//                       lambda(n)+1 and lambda(n)+wt(n)-1.
// min_l0(n, lc)       : returns lc if n admits a minimal l0-chain
//                       (Hansen subset rule), lc+1 otherwise. Prunes on
//                       the exact defect budget s(n) and Schonhage's
//                       exponential weight bound wt(n) <= 2^s(n).
//
// Build: g++ -O2 -std=c++17 addition_chains.cpp -o solver
// Run:   ./solver [n]      (default n = 1087, cf. Theorem 7.1)

#include <vector>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <cstdio>
#include <cstdlib>

using u64 = uint64_t;

inline int bit_length(u64 n) { return n ? 64 - __builtin_clzll(n) : 0; }
inline int popcount(u64 n) { return __builtin_popcountll(n); }

// ---------------------------------------------------------------------
// Minimal Addition-Chain Solver (Appendix A.2)
// ---------------------------------------------------------------------

bool dfs_chain(u64* chain, int len, u64 target, int remaining) {
    u64 last = chain[len - 1];
    if (last == target) return true;
    if (remaining <= 0) return false;

    // Pruning: if pure doublings can't reach the target
    if (last > 0 && remaining > 0) {
        int bl = bit_length(last);
        if (bl + remaining < 64 && (last << remaining) < target)
            return false;
    }
    if (last > target) return false;

    std::vector<u64> cands;
    for (int i = len - 1; i >= 0; --i) {
        for (int j = i; j >= 0; --j) {
            u64 s = chain[i] + chain[j];
            if (s <= last) break;
            if (s <= target) cands.push_back(s);
        }
    }
    std::sort(cands.begin(), cands.end(), std::greater<u64>());
    cands.erase(std::unique(cands.begin(), cands.end()), cands.end());

    for (u64 val : cands) {
        chain[len] = val;
        if (dfs_chain(chain, len + 1, target, remaining - 1))
            return true;
    }
    return false;
}

int min_chain_length(u64 n) {
    if (n <= 2) return n - 1;
    if ((n & (n - 1)) == 0) return bit_length(n) - 1;

    int lambda = bit_length(n) - 1;
    int weight = popcount(n);
    int lb = lambda + 1;
    int ub = lambda + weight - 1;

    u64 chain[64] = {1};
    for (int d = lb; d <= ub; ++d) {
        if (dfs_chain(chain, 1, n, d)) return d;
    }
    return ub;
}

// ---------------------------------------------------------------------
// Hansen l0-Chain Validator (Appendix A.3)
//
// A chain is an l0-chain if every step a_k adds an earlier element to
// the largest element of a designated subset H strictly less than a_k;
// the `anchor` index tracks the most recently promoted member of H.
// ---------------------------------------------------------------------

bool dfs_l0(u64* chain, int len, u64 target, int rem, int anchor, int small, int maxw, int target_w, int budget) {
    u64 last = chain[len - 1];
    if (last == target) return true;
    if (rem <= 0) return false;

    int bll = bit_length(last);

    // Pruning: reachability and exact defect budget constraints
    if (bll - 1 + rem < bit_length(target) - 1) return false;
    if (bll < 64 && (last << rem) < target) return false;
    if (small > budget) return false;

    // Pruning: Schonhage's exponential weight bound
    int qsmall = budget - small;
    if (qsmall < 31 && ((u64)maxw << qsmall) < (u64)target_w) return false;

    // Generate candidates strictly from the current H anchor
    std::vector<u64> cands;
    u64 A = chain[anchor];
    for (int j = len - 1; j >= 0; --j) {
        u64 s = A + chain[j];
        if (s > last && s <= target) cands.push_back(s);
    }

    std::sort(cands.begin(), cands.end(), std::greater<u64>());
    cands.erase(std::unique(cands.begin(), cands.end()), cands.end());

    for (u64 s : cands) {
        int is_big = (bit_length(s) == bll + 1) ? 1 : 0;
        int next_small = small + (is_big ? 0 : 1);
        if (next_small > budget) continue;

        int next_w = std::max(maxw, (int)popcount(s));
        chain[len] = s;

        // Option 1: Promote s into H (it becomes the new anchor)
        if (dfs_l0(chain, len + 1, target, rem - 1, len, next_small, next_w, target_w, budget))
            return true;

        // Option 2: Do not promote (keep the current anchor)
        if (dfs_l0(chain, len + 1, target, rem - 1, anchor, next_small, next_w, target_w, budget))
            return true;
    }
    return false;
}

int min_l0(u64 n, int optimal_lc) {
    u64 chain[80] = {1};
    int lambda = bit_length(n) - 1;
    int budget = optimal_lc - lambda;
    int target_w = popcount(n);

    // Begin Hansen search anchored at index 0, with 0 small steps and max_weight 1
    if (dfs_l0(chain, 1, n, optimal_lc, 0, 0, 1, target_w, budget))
        return optimal_lc;
    return optimal_lc + 1; // n does not admit a minimal l0-chain
}

// ---------------------------------------------------------------------
// Demo driver
// ---------------------------------------------------------------------

int main(int argc, char** argv) {
    u64 n = (argc > 1) ? std::strtoull(argv[1], nullptr, 10) : 1087ULL;
    int lambda = bit_length(n) - 1;
    int lc = min_chain_length(n);
    std::printf("n = %llu  lambda = %d  wt = %d  l(n) = %d  s(n) = %d\n",
                (unsigned long long)n, lambda, popcount(n), lc, lc - lambda);
    int l0 = min_l0(n, lc);
    std::printf("l0(n) = %d  (%s)\n", l0,
                l0 == lc ? "admits a minimal l0-chain" : "no minimal l0-chain at length l(n)");
    return 0;
}
