# c7-five

**Can 368 pairwise nonconfusable words of length five be chosen over a seven-symbol cycle?**

This repository records computational work on the independence number of the
fifth strong power of the cycle graph, **α(C₇⁵)**. It contains exact certificates
for restricted domains, constructions of size 367, search code, and the limits
of the approaches tried.

**Status, 22 September 2026: paused. No 368-word construction and no global proof
that 368 is impossible.** The work here does not improve the published global
bounds **367 ≤ α(C₇⁵) ≤ 401**. There is no justified completion percentage for a
global impossibility proof.

## The problem in one paragraph

A word is a tuple `(x0, x1, x2, x3, x4)` with each entry in `0,…,6`. Two distinct
words conflict when every coordinate is equal or adjacent around the cycle
`0—1—2—3—4—5—6—0`. We seek a set with no conflicting pair. Equivalently, every
pair must have a coordinate whose difference modulo seven is in `{2,3,4,5}`.
There are `7^5 = 16,807` possible words. Checking a proposed set of 368 needs only
**67,528 pair tests**; proving that every such set fails is a much harder task.

The 367-word lower bound is due to
[Polak and Schrijver](https://arxiv.org/abs/1808.07438). Their construction and
the distinction between a finite graph power and Shannon capacity are the
starting point, not discoveries of this project.

## What is established here

| Result | What it says | Evidence shipped here |
|---|---|---|
| **α(U₁) = 367** | One particular 896-word domain cannot contain 368 | Standalone integer replay |
| **α(U₂) = 367** | One particular 2,217-word domain cannot contain 368 | 1,026 boxes, weight **367.999845**, plus a 367-word witness |
| **U₃ box-cover obstruction** | Every fractional clique cover of this 4,859-word domain costs at least **370.25** | Exact dual witness; this is not an independent set |
| **Family F** | A 36-state one-word-exchange component; keeping its 360-word core gives optimum 367 | Full component and fixed-core replay |
| **A 315-word overlap restriction** | Any independent set of 368 must omit at least four words of one specified core | Integer replay covering all **5,159,805** triples |
| **A fixed-155/radius-24 region is closed** | No 368 in that particular region | Historical 1,069-node tree and 535 integer leaf certificates; conditional on the F9 computational result |
| **Global pair-method obstruction** | The implemented Fourier, pair, triangle and cylinder constraints admit an exact rational point of weight **368** | Solver-free exact replay, using NumPy for integer arrays |

The first five results and the pair-method obstruction have portable verification
entry points. The larger search history is an archive, with a separate evidence
level and explicit limitations. See [the result ledger](docs/results.md).

## Start with a verification

From this directory, with ordinary Python 3.10+ and **without `-O`**:

```sh
python3 verify/u2.py
python3 verify/family_f.py
python3 verify/core_overlap.py
```

These use the standard library and require no network access or solver.
To run all five standard-library replays:

```sh
make verify
```

For the global pair certificate, install the dependency into a virtual environment:

```sh
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
mkdir -p build
.venv/bin/python verify/global_pair.py \
  --input data/global-pair-368.json --output build/global-pair-verification.json
```

The output path must be new. The checker validates rational and integer
inequalities; it does not invoke an LP solver. It certifies feasibility of a
relaxation, **not existence of 368 words**.

## Repository guide

- [Problem and proof ideas](docs/problem.md): conflict graph, boxes, restricted domains.
- [Results and evidence levels](docs/results.md): exact results, completed searches, open cases.
- [Reproduction](docs/reproduce.md): commands, dependencies and archival limitations.
- [Continuation](docs/continuation.md): where work stopped and useful next questions.
- [Sources and attribution](docs/sources.md): prior work and contributor attribution.
- [`verify/`](verify/), [`data/`](data/): portable checkers and mathematical certificates.
- [`search/`](search/): C++ exchange search and its input exporter.
- [`results/`](results/): actual replay outputs from packaging this repository.
- [`archive/`](archive/README.md): historical source, selected records and local inventory.
- [`provenance.json`](provenance.json): source/export hashes and transformations.

No new search was started to prepare this repository. Research remains paused;
the repository preserves what was learned and makes the central certificates
inspectable.
