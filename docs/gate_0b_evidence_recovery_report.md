# Gate 0B Evidence Recovery Report

## 1. Executive findings

Gate 0B materially changes the reconstruction basis. The complete Git history contains a deleted official source archive, `Code_extSDDiP.zip`, with the two-phase subtree implementation, all three value-function cut families, path and tree separators, and upper-bound evaluators. The repository also contains 900 original instances and raw ODS observations underlying the paper and supplement tables.

The earlier conclusion that these components had to be reconstructed solely from the paper is superseded. Recovered code remains forensic evidence—not automatically correct production code—and must be ported only after paper/supplement validation and small-instance validity tests.

No algorithmic source was implemented in this gate. Code A and Code B were not modified and still match their pre-audit SHA-256 manifest.

## 2. Git history findings

- Public repository HEAD: `49adb0d0067ceaa7994042c434975de3c8a22911`.
- One branch (`main`), no tags, 18 linear commits.
- Full mirror and all-ref bundle are frozen under `evidence/historical_repository/`.
- `Instances.zip` was added at `604382d` and remains at HEAD.
- `Code_extSDDiP.zip` was added at `80e6fd9` and deleted at `dd2b299` roughly three hours later.
- Three old ODS names were added and deleted, then descriptive replacements were added. ExtSDDiP-II and Sigma=20 replacements are byte-identical to their old versions.
- Every commit tree, deleted path, and source-like archive member was searched.

Detailed commit graph, timestamps, archive hashes, and historical source inventory: `docs/historical_repository_forensics.md`.

## 3. Recovered source-code components

The 29,403-byte source ZIP has SHA-256 `f26db63bd8aa0f53231c904a7801d3546c2b56ef2277152d42c4bf2d413cc009` and contains 14 C++/header files plus `compilar.txt`.

Recovered with high confidence:

- macro-stage decomposition and stochastic subtree mapping;
- Phase I and Phase II drivers;
- integer optimality, Lagrangian, Benders, and strengthened-Benders cut generation;
- path inequality separation;
- Dijkstra-based tree inequality separation;
- continuous-to-binary state transition;
- extensive MILP construction;
- exact and sampled upper-bound evaluation;
- timing, stopping, output schema, and CPLEX subproblem parameters.

Not recovered from any commit:

- the instance generator and seed/config scripts;
- the Set-1 custom CPX callback;
- the Code-A-referenced `Compute_Inequality.cpp`, `Cut_k_U_Model.cpp`, and `CutSeparation_1_MA3.cpp`;
- Makefiles or experiment launch scripts.

The build command explicitly names CPLEX Studio 12.8 and C++11. The exact recovered-source file table is in `historical_repository_forensics.md`.

## 4. Published-instance inventory

- Original ZIP SHA-256: `e79a3376becda8d685db14963cdccaca4ae46c51ac286c2ca0141df354fec475`.
- Files: 900 across `Instances_s4_s6` (260), `Instances_s8_s12` (600), and `Instances_s20` (40).
- Every extracted file has an individual SHA-256 in `evidence/published_instances_manifest.csv`.
- ODS raw instance IDs identify the exact 140 files for Tables 1–4.
- Appendix E's 40 Sigma=20 instances cover `R={2,3}`.

The archive also exposes a scientific discrepancy: the paper/ODS labels `(g/h,f/h)={2,4}x{200,400}`, but the cost columns actually used by the recovered models have empirical ratios near `{1,2}x{100,200}`. Exact reproduction must use archived values without correction.

Full structure inventory and paper-ID map: `docs/published_instance_inventory.md`.

## 5. Instance-format analysis

Every file contains exactly five arrays in README order: demand, setup cost, holding cost, probability, and production cost. With `I=1`, their dimensions are `n`, `n x 3`, `n x 4`, `n`, and `n`, hence `10n` numeric values.

All 900 satisfy

`n = b[1+R(Sigma-1)]`.

They therefore store stage-wise-independent realization templates, not independent records for full-tree nodes. First come `b` periods for the deterministic root realization; then, stage by stage, `R` blocks of `b` periods. The probability block for stage `sigma` equals `R^(1-sigma)`. The recovered `NodeModel` maps/reuses these compact records throughout a stochastic subtree and full-tree expansion.

A12 is resolved by historical data, and A13's deterministic-root convention is also resolved.

## 6. Historical-results inventory

All sheets, formulas, raw rows, summaries, comments, visibility, and ODF metadata were inspected.

- no hidden sheets;
- no comments;
- 140 formulas, all in `SDDiP_Zou`, computing percent gap as `100*abs(UB-LB)/UB`;
- 6,080 normalized raw observations;
- main extSDDiP: 3,320;
- main SDDiP: 140;
- ExtSDDiP-II: 2,080;
- Sigma=20 extSDDiP: 480;
- Sigma=20 SDDiP: 40;
- Sigma=20 CPX: 20.

The normalized table is `evidence/published_historical_results.csv`. All raw source fields are retained in `original_fields_json`; original ODS files remain untouched. Detailed workbook metadata and field mappings are in `docs/published_results_inventory.md`.

## 7. Paper-table verification

The raw ODS rows reproduce Tables 1–4 after filtering to the 140 IDs recorded in `SDDiP_Zou` and averaging 20 instances per printed structure.

Representative checks include:

- Set 1 SDDiP: 9.590682% -> 9.59%;
- Set 1 extSDDiP-I/II levels 0/1/2: 4.175003/3.210289/2.805379% -> 4.18/3.21/2.81%;
- Set 2, G=3, levels 0/1/2: 7.191998/4.007149/4.178283% -> 7.19/4.01/4.18%;
- Set 3, b=2, G=4, levels 0/1/2: 5.377908/2.891860/3.220393% -> 5.38/2.89/3.22%;
- Set 4, G=6, levels 0/1/2: 5.280697/4.106797/3.899421% -> 5.28/4.11/3.90%.

Time, iterations, and valid-inequality means also reproduce the printed values under ordinary two-decimal/nearest-integer display rounding. This establishes provenance for the archived workbook rows.

## 8. Supplement findings

The publisher's official 13-page supplement was located and inspected. It contains all requested appendices:

- Appendix A: exact families (31) and (32), exact path separation citation, and tree heuristic citation;
- Appendix B: the nondominance example for the three strengthened-Benders cuts;
- Appendix C: strengthening, approximate Phase-I, and complete extSDDiP algorithms;
- Appendix D: Tables 6–13;
- Appendix E: Sigma=20, `b=1`, `R={2,3}`, `G={2,4,5,10}`, and Table 14.

The execution host receives HTTP 403/Cloudflare for direct PDF bytes, although the publisher-rendered PDF is inspectable. Its canonical URL and the exact limitation are recorded in `evidence/supplement/provenance.txt`; no altered surrogate was created. The equation/code mapping is in `docs/polyhedral_evidence_report.md`.

## 9. Code A/B compatibility with published instances

- **Code B: format-compatible.** Its `ReadData.cpp` is byte-identical to the recovered official reader. Correct compile-time `Sigma,b,R` values are still required.
- **Code A: not format-compatible.** It expects additional returns, bill-of-material, discarding, and recoverable-proportion arrays and compiles for different row widths with `I=2`.

Code B's preprocessing represents deterministic `b`-period subproblems; the recovered source is still required for genuine stochastic-subtree expansion.

## 10. Resolved ambiguities

Resolved by source, data, supplement, or derivation: A1–A9, A12–A13, A15–A23, and A25.

Notable recovered behaviors are:

- 20-iteration level blocks rather than a recorded violated-cut counter;
- Phase-I LB window tolerance 0.1 and Phase-II tolerance 0.01;
- 30 subgradient iterations, factor-2 Polyak step, and `1e-5` residual threshold;
- no VI deduplication and retention of all violations at most `-0.001`;
- shared Phase-I/II LB timer and possible partial backward sweep at timeout;
- one global `rand()` stream for training and evaluation;
- UB endpoint `mean + 1.96*s/sqrt(actual sample count)`;
- active exhaustive Set-1 evaluator.

The authoritative A1–A25 status/evidence table is now in Section 6.1 of `docs/scientific_reconstruction_plan.md`.

## 11. Remaining ambiguities

Four entries require explicit assumptions or further evidence:

- **A10:** historical generation seeds;
- **A11:** generator RNG, draw order, and serialization/rounding procedure;
- **A14:** exact CPLEX 12.8 defaults and Set-1 callback behavior/options;
- **A24:** deterministic solver tie-breaking across environments.

The official supplement PDF bytes are also not locally frozen because of publisher access control. Its content was available for this audit, so this is an archival limitation rather than a mathematical blocker.

## 12. Changes required to the reconstruction plan

The plan now:

- separates Target A exact reproduction on the 140 original files from Target B regenerated replicas;
- changes macro-stage, subtree, phase drivers, cut families, path/tree separators, and upper-bound evaluators from paper-only implementation to recovery plus validation;
- preserves reconstruction of a canonical scientific model and validity oracles rather than copying historical defects;
- keeps the Set-1 CPX callback as paper/primary-source implementation because callback code was not recovered;
- records the paper/data cost-ratio discrepancy;
- replaces all A1–A25 statuses with the permitted evidence classifications while retaining the original uncertainty text.

## 13. Recommended first implementation gate

Authorize a limited first implementation gate containing only:

1. immutable evidence loaders and schema validation for the five-array format;
2. canonical compact-template, scenario-tree, and stochastic-subtree objects validated against all 900 manifests;
3. tiny extensive-form oracle and probability/Big-M tests;
4. provenance-preserving ports of recovered components behind historical-vs-paper behavior flags;
5. mathematical validity tests for (31), (32), and all value-function cuts before any large experiment.

Do not yet claim exact CPX reproduction, implement a historical-data generator, or run/tune Tables 1–4. Those remain blocked on the callback evidence/environment and Target-B RNG decisions.

# Gate decision: READY FOR LIMITED IMPLEMENTATION
