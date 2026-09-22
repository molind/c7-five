# Result ledger — 22 September 2026

## Overall conclusion

No construction of 368 and no global exclusion of 368. Published global bounds
remain 367–401; the computation did not lower the global upper bound to 367.
There is no known reduction putting every hypothetical 368-word set into the
restricted regions checked here. The number of closed branches is therefore
not a measure of progress toward a global proof.

Evidence levels used below:

1. **Portable exact replay:** shipped data and a solver-free checker reproduce the claim.
2. **Historical exact replay:** the working archive recorded an exact replay; the public archive preserves evidence, but its old path/hash chain is not a turnkey portable program.
3. **Tested exhaustive computation:** a completed search with controls, without a separately replayable formal proof tree.
4. **Exploration:** samples, numerical relaxations and time-limited runs. Failure to find a set is not an exclusion.

## Portable exact results

### U₁ and U₂

`verify/u1.py` rebuilds 896 vertices and covers them with 367 boxes.
`verify/u2.py` rebuilds 2,217 vertices, verifies 1,026 positive box weights
totalling 367.999845, and checks the 367-word witness. Both establish exact
independence number 367 only for their respective domains.

The embedded data are deliberately self-contained, so the proof can be rerun
without downloading a solver or reconstructing the search that found a cover.

### U₃ method limitation

`verify/u3.py` rebuilds 4,859 vertices and checks 2,569 positive vertex weights.
Maximum box load is `64/64`; total weight is `23,696/64 = 370.25`. No fractional
clique-cover proof below 368 exists for this domain. This is a dual certificate,
not an independent set.

### Family F

`verify/family_f.py` validates 36 distinct independent sets of size 367,
reconstructs every possible one-for-one neighbor and checks connectedness and
maximality. There are 150 directed exchanges, intersection 360 and union 374.
Keeping that 360-word core leaves exactly 14 eligible words; enumeration of all
16,384 subsets gives maximum seven, with exactly the same 36 maxima.

F differs from the six earlier recorded components by component size. This is
novelty within our collection, not a claim of novelty in the literature.

### Any 368-word set omits at least four of one particular 315-word core

`verify/core_overlap.py` reconstructs all blocker buckets and covers. The
parents and the core are explicit in `data/core-overlap.json.gz`.

| Omitted core words | Cases | Certificate breakdown | Largest cover weight |
|---|---:|---|---:|
| 2 | 49,455 | 49,097 extensions; 358 explicit covers | 54.000039 < 55 |
| 3 | 5,159,805 | 5,136,228 implicit extensions; 19,339 explicit extensions; 4,238 explicit covers | 55.000049 < 56 |

After retaining 312 words, fewer than 56 replacements are possible. An
independent set reaching 368 must therefore retain at most 311 of this core.
The portable checker verifies both parents directly and replays every triple;
the LP solver used to discover weights is outside the verification path.

### The implemented global pair constraints do not exclude 368

`verify/global_pair.py` checks the rational point in `data/global-pair-368.json`:

- all 16,807 word values and exact sum 368;
- 941,192 anchored pair placements modulo first-word symmetry;
- 19,607 translated cylinders in representative coordinate sets;
- 56 Fourier classes covering all 16,807 characters.

The minimum certified eigenvalue lower bound is positive (approximately
0.0846571389); its exact rational value is in the verification output. This is
a global method limitation. It establishes no new lower bound on α(C₇⁵).

The numerical exploratory optimum was about 401.942658501, unchanged by the
triangle constraints tested. That numerical optimization is not advertised
as a new certified upper bound.

## Larger historical results and their conditions

Find records by filename in [`archive/records-manifest.json`](../archive/records-manifest.json).
Each listed JSON is shipped gzip-compressed under `archive/records/`.

| Result | Scope and evidence | Main record |
|---|---|---|
| F is closed through width 9 | All 36 states; no improving exchange deleting at most nine. Tested native exhaustion and checked induction, not formal proof-tree replay | `component-F-incremental9-final-verification-sep22.json` |
| All C/F translation hybrids have optimum 367 | The chosen two parents, 16,807 pure translations, and subsets of each union. Does not cover arbitrary words outside those unions | `translation-hybrids-CF-verification-sep22.json` |
| Fixed 155 words, deletion radius ≤24: no 368 | Pool 4,383; complete 1,069-node partition, 535 integer leaf contradictions, minimum positive gap 24. Conditional on the F9 computation | `CF-core160-r24-full1069-verification-sep22.json` |
| Fixed 115 words, radius ≤24: still open | Pool 6,796; latest refined tree has 256 nodes and 125 checked integer leaves, **5 pending branches and 1 unknown** | `CF-core200-r24-refined-prefix256-verification-sep22.json` |
| 59 learned clauses, then 7 shorter clauses | Exact subtree coverage/resolution in that same fixed-115/radius-24 domain. The seven new clauses have 5 or 6 positive literals | `CF-core200-subtree-cuts-sep22.json`, `CF-core200-refined-cuts-verification-sep22.json` |

The labels `core160` and `core200` in filenames refer to omitted words from a
315-word core. They retain **155** and **115** words respectively.

The seven refined clauses remove fractional solutions previously allowed:
one selected five-variable sum fell from 5 to 4 in a checked LP comparison.
This shows local pruning, not an end-to-end speedup. Certificates from different
model hashes cannot be interchanged: row ordering and meaning differ.

## Other explored directions

The historical source archive includes unfrozen local exchanges, neutral moves,
low-conflict walks, component classification under cycle symmetries, layer-shape
recombination, core releases, matching hybrids, cover LPs, and branching with
integer dual certificates. The 1,668-shape layer library had computational
optimum 367 within that library; this did not enumerate all possible layers.

The C+B width-eight scan stopped after 1,009 of 6,049 states. It is incomplete.
Earlier timeouts were sometimes later superseded by completed runs; use this
ledger and the final named record, not an isolated old log, for current status.

## What would count as a breakthrough

- **Construction:** 368 explicit distinct words passing all 67,528 pair tests.
- **Global exclusion:** a complete argument covering every independent set of
  368, including the domains not represented by these fixed cores, radii or libraries.
- **Method progress:** a justified new constraint or reduction, with its scope
  and assumptions stated, plus a checkable artifact showing what it actually rules out.

None of those first two outcomes has been achieved here.
