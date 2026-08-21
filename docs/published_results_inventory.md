# Published results inventory

## Frozen workbooks

The original and deleted workbooks were recovered without modification. The three descriptive current files are the normalization sources; deleted aliases are retained as evidence.

| Workbook | SHA-256 | Sheets | Raw result rows |
|---|---|---|---:|
| `Performance ... (extSDDiP-I and extSDDiP-I-II.ods` | `ecd3fdf3ab550964d4cbeb3866968273f45bafe4466f487d53a6e8d17a3048be` | `ExtSDDiP`, `SDDiP_Zou` | 3,320 + 140 |
| `Performance ... (ExtSDDiP-II).ods` | `f9cb45cf43f7a15697aba0eca42244d0d63c38d4990f38384a81d2edffdde8e5` | `ExtSDDiP-II` | 2,080 |
| `Performance ... Sigma=20 ... (ExtSDDiP-I-II).ods` | `1669b538ef0855ef2659313516e659f317f49c7dfbf3e855aaa3d625a8c22fc5` | `extSDDiP s20 T900`, `SDDiP s20 T900`, `CPX_s20` | 480 + 40 + 20 |
| `Resume_SULS_IJOC2021.ods` (deleted) | `0833c6d414a143c8426546adac0597541dc85bcd99b7c672c49116ec779bbbe9` | `Results ExtSDDiP ` | 3,320 |
| `Additional_Results_ExtSDDiP-II.ods` (deleted) | same as current ExtSDDiP-II | `ExtSDDiP-II` | 2,080 |
| `Additional_Results_R1.ods` (deleted) | same as current Sigma=20 | three sheets | 540 |

There are no hidden sheets and no cell comments in any workbook. All were created with LibreOffice 5.1.6.2 on Linux. Exact creation/modification timestamps, sheet visibility, formula counts, and hashes are in `evidence/published_results/ods_sheet_inventory.csv`.

The top-level `results/` directory holds an unmodified copy of the three current (non-deleted) workbooks above, plus `published_historical_results.csv` below, as the release-facing location; `evidence/published_results/original/` remains the hash-linked forensic copy and additionally keeps the three deleted aliases for provenance.

Only `SDDiP_Zou` contains formulas: 140 formulas of the form `100*ABS(UB-LB)/UB`, one per raw row. Other result values and pivot-style summaries are stored as values, not formulas. The workbooks also contain right-hand summary/pivot areas; these were inspected but were not mistaken for raw observations.

The deleted `Resume` workbook and current `ExtSDDiP` sheet contain the same multiset of 3,320 raw result records. The current workbook reorders the records, adds named `beta` and `gamma` columns, and adds the 140 SDDiP observations. In those columns, `beta` is the published `g/h` label and `gamma` is the published `f/h` label.

## Normalized forensic table

`evidence/published_historical_results.csv` contains 6,080 observations plus a header (SHA-256 `8205632dda35d3998d2cb3038b2e23782fdf6e54556700f10b64478cb136d65d`). It includes source workbook hash, sheet and row, original instance and structure fields, normalized set/method/phase/level, LB, UB, both historical gap fields where present, LB and total time, iterations, inequalities, sample count, `beta`, `gamma`, tree dimensions, and all CPX-specific diagnostics. `original_fields_json` preserves every raw-region workbook field for each observation.

Method normalization is evidence-preserving:

- `Phase_I` + `NoCut/Path/Tree` becomes `extSDDiP-I-0/1/2`;
- `Phase_I-II` + `NoCut/Path/Tree` becomes `extSDDiP-I/II-0/1/2`;
- the ExtSDDiP-II sheet's cut labels become `extSDDiP-II-0/1/2`;
- `SDDiP_Zou` rows become `SDDiP`, with `G=1` and level 0;
- original strings remain in `phase`, `cut_type`, `model`, and `original_fields_json`.

## Paper-table verification

Tables 1–4 are arithmetic means over the 20 instance IDs for each printed structure. Workbook `Gap` values are percent values. The paper rounds means to two decimal places, total time to two decimal places, iterations to the nearest integer in display, and `# VI` to the nearest integer in display.

Representative calculations follow. Every value shown before the arrow is recomputed from raw rows; the value after the arrow is the paper display.

| Published configuration | Method | Raw-row mean `(Gap, Total Time, #ite, #VI)` | Paper |
|---|---|---|---|
| Set 1, Sigma=4, b=1, R=10, G=1 | SDDiP | `(9.590682, 1100.955, 135.45, 0)` | `(9.59, 1100.96, 135, 0)` |
| Set 1, Sigma=4, b=1, R=10, G=2 | extSDDiP-I/II-0 | `(4.175003, 875.47085, 90.10, 0)` | `(4.18, 875.47, 90, 0)` |
| same | extSDDiP-I/II-1 | `(3.210289, 869.65645, 104.50, 822.55)` | `(3.21, 869.66, 105, 823)` |
| same | extSDDiP-I/II-2 | `(2.805379, 852.25430, 139.85, 866.95)` | `(2.81, 852.25, 140, 867)` |
| Set 2, Sigma=6, b=1, R=10, G=3 | extSDDiP-I/II-0 | `(7.191998, 1065.16185, 36.15, 0)` | `(7.19, 1065.16, 36, 0)` |
| same | extSDDiP-I/II-1 | `(4.007149, 1084.60285, 49.75, 9993.60)` | `(4.01, 1084.60, 50, 9994)` |
| same | extSDDiP-I/II-2 | `(4.178283, 1100.40825, 71.85, 22038.85)` | `(4.18, 1100.41, 72, 22039)` |
| Set 3, Sigma=8, b=2, R=5, G=4 | extSDDiP-I/II-0 | `(5.377908, 1380.34360, 24.55, 0)` | `(5.38, 1380.34, 25, 0)` |
| same | extSDDiP-I/II-1 | `(2.891860, 1438.71150, 30.85, 7761.80)` | `(2.89, 1438.71, 31, 7762)` |
| same | extSDDiP-I/II-2 | `(3.220393, 1358.63450, 41.90, 10117.95)` | `(3.22, 1358.63, 42, 10118)` |
| Set 4, Sigma=12, b=1, R=3, G=6 | extSDDiP-I/II-0 | `(5.280697, 1625.97650, 17.10, 0)` | `(5.28, 1625.98, 17, 0)` |
| same | extSDDiP-I/II-1 | `(4.106797, 1677.38250, 20.90, 4260.35)` | `(4.11, 1677.38, 21, 4260)` |
| same | extSDDiP-I/II-2 | `(3.899421, 1557.63165, 28.80, 6600.75)` | `(3.90, 1557.63, 29, 6601)` |

These matches cover SDDiP and levels 0, 1, and 2 in each required set. The same workbook summary cells reproduce all remaining Tables 1–4 rows. The ExtSDDiP-I subset reproduces supplement Tables 6–9; the separate ExtSDDiP-II workbook reproduces Tables 10–13; the Sigma=20 workbook contains the raw source for Table 14.

## Configuration fingerprints

The combined source/results evidence contains the following strong fingerprint:

- output executable names encode `s`, `c=R`, `p=b`, and macro-stage size;
- raw fields encode `Sigma`, `b`, `R`, `G`, full-tree node count, scenarios, method, phase, and cut level;
- `beta={2,4}` and `gamma={200,400}` are present in every main extSDDiP row;
- 1,000 maximum iterations, 1,000 UB scenarios, `K=1` in the paper, and the three strengthening levels are mutually consistent;
- source build command explicitly uses CPLEX Studio 12.8 and two solver threads are set throughout;
- the source resets the UB evaluator's budget to 900 seconds and uses a shared LB timer;
- main ODS LB times cluster at 900 seconds while total times include UB evaluation; Sigma=20 sheet names explicitly say `T900`;
- CPX Sigma=20 totals cluster at 1,805 seconds, corroborating the published 1,800-second CPX budget even though recovered `GlobalModel.cpp` itself says 900 seconds.

## Ambiguity evidence from ODS files

The ODS files do not expose transition thresholds, subgradient settings, seeds, cut-deduplication flags, or explicit partial-iteration state. Therefore A3, A4, A5, A10, A17, A18, A19, and A20 are **not resolved by historical results**. They are resolved, where possible, by recovered source instead. The files do support A15 by separating LB `Time` from `Total Time`, and they corroborate the published configuration and A25 attribution through exact executable/structure/method fingerprints.
