# Scientific Reconstruction Plan for the Published extSDDiP Implementation

Status: proposed for scientific review before algorithmic implementation  
Normative source: Quezada, Gicquel, and Kedad-Sidhoum, *Combining polyhedral approaches and stochastic dual dynamic integer programming for solving the uncapacitated lot-sizing problem under uncertainty*  
Historical implementation baseline: `recoderelaxationlagrangienne` (Code B)  
Historical reference only: `SDDIP-ULS Code` (Code A)

## 1. Scope and scientific precedence

This document is the gate between the completed code-to-paper audit and implementation work. No algorithmic source is to be written until this plan has been reviewed.

Conflicts will be resolved in this order:

1. published article;
2. mathematical consequences of the article;
3. historical Code B;
4. historical Code A;
5. comments, filenames, scripts, timestamps, and inferred intent.

The historical source directories are immutable evidence. The reconstructed implementation will live under `reproducible_extSDDiP/` and will not include edits copied back into either historical directory.

The scientific target is the method in Sections 2--5, including the extensive formulation, baseline SDDiP, subtree-based SDDiP, Phase I, Phase II, the three cut families, path/tree strengthening, algorithm variants, instance design, and final upper-bound protocols. Software modernization and performance tuning are outside this stage.

## 2. Component reconstruction matrix

Action labels have the following meanings:

- `PRESERVE`: retain behavior after an equivalence test.
- `FIX`: correct behavior contradicted by the paper or its mathematics.
- `EXTEND`: build on an existing faithful component.
- `RECONSTRUCT`: replace an incomplete historical structure with a paper-faithful one.
- `IMPLEMENT FROM PAPER`: no usable implementation is present.
- `VALIDATE BEFORE DECISION`: evidence is not yet sufficient to select exact behavior.

| Component | Paper reference | Existing in B | Existing in A | Required action | Scientific risk | Validation method |
|---|---|---|---|---|---|---|
| SULS data and decision domains | Section 2; Equations (1)--(4) | Single-item `D`, `C[][0]`, `H[][0]`, `g`; `x,s` continuous and `y` binary | Active model is single-item, but input retains unrelated returns/remanufacturing fields | `RECONSTRUCT` a canonical SULS data model; `PRESERVE` only algebra proven equivalent | CRITICAL | Hand-built one- and two-period cases; domain and coefficient inspection |
| Expected-cost extensive objective | Equation (1) | Full balanced tree is expanded from compact stage data and weighted by `Prob` | Same idea for `p=1` | `RECONSTRUCT` directly over canonical nodes; compare to historical expansion | CRITICAL | Direct objective evaluation on enumerated policies and tiny solver instances |
| Production/setup link | Equation (2) | Correct in integer and extensive models; incorrectly inserted into the wrong LP model | Correct in integer, LP, Lagrangian, and extensive models | `FIX` Code B discrepancy in new code; retain a historical-behavior regression | CRITICAL | Model-row inspection plus an instance with positive setup cost and demand |
| Inventory balance | Equation (3) | Correct along deterministic stage paths | Correct for one-period stage models | `PRESERVE` algebra in canonical node form | CRITICAL | Balance residual tests on every node of tiny trees |
| Big-M calculation | Section 2.1 after Equation (4) | Backward recursion computes current demand plus maximum child bound | Stage-wise maximum for `p=1` | `PRESERVE` recurrence after proving equivalence on canonical trees | HIGH | Compare recurrence against explicit maximum root-to-leaf demand sums |
| Node probability calculation | Section 2; Section 5.1 | Read from input; not calculated | Read from input; not calculated | `IMPLEMENT FROM PAPER` with consistency checks | HIGH | Verify conditional probabilities sum to one and `rho_n=(1/R)^(sigma_n-1)` for published balanced trees |
| Planning periods and original stages | Section 2 | Supports `p` consecutive periods per stage | Effectively restricted to `p=1` | `EXTEND` B's distinction into explicit `Period` and `OriginalStage` objects | HIGH | Stage/period partition tests, including `b=1,2,5` |
| Canonical scenario tree | Section 2 and Figure 1 | Compact stage-realization lattice, not a canonical full tree | Similar compact lattice | `RECONSTRUCT` explicit parent/children/scenario semantics | CRITICAL | Tree invariants and manually specified examples |
| Stage-wise independence | Section 3 opening paragraphs | Assumed through reused compact realization data; not validated | Same | `RECONSTRUCT` validation and representative-realization mapping | CRITICAL | Compare data and conditional distributions at every pair of equivalent history nodes |
| Root-to-leaf scenarios | Section 2 | Sampled implicitly as model-index paths | Sampled implicitly | `EXTEND` into explicit scenario paths with probabilities | HIGH | Enumerate all scenarios in small trees and verify total probability one |
| Macro-stage partition | Section 2.2 | Missing; every original stage is traversed directly | Missing | `IMPLEMENT FROM PAPER` | CRITICAL | Contiguity/coverage tests and invalid-partition rejection |
| Stochastic subtree realization `X^(gamma,r)` | Section 3; Equations (5)--(8) | Missing; a subproblem is one deterministic `p`-period path | Missing; one node per subproblem | `IMPLEMENT FROM PAPER` | CRITICAL | Exact node/root/leaf membership on hand-drawn branching trees |
| No-decomposition limit | Section 3.6, `Gamma=1` | Not represented as an algorithm configuration | Not represented | `IMPLEMENT FROM PAPER` | CRITICAL | Model fingerprint and optimal value equal to extensive MILP |
| Full-decomposition limit | Section 3.6, `G` equivalent to `S` | This is the only decomposition represented | This is the only decomposition represented | `PRESERVE` as an explicit `SDDiP` configuration after canonicalization | HIGH | Subproblem membership and bounds compared with historical baseline on tiny inputs |
| Phase-II binary state encoding | Section 3.1; Equations (9)--(17) | Boundary inventory encoded; internal `p`-period inventory continuous | Boundary inventory encoded for one-period subproblems | `EXTEND` to every leaf of a stochastic subtree | CRITICAL | Encode/decode all integers through `smax`; verify only coupling states are binary |
| Binary dimension | Section 5.2, `B=ceil(log2(smax))` | Hard-coded `ExpSize=11` | Chosen from root-demand thresholds | `FIX` using published `smax`; document endpoint/index convention | HIGH | Boundary tests at powers of two and at published maximum inventory |
| Local state-copy constraints | Equations (11)--(16) | Continuous local copies fixed to sampled binary state | Same | `EXTEND` to each entering/leaf boundary required by the subtree | CRITICAL | Constraint-structure inspection and fixed-state solve comparisons |
| Cost-to-go approximation storage | Equations (18)--(20) | One `PSI` variable per original stage; cuts persist in Concert models | Same | `RECONSTRUCT` explicit cut objects and one approximation per macro-stage | CRITICAL | Evaluate stored cuts independently of solver models and compare epigraph values |
| Sampling with replacement | Section 3.2; Proposition 1 | `rand()` with implicit seed; one draw per stage | Same | `FIX` with explicit configurable RNG and recorded seed | HIGH | Deterministic replay, empirical support, and repeated-draw allowance |
| Forward pass | Section 3.3; Algorithm 1 lines 5--14 | Correct baseline traversal over original stages | Correct baseline traversal over original stages | `RECONSTRUCT` over macro-stages and stochastic subtrees | CRITICAL | Trace expected subproblem sequence and propagated states on tiny trees |
| Candidate policy cost | Algorithm 1 lines 11--14 | Accumulates stage subproblem cost less `PSI` | Same | `RECONSTRUCT` with explicit immediate-cost accounting | HIGH | Compare sampled policy cost with direct node-by-node evaluation |
| Backward pass | Section 3.4; Algorithm 1 lines 15--28 | Reverse original stages, all next-stage realizations | Same | `RECONSTRUCT` over macro-stages and every required next realization | CRITICAL | Call trace plus root LB comparison to exact downstream enumeration |
| Lower-bound computation | Section 3.4; Algorithm 1 line 28 | Resolves root model after cuts | Same | `PRESERVE` principle in canonical root subproblem | CRITICAL | Monotonicity and `LB <= extensive optimum` on tiny instances |
| Integer optimality cut | Section 3.4.1 | Historical mismatch expression is accumulated once per child, multiplying its slope by `R` | Same defect pattern | `FIX` by direct transcription and derivation from the paper | CRITICAL | Enumerate all binary states; verify validity and tightness at generating state |
| Lagrangian relaxation | Section 3.4.1 | Copy constraints removed; objective multipliers inserted with historical sign convention | Same | `RECONSTRUCT` with explicit `constant + slope*state` convention | CRITICAL | Directly compare constrained value, relaxed value, and reconstructed cut at enumerated states |
| Lagrangian dual/subgradient solver | Section 5.2 | Polyak-type update, at most 30 iterations, invoked every fifth outer iteration | Similar, at most 100 iterations, every tenth outer iteration | `VALIDATE BEFORE DECISION`; paper specifies subgradient but omits operational parameters | CRITICAL | Concave-dual trace tests, sign tests, dual-bound tests, and documented sensitivity |
| Strengthened Benders slope | Section 3.4.1 | LP copy-constraint dual is collected, but LP omits `x<=My` because it was added to `NodalModels` | Correct LP target in A | `FIX`; implement paper-consistent LP and preserve a test fixture for historical B behavior | CRITICAL | Side-by-side LP row sets, duals, cuts, validity, and strength |
| Strengthened Benders constant | Section 3.4.1 | Solves a Lagrangian MILP with fixed LP-derived multipliers | Same | `PRESERVE` only after multiplier-sign and constant reconstruction tests | CRITICAL | Compare cut evaluation against exact downstream values |
| Continuous-state Phase I | Section 4.1; Equations (21)--(29) | Continuous state and strengthened cuts, but only full decomposition | Same for `p=1` | `RECONSTRUCT` using genuine stochastic subtrees | CRITICAL | Tiny subtree Phase-I LB validity and cut checks |
| Phase-I-to-II cut transfer | End of Section 4.1 | Implicitly retains continuous cuts while adding binary equality | Same | `RECONSTRUCT` as an explicit, inspectable conversion | HIGH | Equality of continuous and binary-substituted cut evaluations |
| `SDDiP` variant | Sections 3.6 and 5.2 | Representable by `PHASE_II=1`, full original-stage decomposition | Represented by Phase II loop | `EXTEND` into a named configuration | HIGH | Configuration-to-structure test |
| `extSDDiP-I` variant | Section 5.2 | Boolean Phase I exists, but it is not subtree-based | Not separately configurable | `IMPLEMENT FROM PAPER` | CRITICAL | Verify continuous subtree states, strengthened cuts only, and termination |
| `extSDDiP-II` variant | Section 5.2 | Boolean Phase II exists, but it is not subtree-based | Not separately configurable | `IMPLEMENT FROM PAPER` | CRITICAL | Verify binary subtree states and all three cut families |
| `extSDDiP-I/II` variant | Section 5.2 | Two flags run two baseline phases | Always runs two baseline phases | `IMPLEMENT FROM PAPER` with explicit phase transition | CRITICAL | Verify Phase-I cuts present and equivalent at Phase-II start |
| Path inequalities | Section 4.2; inequality (31); supplementary Appendix A | Deterministic path separator exists, disabled by default and applied only to LP models | Only a fixed one-period strengthening and commented declarations | `VALIDATE BEFORE DECISION`, then `RECONSTRUCT` against exact supplementary formulation | CRITICAL | Feasible-integer validity enumeration and controlled fractional-separation cases |
| Tree inequalities | Section 4.2; inequality (32); Guan et al. (2009); supplementary Appendix A | Missing | Missing; referenced historical files are absent | `IMPLEMENT FROM PAPER` after specification approval | CRITICAL | Written mathematical oracle, exhaustive validity tests, and controlled violations |
| Persistent pools `phi^(gamma,r)` | Section 4.2.1 | Path cuts persist only inside an LP model | Missing | `IMPLEMENT FROM PAPER` as explicit per-subtree-realization pools | HIGH | Pool ownership, reuse, deduplication, and iteration-persistence tests |
| Sequential strengthening level 0 | Section 4.2.1 | Single boolean `cutting`; no level state | No level state | `IMPLEMENT FROM PAPER` | HIGH | Confirm initial formulation only |
| Sequential strengthening level 1 | Section 4.2.1 | Optional one-pass deterministic separator; no transition controller | Missing | `IMPLEMENT FROM PAPER` after transition threshold is resolved | CRITICAL | Path-only pool growth and transition-condition tests |
| Sequential strengthening level 2 | Section 4.2.1 | Missing | Missing | `IMPLEMENT FROM PAPER` | CRITICAL | Tree-cut generation only after level 1 and pool preservation |
| Reuse in forward/backward/Lagrangian MILPs | Section 4.2.2 | Valid inequalities generated by `CuttingPlaneGeneration` are added only to `LinearModels` | No active separator | `IMPLEMENT FROM PAPER` | HIGH | Model-row fingerprints for all solver contexts |
| Plain extensive CPX baseline | Sections 2.1 and 5.2 | Present; current limit 900 seconds | Present; current limit 600 seconds | `RECONSTRUCT` over canonical tree with 1800-second configuration | HIGH | Objective equivalence and configuration inspection |
| Set-1 custom branch-and-cut | Section 5.2 | Missing | Build notes reference absent cut/callback files | `IMPLEMENT FROM PAPER` after exact separator behavior is specified | CRITICAL | Callback tests and equivalence to root-separated formulation on tiny instances |
| Instance generator: structures | Section 5.1 | Missing | Missing | `IMPLEMENT FROM PAPER` | HIGH | Programmatic count: Set 1=40, Set 2=40, Set 3=40, Set 4=20, total=140 |
| Instance generator: demand/cost distributions | Section 5.1 | Missing | Missing | `IMPLEMENT FROM PAPER`; RNG details require documented assumption | HIGH | Range, discreteness, mean-based cost intervals, replay, and distribution sanity tests |
| Five replications and manifest | Section 5.1 | Missing | Missing | `IMPLEMENT FROM PAPER` | HIGH | Unique IDs, seeds, parameter coverage, and checksums |
| Published SDDiP settings | Section 5.2 | 1000 iterations, 30 stagnant, 900 seconds; hard-coded | Different values | `EXTEND` into explicit immutable experiment configurations | MEDIUM | Configuration snapshot tests |
| Lagrangian cut frequency | Algorithm 1 versus historical code | Every fifth iteration | Every tenth iteration | `FIX` paper default to every iteration unless supplementary evidence says otherwise; retain historical modes only as non-paper diagnostics | HIGH | Invocation-count test |
| Set-1 true upper bound | Section 5.2 | Missing; always samples | Commented/incomplete | `IMPLEMENT FROM PAPER` by exhaustive scenario evaluation | HIGH | Compare with explicit enumeration on small trees |
| Sets 2--4 statistical upper bound | Section 5.2 | 1000 samples and mean plus `1.96*SE`; no separate time budget or explicit seed | Missing | `EXTEND` with explicit seed, variance/CI outputs, and 900-second budget | HIGH | Known-sample estimator test and deterministic replay |
| Termination metadata | Section 5.2 and reproduction requirements | Partial output only | Partial output only | `IMPLEMENT FROM PAPER` plus explicit termination reason | MEDIUM | Schema and round-trip tests |
| Raw run metadata | Section 5 and reproducibility requirements | Partial tab-separated output | Partial output | `IMPLEMENT FROM PAPER` | MEDIUM | Required-column validation and environment capture |
| Tables 1--4 experiment matrix | Section 5.3 | No driver | No driver | `IMPLEMENT FROM PAPER` as separate expensive workflow | HIGH | Expected configuration-count and dry-run manifest tests |
| Historical discrepancy log | Scientific traceability requirement | Not present | Not present | `IMPLEMENT FROM PAPER` project requirement before first behavior change | HIGH | Every `FIX`/`RECONSTRUCT` change references a test and paper section |

## 3. Proposed canonical scientific architecture

The architecture is deliberately centered on the paper's mathematical objects. It is not a general solver abstraction and should remain close to the Concert/CPLEX implementation style until scientific equivalence is established.

### 3.1 Repository boundary

Proposed planning-level layout:

```text
reproducible_extSDDiP/
  docs/
  config/
  include/
  src/
    data/
    model/
    decomposition/
    cuts/
    inequalities/
    algorithm/
    experiments/
  tests/
    unit/
    integration/
    fixtures/
  instances/
    manifest/
  results/
```

Only `docs/` is created during the current planning task. The remaining directories are proposed, not authorized implementation work.

### 3.2 Identifiers and immutable input data

Use distinct typed concepts, even if their first C++ representation is an integer index:

- `PeriodId` for `t in T`;
- `StageId` for `sigma in S`;
- `NodeId` for `n in V`;
- `MacroStageId` for `gamma in G`;
- `RealizationId` for `r in R_gamma`;
- `ScenarioId` for a root-to-leaf path.

Canonical node data must contain:

```text
ScenarioNode
  id
  period
  original_stage
  parent (none only for the root)
  children
  conditional_probability
  node_probability rho
  demand d
  setup_cost f
  holding_cost h
  production_cost g
```

`ScenarioTree` owns nodes and provides read-only traversals. Its constructor or validator must enforce:

- exactly one root;
- one parent for every nonroot node;
- children and parent links are reciprocal;
- periods increase by one along each edge except for the root convention explicitly selected by the model;
- original stages partition periods into nonempty consecutive blocks;
- conditional child probabilities sum to one;
- node probabilities agree with parent and conditional probabilities;
- every scenario has the complete planning horizon;
- all model data are finite and costs/demands satisfy the paper's domains.

### 3.3 Original-stage and stage-wise-independence representation

`OriginalStage` contains its consecutive planning periods. `StageRealizationTemplate` describes the data and conditional probabilities that recur below every equivalent history at that stage.

The canonical tree remains the semantic source of truth. A compact stage-realization template may be used to construct it and to identify equivalent subproblems, but compact indexing must not replace the parent/child semantics.

`StagewiseIndependenceCertificate` should map every node at the final period of stage `sigma-1` to the same ordered set of stage-`sigma` realization signatures. Construction fails, or explicitly marks the input unsupported by the published algorithm, if the signatures differ.

### 3.4 Macro-stage partition

`MacroStagePartition` contains ordered, consecutive, nonoverlapping sets `S(gamma)` whose union is `S`. It provides:

- `first_stage(gamma)` and `last_stage(gamma)`;
- `first_period(gamma)=t(gamma)`;
- `last_period(gamma)=t'(gamma)`;
- `macro_stage_of(original_stage)`;
- boundary periods and valid entering-state parent nodes.

Published equal-size settings are constructed from `stages_per_macro_stage G`, but the core representation accepts unequal nonempty blocks because the mathematical definition does.

### 3.5 Stochastic subtree realization

`SubtreeRealization` represents `X^(gamma,r)` by references to canonical nodes:

```text
SubtreeRealization
  macro_stage gamma
  realization r
  root_node xi^(gamma,r)
  entering_parent_boundary
  nodes (all descendants of root whose periods lie in the macro-stage)
  internal_nodes
  leaf_nodes L(gamma,r) at t'(gamma)
  within-subtree edges
  node/conditional probability weights
```

Construction rule:

1. choose a root in the first period of the macro-stage corresponding to realization `r`;
2. include the root and every descendant through the macro-stage's last period;
3. retain every internal branch created by original stages inside the macro-stage;
4. define leaves as included nodes in the last macro-stage period;
5. never collapse the subtree to one deterministic path when the block contains multiple branching stages.

Equivalent subtrees below different histories may share one representative only after the stage-wise-independence certificate proves identical data and conditional distributions. The mapping from every actual boundary node to its representative must remain inspectable.

For `Gamma=1`, the only subtree is the whole scenario tree. For `G` equivalent to `S`, each subtree is one stage-realization path through the `b` periods in that stage, which recovers the historical Code B structure.

### 3.6 Subproblem specifications rather than mutable monolithic models

Separate the mathematical specification from its Concert model instance:

- `ExtensiveSulsSpec` for Equations (1)--(4);
- `ContinuousSubproblemSpec` for Phase I, Equations (21)--(29);
- `BinarySubproblemSpec` for Phase II, Equations (9)--(20);
- `SubproblemKey=(gamma,r)`;
- `CostToGoApproximation[gamma]` storing explicit cut coefficients;
- `ValidInequalityPool[gamma,r]` storing explicit path/tree inequalities.

Solver model builders consume these immutable specifications and a declared formulation level. This makes row-set comparison possible and prevents the Code B error in which a constraint intended for one model is inserted into another.

### 3.7 Explicit state and cut objects

State objects:

- `ContinuousBoundaryState { inventory }`;
- `BinaryBoundaryState { bits, weights, decoded_inventory }`;
- one leaving state per subtree leaf;
- entering local-copy variables distinct from fixed parent state values.

Cut objects should expose `evaluate(state)` independently of CPLEX:

- `IntegerOptimalityCut`;
- `LagrangianCut`;
- `StrengthenedBendersCut`;
- `ContinuousStateCut` for Phase I.

Each cut records its family, generating iteration, macro-stage, sampled boundary state, realization-level coefficients, probability aggregation convention, formulation level, and validation status.

### 3.8 Explicit algorithm variants and configuration

Use a named variant rather than undocumented booleans:

```text
AlgorithmVariant = SDDiP | extSDDiP-I | extSDDiP-II | extSDDiP-I/II
```

An experiment configuration records at least:

- variant;
- macro-stage partition or `G`;
- `lambda_max`;
- `K=1` default;
- main and upper-bound seeds;
- maximum iterations and stagnant iterations;
- 900-second LB and 900-second UB budgets;
- subgradient configuration;
- solver configuration and version.

Invalid combinations are rejected, for example `SDDiP` with a multi-stage macro-stage block or `extSDDiP-I` configured to generate integer/Lagrangian cuts.

## 4. Scientific regression-test strategy

The test suite will be split into solver-free mathematical tests and small CPLEX integration tests. Full experiments are never part of the lightweight test target.

### 4.1 Level 1: algebraic unit tests

#### Tree and probability tests

- Construct a hand-defined `Sigma=3`, `b=1`, `R=2` tree.
- Check parent/children reciprocity, stage and period mappings, four root-to-leaf scenarios, and total scenario probability one.
- Check published balanced probabilities `1`, `1/2`, and `1/4` by stage.
- Reject a tree whose equivalent stage histories have different realization signatures.

#### Big-M tests

- Compute every root-to-leaf future-demand sum by enumeration.
- Compare the maximum with the backward recursion at every node.
- Include ties, zero demand, multiple periods per stage, and nonuniform branching diagnostic fixtures.

#### State-encoding tests

- Compute `smax` from the maximum root-to-leaf demand.
- Test the exact digit/index convention around `smax=1,2,3,4,7,8`.
- Encode/decode all integers from zero through `smax`.
- Verify internal subtree inventories are not binary-state variables.
- Verify every subtree leaf has one state vector.

#### Cut tests

- Evaluate integer-optimality cuts at the incumbent state and every alternative bit vector.
- For tiny downstream problems, enumerate all feasible inventory states and verify no cut exceeds the exact value function.
- Verify Lagrangian multiplier signs and `constant + slope*state` reconstruction.
- Verify strengthened-Benders cuts use duals from a row-complete LP relaxation.
- Verify families declared tight are tight at the generating state to numerical tolerance.

#### Valid-inequality tests

- Enumerate integer-feasible policies on tiny paths/trees and check every generated inequality.
- Supply controlled fractional points with known violations.
- Check that each cut is stored only in its owning `(gamma,r)` pool and remains present in later iterations.

#### Generator tests

- Verify each parameter domain and distribution support.
- Verify `h_bar` is calculated after holding-cost generation and before production/setup cost intervals are generated.
- Verify counts `40+40+40+20=140`.
- Verify fixed-seed byte-for-byte replay and manifest checksums.

#### Upper-bound estimator tests

- Test sample mean, unbiased variance, standard error, and `mean+1.96*SE` on a known vector.
- Verify the Set-1 path enumerator visits every scenario exactly once.

### 4.2 Level 2: tiny deterministic cases

- One-period demand with setup cost: manually compare producing exactly demand against infeasibility/no production.
- Two-period Wagner--Whitin cases where producing early is respectively optimal and nonoptimal.
- Zero-demand and zero-holding-cost boundary cases.
- Compare manual objective, extensive model objective, and reconstructed inventory trajectory.

### 4.3 Level 3: tiny stochastic trees

Use at least two fixtures:

1. `Sigma=3`, `b=1`, `R=2` with small integer demands and analytically enumerable setup policies.
2. `Sigma=2`, `b=2`, `R=2` to distinguish periods from decision stages.

For each fixture:

- solve the extensive MILP as the oracle;
- enumerate exact downstream value functions by boundary inventory;
- validate every generated cut against those functions;
- confirm all decomposition lower bounds do not exceed the oracle;
- evaluate the final policy directly on every scenario.

### 4.4 Level 4: structural equivalence

#### `Gamma=1`

- Verify subtree membership equals the entire tree.
- Verify there is no backward pass.
- Verify the single subproblem has the same variables, constraints, and objective as the extensive oracle.

#### Full decomposition

- Set one original stage per macro-stage.
- Verify every subproblem is a deterministic `b`-period stage-realization path.
- Compare small-instance traces and bounds with a corrected Code B baseline.

#### Phase variants

- `extSDDiP-I`: continuous states and strengthened-Benders cuts only.
- `extSDDiP-II`: binary states and all three cut families.
- `extSDDiP-I/II`: Phase-I cut set is visible at Phase-II initialization.
- Verify continuous-to-binary cut evaluation equality for every encodable state.

#### Strengthening levels

- Level 0 has no path/tree inequalities.
- Level 1 generates and persists path inequalities only.
- Level 2 retains path inequalities and adds tree inequalities.
- Forward, backward, and Lagrangian MILP fingerprints contain the appropriate pool.

### 4.5 Historical discrepancy regression: Code B LP insertion

Create two builders over the same tiny subproblem:

- `historical_B_fixture`: reproduces the missing LP `x<=My` row without becoming production code;
- `paper_consistent`: contains the row in the LP relaxation.

With positive setup cost and positive demand, demonstrate:

1. the historical LP can set the relaxed setup variable to zero while producing;
2. the corrected LP cannot;
3. copy-constraint duals and the resulting strengthened-Benders slopes differ;
4. the corrected cut remains valid against exact downstream values.

The result will be recorded in `docs/historical_code_discrepancies.md` when implementation begins.

### 4.6 Level 5: published-instance workflow

Only after Levels 1--4 pass:

- freeze the 140-instance manifest;
- dry-run the complete method/configuration matrix;
- execute raw experiments separately from aggregation;
- compare dimensions, feasibility, bounds, gaps, and qualitative rankings with Tables 1--4;
- treat time, exact iterations, and CPLEX search paths as environment-sensitive;
- never tune scientific settings to match published numbers.

### 4.7 Numerical tolerances and blockers

Tolerance constants must be centralized and recorded. Cut validity failures above tolerance are scientific blockers. No large experiment may run while any of the following remains unresolved:

- a generated cut overestimates an exact tiny-instance value function;
- a claimed-tight cut is not tight at the generating state;
- `Gamma=1` differs from the extensive model;
- full decomposition does not match the corrected baseline structure;
- stage-wise independence is assumed but not verified;
- path/tree inequality validity tests fail.

## 5. Reconstruction order and gates

### Gate 0: acquire and freeze evidence

1. Preserve checksums of both historical directories.
2. Acquire the official supplemental Appendix A/C, original 140 instance files if available, historical result files, and any missing sources referenced by Code A's build notes.
3. Create `docs/historical_code_discrepancies.md` before the first scientific behavior change.
4. Resolve or formally record all evidence gaps that block exact formulas.

### Gate 1: testable skeleton and configurations

1. Add build/test skeleton without algorithm code.
2. Add explicit published configurations and result schemas.
3. Add pure mathematical helper tests.

### Gate 2: canonical data, tree, and generator

1. Implement SULS data objects and scenario-tree validation.
2. Implement original-stage and stage-wise-independence representations.
3. Implement the deterministic generator and 140-instance design manifest.
4. Freeze fixture checksums.

### Gate 3: extensive oracle

1. Implement Equations (1)--(4).
2. Pass algebraic, deterministic, and tiny stochastic tests.
3. Use this model as the sole optimization oracle for later cut/decomposition tests.

### Gate 4: macro-stages and subtrees

1. Implement arbitrary contiguous macro-stage partitions.
2. Construct true branching `X^(gamma,r)` subtrees.
3. Pass subtree membership, `Gamma=1`, and full-decomposition tests.

### Gate 5: state formulations and cost-to-go storage

1. Implement Phase-I continuous copies.
2. Implement Phase-II binary entering/leaving states from published `smax`.
3. Implement explicit cost-to-go cut objects and epigraph builders.
4. Validate state substitution and cut evaluation without an outer algorithm.

### Gate 6: individual backward cut families

1. Implement corrected strengthened Benders first because Phase I depends on it.
2. Implement integer-optimality cuts directly from Section 3.4.1.
3. Implement Lagrangian relaxation and subgradient dual solver.
4. Pass exhaustive tiny-state validity tests for every family.

### Gate 7: Algorithm 1 and Phase II

1. Implement explicit sampling, forward pass, backward pass, and root LB modules.
2. Recover baseline `SDDiP` as the full-decomposition configuration.
3. Implement subtree-based `extSDDiP-II` level 0.
4. Compare against the extensive oracle.

### Gate 8: Phase I and two-phase transfer

1. Implement subtree-based `extSDDiP-I` level 0.
2. Implement explicit Phase-I-to-II cut conversion.
3. Implement `extSDDiP-I/II` level 0.
4. Pass phase and transfer tests.

### Gate 9: polyhedral strengthening

1. Approve `docs/tree_inequality_specification.md` and the corresponding path specification.
2. Implement path separation and per-realization pools.
3. Implement tree separation only after its mathematical oracle passes.
4. Implement sequential levels and transition tests.
5. Add pool reuse to every prescribed LP/MILP context.

### Gate 10: CPX baseline and upper bounds

1. Implement plain 1800-second extensive CPX configuration.
2. Implement Set-1 custom branch-and-cut from the validated separators.
3. Implement exhaustive and statistical upper-bound workflows with separate budgets.

### Gate 11: complete reproduction workflow

1. Generate/freeze instances and checksums.
2. Produce the complete dry-run configuration manifest.
3. Execute raw runs when the required CPLEX environment is available.
4. Generate `results/paper_reproduction_comparison.csv` and `docs/paper_reproduction_report.md` without tuning toward published results.

## 6. Ambiguities and evidence required before implementation decisions

The following are not licenses to invent behavior. Each must be resolved from supplemental material, original data/code, author records, or an explicitly documented reproducibility assumption.

| ID | Ambiguous point | Available evidence | Why it matters | Proposed resolution path |
|---|---|---|---|---|
| A1 | Exact path inequality (31) and separation details | Main paper names Barany et al. and says one cutting-plane run; supplied historical B has only a deterministic-path routine | Coefficients, candidate selection, tolerance, and number of cuts affect slopes and timings | Obtain official supplement Appendix A and compare every term with historical code before specifying tests |
| A2 | Exact tree inequality (32) and Guan et al. separation procedure | Missing from both trees; only comments and absent filenames remain | This is a defining published enhancement and Set-1 callback component | Obtain supplement and primary Guan et al. formulation; write and review `tree_inequality_specification.md` before code |
| A3 | Level-0 to level-1 iteration threshold | Section 4.2.1 says a predefined number of consecutive iterations but gives no value in the main paper | Changes time allocation and reported variants | Check supplemental Algorithm 2, historical configs/results, and author records; otherwise expose and label a reproduction assumption |
| A4 | Level-1 to level-2 no-violation threshold | Main paper again says a predefined number of consecutive iterations | Determines when tree cuts begin | Same evidence path as A3; no arbitrary default before review |
| A5 | Subgradient maximum iterations, tolerance, and exact initialization | Paper says a subgradient algorithm and theoretical text says solve the dual to optimality; B uses 30 iterations/every fifth outer iteration, A uses 100/every tenth | Directly changes Lagrangian cuts | Seek supplement/original configuration; default outer frequency to Algorithm 1 (every iteration) unless evidence overrides it; document remaining assumptions |
| A6 | Polyak step factor and treatment of dual values above the primal subproblem value | Historical versions use factor 2 and differ in acceptance conditions | Affects dual monotonicity and cut validity | Derive sign/bound conditions, compare A/B traces, and validate on exactly enumerable dual functions |
| A7 | Probability normalization inside representative subtree subproblems | Equations display node probabilities while historical code uses conditional ratios during cut aggregation | An unconditional/conditional mismatch rescales objectives and cuts | Derive equivalence symbolically for balanced trees and confirm `Gamma=1`/full-decomposition objectives numerically |
| A8 | Phase-I-to-II slope conversion | The main-paper text appears to assign the same continuous slope to each bit, while mathematical substitution of `s=sum 2^beta u_beta` requires power-of-two scaling; historical code transfers implicitly through `s` | Incorrect conversion invalidates equivalence of phases | Inspect typeset supplement/source if available; use algebraic substitution as precedence-2 evidence and require evaluation-equality tests |
| A9 | Binary digit indexing at exact powers of two | Paper writes `ceil(log2(smax))` and uses an indexing convention that is not fully explicit in extracted text | Off-by-one errors may make `smax` unrepresentable | Define representable maximum from the actual weight set and test powers of two; document any corrected mathematical convention |
| A10 | Exact historical random seed(s) | Paper reports no seed; neither code calls `srand`; original instances are absent | Exact numerical instance reproduction may be impossible | Search for archived instances/seeds. If absent, distinguish exact-paper-data reproduction from deterministic paper-design reproduction and publish new fixed seeds/checksums |
| A11 | Random-number engine and continuous-uniform discretization/precision | Paper gives distributions only | Different engines/draw order produce different instances | Prefer recovery of original files; otherwise specify a stable engine, draw order, serialization precision, and label it a reproducibility assumption |
| A12 | Whether random data are generated for every full-tree node or for stage-wise realization templates and then reused | Section 5.1 speaks of nodes, while stage-wise independence and historical compact files imply template reuse | Independent full-node draws would violate the algorithm's stage-wise-independence assumption | Inspect original instance files/generator; absent that, use template reuse as the mathematically necessary interpretation and document it |
| A13 | First-stage randomness/root convention | Published probability formula gives root-stage probability one; historical structures have a single root | Changes scenario count and generator structure | Preserve a deterministic single first-stage realization unless original instances show otherwise |
| A14 | Exact CPLEX 12.8 default parameters and callback cut-management options | Paper says defaults, two cores, custom Set-1 branch-and-cut | Version-specific defaults and callback purgeability affect search | Record version; explicitly set only published deviations; document callback context, tolerances, and cut type from supplement/historical code |
| A15 | Time measurement semantics | Historical code uses `CLOCK_MONOTONIC`; paper reports computation time | CPU and wall time differ on two cores | Record both when possible; enforce limits by solver wall clock/configuration and identify the reported field |
| A16 | Lower-bound improvement tolerance | Paper states 30 iterations without improvement but not the numerical threshold; A uses an improvement of more than 1 in its loop, B uses strict comparison | Changes termination | Seek supplement/results; otherwise centralize a documented tolerance and report it in run metadata |
| A17 | Cut deduplication and numerical equality tolerance | Paper says persistent sets but not duplicate policy | Pool size is reported in tables | Recover historical separator behavior or document an exact canonicalization/tolerance rule before comparing cut counts |
| A18 | Number and choice of violated inequalities retained in a "single run" | Main paper does not say whether all, first, or most violated cuts are added | Affects `# VI` and model size | Resolve from supplement/historical missing separator files; treat cut-count comparison as blocked until then |
| A19 | Phase-I versus Phase-II time allocation within the common 900-second LB budget | Paper gives a 900-second LB budget for SDDiP/extSDDiP but does not state an independent Phase-I cap in the main text | Determines how many iterations each phase receives | Check supplemental algorithm/configuration; otherwise share one measured budget and document the phase-transition rule |
| A20 | Solver behavior when the 900-second limit is reached during a backward sweep | Historical B polls time between models; paper gives only total limit | Partial iterations can leave uneven cut sets | Look for supplement/original logs; define an atomic-iteration policy only after scientific review |
| A21 | Statistical-UB sampling seed independence from the training stream | Paper reports 1000 new sampled scenarios but no seed policy | Reusing the same stream can bias reproducibility comparisons | Use a separately recorded evaluation stream unless historical evidence establishes another convention |
| A22 | Confidence quantile | Historical B uses 1.96; paper says 95% confidence interval without specifying normal versus Student quantile | Small effect at `K0=1000`, but should be explicit | Use 1.96 as historical evidence and record the formula; add an assumption note |
| A23 | Exact Set-1 exhaustive-policy evaluation mechanics | Paper requires all scenarios; historical code lacks active implementation | State propagation and policy tie-breaking affect UB | Define from Algorithm 1 policy evaluation and validate against full scenario enumeration |
| A24 | Deterministic solver tie-breaking | Proposition 1 assumes the same input yields the same solution; CPLEX may choose alternative optima across versions | Forward states and cuts can diverge | Record solver/version; introduce no artificial tie-breaker unless paper or historical evidence supports one; classify resulting variation as environment-sensitive |
| A25 | Published source/version corresponding to Tables 1--4 | No Git history or complete code snapshot is present | Prevents exact attribution of historical choices | Continue searching archives; maintain provenance for every recovered file and never infer from timestamps alone |

### 6.1 Gate 0B status addendum

The original uncertainty statements above are retained for traceability. The following statuses supersede their pre-forensics descriptions for implementation decisions.

| ID | Gate 0B status | Evidence and consequence |
|---|---|---|
| A1 | `RESOLVED — HISTORICAL SOURCE` | Supplement Appendix A gives (31); recovered `LinearModel.cpp::CuttingPlaneGeneration` supplies the exact historical `-0.001` separation rule, all-violations retention, and model insertion. |
| A2 | `RESOLVED — HISTORICAL SOURCE` | Supplement Appendix A gives (32); recovered `TreeInequalities.cpp` supplies the Dijkstra-based historical heuristic and persistence context. |
| A3 | `RESOLVED — HISTORICAL SOURCE` | `Main.cpp` starts level 1 after a 20-iteration LB-stagnation window with `0.1` threshold. This is historical behavior even though the paper reports 30 stagnant iterations for overall termination. |
| A4 | `RESOLVED — HISTORICAL SOURCE` | `Main.cpp` starts level 2 after one further 20-iteration block at level 1. It does not test a separate no-violation counter. |
| A5 | `RESOLVED — HISTORICAL SOURCE` | Maximum 30 subgradient iterations; current dual initialization from LP duals; factor-2 Polyak step; squared binary-state residual denominator; `1e-5` zero-residual threshold; dual acceptance bounded by the integer value. |
| A6 | `RESOLVED — HISTORICAL SOURCE` | Same `DualLagrangianModel.cpp` evidence fixes factor 2 and rejects candidate dual values above the primal integer subproblem value. |
| A7 | `MATHEMATICALLY DERIVED` | Compact probabilities are unconditional stage probabilities; recovered cut aggregation divides child by parent probabilities, producing conditional `1/R` weights. Algebra and all archived probability vectors agree. |
| A8 | `MATHEMATICALLY DERIVED` | Phase-I cuts remain written on continuous inventory `S`; Phase II adds `S=sum 2^beta u_beta`. Substitution therefore multiplies bit slopes by powers of two without a separate coefficient-copy operation. |
| A9 | `RESOLVED — HISTORICAL SOURCE` | The published archive uses ten bits with coefficients `2^h`, `h=0,...,9`; representable range is 0–1023. A generic reconstruction must still validate a computed endpoint rather than blindly hard-code ten. |
| A10 | `UNRESOLVED — ASSUMPTION REQUIRED` | Original instances are recovered, so Target A no longer needs seeds. No generator, seed file, or `srand` call exists for Target B. |
| A11 | `UNRESOLVED — ASSUMPTION REQUIRED` | Files reveal integer costs/demands and probability serialization, but not RNG engine, draw order, or generator rounding. |
| A12 | `RESOLVED — HISTORICAL DATA` | Every file has `b[1+R(Sigma-1)]` compact records; recovered `NodeModel` expands/reuses stage templates. Full-tree independent draws are ruled out. |
| A13 | `RESOLVED — HISTORICAL DATA` | The first `b` compact records form one deterministic first-stage realization with probability 1; subsequent stages have `R` realization blocks. |
| A14 | `UNRESOLVED — ASSUMPTION REQUIRED` | CPLEX 12.8 and two threads are recovered, but the Set-1 callback source/options are absent and recovered `GlobalModel.cpp` uses 900 rather than the published 1800 seconds. |
| A15 | `RESOLVED — HISTORICAL SOURCE` | Source uses elapsed `CLOCK_MONOTONIC` time. ODS distinguishes LB `Time` from `Total Time`, the latter including UB evaluation. |
| A16 | `RESOLVED — HISTORICAL SOURCE` | Phase I requires improvement over the 20-iteration comparison by `0.1`; Phase II uses `0.01` over a ten-iteration window. Root LB updates also use `0.001` and `0.01`, respectively. |
| A17 | `RESOLVED — HISTORICAL SOURCE` | No deduplication/canonicalization exists; every retained row is appended to Concert models. |
| A18 | `RESOLVED — HISTORICAL SOURCE` | Path separation retains every candidate violation `<=-0.001`; tree separation likewise retains every violated reconstructed candidate. The optional `MultiCuts` argument controls state/leaf coverage for value-function cuts, not VI deduplication. |
| A19 | `RESOLVED — HISTORICAL SOURCE` | Phase I and Phase II share one `start_SND` and one caller-supplied LB time limit; no independent Phase-I quota is reset. |
| A20 | `RESOLVED — HISTORICAL SOURCE` | Time is polled between models/stages and can break a backward sweep before an atomic outer iteration is complete. |
| A21 | `RESOLVED — HISTORICAL SOURCE` | Training and evaluation both consume the same process-global `rand()` stream; no reseed or independent evaluation stream is created. Cross-platform replay remains covered by A10/A11. |
| A22 | `RESOLVED — HISTORICAL SOURCE` | `Main.cpp` uses the sample standard deviation and `mean + 1.96*s/sqrt(actual samples)`. |
| A23 | `RESOLVED — HISTORICAL SOURCE` | `UpperBounds.cpp::Optimal_UpperBound` exhaustively traverses Set-1 scenarios when the scenario count is at most 1,000; other structures use the recovered sampled evaluators. |
| A24 | `UNRESOLVED — ASSUMPTION REQUIRED` | No tie-breaker is set; CPLEX version/platform behavior can select alternative optima. |
| A25 | `RESOLVED — HISTORICAL SOURCE` | The public repository's deleted `Code_extSDDiP.zip` is frozen from commit `80e6fd9`; its driver schema, executable fingerprints, CPLEX 12.8 build, instance reader, and ODS columns align. Known CPX and stopping discrepancies remain explicitly recorded rather than hidden. |

### 6.2 Reproducibility targets after Gate 0B

- **Target A — exact experimental reproduction:** consume the 140 frozen original instance files identified by the historical ODS rows. This is the only target used to reproduce Tables 1–4.
- **Target B — reproducible instance generation:** reconstruct Section 5.1 with an explicit new RNG specification and deterministic replacement seeds. Because A10 and A11 remain unresolved, this output is a new replica dataset and must not be substituted for Target A.

### 6.3 Priority overrides after source recovery

These overrides supersede the initial component-matrix actions where they conflict. They do not authorize implementation during Gate 0B.

| Component | Pre-Gate action | Revised action |
|---|---|---|
| Macro-stage/subtree construction | `IMPLEMENT FROM PAPER` | `RECOVER + VALIDATE AGAINST PAPER AND ARCHIVED DIMENSIONS` |
| Phase-I/Phase-II drivers and cut order | `IMPLEMENT FROM PAPER` | `RECOVER + VALIDATE AGAINST SUPPLEMENT`; isolate documented paper/source discrepancies |
| Path inequality (31) | `VALIDATE BEFORE DECISION` / reconstruct | `RECOVER + VALIDATE AGAINST SUPPLEMENT` |
| Tree inequality (32) | `IMPLEMENT FROM PAPER` | `RECOVER + VALIDATE AGAINST SUPPLEMENT AND GUAN ET AL.` |
| Persistent VI pools and strengthening levels | `IMPLEMENT FROM PAPER` | `RECOVER + VALIDATE`; explicitly test the historical model-context asymmetry |
| Upper-bound evaluation | `IMPLEMENT/EXTEND FROM PAPER` | `RECOVER + VALIDATE AGAINST PAPER`; preserve exhaustive and statistical protocols as separate paths |
| Instance reader and exact dataset | missing/reconstruct | `RECOVER + PRESERVE`; Code B reader is byte-identical to the published reader |
| Instance generator | `IMPLEMENT FROM PAPER` | `IMPLEMENT AS TARGET B ONLY`; no historical generator or seed was recovered |
| Set-1 CPX callback | `IMPLEMENT FROM PAPER` | `IMPLEMENT FROM SUPPLEMENT/PRIMARY SEPARATOR SOURCES`; callback source remains unrecovered |
| Canonical scientific model and validity tests | reconstruct | unchanged: recovered code is evidence, not automatically correct |

## 7. Planned historical discrepancy log entries

When implementation is authorized, `docs/historical_code_discrepancies.md` will initially contain entries for:

1. Code B LP setup-linking row inserted into `NodalModels` rather than `LinearModels`.
2. Integer-optimality mismatch expression repeated inside the realization loop.
3. Hard-coded binary dimension versus published `smax` computation.
4. Lagrangian-cut generation every fifth/tenth iteration versus Algorithm 1.
5. Deterministic stage paths versus published stochastic macro-stage subtrees.
6. Continuous baseline Phase I versus published subtree Phase I.
7. Missing tree inequalities and sequential strengthening levels.
8. Path inequalities added only to LP models rather than reused in all prescribed MILPs.
9. Missing Set-1 callback branch-and-cut.
10. CPX and SDDiP time-limit differences.
11. Missing exhaustive Set-1 upper bound.
12. Implicit unseeded RNG behavior.
13. Missing instance generator and original instance files.

Each entry will include historical behavior, paper requirement, reconstructed behavior, reason, validating test, and expected scientific effect.

## 8. Review decisions required before implementation authorization

Implementation can begin with the canonical tree and extensive oracle after this plan is approved. The following decisions or evidence are required before the indicated later gates:

1. Acquire or approve a source for the exact path/tree inequality specifications before Gate 9.
2. Decide how to record deterministic replacement seeds if the original instances and seeds cannot be recovered.
3. Approve the probability-normalization derivation before subproblem objectives and cuts are frozen.
4. Approve the binary-dimension endpoint convention and Phase-I cut conversion derivation.
5. Resolve or explicitly classify as assumptions the subgradient configuration and strengthening-level transition thresholds.
6. Decide whether exact CPLEX 12.8 execution is required or whether a newer solver may be used with solver-dependent differences documented.

Until those decisions are made, work can safely proceed only through the components whose mathematical behavior is fully specified and independently testable: canonical data/tree structures, probability validation, Big-M, instance-design enumeration, and the tiny extensive-form oracle.
