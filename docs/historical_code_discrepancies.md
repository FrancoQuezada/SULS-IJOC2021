# Historical code discrepancy log

## Policy

`historical_source/` is immutable evidence. `src/` is currently byte-identical
and has not been corrected. Every future scientific edit must refer to an entry
below and preserve enough information to reproduce historical behavior when
that behavior plausibly generated published observations.

Categories used here are: harmless implementation detail, equivalent
formulation, undocumented experimental parameter, probable historical bug,
and paper/source inconsistency.

## D01 -- Compile-time experiment structure and absolute paths

- **Location:** `Header.h` constants; `GlobalVariables.cpp` paths;
  `compilar.txt`.
- **Historical behavior:** each executable is compiled for one
  `(Sigma,R,b,G)` structure and uses author-machine input/result/CPLEX paths.
- **Paper requirement:** Section 5.2 reports several structures and CPLEX 12.8,
  without prescribing a software configuration interface.
- **Category:** harmless implementation detail.
- **Scientific effect:** none if the same constants and files are selected.
- **ODS evidence:** executable names encode all four structure constants.
- **Decision:** preserve in `historical_source`; replace paths/configuration in
  a later build-only change, with behavior-neutral equivalence tests.

## D02 -- Fixed ten-bit state expansion

- **Location:** `Main.cpp` (`ExpSize=10`), `Pre_Solving.cpp::variables_definition`.
- **Historical behavior:** coefficients are `2^0,...,2^9`, giving range
  0--1023 for every instance.
- **Paper requirement:** Section 5.2 sets the bit count from
  `ceil(log2(smax))` (subject to the paper's indexing convention).
- **Category:** undocumented experimental parameter / paper-source
  inconsistency.
- **Scientific effect:** no loss if all relevant inventories fit; otherwise
  Phase II truncates the state space and can solve a restricted problem.
- **Evidence:** direct archive evaluation gives paper-rule `smax` values from
  267 to 2,838 on the exact 140 table instances; 37 exceed 1,023. Across all
  900 files, 418 exceed 1,023. The archive and ODS nevertheless identify ten as
  the historical source setting.
- **Decision:** requires review. Preserve ten bits in a historical mode, but
  compare it with an instance-sized corrected mode on exact small/extensive
  oracles before making an equivalence claim.

## D03 -- Stopping windows differ from the paper

- **Location:** `Main.cpp` Phase-I and Phase-II loop conditions.
- **Historical behavior:** Phase I uses 20-iteration windows with `0.1` LB
  tolerance; Phase II uses ten iterations with `0.01`; total iterations remain
  capped at 1,000.
- **Paper requirement:** Section 5.2 states 30 consecutive iterations without
  LB improvement.
- **Category:** paper/source inconsistency.
- **Scientific effect:** changes iteration counts, strengthening transitions,
  and time assigned to each phase.
- **ODS evidence:** result iterations are compatible with this being historical
  behavior but do not independently expose the thresholds.
- **Decision:** preserve as a candidate historical mode; corrected/paper mode
  needs an explicit reviewed stopping specification.

## D04 -- Strengthening-level transition criterion

- **Location:** `Main.cpp` updates of `Start_Cutting`, `Start_Tree`, and
  `LimStop2`.
- **Historical behavior:** level 0 changes after an LB-stagnation block; level
  1 receives another fixed 20-iteration block.
- **Paper/supplement requirement:** Section 4.2.1 changes from level 1 after a
  predefined number of consecutive iterations with no violated path
  inequalities.
- **Category:** paper/source inconsistency.
- **Scientific effect:** changes which strengthened-Benders cuts are generated
  and when.
- **ODS evidence:** level-specific VI counts corroborate the use of sequential
  levels but cannot identify the transition rule.
- **Decision:** preserve now and expose the criterion in future run metadata;
  do not silently substitute the paper rule.

## D05 -- Tree separation runs in nominal Phase-II level 1

- **Location:** `LinearModel.cpp::B_SolveLinearModel`, inside
  `if(cutting==1)`.
- **Historical behavior:** `dijkstra(...)` is called unconditionally after path
  separation. If the separate tree flag is one, it is called a second time.
- **Paper/supplement requirement:** level 1 generates path inequalities only;
  level 2 adds tree inequalities.
- **Category:** probable historical bug.
- **Scientific effect:** nominal `lambda_max=1` Phase-II models can contain tree
  inequalities, blurring the scientific comparison between levels 1 and 2.
- **ODS evidence:** positive VI counts establish that cuts were used, but rows
  do not identify family membership, so actual binary behavior cannot be
  disambiguated from results alone.
- **Decision:** requires scientific review. A corrected call guard is small but
  changes historical trajectories and must not be applied silently.

## D06 -- Tree-separator numeric types

- **Location:** `TreeInequalities.cpp::dijkstra`.
- **Historical behavior:** numeric/fractional Concert arc costs are assigned to
  `int distance[]` and `int mindistance`; `IloInfinity` is also assigned to an
  `int`. GNU variable-length stack arrays are used.
- **Paper/supplement requirement:** Appendix A requires the Guan et al.
  heuristic separation over the LP solution, whose setup values are generally
  fractional.
- **Category:** probable historical bug.
- **Scientific effect:** truncation/overflow can select the wrong predecessor
  paths, miss violations, or destabilize separation. Rows reconstructed from a
  valid predecessor order still have the form of (32), so this primarily
  threatens separation fidelity and robustness rather than the family itself.
- **Evidence:** direct static source inspection; mathematical exhaustive tests
  validate (32), not this numeric implementation.
- **Decision:** requires CPLEX-backed before/after trace tests. Preserve the
  historical file; consider numeric Concert/double distances in corrected mode.

## D07 -- Tree rows are not reused in integer/Lagrangian models

- **Location:** end of `TreeInequalities.cpp::dijkstra`.
- **Historical behavior:** rows are added to `LinearModels[m]`; the nodal and
  Lagrangian additions are commented out.
- **Paper/supplement requirement:** Section 4.2.2 says the accumulated valid
  inequalities strengthen the MILPs solved during the algorithm.
- **Category:** paper/source inconsistency.
- **Scientific effect:** lower-bound validity is unchanged, but MILP/Lagrangian
  solve strength and performance can be weaker than specified.
- **ODS evidence:** tree-level VI counts and published performance reflect some
  historical model context, not enough to establish the commented intent.
- **Decision:** preserve for historical reproduction; broader reuse belongs to
  an explicit corrected mode only after validation.

## D08 -- Value-function cut frequencies

- **Location:** `Main.cpp` modulo tests using `IntegerGeneration` and
  `LagGeneration`.
- **Historical behavior:** integer-optimality and optimized Lagrangian cuts may
  be generated only every command-line-selected number of iterations.
- **Paper/supplement requirement:** Algorithm 1 presents all three families in
  each backward update.
- **Category:** undocumented experimental parameter.
- **Scientific effect:** changes convergence, iteration cost, and cut pool.
- **ODS evidence:** executable names do not retain these arguments and ODS has
  no family-specific counters.
- **Decision:** real launch-configuration blocker. Do not guess frequencies;
  start reproduction only after an explicit evidence/assumption decision.

## D09 -- Unchecked solver termination can invalidate lower cuts

- **Location:** every `cplex.solve()` call in `NodalModel.cpp`,
  `LinearModel.cpp`, `LagrangianModel.cpp`, and
  `DualLagrangianModel.cpp`; root LB calls in `Main.cpp`.
- **Historical behavior:** the Boolean solve result and CPLEX status are
  ignored; `getObjValue()`/duals are used after fixed 30/120/240-second local
  limits. Phase-I nodal MIP gaps are 1% at the root and 0.5% elsewhere.
- **Paper/supplement requirement:** backward subproblems and the Lagrangian
  problems used to form valid cuts are solved to the required optimum;
  Section 5.2 otherwise says CPLEX default settings.
- **Category:** probable historical bug / undocumented experimental parameter.
- **Scientific effect:** an incumbent objective is an upper bound for a
  minimization recourse problem, not necessarily the true value required by an
  integer-optimality cut; a nonoptimal Lagrangian incumbent need not be a valid
  cut intercept; a root incumbent need not be a valid LB. This is a correctness
  risk whenever a local solve reaches its limit or gap.
- **ODS evidence:** no status, best-bound, or per-subproblem logs were archived,
  so historical optimality cannot be established.
- **Decision:** scientific blocker. Future guarded behavior must require an
  appropriate optimal status or use a mathematically valid bound; historical
  mode must report any unchecked event rather than silently claim validity.

## D10 -- Set-1 exhaustive upper-bound threshold

- **Location:** `Main.cpp` call-selection condition around
  `Optimal_UpperBound`.
- **Historical behavior in this snapshot:** exhaustive evaluation is used only
  if the full scenario count is at most `NbScen=1000`; otherwise a sampled
  routine is selected.
- **Paper requirement:** Section 5.2 evaluates all scenarios for every Set-1
  instance, including `R=20` with 8,000 scenarios.
- **Category:** paper/source inconsistency and snapshot/configuration mismatch.
- **Scientific effect:** changes UB, gap, sample variance, and runtime.
- **ODS evidence:** Set-1 `R=20` rows record `# Sampled scen=0`, consistent with
  exhaustive historical evaluation, so the published executable likely used a
  different `NbScen` or branch condition than this frozen snapshot.
- **Decision:** reconstruct the Set-1 run configuration from paper/ODS; retain
  current snapshot as evidence but do not use its default condition for a
  Tables 1--4 claim.

## D11 -- Statistical upper-bound batching

- **Location:** `UpperBounds.cpp::UpperBound_ScenarioBased_II` and `_III`.
- **Historical behavior:** scenarios are generated in batches that reuse
  earlier macro-stage policy solves and enumerate or resample groups of later
  leaves; `_III` caps groups at 20.
- **Paper requirement:** Section 5.2 describes 1,000 randomly sampled scenarios
  and a 95% confidence interval.
- **Category:** undocumented experimental parameter.
- **Scientific effect:** the mean can remain appropriate for a balanced tree,
  but observations within a batch are not plainly independent, so the usual
  sample standard-error interpretation needs review.
- **ODS evidence:** non-Set-1 rows normally report 1,000 evaluated scenarios;
  no sample identities were retained.
- **Decision:** preserve historical behavior for comparison, but validate the
  confidence calculation under its actual dependence structure before making
  a statistical coverage claim.

## D12 -- Reported gap denominator differs across artifacts

- **Location:** `Main.cpp` result output.
- **Historical behavior in this snapshot:** writes
  `100*abs(UB-LB)/LB` for extSDDiP.
- **Paper requirement:** Section 5.3 defines `abs(UB-LB)/UB`.
- **Category:** paper/source inconsistency and snapshot/version fingerprint.
- **Scientific effect:** affects reporting only, not the optimization path.
- **ODS evidence:** 3,312 of 3,320 main extSDDiP rows match `/UB`; Sigma=20
  extSDDiP rows mostly track `/LB`. This source snapshot is consequently more
  directly aligned with the later Sigma=20 run set than the main-table output
  expression.
- **Decision:** always recompute paper gaps from archived LB/UB; preserve raw
  historical fields and never tune optimization to the displayed gap.

## D13 -- CPX time limit and missing callback

- **Location:** `GlobalModel.cpp::GlobalModel`; absent callback source.
- **Historical behavior:** plain extensive formulation with a 900-second limit.
- **Paper requirement:** Section 5.2 uses 1,800 seconds and a custom Set-1
  branch-and-cut callback for (31)/(32).
- **Category:** paper/source inconsistency.
- **Scientific effect:** prevents exact CPX baseline reproduction, not extSDDiP
  validation.
- **ODS evidence:** CPX runs cluster near 1,800 seconds.
- **Decision:** keep separate from extSDDiP; do not block its review, and do not
  claim exact CPX reproduction.

## D14 -- Optional extended-formulation index shadowing

- **Location:** `LinearModel.cpp::InitialLinearModel` under `Extended==1`,
  especially nested loops using `Ext_D[m]`.
- **Historical behavior:** inner loop variables named `m` shadow the model
  index and can select the wrong `Ext_D` row or exceed its model dimension.
- **Paper requirement:** Tables 1--4 are reproducible from the ODS `Natural`
  rows; the optional extended formulation is not needed for that target.
- **Category:** probable historical bug in an optional path.
- **Scientific effect:** can corrupt or crash the optional LP formulation.
- **ODS evidence:** both Natural and Extended experimental rows exist, but the
  paper-table values verified in Gate 0B select Natural rows.
- **Decision:** exclude the optional path from the first reproduction gate;
  investigate separately before advertising it.

## D15 -- Robustness-only hazards

- **Location:** `Main.cpp` and `UpperBounds.cpp`.
- **Historical behavior:** no `argc` validation; modulo by integer/Lagrangian
  frequency assumes nonzero arguments; unused `Uppercheck` divides by
  `LimUpper=0`; one unused upper-bound routine contains a stray semicolon after
  `if(aux_ite>=10)`.
- **Paper requirement:** not applicable.
- **Category:** harmless implementation detail unless invalid launch arguments
  or unused routines are activated.
- **Scientific effect:** potential crash/NaN, not a defining algorithm choice.
- **Decision:** later fail-fast input validation is allowed as a build/runtime
  safety change, separately reviewed from scientific corrections.

## Current decision

No discrepancy has been corrected in `src/`. The core recovered architecture
is retained, but D05, D06, D09, and the snapshot/configuration evidence in D08,
D10, and D12 require review before historical reproduction runs are presented
as implementing the published experiment exactly.
