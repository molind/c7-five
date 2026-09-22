# Historical archive

This directory is supporting research material. The supported portable checks
are in `verify/`; this archive is not a second set of turnkey entry points.

- `source.tar.gz`: 660 top-level Python and C++ research scripts, preserving
  versions and attribution comments. Extract into a separate directory if needed.
- `source-manifest.json`: original and exported source hashes.
- `records/`: selected historical JSON records compressed individually with gzip.
  These include the full core covers, branch trees, leaf multipliers and their
  verification reports, plus supporting records reached through their references.
- `records-manifest.json`: filenames, original SHA-256 values, compressed-file
  SHA-256 values and sizes. This is the index for filenames mentioned in the docs.
- `local-inventory.json`: names and sizes of the larger retained local archive.
  An inventory entry does **not** mean that artifact is shipped here.

Example inspection:

```sh
python3 - <<'PY'
import gzip, json
p = 'archive/records/CF-core200-r24-refined-prefix256-verification-sep22.json.gz'
with gzip.open(p, 'rt') as f:
    print(json.dumps(json.load(f), indent=2))
PY
```

## Important portability boundary

Absolute workspace prefixes in historical records are replaced with
`ORIGINAL_WORKSPACE`. Existing dependency digests still refer to the **original**
bytes; export digests in the manifests identify the shipped bytes. They are not
interchangeable. Original records were not rewritten in place. Non-mathematical publication
routing references in archived comments and descriptive metadata are omitted;
source transformations are recorded in `source-manifest.json`.

The old proof chains also reference prepared text graphs, native executables,
JSONL traversal logs and compressed component caches that are not all exported.
Their large originals remain local; most prepared graphs can be regenerated
from the corresponding scripts and pinned inputs. Binary hashes are historical
provenance, not cross-platform reproducible-build claims.

Therefore reading a historical `passed: true` report is evidence of a recorded
run, not a new replay of its entire dependency chain. To extend a large proof
on a different machine, retrieve the required omitted inputs or deliberately
port and re-verify the chain, as done for the maintained portable checkers.

Historical status fields can say `active`, `live`, `timeout` or `open` when later
work superseded them. The current state is the paused snapshot documented in
`docs/results.md` and `docs/continuation.md`.

Private session files, credentials, unrelated projects, virtual
environments and compiled executables are excluded.
