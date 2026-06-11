# scholz-brauer

Companion code for *Computational Bounds and Trailing Shift-Stability for the Scholz–Brauer Conjecture at Hamming Weight Seven* (Eric Tam).

Verifies Scholz's inequality `l(2^n − 1) ≤ (n − 1) + l(n)` over subdomains of `S_7 = {n : wt(n) = 7}`, partitioned by the maximal zero-gap parameter `γ(n)`.

## Contents

| File | Paper section | Purpose |
|---|---|---|
| `compact_domain.py` | Appendix A.1 | Enumerates the compact domain `D_C` (`γ(n) ≤ 3`, all `4^7 = 16,384` gap vectors) and the 345 outliers above Clift's verified Hansen bound `N0 = 5,784,688` |
| `addition_chains.cpp` | Appendix A.2–A.3 | Exact IDDFS minimal addition-chain solver `l(n)`, plus the Hansen `l0`-chain validator with exact defect-budget and Schönhage weight-bound pruning |

## Usage

Compact-domain enumeration:

```sh
python3 compact_domain.py
```

Chain solver and `l0` validator (C++17):

```sh
g++ -O2 -std=c++17 addition_chains.cpp -o solver
./solver 1087        # l(1087) = 14, cf. Theorem 7.1
```

`min_chain_length(n)` computes `l(n)` by iterative-deepening DFS between `λ(n)+1` and `λ(n)+wt(n)−1`. `min_l0(n, l)` checks whether `n` admits a minimal `l0`-chain (Hansen's subset rule), pruning on the exact Knuth small-step defect budget `s(n)` and Schönhage's bound `wt(n) ≤ 2^s(n)`.
