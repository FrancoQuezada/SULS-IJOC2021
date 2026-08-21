# Published instance inventory

## Frozen archive

`Instances.zip` was recovered from Git commit `604382dad677aba09b749bcf59b9a7279d8db7a3` and copied without modification to `evidence/recovered_archives/Instances.zip`.

- ZIP SHA-256: `e79a3376becda8d685db14963cdccaca4ae46c51ac286c2ca0141df354fec475`
- ZIP size: 1,140,495 bytes
- extracted files: 900
- extracted directories: 3
- extracted byte total: 3,176,966
- modified extracted files: none

Every file hash and parsed field is in `evidence/published_instances_manifest.csv` (901 CSV lines including the header; manifest SHA-256 `cb093eb08d958588a2eb5391c35458518c1347c58a30c48a2617a219eec3a7b6`). The parser and validation checks are reproducible with `evidence/tools/forensic_extract.py`.

The top-level `instances/` directory holds an unmodified copy of these same 900 files (same three subfolders, same names and contents) as the release-facing location; `evidence/published_instances/extracted/` remains the manifest-linked forensic copy.

| Directory | Files | Bytes | Purpose indicated by contents |
|---|---:|---:|---|
| `Instances_s4_s6` | 260 | 934,156 | Sigma 4–8 compact designs, including paper Sets 1–2 |
| `Instances_s8_s12` | 600 | 2,138,559 | Sigma 8–12 compact designs, including paper Sets 3–4 |
| `Instances_s20` | 40 | 104,251 | Appendix E, Sigma=20 and R in {2,3} |

The archive is broader than the 140 instances used for Tables 1–4. It includes 25 distinct `(Sigma,b,R)` structures. The ODS `SDDiP_Zou` rows identify the exact 140-file subset.

## Exact paper subset

| Set | Sigma | b | R | Archived instance IDs | Count |
|---|---:|---:|---:|---|---:|
| 1 | 4 | 1 | 10 | 161–180 | 20 |
| 1 | 4 | 1 | 20 | 181–200 | 20 |
| 2 | 6 | 1 | 10 | 41–60 | 20 |
| 2 | 6 | 1 | 20 | 61–80 | 20 |
| 3 | 8 | 2 | 5 | 160–164, 170–174, 180–184, 190–194 | 20 |
| 3 | 8 | 5 | 5 | 320–324, 330–334, 340–344, 350–354 | 20 |
| 4 | 12 | 1 | 3 | 440–444, 450–454, 460–464, 470–474 | 20 |

Within each structure the four five-file blocks map, in order, to the paper labels `(g/h,f/h)=(2,200),(2,400),(4,200),(4,400)`. The replication index is 1–5 within a block and is recorded in the manifest.

## File grammar and dimensions

Each text file consists of exactly five Concert-compatible array literals, followed by a dashed separator and a structure footer. Direct parsing confirms the README's order:

1. demand: one-dimensional, length `n`;
2. setup cost: two-dimensional, `n x 3` for `I=1`;
3. holding cost: two-dimensional, `n x 4` for `I=1`;
4. unconditional node probability: one-dimensional, length `n`;
5. production cost: one-dimensional, length `n`.

Thus each file stores five top-level realization vectors and exactly `10n` numeric values. Demand and all cost fields are serialized as integers. Probabilities use decimal or scientific notation, normally six significant digits when a power of `1/R` is nonterminating in the chosen decimal representation.

For all 900 files:

`n = b [1 + R(Sigma - 1)]`.

This is a compact stage-realization template, not a full scenario tree. Ordering is:

- first `b` entries: the single first-stage realization, periods 1 through `b`;
- for each stage `sigma=2,...,Sigma`: `R` consecutive realization blocks, each containing `b` consecutive periods;
- the five arrays use the same record order.

All 900 probability vectors were verified against this ordering. The first `b` entries equal 1; the `bR` entries belonging to stage `sigma` equal `R^(1-sigma)`, within the archive's serialization precision. Probabilities therefore sum over compact records to more than one and must not be normalized as if the compact records were mutually exclusive full-tree nodes.

The root is deterministic as a realization but contains `b` sequential time-period records. It is not a separately stored sixth object.

## Resolution of A12

**A12: RESOLVED — HISTORICAL DATA.** The archive stores `b[1+R(Sigma-1)]` parameter records and the recovered `NodeModel` expansion reuses each stage-realization template in the relevant full-tree nodes. Independent draws for every full-tree node are ruled out by file dimensions and repeated expansion.

## Reader compatibility

### Code B

Code B's `ReadData.cpp` is byte-identical to the recovered published reader. It consumes the five arrays in the observed order and allocates exactly `D[n]`, `C[n][3]`, `H[n][4]`, `Prob[n]`, and `g[n]` for `I=1`. With `N+1=b[1+R(Sigma-1)]` and matching compile-time `Sigma,b,R`, Code B is directly format-compatible. Its existing `Pre_Solving.cpp` handles a deterministic `b`-period subproblem representation; it does not by itself implement the recovered stochastic-subtree expansion.

### Code A

Code A is not format-compatible. Its active reader expects eight arrays in this order: demand, returns, bill of material, setup cost, holding cost, probability, discarding cost, recoverable proportion, then production cost (nine reads in total). It also compiles with `I=2`, requiring setup rows of width 4 and holding rows of width 6. A published five-array file would be shifted at the second read and fail dimensionally.

Conclusion: **Code B is reader-compatible; Code A is not.**

## Cost-ratio forensic discrepancy

The manifest's `g/h` and `f/h` columns preserve the paper design labels. However, using the exact cost columns read by the recovered models (`g`, `C[][0]`, and `H[][0]`) gives empirical group means close to half those labels:

| Paper labels `(g/h,f/h)` | Files | Mean observed `mean(g)/mean(H[][0])` | Mean observed `mean(C[][0])/mean(H[][0])` |
|---|---:|---:|---:|
| (2,200) | 35 | 0.945 | 99.31 |
| (2,400) | 35 | 0.947 | 204.51 |
| (4,200) | 35 | 1.922 | 98.76 |
| (4,400) | 35 | 1.960 | 202.47 |

The recovered public history contains no generator, so the cause cannot be resolved. Exact experimental reproduction must use the frozen values, not rescale them. A future generator target must explicitly distinguish the paper's stated ratios from the effective ratios evidenced by the archived data.

## Two reproducibility targets

- **Target A — exact experimental reproduction:** use only the 140 identified original files for Tables 1–4. No regenerated file may replace them.
- **Target B — reproducible instance generation:** implement Section 5.1 separately, with explicit RNG, draw order, precision, and new deterministic seeds if historical seeds remain unavailable. Output is a replica design, never labeled as the original table data.
