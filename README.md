# extSDDiP for Stochastic Uncapacitated Lot-Sizing

## About

This repository implements **extSDDiP**, an extension of stochastic dual
dynamic integer programming (SDDiP) for the multi-stage stochastic
uncapacitated lot-sizing problem (SULS) under uncertain demand and costs.
The scenario tree is partially decomposed into macro-stage subtrees; a
cutting-plane procedure strengthens the linear relaxation of each subtree
with path and tree valid inequalities derived from the polyhedral structure
of SULS, on top of the usual Benders/strengthened-Benders/Lagrangian/integer
optimality value-function cuts.

## Reference

Quezada F, Gicquel C, Kedad-Sidhoum S (2022) Combining Polyhedral Approaches
and Stochastic Dual Dynamic Integer Programming for Solving the
Uncapacitated Lot-Sizing Problem Under Uncertainty. *INFORMS Journal on
Computing* 34(2):1024-1041. https://doi.org/10.1287/ijoc.2021.1118

## Repository provenance

This repository contains the historical C++ source that implements the
extSDDiP algorithm described in the article above, recovered from the
public code repository the authors published alongside it. `src/` is the
working copy prepared for this release; `historical_source/` is an
unmodified, frozen copy of the recovered code kept for provenance. The
recovery process, validation against the paper/supplement, and open
scientific questions are documented in `docs/` (see
`docs/recovered_code_validation.md` for the main assessment).

## Requirements

- A C++ compiler with C++11 support (tested with GNU `g++` 11; the code also
  relies on GNU variable-length-array extensions in `TreeInequalities.cpp`)
- IBM ILOG CPLEX Studio, Concert Technology included
  - **Historical environment: CPLEX Studio 12.8**, 64-bit Linux, static PIC
    libraries (see `historical_source/compilar.txt`)
  - Other 12.x installations with the standard `cplex/`/`concert/` layout are
    expected to work but have not been verified against this source

## Building

```
make CPLEX_ROOT=/opt/ibm/ILOG/CPLEX_Studio128
```

produces `build/extSDDiP`. This mirrors the original author command
(preserved verbatim in `historical_source/compilar.txt`) with the same
source list and flags; only the CPLEX install path is a variable instead of
being hard-coded to an author's home directory. Details, including the
original command and constants that vary per experiment structure, are in
[docs/build_and_run.md](docs/build_and_run.md).

Compilation requires a licensed CPLEX installation and has not been
validated in an environment without one -- see
[docs/repository_preparation_report.md](docs/repository_preparation_report.md).

## Running

```
./build/extSDDiP 161 0 1 1 1 1 0 1 0 0 0 60 0 1
```

runs instance 161 (`instances/Instances_s4_s6/Scenario_Tree_161.txt`, the
`Header.h` build must be compiled for that structure -- `Nbstage=4, c=10,
p=1, BkStg=2`) with Phase I + Phase II, generating Benders, integer, and
Lagrangian cuts every iteration but no strengthened-Benders/path/tree cuts
(strengthening level 0), a 60-second lower-bound budget, and verbose output.
Every argument is explained in
[docs/build_and_run.md](docs/build_and_run.md#command-line-arguments-srcmaincpp).

## Input data

Each instance file stores five arrays, in this order:

1. **Demand**
2. **Setup cost**
3. **Holding cost**
4. **Realization probability**
5. **Production cost**

These are not per-node values for a fully enumerated scenario tree. Because
the tree is stage-wise independent, each array stores one record per
*stage-realization*, not per tree node: `n = b*(1 + R*(Sigma-1))` compact
records describe the `b`-period root followed by `R` realization blocks (of
`b` periods each) per remaining stage, and `NodeModel` (in
`Pre_Solving.cpp`) re-expands this compact template into the full local
subtree at run time. See
[docs/published_instance_inventory.md](docs/published_instance_inventory.md)
for the exact file grammar and dimensions, and
`scripts/check_instance_format.py` for a runnable format check.

## Algorithms

This code implements, selectable via the command-line flags in
[docs/build_and_run.md](docs/build_and_run.md#command-line-arguments-srcmaincpp):

- **Phase I** -- continuous-state SDDiP with (strengthened) Benders cuts
- **Phase II** -- binary-state SDDiP adding Lagrangian and integer-optimality
  cuts over a fixed-width binary state expansion
- **Strengthening levels 0/1/2** -- no cutting-plane strengthening; path
  inequalities (31); path + tree inequalities (32)
- an optional plain extensive-formulation CPLEX baseline (`CPX`)

The cut families, path/tree inequalities, and strengthening levels are
defined in the paper (Sections 3-4, Appendix A of the supplement); this
README does not restate that mathematics. `docs/recovered_code_validation.md`
maps every algorithmic component to its source location and paper reference.

## Instances

The original 900 published instances are provided under `instances/`, split
into the three subfolders used in the original distribution
(`Instances_s4_s6/`, `Instances_s8_s12/`, `Instances_s20/`); the exact 140
files underlying Tables 1-4 are identified in
[docs/published_instance_inventory.md](docs/published_instance_inventory.md).
Do not merge the three subfolders into one directory -- instance ids are
reused with different contents across them.

## Historical results

The original result workbooks (`.ods`) are provided under `results/`,
together with `results/published_historical_results.csv`, a normalized
6,080-row table combining all sheets with source workbook/row provenance.
`docs/published_results_inventory.md` documents the workbooks and verifies
several Tables 1-4 entries against the raw rows. Output from new runs of
this code goes to `results/runs/` (not tracked in version control) and is
never mixed with the curated historical files above.

## Reproducing experiments

To run a single historical configuration:

1. Pick a paper set/structure and set the matching constants in
   `src/Header.h` (table in
   [docs/build_and_run.md](docs/build_and_run.md#compile-time-structure-constants-srcheaderh)).
2. Point `INPUT` in `src/GlobalVariables.cpp` at the matching `instances/`
   subfolder (also tabulated there) and rebuild.
3. Run with the 14 arguments documented in
   [docs/build_and_run.md](docs/build_and_run.md#command-line-arguments-srcmaincpp),
   choosing the instance id and cut/phase flags for the configuration you
   want.

A full automated reproduction of all 140 Tables 1-4 instances across every
method/level combination is not set up by this repository. Reconstructing
the exact per-row argument combinations from the recovered archive alone is
an open item -- see D08 and D10 in
[docs/historical_code_discrepancies.md](docs/historical_code_discrepancies.md).

## Historical implementation notes

The recovered source has a few characteristics worth knowing before reading
results from it. These are retained as historical implementation
characteristics, not defects to silently correct; each is discussed in full
in [docs/historical_code_discrepancies.md](docs/historical_code_discrepancies.md)
and [docs/recovered_code_validation.md](docs/recovered_code_validation.md).

- CPLEX solve status/optimality is never checked before an objective value
  or dual is consumed as a lower bound or cut coefficient (D09).
- The Phase-II binary state expansion is a fixed 10 bits (range 0-1023);
  the paper's own `smax` rule exceeds that range for 37 of the 140 Tables
  1-4 instances (D02).
- Phase II's tree-inequality separator (`TreeInequalities.cpp::dijkstra`)
  is called whenever path cutting is enabled, even at nominal strengthening
  level 1, which is documented as path-inequalities-only (D05); the same
  routine stores fractional shortest-path distances in `int` (D06).
- The source's stopping windows (20/10 iterations, 0.1/0.01 LB tolerance)
  differ from the paper's stated 30-consecutive-iteration rule (D03).
- The reported extSDDiP gap denominator in this snapshot is `/LB`; the main
  Tables 1-4 published gaps use `/UB` (D12) -- always recompute gaps from
  the raw LB/UB fields rather than trusting the printed column.

## Citation

Please cite the article above when using this code, and see
[CITATION.cff](CITATION.cff) for a machine-readable citation of both the
article and this repository. This code is released under the [MIT
License](LICENSE).

Quezada, F., Gicquel, C., & Kedad-Sidhoum, S. (2022). Combining polyhedral approaches and stochastic dual dynamic integer programming for solving the uncapacitated lot-sizing problem under uncertainty. INFORMS Journal on Computing, 34(2), 1024-1041.
