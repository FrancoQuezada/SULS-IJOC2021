# Build and run reference

This page documents the compile-time constants and command-line arguments of
the recovered `extSDDiP` executable. It is a reference for
[README.md](../README.md)'s Building/Running sections, not a new
specification -- every field below is read directly from `src/Header.h` and
`src/Main.cpp`.

## Compile-time structure constants (`src/Header.h`)

One compiled binary corresponds to exactly one scenario-tree structure. To
build for a different structure, edit these constants and rebuild (this is
historical behavior, not a limitation introduced for this release --
see `docs/historical_code_discrepancies.md`, D01):

| Constant | Meaning | Paper symbol |
|---|---|---|
| `Nbstage` | Number of stages in the full tree | `Sigma` |
| `c` | Number of children per node in the last period of each stage | `R` |
| `p` | Length of each stage, in periods | `b` |
| `BkStg` | Number of stages per macro-stage subtree | `G` |

`src/Header.h` ships configured for `Sigma=20, b=1, R=3, BkStg=10` (an
Appendix E / Table 14 configuration). The four Tables 1-4 structures are:

| Set | `Nbstage` (Sigma) | `c` (R) | `p` (b) | `BkStg` (G) |
|---|---:|---:|---:|---:|
| 1 | 4 | 10 or 20 | 1 | 2 |
| 2 | 6 | 10 or 20 | 1 | 3 |
| 3 | 8 | 5 | 2 or 5 | 4 |
| 4 | 12 | 3 | 1 | 6 |

Source: `docs/published_instance_inventory.md` and executable-name
fingerprints in `docs/historical_code_discrepancies.md` (D01) /
`docs/recovered_code_validation.md`.

The `INPUT` default in `src/GlobalVariables.cpp` must point at the
`instances/` subfolder that matches the structure you compiled (see
[Input data](#input-data-directories) below); it is not derived
automatically from the constants above.

## Command-line arguments (`src/Main.cpp`)

The program takes 14 positional arguments. There is no `argc` validation
(historical behavior, `docs/historical_code_discrepancies.md` D15) --
omitting an argument crashes rather than printing usage.

| # | Argument | Meaning |
|---:|---|---|
| 1 | `instance-id` | Numeric id; the reader opens `<INPUT><instance-id>.txt`, i.e. `Scenario_Tree_<instance-id>.txt` under the configured input directory |
| 2 | `CPX` | 1 = also solve the plain extensive formulation (`GlobalModel`, a 900s CPLEX baseline; see D13) |
| 3 | `phase-I` | 1 = run Phase I (continuous-state strengthened Benders) |
| 4 | `phase-II` | 1 = run Phase II (binary-state cuts) |
| 5 | `Benders` | 1 = generate (non-strengthened) Benders cuts |
| 6 | `integer-frequency` | Generate integer-optimality cuts every N iterations (`it % N == 0`); values 1-10 also add an `Int=N_` tag to the output filename, but the modulo behavior itself is not range-checked -- 0 causes a division by zero (D08, D15) |
| 7 | `strengthened-Benders` | 1 = generate strengthened Benders cuts |
| 8 | `Lagrangian-frequency` | Generate Lagrangian cuts every N iterations (`it % N == 0`), same filename-tag/no-range-check caveat as column 6 |
| 9 | `path` | 1 = generate path inequalities (31) (strengthening level >= 1) |
| 10 | `multicuts` | 1 = use the multi-cut cut-generation variant |
| 11 | `tree` | 1 = generate tree inequalities (32) (strengthening level 2; note D05 -- Phase II calls the tree separator whenever `path` cutting is on, regardless of this flag) |
| 12 | `LB-seconds` | Wall-clock budget (seconds) for the lower-bound/SDDiP loop before it returns and upper-bound evaluation begins |
| 13 | `extended` | 1 = build the optional extended formulation (D14, not needed for Tables 1-4; leave at 0) |
| 14 | `print` | Verbosity flag |

These names and the family/level mapping are read from `src/Main.cpp`; the
exact per-table-row argument combinations used for Tables 1-4 are not fully
pinned down by the recovered archive alone (see
`docs/historical_code_discrepancies.md`, D08 and D10, and
`docs/recovered_code_validation.md`). Treat any command below as an
illustration of the CLI, not a certified reproduction of a specific
published row.

## Input data directories

`INPUT` in `src/GlobalVariables.cpp` is a compile-time default directory
prefix; the reader concatenates it with `Scenario_Tree_<id>.txt` at runtime
(`src/Main.cpp`). Because the same numeric instance id is reused across the
three published subfolders with different contents (e.g. `Scenario_Tree_1.txt`
exists in all of `instances/Instances_s4_s6/`, `instances/Instances_s8_s12/`,
and `instances/Instances_s20/`), `INPUT` must point at the one subfolder that
matches the structure compiled into `Header.h`:

| `Header.h` structure | `INPUT` |
|---|---|
| Sets 1-2 (`Nbstage` 4 or 6) | `instances/Instances_s4_s6/` |
| Sets 3-4 (`Nbstage` 8 or 12) | `instances/Instances_s8_s12/` |
| Appendix E (`Nbstage=20`) | `instances/Instances_s20/` |

Output goes to `RESULTS` (default `results/runs/Results_`), also in
`src/GlobalVariables.cpp`; `results/runs/` is created empty by this
repository (`.gitignore`d after the first run) and is separate from the
curated historical result workbooks in `results/*.ods` and
`results/published_historical_results.csv`.

## Building

Original author command (preserved verbatim in
`historical_source/compilar.txt`):

```
g++ Header.h Main.cpp ReadData.cpp GlobalVariables.cpp Pre_Solving.cpp NodalModel.cpp \
    LinearModel.cpp LagrangianModel.cpp DualLagrangianModel.cpp CutGeneration.cpp \
    GlobalModel.cpp UpperBounds.cpp TreeInequalities.cpp -o Test.o \
    -I /home/oc/Documents/opt/ibm/ILOG/CPLEX_Studio128/cplex/include/ \
    -I /home/oc/Documents/opt/ibm/ILOG/CPLEX_Studio128/cplex/include/ilcplex/ \
    -I /home/oc/Documents/opt/ibm/ILOG/CPLEX_Studio128/concert/include/ \
    -DIL_STD \
    -L /home/oc/Documents/opt/ibm/ILOG/CPLEX_Studio128/cplex/lib/x86-64_linux/static_pic/ \
    -L /home/oc/Documents/opt/ibm/ILOG/CPLEX_Studio128/concert/lib/x86-64_linux/static_pic/ \
    -lilocplex -lconcert -lcplex -lm -lpthread -ldl -w -std=c++11
```

Portable equivalent, using the top-level `Makefile`:

```
make CPLEX_ROOT=/opt/ibm/ILOG/CPLEX_Studio128
```

This produces `build/extSDDiP` from the same 12 source files with the same
flags; only the CPLEX path is parameterized. `Pre_Solving_Tree.cpp` is not
part of either command -- it is an incomplete legacy helper referring to
globals absent from this archive
(`docs/recovered_code_validation.md`).

CPLEX Studio 12.8 is the historical toolchain. Compilation has **not** been
validated in an environment without a licensed CPLEX installation; see
`docs/repository_preparation_report.md` for what remains to be checked once
one is available.

## Running

```
./build/extSDDiP 161 0 1 1 1 1 0 1 0 0 0 60 0 1
```

With `Header.h` compiled for Set 1 (`Nbstage=4, c=10, p=1, BkStg=2`) and
`INPUT` set to `instances/Instances_s4_s6/`, this reads
`Scenario_Tree_161.txt` and runs Phase I + Phase II, generating Benders,
integer, and Lagrangian cuts every iteration (frequency arguments 6 and 8
are both 1) but no strengthened-Benders/path/tree cuts (strengthening level
0), with a 60-second LB budget, the natural (non-extended) formulation, and
verbose output.

## Smoke test (no CPLEX required)

`scripts/check_instance_format.py` validates that an instance file matches
the five-array format `read_data` expects, without needing CPLEX:

```
python3 scripts/check_instance_format.py instances/Instances_s4_s6/Scenario_Tree_161.txt
```
