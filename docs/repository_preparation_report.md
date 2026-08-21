# Repository preparation report

Scope: prepare `reproducible_extSDDiP/` for public GitHub release with the
smallest possible number of changes to the recovered source. This gate does
not redo scientific reconstruction or validation; those remain governed by
`docs/recovered_code_validation.md`, `docs/historical_code_discrepancies.md`,
and `docs/scientific_reconstruction_plan.md`, unchanged.

## 1. Files added

Top level: `README.md`, `LICENSE` (MIT), `CITATION.cff`, `Makefile`,
`.gitignore`.

- `docs/build_and_run.md` -- compile-time structure constants, the 14
  command-line arguments, input-directory-per-structure mapping, and
  build/run commands, all read directly from `src/Header.h` and
  `src/Main.cpp`.
- `docs/repository_preparation_report.md` -- this report.
- `scripts/check_instance_format.py` -- dependency-free smoke test that
  validates an instance file's five-array structure and the
  `n = b*(1+R*(Sigma-1))` relation without requiring CPLEX. Verified against
  three real instances (`Instances_s4_s6/Scenario_Tree_161.txt`,
  `Instances_s8_s12/Scenario_Tree_440.txt`, `Instances_s20/Scenario_Tree_1.txt`)
  and against a deliberately truncated file (fails as expected, exit 1).
- `instances/Instances_s4_s6/`, `instances/Instances_s8_s12/`,
  `instances/Instances_s20/` -- copies of the 900 published instance files
  from `evidence/published_instances/extracted/`, same subfolder names and
  contents, byte-for-byte. **Not merged into one directory**: instance ids
  collide across subfolders with different contents (e.g.
  `Scenario_Tree_1.txt` exists in all three with different data), verified
  by direct filename comparison during this gate.
- `results/*.ods`, `results/published_historical_results.csv` -- copies of
  the three current (non-superseded) original result workbooks and the
  normalized cross-workbook table, from `evidence/published_results/`. The
  three deleted/superseded workbook aliases already documented as
  byte-identical or historical duplicates in
  `docs/published_results_inventory.md` were not duplicated into this
  public-facing copy.
- `results/runs/` (empty, `.gitkeep`) -- default destination for output of
  new runs, kept separate from the curated historical files above and
  `.gitignore`d except for the placeholder.

`evidence/` and `historical_source/` are unchanged and still contain the
full forensic record (frozen ZIPs, git bundle, manifests, ODS sheet
inventory, and this file's sibling audit docs).

## 2. Files modified

Exactly two source files, both by literal string-constant substitution only
(full diffs below; also reproducible with
`diff historical_source/<file> src/<file>`):

- `src/GlobalVariables.cpp` -- `INPUT`, `RESULTS`, `RESULTS_3`, `RESULTS_4`,
  `RESULTS_5` default values changed from author-machine absolute paths
  (`/home/quezada/...`, `/home/oc/...`) to repository-relative paths
  (`instances/Instances_s4_s6/`, `results/runs/...`). Added comments
  pointing at D01 in `docs/historical_code_discrepancies.md` and at
  `docs/build_and_run.md`.
- `src/DualLagrangianModel.cpp` -- `RESULTS_1` default value changed the
  same way. This global is unused elsewhere (its only write call is already
  commented out in the historical source), so the change has no runtime
  effect regardless of value.

One file removed from `src/` (not from `historical_source/`, which still
has it verbatim): `src/compilar.txt`. It is a plain-text build note (no
code), fully superseded by the new top-level `Makefile`, which reproduces
its first command exactly except for the CPLEX path. Removing it avoids two
different build recipes with different personal paths living side by side
in the same source directory.

No other file under `src/` was touched. `diff -rq historical_source src`
confirms exactly these two modified files and the one removed file, nothing
else.

## 3. Infrastructure vs. scientific changes

Every change above is infrastructure-only:

| File | Change | Category |
|---|---|---|
| `src/GlobalVariables.cpp` | 5 string literals: absolute -> relative path | infrastructure |
| `src/DualLagrangianModel.cpp` | 1 string literal (dead global): absolute -> relative path | infrastructure |
| `src/compilar.txt` | removed (superseded by `Makefile`) | infrastructure |
| `Makefile` (new) | portable build, same source list/flags/link order as `historical_source/compilar.txt`'s first command | infrastructure |
| `README.md`, `docs/build_and_run.md`, `docs/repository_preparation_report.md` (new) | documentation | infrastructure |
| `LICENSE`, `CITATION.cff`, `.gitignore` (new) | release metadata | infrastructure |
| `scripts/check_instance_format.py` (new) | format smoke test, does not implement or touch the algorithm | infrastructure |
| `instances/`, `results/` (new) | copies of already-published data | infrastructure |

**No scientific source behavior was changed.** No algorithm file
(`Main.cpp`, `Pre_Solving.cpp`, `NodalModel.cpp`, `LinearModel.cpp`,
`LagrangianModel.cpp`, `DualLagrangianModel.cpp` algorithmic code,
`CutGeneration.cpp`, `GlobalModel.cpp`, `UpperBounds.cpp`,
`TreeInequalities.cpp`, `Header.h`) was edited. The known historical
implementation characteristics catalogued in
`docs/historical_code_discrepancies.md` (D01-D15) -- the fixed 10-bit state
expansion, unchecked CPLEX solve status, the Phase-II tree-separator call
under nominal level 1, `int`-truncated shortest-path distances, the
stopping-window thresholds, the `/LB` vs `/UB` gap denominator, etc. -- are
all retained exactly as recovered and are summarized, not corrected, in
`README.md`'s "Historical implementation notes" section.

## 4. Build requirements

- GNU `g++` with C++11 support (verified present in this environment:
  `g++ (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0`); the source uses GNU
  variable-length-array extensions in `TreeInequalities.cpp`, so a
  strictly-conforming C++11 compiler is not sufficient.
- IBM ILOG CPLEX Studio with Concert Technology. Historical toolchain:
  **CPLEX Studio 12.8**, 64-bit Linux, static PIC libraries.
- **This environment has no CPLEX installation.** `ilcplex/ilocplex.h` and
  the CPLEX/Concert libraries are absent, so `make CPLEX_ROOT=...` cannot be
  exercised end-to-end here, and no compilation or run of the resulting
  binary has been validated in this gate. This is a pre-existing constraint
  already recorded in `docs/recovered_code_validation.md` ("Compilation
  assessment"), not a new blocker introduced by this preparation work.
- The `Makefile`'s source list, flags, macro (`-DIL_STD`), and link order
  were transcribed directly from `historical_source/compilar.txt`'s first
  command. Its wiring was exercised in this environment with a bogus
  `CPLEX_ROOT`: `g++` receives the full, correctly-ordered source list and
  flags and fails only at `#include <ilcplex/ilocplex.h>` (the expected
  failure with no CPLEX installed), not at argument parsing. Compiling
  against a real CPLEX install is still the first thing to do once one is
  available.

## 5. Remaining repository/documentation tasks

None of the following block release; they are follow-ups for whoever has a
licensed CPLEX installation or wants to extend the reproduction:

1. Compile with `make CPLEX_ROOT=...` against a real CPLEX Studio 12.8 (or
   nearby 12.x) install and confirm the binary runs on at least one small
   instance (e.g. instance 161 as in `docs/build_and_run.md`).
2. The open scientific-review items already tracked in
   `docs/historical_code_discrepancies.md` (D05, D06, D08, D09, D10, D12)
   and the "Real blockers and recommended next gate" section of
   `docs/recovered_code_validation.md` are unchanged by this work and still
   apply to anyone trying to reproduce Tables 1-4 exactly.
3. This repository does not automate the full 140-instance / multi-level /
   multi-method reproduction; `README.md`'s "Reproducing experiments"
   section only documents how to run one configuration by hand.
4. Consider adding a `.zenodo.json` or archiving the release to get a
   version-specific DOI, if the authors want one distinct from the article
   DOI already in `CITATION.cff`.

## Outcome

**READY FOR PUBLIC GITHUB RELEASE**, subject to the CPLEX-dependent
verification in item 1 above, which requires an environment this gate does
not have access to.
