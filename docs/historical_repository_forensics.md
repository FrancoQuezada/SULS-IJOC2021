# Historical repository forensics

## Scope and preservation

Repository: `https://github.com/FrancoQuezada/SULS-IJOC2021`

The repository was cloned as a bare mirror on 2026-08-21. No remote write was made. The forensic mirror is at `evidence/historical_repository/SULS-IJOC2021.git`; an independently portable all-ref bundle is at `evidence/historical_repository/SULS-IJOC2021-all-refs.bundle` (SHA-256 `f03328baa8ffc61c67a894ecaaba508ea8a5f8fe2d2eb40034acd8d54b2d5cd0`). The mirror passed `git fsck --full` without diagnostics.

- HEAD: `49adb0d0067ceaa7994042c434975de3c8a22911`
- branches: `main` only
- tags: none
- other public refs: none
- commits: 18, one linear history
- author/committer time zone: `+02:00`

Machine-verifiable captures are in `complete_commit_graph.tsv`, `git_log_all_stat.txt`, `git_log_all_name_status.txt`, `git_log_deleted_paths.txt`, and `relevant_commit_trees.txt`. The last file contains the complete recursive tree for each of the 18 commits, not only the current tree.

## Complete commit graph

| Commit | Timestamp | Subject | Forensic significance |
|---|---|---|---|
| `6acf403` | 2021-06-22 08:33:09 +02:00 | Initial commit | README only |
| `46b16ab` | 2021-06-22 09:56:59 +02:00 | Add files via upload | adds `Resume_SULS_IJOC2021.ods` |
| `9adc5a8` | 2021-06-22 10:28:56 +02:00 | Add files via upload | adds `Additional_Results_R1.ods` |
| `de5e48e` | 2021-06-22 10:33:44 +02:00 | Add files via upload | adds `Additional_Results_ExtSDDiP-II.ods` |
| `604382d` | 2021-06-22 11:15:31 +02:00 | Add files via upload | adds `Instances.zip` |
| `80e6fd9` | 2021-06-22 11:25:58 +02:00 | Add files via upload | adds `Code_extSDDiP.zip` |
| `cf42382` | 2021-06-22 11:43:54 +02:00 | Update README.md | documentation only |
| `0afac16` | 2021-06-22 11:55:27 +02:00 | Update README.md | documentation only |
| `c00ffc2` | 2021-06-22 14:09:04 +02:00 | Delete Additional_Results_ExtSDDiP-II.ods | deletes old result filename |
| `54aa888` | 2021-06-22 14:09:15 +02:00 | Delete Additional_Results_R1.ods | deletes old result filename |
| `88832bf` | 2021-06-22 14:09:28 +02:00 | Delete Resume_SULS_IJOC2021.ods | deletes old main workbook |
| `c5f2369` | 2021-06-22 14:10:16 +02:00 | Add files via upload | adds the three descriptive current ODS filenames |
| `80e6d9f` | 2021-06-22 14:12:10 +02:00 | Update README.md | documentation only; distinct from `80e6fd9` |
| `4a644d8` | 2021-06-22 14:19:41 +02:00 | Update README.md | documentation only |
| `dd2b299` | 2021-06-22 14:34:53 +02:00 | Delete Code_extSDDiP.zip | removes source from current branch |
| `5205eb1` | 2021-06-22 14:35:30 +02:00 | Update README.md | documentation only |
| `f7d4fe0` | 2021-06-22 14:35:46 +02:00 | Update README.md | documentation only |
| `49adb0d` | 2021-06-22 14:36:19 +02:00 | Update README.md | current HEAD |

## Recovered archives

| Artifact | Source commit | SHA-256 | Result |
|---|---|---|---|
| `Code_extSDDiP.zip` | `80e6fd911724efc98172b9f218d10b5c0ebf2fd8` | `f26db63bd8aa0f53231c904a7801d3546c2b56ef2277152d42c4bf2d413cc009` | 15 files recovered and frozen |
| `Instances.zip` | `604382dad677aba09b749bcf59b9a7279d8db7a3` | `e79a3376becda8d685db14963cdccaca4ae46c51ac286c2ca0141df354fec475` | 900 files recovered and frozen |
| `Resume_SULS_IJOC2021.ods` | `46b16ab2c910e23eab0138a702f33aab7306cf04` | `0833c6d414a143c8426546adac0597541dc85bcd99b7c672c49116ec779bbbe9` | old main-results workbook recovered |
| `Additional_Results_R1.ods` | `9adc5a8f8a66588209d529f844e95ac0372682b6` | `1669b538ef0855ef2659313516e659f317f49c7dfbf3e855aaa3d625a8c22fc5` | byte-identical to current Sigma=20 workbook |
| `Additional_Results_ExtSDDiP-II.ods` | `de5e48e4473412da47285df4e049cc065e92398c` | `f9cb45cf43f7a15697aba0eca42244d0d63c38d4990f38384a81d2edffdde8e5` | byte-identical to current ExtSDDiP-II workbook |

The old main workbook and current main workbook contain the same multiset of 3,320 extSDDiP raw records. The current file reorders them, labels `beta` and `gamma`, and adds the 140-row `SDDiP_Zou` sheet.

## Historical source inventory

All source rows below originate at commit `80e6fd9`, dated 2021-06-22. Confidence is high unless noted. `ReadData.cpp` is byte-identical to Code B; the other files are later or divergent relatives of Code B. None is byte-identical to Code A.

| File | Function | Related paper component | Relationship to Code A/B | Confidence |
|---|---|---|---|---|
| `Header.h` | compile-time tree/subtree dimensions and declarations | macro-stage and stochastic-subtree architecture | materially extends B; A is a different model lineage | high |
| `Main.cpp` | argument parsing, preprocessing, Phase I/II, sampling, backward cuts, stopping, UB, output | Algorithms 1–3 and experimental driver | extends B into the published two-phase/subtree driver | high |
| `Pre_Solving.cpp` | compact stage template, subtree, macro-model, parent/child/leaf maps, Big-M | partial decomposition and stochastic subtree construction | major extension of B | high |
| `Pre_Solving_Tree.cpp` | full-tree indexing helpers | extensive-tree utilities | unique recovered file; not named by active compile command | medium |
| `NodalModel.cpp` | continuous and binary-state MILPs; forward and UB solves | Phase I/II primal subproblems | extends B | high |
| `LinearModel.cpp` | LP relaxations, binary-state copy constraints, extended formulation, path separator | strengthened Benders cuts and inequality (31) | extends B and contains recovered missing polyhedral logic | high |
| `LagrangianModel.cpp` | semi-Lagrangian models and solves | strengthened Benders/Lagrangian cut constants | extends B | high |
| `DualLagrangianModel.cpp` | Polyak subgradient dual routine | Lagrangian cuts | close relative of B with 30-iteration configuration | high |
| `CutGeneration.cpp` | Benders, strengthened Benders, integer-optimality, and Lagrangian cuts | all three Phase-II cut families plus Phase-I strengthened cuts | materially extends B with subtree/multicut indexing | high |
| `TreeInequalities.cpp` | cumulative-demand graph and Dijkstra-based heuristic separator | inequality (32), strengthening level 2 | absent from A/B; exact historical implementation recovered | high |
| `UpperBounds.cpp` | exhaustive and sampled policy evaluation | Set-1 true UB and other-set statistical UB | absent from A/B; exact historical implementation recovered | high |
| `GlobalModel.cpp` | expands compact templates into extensive MILP/LP | CPX baseline | extends B; uses 900 seconds, not the paper's 1800 | high for model, low for published CPX run |
| `ReadData.cpp` | reads the five arrays | published instance format | byte-identical to B; A expects additional remanufacturing fields | high |
| `GlobalVariables.cpp` | global Concert state | implementation plumbing | extends B | high |
| `compilar.txt` | GNU C++/Concert link command | build environment | names CPLEX Studio 12.8, C++11, and the recovered tree/UB files | high |

## Component findings

- Macro-stage decomposition and stochastic subtrees are explicit: `BkStg` is stages per subtree, `N_SubTree` is the full subtree size, and `NodeModel` maps full subtree nodes onto compact stage-realization records.
- Phase I keeps state continuous; Phase II adds binary expansion equalities. The shared driver, source order, and result labels match the paper's algorithm families.
- Phase-II backward order is strengthened-Benders preparation, optional integer/Lagrangian solves, then strengthened Benders, Lagrangian, and integer-optimality cuts for each predecessor.
- The path separator scans every subtree node, constructs the violated `(l,S)` inequality selected by the LP point, retains all violations at most `-0.001`, and persists them in the appropriate models.
- The tree separator enumerates candidate terminal nodes after a Dijkstra shortest-path construction, retains all violations at most `-0.001`, and persists them in the linear model. It does not add its generated rows to nodal/Lagrangian models; those statements are commented out.
- No cut canonicalization or duplicate test is present.
- The driver uses `CLOCK_MONOTONIC`; Phase I and Phase II share one LB timer. Time is polled inside backward sweeps, so a time limit can end a partial iteration.
- The source contains the exhaustive Set-1 and statistical upper-bound evaluators. The statistical endpoint is mean plus `1.96*s/sqrt(actual_sample_count)`.
- No `srand` call or stored seed was found.

## Negative search results

Every commit tree, archive member, deleted path, and reachable blob name was searched. No Makefile, experiment shell script, parameter file, instance generator, CPLEX callback, or files named like `Compute_Inequality.cpp`, `Cut_k_U_Model.cpp`, or `CutSeparation_1_MA3.cpp` occur in the public history. The recovered `GlobalModel.cpp` is not the paper's Set-1 custom callback implementation and sets 900 rather than 1800 seconds. These components remain unrecovered; this conclusion is based on the complete history rather than current HEAD.
