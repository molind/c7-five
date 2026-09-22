# Reproduction

## Supported portable checks

Run from the repository root using Python 3.10 or newer, without `-O` or
`PYTHONOPTIMIZE`. The checkers deliberately use assertions and reject optimized
execution. They read local files only and do not require a network connection or solver.

```sh
make verify
```

This runs U₁, U₂, U₃, family F, and the 315-core overlap proof. Each entry point
can also be run separately. The first three carry their certificate data inline;
the other two read `data/`. The core proof enumerates over five million triples,
so expect it to take longer than the small U₂ replay.

The optional global pair checker uses NumPy's integer arrays:

```sh
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
mkdir -p build
.venv/bin/python verify/global_pair.py \
  --input data/global-pair-368.json --output build/global-pair-verification.json
```

The output must not already exist. Fourier verification uses exact rational
intervals and integer coefficients, not floating-point eigenvalue tolerances.
The approximate minimum eigenvalue printed in the report is descriptive only.

[`results/`](../results/) contains actual outputs from the packaging check.
These logs are evidence of those executions; the scripts and data are what
allow another reader to check the results.

## C++ exchange-search smoke test

The native kernel accepts blocker bitsets and compatibility rows. A candidate
is an outside word; its blockers are the parent words that must be removed to
insert it. The search seeks `r+1` compatible outside words whose combined
blockers have size at most `r`.

```sh
make cpp
python3 - <<'PY'
import hashlib
import subprocess
from pathlib import Path
p = Path('data/family-f-367.txt')
digest = hashlib.sha256(' '.join(p.read_text().split()).encode('ascii')).hexdigest()
with Path('build/family-f-radius2.txt').open('w') as f:
    subprocess.run(['python3', 'search/export_exchange_graph.py', str(p), digest, '2'],
                   stdout=f, check=True)
PY
build/exchange_dfs 5 --subset < build/family-f-radius2.txt
```

The first argument is a time limit in seconds. A `timeout` status is unresolved.
An `exhausted` result has the scope of the given input/radius; it is not a global
upper bound. The exporter's digest is over space-joined words, without a trailing
newline, rather than the raw text file. Its accepted radii are 1–8. It is not the
specialized F9 kernel from the historical computation.

The smoke test checks that the shipped C++ builds and the actual exporter/kernel
pipeline works. It does not repeat the full historical research budget.

## Provenance and verification independence

`provenance.json` records original source hashes, exported hashes and intentional
transformations. U₂ and U₃ are copied verbatim. U₁ adds an explicit `-O` guard.
The global checker removes old discovery-input hash requirements and adds an
explicit `-O` guard; all mathematical checks remain. The overlap checker joins the old certificate chain
into one data file, inlines its arithmetic helpers and verifies the parents.
The family F entry point reconstructs the graph from coordinates.

The portable adaptations were checked again, including deliberately damaged
inputs. They were prepared by the same research agent: do not describe them as
third-party verification. An external execution of a supplied checker is also
different from an independently authored proof checker.

## Historical archive

The portable checkers above are the maintained entry points. The archive
preserves 660 source snapshots and selected full JSON records, including the
larger branch certificates. Its old files contain path-bound dependencies,
native binary hashes and historical status fields. They require an explicit
portability pass before being treated as a fresh, complete proof-chain replay.
See [`archive/README.md`](../archive/README.md).

The original computational environment used Python 3.12, NumPy, SciPy 1.18
with its internal HiGHS 1.12 interface, Z3 and native C++. These are historical
environment notes, not dependencies of `make verify`. Rebuilding a native
binary on another platform need not reproduce its old executable hash.
