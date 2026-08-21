#!/usr/bin/env python3
"""Minimal smoke test for the SULS instance file format.

Validates that a Scenario_Tree_*.txt file matches the compact
stage-wise-independent format described in docs/instance_format.md:
five top-level array literals (demand, setup cost, holding cost,
probability, production cost) followed by a "-----" separator and a
"# stage = S  # children c = R  length of each stage p = b  nodes = n
# items I = 1" footer, with n = b*(1 + R*(Sigma - 1)).

This does not require CPLEX, pandas, or any third-party package -- it
only checks that the reader's expected structure holds, so it can run
before a CPLEX build is available.

Usage:
    python3 scripts/check_instance_format.py instances/Instances_s4_s6/Scenario_Tree_161.txt
"""
import ast
import re
import sys


def split_top_level_arrays(text):
    arrays = []
    depth = 0
    start = -1
    for index, char in enumerate(text):
        if char == "[":
            if depth == 0:
                start = index
            depth += 1
        elif char == "]":
            depth -= 1
            if depth == 0 and start >= 0:
                arrays.append(ast.literal_eval(text[start:index + 1]))
                start = -1
    if depth != 0:
        raise ValueError("unbalanced [ ] in instance file")
    return arrays


def check(path):
    text = open(path, encoding="utf-8").read()
    body, _, footer = text.partition("---")
    arrays = split_top_level_arrays(body)
    if len(arrays) != 5:
        raise AssertionError(f"expected 5 top-level arrays (demand, setup cost, "
                              f"holding cost, probability, production cost), found {len(arrays)}")
    demand, setup, holding, prob, production = arrays
    n = len(demand)

    assert len(setup) == n, f"setup cost has {len(setup)} rows, expected {n}"
    assert all(len(row) == 3 for row in setup), "setup cost rows must have width 3 (I=1)"
    assert len(holding) == n, f"holding cost has {len(holding)} rows, expected {n}"
    assert all(len(row) == 4 for row in holding), "holding cost rows must have width 4 (I=1)"
    assert len(prob) == n, f"probability vector has {len(prob)} entries, expected {n}"
    assert len(production) == n, f"production cost has {len(production)} entries, expected {n}"

    match = re.search(
        r"#\s*stage\s*=\s*(\d+).*?children\s*c\s*=\s*(\d+).*?"
        r"length of each stage p\s*=\s*(\d+).*?nodes\s*=\s*(\d+)",
        footer, re.DOTALL)
    if not match:
        raise AssertionError("footer does not match the expected "
                              "'# stage = S  # children c = R  length of each stage p = b  nodes = n' pattern")
    sigma, r, b, nodes = (int(g) for g in match.groups())

    expected_n = b * (1 + r * (sigma - 1))
    assert n == expected_n, f"n={n} but b*(1+R*(Sigma-1))={expected_n} for Sigma={sigma}, b={b}, R={r}"
    # The footer's "nodes" count is the highest 0-based node label N, i.e. n-1
    # records (nodes 0..N); it is not the record count itself.
    assert nodes == n - 1, f"footer 'nodes' ({nodes}) should equal n-1 ({n - 1})"

    print(f"OK  {path}")
    print(f"    Sigma={sigma}  b={b}  R={r}  n={n}  (5 arrays x {n} records)")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(__doc__)
        sys.exit(1)
    try:
        check(sys.argv[1])
    except Exception as exc:  # noqa: BLE001 -- smoke test: report and exit non-zero
        print(f"FAIL {sys.argv[1]}: {exc}")
        sys.exit(1)
