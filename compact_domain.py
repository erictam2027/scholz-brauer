"""Compact domain enumeration (Appendix A.1).

Enumerates the compact domain D_C = {n : wt(n) = 7, gamma(n) <= 3} via the
gap-vector recursion (Lemma 2.3), and lists the outliers above Clift's
exhaustively verified Hansen bound N0 = 5,784,688.
"""

import itertools


def compact_domain():
    results = []
    for gaps in itertools.product(range(4), repeat=7):
        internal_gaps = gaps[:6]
        trailing_gap = gaps[6]
        n = 1
        for g in internal_gaps:
            n = (n << (g + 1)) | 1
        n = n << trailing_gap
        results.append((n, gaps))
    return results


def compact_outliers(N0=5784688):
    data = compact_domain()
    return sorted(n for (n, gaps) in data if n > N0)


if __name__ == "__main__":
    domain = compact_domain()
    outliers = compact_outliers()
    print(f"|D_C| = {len(domain)} configurations")
    print(f"max n = {max(n for n, _ in domain)} (< 2^28 = {2**28})")
    print(f"compact outliers above N0 = 5,784,688: {len(outliers)}")
    print(f"smallest outlier = {outliers[0]}, largest = {outliers[-1]}")
