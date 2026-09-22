"""Deliberately damage portable certificates and require rejection.

Uses temporary data files only; no original certificate is changed.
Run with the same NumPy environment as the global pair checker.
"""
from copy import deepcopy
import gzip
import json
from pathlib import Path
import tempfile

import core_overlap
import family_f
import global_pair


def run():
    root = Path(__file__).resolve().parents[1]
    with gzip.open(root/'data/core-overlap.json.gz', 'rt') as f:
        core = json.load(f)
    family = json.loads((root/'data/family-f.json').read_text())
    pair = json.loads((root/'data/global-pair-368.json').read_text())
    passed = []

    def reject(name, value, change, verifier, compressed=False):
        bad = deepcopy(value)
        change(bad)
        with tempfile.TemporaryDirectory(prefix='c7-control-') as tmp:
            path = Path(tmp)/('input.json.gz' if compressed else 'input.json')
            raw = json.dumps(bad).encode()
            path.write_bytes(gzip.compress(raw) if compressed else raw)
            try:
                verifier(path)
            except AssertionError:
                passed.append(name)
                return
        raise AssertionError('Damaged certificate accepted: '+name)

    reject('core_zero_cover_weight', core,
           lambda d: d['singles']['rows'][0]['cover'][0].update(units=0), core_overlap.verify, True)
    reject('core_missing_pair', core,
           lambda d: d['pairs']['rows'].pop(), core_overlap.verify, True)
    reject('core_missing_triple', core,
           lambda d: d['triples']['rows'].pop(), core_overlap.verify, True)
    reject('family_missing_state', family,
           lambda d: d['states'].pop(), family_f.verify)
    reject('family_out_of_range_word', family,
           lambda d: d['states'][0].__setitem__(0, 16807), family_f.verify)
    reject('pair_wrong_target', pair,
           lambda d: d.update(target=369), global_pair.verify)
    reject('pair_wrong_mass', pair,
           lambda d: d['values'].__setitem__(0, '0'), global_pair.verify)
    return dict(passed=True, controls=passed)


if __name__ == '__main__':
    if not __debug__:
        raise RuntimeError('Run without -O')
    print(json.dumps(run(), indent=2))
