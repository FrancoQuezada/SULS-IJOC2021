# Recovered extSDDiP code validation

## Scope and evidence

This gate validates the source recovered from Git commit
`80e6fd911724efc98172b9f218d10b5c0ebf2fd8`. The frozen ZIP remains at
`evidence/recovered_archives/Code_extSDDiP.zip` with SHA-256
`f26db63bd8aa0f53231c904a7801d3546c2b56ef2277152d42c4bf2d413cc009`.

The ZIP was extracted byte-for-byte to `historical_source/`. A second,
initially identical copy was placed in `src/`. Both contain the same 15 files
as the prior forensic extraction under `evidence/recovered_source/`; no
scientific or build edits have been made to either new directory.

The validation hierarchy was:

1. Sections 2--5 of `IJOC2021_R3.pdf`;
2. official supplement Appendices A--E, with provenance recorded in
   `evidence/supplement/provenance.txt`;
3. the recovered source;
4. the 900 archived instances and their manifest;
5. the original ODS files and the 6,080-row normalized historical table;
6. Code A/B only where already documented as secondary evidence.

The complete 4,341 lines in the 15 recovered files were inspected. The build
command does not compile `Pre_Solving_Tree.cpp`; it is an incomplete legacy
full-tree helper referring to globals absent from this archive and is not part
of the extSDDiP executable.

## Executive assessment

The recovered archive is the correct implementation baseline for the
published extSDDiP architecture. The extensive SULS model, compact
stage-wise-independent data representation, macro-stage decomposition,
stochastic subtrees, two phases, forward/backward passes, three value-function
cut families, path/tree inequalities, and upper-bound policy evaluators are all
present and recognizably implement the paper.

It is not yet safe to proceed directly to reproduction. Three findings require
a scientific decision before changing or running the public-facing source:

- solver status and optimality are never checked before objective values or
  duals are used as lower bounds and cut coefficients; local time limits and
  nonzero MIP gaps can therefore invalidate a cut or a reported lower bound if
  a subproblem is not solved to optimality;
- the tree separator stores numeric shortest-path distances in `int` arrays and
  Phase II invokes tree separation even in the nominal path-only case;
- the recovered snapshot is configured for the later Sigma=20 experiments and
  does not match all Tables 1--4 execution fingerprints, notably the main-table
  gap denominator and exhaustive Set-1 `R=20` upper-bound behavior.
- Phase II fixes the binary state expansion at ten bits (range 0--1023), while
  the paper's own `smax` rule exceeds 1023 for 37 of the 140 table instances.

These findings do not justify a rewrite. They require a small, explicit choice
between preserved historical behavior and scientifically guarded behavior,
followed by CPLEX-based validation.

## Component-by-component correspondence

| Component | Paper/supplement reference | Historical source file/function | Match status | Notes |
|---|---|---|---|---|
| SULS extensive formulation | Section 2.1, (1)--(4) | `GlobalModel.cpp::GlobalModel` | `MATCH` | Binary setup, nonnegative production/inventory, setup linking, balance, and expected objective are present. Compact stage templates are expanded into full-tree variables. The separate CPX time-limit discrepancy is not a formulation difference. |
| Big-M | Section 2.1, after (4) | `Pre_Solving.cpp::BigM` | `MATCH` | Backward recursion computes current demand plus maximum remaining branch demand, equivalent to `max_{ell in L(n)} d_{n,ell}` for the balanced stage-wise-independent tree. |
| Probabilities | Sections 2 and 5.1 | `ReadData.cpp`; `NodalModel.cpp`; `CutGeneration.cpp` | `EQUIVALENT` | Archived probabilities are unconditional `R^(1-sigma)`. Local subtree objectives use the model-0 template as conditional weights; cut aggregation uses child/parent probability ratios. All 900 archived vectors satisfy the published probability rule within serialization precision. |
| Instance reader | Section 5.1 and repository README | `ReadData.cpp::read_data` | `MATCH` | Reads demand, setup, holding, probability, and production arrays in that order with dimensions `n`, `n x 3`, `n x 4`, `n`, `n` for `I=1`. It is compatible with all archived instances when compile-time structure constants match. |
| Macro-stage decomposition | Section 2.2, (5)--(8) | `Header.h`; `Pre_Solving.cpp::find_stage`, `pre_compute_SubModels` | `MATCH` | `BkStg` is the paper's constant `G`; `Nbstage/BkStg` is `Gamma`. The number of models is `1+R(Gamma-1)`, exploiting stage-wise independence. Divisibility is assumed rather than checked. |
| Stochastic subtree construction | Sections 2.2--3 | `Pre_Solving.cpp::find_nodes`, `find_leaf`, `find_children` | `MATCH` | Full internal branching is retained. `NodeModel` maps every full local subtree node to the correct compact stage-realization template. Diagnostics covered the published structures and all reported `G` values. |
| Phase-I continuous state | Section 4.1, (21)--(29); supplement approximate algorithm | `NodalModel.cpp::InitialNodalModel`; `LinearModel.cpp::InitialLinearModel`; `Main.cpp` Phase I | `MATCH` | Entering and leaving stocks are continuous; Phase I generates strengthened Benders cuts and performs sampled forward/full-realization backward sweeps. |
| Phase-II binary state | Section 3.1, (9)--(17) | `Pre_Solving.cpp::variables_definition`; `Update_*Model`; `Main.cpp` Phase II | `SCIENTIFIC DIFFERENCE` | Copy bits are continuous in `[0,1]`, leaf state bits are binary, and powers of two reconstruct stock. The source hard-codes ten bits (range 0--1023) instead of computing the paper's instance-specific bit count. The paper's `smax` exceeds 1023 for 37 of the exact 140 table instances. |
| Phase-I to Phase-II cut transfer | Section 4.1 and supplement | `Update_NodalModel`, `Update_LinearModel`, `Update_LagrangianModel` | `EQUIVALENT` | Phase-I cuts remain written on `S`; adding `S=sum_h 2^h u_h` performs the mathematically required coefficient scaling implicitly. No explicit cut-copy conversion is required. |
| Sampling | Sections 3.2 and 5.2 | `Main.cpp::randu` and forward loops | `EQUIVALENT` | One realization/model and one subtree leaf are sampled per macro-stage (`K=1`) with replacement. The unreseeded global `rand()` stream is historical behavior, not a portable RNG specification. |
| Forward pass | Section 3.3; Algorithm 1 | `Main.cpp`; `SolveNodalModel`; `B_SolveNodalModel` | `MATCH` | Proceeds from the root macro-stage forward, passes the sampled leaving state, and stores the sampled leaf state for the backward pass. |
| Backward pass | Section 3.4; Algorithm 1; supplement | `Main.cpp` backward loops | `MINOR DIFFERENCE` | Sweeps macro-stages backward and every stage realization model. Integer and optimized-Lagrangian cuts may be generated only at command-line frequencies rather than at every iteration. Timeout can leave a partial sweep. |
| Strengthened Benders cuts | Section 3.4.1; Section 4.2.1; Appendix B | `SolveLinearModel`, `SolveLagrangianModel`; `StrengthenedBendersCutsGeneration`; binary variants | `MATCH` | Sign convention is consistent: the copy-constraint dual is negated in the Lagrangian objective and restored as the cut slope. Conditional realization weights are correct. Validity still depends on optimal LP/Lagrangian solves, which the code does not verify. |
| Lagrangian cuts | Section 3.4.1; Algorithm 1 | `DualLagrangianModel.cpp::SubGradient`; `LagrangianCutsGeneration`; linear-model variant | `MINOR DIFFERENCE` | The affine form and multiplier signs are correct. A 30-iteration Polyak subgradient heuristic with factor 2 and local solver limits replaces an assured solution of each Lagrangian dual to optimality. |
| Integer optimality cuts | Section 3.4.1; Zou et al. construction | `IntegerOptimalityCutsGeneration`; `LIntegerOptimalityCutsGeneration` | `MATCH` | The expression is `Q(ubar)[1-Hamming(u,ubar)]`: tight at the generating binary state and valid elsewhere because recourse costs are nonnegative. Its scientific validity still requires `Q(ubar)` to be the true optimal recourse value. |
| Path inequality (31) | Appendix A, (31); Section 4.2 | `LinearModel.cpp::CuttingPlaneGeneration` | `MATCH` | Walks each root-to-node path, chooses `min(x_n,d_{n,ell}y_n)` at the LP point, includes entering stock, retains every violation at most `-0.001`, and persists rows in linear, nodal, and Lagrangian models. |
| Tree inequality (32) | Appendix A, (32); Section 4.2 | `TreeInequalities.cpp::Pre_TreeInequalities`, `dijkstra` | `POSSIBLE BUG` | The reconstructed coefficients map to (32), but `distance` and `mindistance` are `int` while arc costs are numeric/fractional and `IloInfinity` is assigned to an `int`. This can change or destabilize the separation heuristic. |
| Strengthening level 0 | Section 4.2.1; supplement | `Main.cpp` with `Start_Cutting=0` | `MATCH` | Uses the initial LP formulation. |
| Strengthening level 1 | Section 4.2.1; supplement | `Main.cpp`; `SolveLinearModel`; `B_SolveLinearModel` | `SCIENTIFIC DIFFERENCE` | Phase I adds path inequalities as expected. Phase II calls `dijkstra` unconditionally whenever path cutting is enabled, so nominal level 1 also generates tree rows. |
| Strengthening level 2 | Section 4.2.1; supplement | `Main.cpp`; `TreeInequalities.cpp` | `MINOR DIFFERENCE` | Tree separation is present, but the numeric hazard applies and generated tree rows persist only in `LinearModels`; additions to nodal/Lagrangian MILPs are commented out. |
| Level transitions | Section 4.2.1; supplement detailed algorithms | `Main.cpp` lines controlling `Start_Cutting`, `Start_Tree` | `SCIENTIFIC DIFFERENCE` | Level 0 ends after a 20-iteration LB window at tolerance `0.1`; level 1 receives one further 20-iteration block. The paper says level 1 ends after consecutive iterations without violated path cuts; the source never tests that counter. |
| Phase transition | Section 4.1; experimental variants in Section 5.2 | `Main.cpp` between Phase I and Phase II | `MINOR DIFFERENCE` | Phase II starts after the Phase-I stopping/level logic, using the same wall-clock origin and accumulated cut models. There is no separately published Phase-I quota. |
| Stopping rules | Sections 3.5 and 5.2 | `Main.cpp` | `SCIENTIFIC DIFFERENCE` | Paper: 30 consecutive nonimproving iterations and 1,000 total. Source: Phase-I windows of 20 with tolerance `0.1`, then Phase-II window 10 with tolerance `0.01`; 1,000 total remains. |
| Lower-bound computation | Section 3.4 and Algorithm 1 | root calls in `Main.cpp`; nodal solvers | `POSSIBLE BUG` | The code reports `getObjValue()` as LB without checking `solve()`, status, or best bound. Phase-I root solves permit 1% MIP gap and all nodal solves have 30-second limits, so a nonoptimal incumbent need not be a valid lower bound. |
| Upper-bound evaluation | Section 5.2; supplement algorithms | `UpperBounds.cpp` | `SCIENTIFIC DIFFERENCE` | Exact and sampled policy evaluators exist, and the statistical endpoint is `mean+1.96*s/sqrt(actual N)`. The archived condition exhausts only when scenario count is at most `NbScen=1000`, whereas the paper and ODS indicate exhaustive evaluation for Set-1 `R=20` with 8,000 scenarios. Sampled routines use source-specific batching. |
| CPX baseline | Section 5.2 | `GlobalModel.cpp::GlobalModel` | `SCIENTIFIC DIFFERENCE` | Formulation matches, but source uses 900 seconds; paper and ODS use about 1,800. The missing Set-1 callback remains outside extSDDiP validation. |
| Historical output schema | Tables 1--4 and ODS | `Main.cpp` result recording | `SCIENTIFIC DIFFERENCE` | Columns and executable fingerprints match the ODS. The current source writes extSDDiP gap as `|UB-LB|/LB`; Tables 1--4 ODS values use `/UB`. Sigma=20 rows mostly match `/LB`, tying this snapshot more closely to the later run set. |
| Optional extended formulation | not the natural formulation used for Tables 1--4 | `LinearModel.cpp` under `Extended==1` | `POSSIBLE BUG` | Loop variable `m` shadows the model index in several `Ext_D[m]` accesses. The published Tables 1--4 values selected in the ODS are the `Natural` rows, so this does not block their natural-formulation reproduction. |

## Reader validation on archived instances

The exact C++ reader order and allocated dimensions were checked against the
archive, including the five requested structures:

| Structure | Example archived file | Compact records | C++-expected dimensions | Result |
|---|---|---:|---|---|
| `Sigma=4, b=1, R=10` | `Instances_s4_s6/Scenario_Tree_161.txt` | 31 | `31`, `31x3`, `31x4`, `31`, `31` | pass |
| `Sigma=6, b=1, R=20` | `Instances_s4_s6/Scenario_Tree_61.txt` | 101 | `101`, `101x3`, `101x4`, `101`, `101` | pass |
| `Sigma=8, b=2, R=5` | `Instances_s8_s12/Scenario_Tree_160.txt` | 72 | `72`, `72x3`, `72x4`, `72`, `72` | pass |
| `Sigma=8, b=5, R=5` | `Instances_s8_s12/Scenario_Tree_320.txt` | 180 | `180`, `180x3`, `180x4`, `180`, `180` | pass |
| `Sigma=12, b=1, R=3` | `Instances_s8_s12/Scenario_Tree_440.txt` | 34 | `34`, `34x3`, `34x4`, `34`, `34` | pass |

For every one of the 900 files, the existing forensic parser confirms five
arrays, `n=b[1+R(Sigma-1)]`, `10n` numeric values, and the probability rule.
No archived instance was modified or rescaled.

The paper's maximum path-demand bound was also computed directly from the
stage-wise-independent templates. It ranges from 267 to 2,838 on the exact 140
table instances; 37 exceed the source's representable ten-bit maximum of
1,023. Across all 900 archived files, 418 exceed 1,023. This does not prove
that an optimal policy reaches those inventory levels, but it rules out treating
the fixed bit count as automatically equivalent to the published `smax` rule.

## Macro-stage and subtree diagnostics

An independent implementation of the recovered index formulas checked every
local node produced by `find_nodes`: mapped indices remained in range and the
mapped compact record belonged to the expected global stage. Root, model, and
leaf counts also matched the recovered formulas.

| Structure | Tested `G` values | Result |
|---|---|---|
| `Sigma=4, b=1, R=10` | 2 | pass |
| `Sigma=6, b=1, R=20` | 2, 3 | pass |
| `Sigma=8, b=2, R=5` | 2, 4 | pass |
| `Sigma=8, b=5, R=5` | 2, 4 | pass |
| `Sigma=12, b=1, R=3` | 2, 3, 4, 6 | pass |

This is a structural check of the exact source formulas, not a replacement
tree framework.

## Mathematical diagnostics independent of CPLEX

Small exhaustive checks were used where solver access was unnecessary:

- integer-optimality form: all 16 four-bit states, with tightness at every
  generating state;
- strengthened-Benders/Lagrangian sign convention: 2,401 integer multiplier
  vectors against all 16 states of a toy nonnegative recourse function;
- path inequality (31): every subset for every prefix over 317 feasible
  integral plans of a three-period instance;
- tree inequality (32): every subset over 57 feasible integral plans of a
  root-with-two-children instance.

All mathematical family checks passed. They establish the validity of the
formulas and recovered sign conventions. They do not clear the C++ tree
shortest-path numeric bug or prove validity of a cut built from a prematurely
terminated CPLEX solve.

## Historical-results fingerprints

The normalized ODS data strongly associate this archive with the published
code family:

- executable names such as `Test_s4_c10_p1_b2.o` encode exactly
  `Nbstage=4`, `c=10`, `p=1`, `BkStg=2`;
- output fields, method/phase/cut labels, node counts, iteration counts, and VI
  counts follow `Main.cpp`'s result layout;
- current `Header.h` is configured as `Sigma=20, R=3, b=1, G=10`, matching an
  Appendix-E executable configuration;
- main Tables 1--4 ODS gaps use `100|UB-LB|/UB`, while Sigma=20 gaps mostly
  follow the recovered source's `/LB` expression;
- Set-1 `R=20` rows report zero sampled scenarios and short UB times consistent
  with the exact evaluator, even though the current `NbScen=1000` condition
  would route 8,000-scenario instances to sampling;
- ODS LB times cluster around 900 seconds and CPX times around 1,800 seconds.

The correct conclusion is that the archive is the historical architectural
baseline and likely a later experiment snapshot, but per-executable constants
and launch arguments must be reconstructed from ODS evidence before claiming
that the exact Tables 1--4 binaries have been recreated.

## Compilation assessment

`compilar.txt` specifies:

- GNU `g++` and C++11;
- IBM ILOG CPLEX Studio 12.8 Concert headers;
- static PIC CPLEX/Concert libraries for 64-bit Linux;
- `-DIL_STD`, `-lilocplex -lconcert -lcplex -lm -lpthread -ldl`;
- 13 `.cpp` files plus `Header.h`; `Pre_Solving_Tree.cpp` is excluded.

The current host has `g++` but no `ilcplex/ilocplex.h`, CPLEX libraries, or
visible CPLEX installation. A syntax/compile attempt therefore stops at the
missing proprietary header. Compilation is **not validated in this
environment**. With a licensed CPLEX-compatible installation, minimal
build-only preparation should be sufficient to attempt compilation:

1. replace absolute CPLEX include/library paths with cache variables;
2. replace absolute input/result directories with runtime paths;
3. expose the five compile-time structural constants per executable/config;
4. retain GNU extensions initially because `dijkstra` uses variable-length
   stack arrays despite requesting C++11;
5. compile only the source list from the first command in `compilar.txt`.

The program expects 14 positional arguments after the executable:

`instance-id CPX phase-I phase-II Benders integer-frequency strengthened-Benders Lagrangian-frequency path multicuts tree LB-seconds extended print`

There is no argument-count or zero-frequency validation. The input filename is
constructed as `Scenario_Tree_<instance-id>.txt`. Build modernization is
scientifically neutral, but it was deliberately deferred; `src/` is still the
untouched baseline.

## Real blockers and recommended next gate

The missing Set-1 CPX callback and instance generator do not block extSDDiP.
The following do block an immediate reproduction claim:

1. obtain a licensed CPLEX environment, preferably 12.8 first, and capture
   version, platform, parameters, statuses, primal values, and best bounds;
2. decide whether solver-status guards are a mandatory corrected behavior and
   whether a historical mode may retain the unchecked behavior;
3. decide how to handle the tree numeric types and the unintended Phase-II
   tree call in nominal level 1;
4. decide whether exact historical reproduction preserves the ten-bit cap and
   separately test whether it changes exact tiny/extensive-form optima;
5. reconstruct per-table compile constants and launch arguments from ODS/file
   fingerprints, including exhaustive Set-1 `R=20` evaluation;
6. run tiny CPLEX-backed exact recourse oracles before any paper instance, then
   one Set-1, Set-2, and Set-3 historical comparison.

No algorithm redesign or broad refactoring is recommended. Once these focused
decisions are reviewed, the recovered architecture should remain the direct
basis of the repository.

### SCIENTIFIC DISCREPANCIES REQUIRE REVIEW
