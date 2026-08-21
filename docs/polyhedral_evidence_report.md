# Polyhedral evidence report

## Evidence and scope

The official 13-page supplement for DOI `10.1287/ijoc.2021.1118` was inspected at the publisher's canonical file URL recorded in `evidence/supplement/provenance.txt`. It contains Appendix A (valid inequalities), Appendix B (strengthened-Benders example), Appendix C (detailed algorithms), Appendix D (Tables 6–13), and Appendix E (Table 14 and Sigma=20 design).

Direct PDF bytes could not be frozen because the publisher returned a Cloudflare HTTP 403 to the execution host. No derivative was substituted for the official file. This affects archival completeness, not the formula/code comparison below: the publisher-rendered content was available for inspection.

## Inequality (31): path inequality

For a node `ell` and subset `S` of its root-to-`ell` path `P(1,ell)`, Appendix A states

`s_0 + sum_{n in S} x_n + sum_{n in P(1,ell)\S} d_{n,ell} y_n >= d_{1,ell}`,

where `d_{n,ell}=sum_{m in P(n,ell)} d_m`. Appendix A identifies exact polynomial separation by the Barany et al. procedure.

### Recovered-code mapping

`LinearModel.cpp::CuttingPlaneGeneration` enumerates each terminal candidate `ell`, walks its ancestor path, and accumulates `d_{n,ell}` backward. At the current LP point it chooses `x_n` when `xbar_n <= d_{n,ell} ybar_n` and `d_{n,ell}y_n` otherwise. It adds entering inventory `Z[m]` for a nonroot macro-subproblem and subtracts the path demand, giving precisely the shifted version of (31). A row is retained when violation is at most `-0.001`.

All violated candidates found in that invocation are retained; the historical `break` after the first violation is commented out. Rows persist in the continuous/nodal, linear, and Lagrangian formulations, except that the root's inequality counter is not incremented. This is stronger evidence than Code B's deterministic-path routine because it is the actual public source archive and uses the recovered stochastic-subtree maps.

Conclusion: **inequality and historical separation behavior recovered.** Required reconstruction action changes from `IMPLEMENT FROM PAPER` to `RECOVER + VALIDATE AGAINST SUPPLEMENT`.

## Inequality (32): tree inequality

Appendix A takes an ordered set `O={n_1,...,n_|O|}` with cumulative demands ordered as `0=d_{1,n_0} <= d_{1,n_1} <= ... <= d_{1,n_|O|}`. For a selected subset `S_O` of nodes in the union of the associated root paths, it states

`s_0 + sum_{n in S_O} x_n + sum_{n in complement(S_O)} Delta_n(O)y_n >= d_{1,n_|O|}`,

where

`Delta_n(O)=sum_{n_o in O intersect V(n)} (d_{1,n_o}-d_{1,n_{o-1}})`.

Appendix A notes that (31) is the singleton-`O` special case and cites Guan et al.'s heuristic separation because the general separation complexity is unknown.

### Recovered-code mapping

`TreeInequalities.cpp::Pre_TreeInequalities` builds cumulative root-path demand `SumD` and an acyclic graph whose edge increments are positive cumulative-demand differences. `dijkstra` weights each edge increment by the sum of current LP setup values on the destination path, constructs predecessor paths, then considers each candidate terminal `k`. For each unique ancestor it reconstructs the increment `Delta`, caps it by the relevant path demand, and chooses either `x_n` or that coefficient times `y_n` according to the current LP point. It adds entering stock `Z[m]`, subtracts `SumD[m][k]`, and retains every row violated by at least `0.001`.

The generated tree rows persist only in `LinearModels[m]`; additions to nodal and Lagrangian models are present but commented out. This implementation detail matters when interpreting the paper's general statement that inequality pools remain available in model-strengthening contexts.

Conclusion: **equation and actual historical heuristic recovered.** Required reconstruction action changes from `IMPLEMENT FROM PAPER` to `RECOVER + VALIDATE AGAINST SUPPLEMENT AND GUAN ET AL.`

## Strengthening levels and order

Supplement Algorithm 1 defines:

- level 0: initial formulation;
- level 1: run path separation, add (31) rows to the persistent pool, and solve the strengthened LP;
- level 2: run tree separation, add (32) rows to the persistent pool, and solve the strengthened LP.

Supplement Algorithms 2–3 specify one strengthening call before the linear and Lagrangian solves in each realization of each backward step. Phase II then solves the integer subproblem and the Lagrangian dual and adds the three cut families to the cost-to-go approximation.

The recovered Phase-I driver uses a source-specific transition rule not quantified by the supplement: level 0 runs until a 20-iteration window fails to improve the LB by more than `0.1`; level 1 then receives another 20-iteration block before level 2 begins. The implementation does not inspect a separate "no violated inequality" counter for the level-1-to-2 transition. In Phase II, `B_SolveLinearModel` calls tree separation unconditionally whenever path cutting is enabled, and calls it a second time when the tree flag is set. Thus the nominal level-1 Phase-II implementation can already contain tree rows; this is logged as a probable historical bug in `docs/historical_code_discrepancies.md`.

## Strengthened-Benders example

Appendix B gives three nondominating cuts from the same state and realization set:

- initial formulation: intercept 682.18, slope -8.24;
- path-strengthened formulation: intercept 882.55, slope -25.40;
- path+tree-strengthened formulation: intercept 887.88, slope -26.18.

This validates the paper's rationale for retaining strengthened-Benders cuts from all sequential formulation levels rather than replacing an earlier cut merely because it came from a weaker formulation.

## Code A and Code B comparison

- Code A's build notes reference absent separation/callback sources; its available tree contains no active equivalent of the recovered subtree tree separator.
- Code B has a path-oriented routine for deterministic `b`-period models but no recovered stochastic-subtree tree separator.
- The Git-recovered source contains both active separation procedures, the sequential driver, persistent model insertion, violation threshold, and cut counting. It is therefore the primary historical implementation evidence.

## Remaining polyhedral cautions

- The public history contains no custom CPLEX callback for the Set-1 extensive-form CPX baseline. The recovered separators operate in extSDDiP subproblem solves, not at branch-and-cut nodes of the published CPX comparison.
- The supplement provides the mathematical family and identifies prior separation work, but not the public source for the Set-1 callback.
- The historical tree separator contains implementation hazards that must be preserved as evidence but reviewed before reuse, including integer arrays for distances despite numeric edge weights and model-context additions commented out.
- No algorithmic code has been implemented or merged during Gate 0B.
