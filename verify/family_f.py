"""Reconstruct family F and its complete fixed-core domain with integers only."""
from itertools import combinations, product
import json
from pathlib import Path


def decode(v):
    return tuple(v // p % 7 for p in (2401, 343, 49, 7, 1))


def encode(word):
    return sum(x * p for x, p in zip(word, (2401, 343, 49, 7, 1)))


def independent(words):
    return all(any((x-y) % 7 in (2, 3, 4, 5) for x, y in zip(decode(a), decode(b)))
               for a, b in combinations(words, 2))


def closed_neighbors(v):
    return (encode(w) for w in product(*[((x-1) % 7, x, (x+1) % 7) for x in decode(v)]))


def verify(path):
    data = json.loads(Path(path).read_text())
    states = data['states']
    assert len(states) == 36
    for state in states:
        assert len(state) == len(set(state)) == 367
        assert all(type(v) is int and 0 <= v < 16807 for v in state)
        assert independent(state)
    known = {frozenset(s) for s in states}
    assert len(known) == 36
    core = set.intersection(*(set(s) for s in states))
    union = set.union(*(set(s) for s in states))
    assert len(core) == 360 and len(union) == 374
    assert core == set(data['core'])
    blocked = set().union(*(set(closed_neighbors(v)) for v in core))
    pool = sorted(set(range(16807)) - blocked)
    assert pool == data['pool'] and len(pool) == 14
    edges = [(i, j) for i, j in combinations(range(14), 2)
             if not independent([pool[i], pool[j]])]
    maxima = set()
    feasible = 0
    for mask in range(1 << 14):
        if any(mask >> i & 1 and mask >> j & 1 for i, j in edges):
            continue
        feasible += 1
        assert mask.bit_count() <= 7
        if mask.bit_count() == 7:
            maxima.add(frozenset(core | {pool[i] for i in range(14) if mask >> i & 1}))
    assert feasible == 1344 and maxima == known
    # Check maximality and EVERY possible one-for-one exchange, including
    # exchanges that might delete a core word. No saved-neighbor list is trusted.
    adjacency = {}
    for frozen in known:
        counts = [0] * 16807
        unique = [-1] * 16807
        for v in frozen:
            for w in closed_neighbors(v):
                counts[w] += 1
                unique[w] = v
        assert min(counts) >= 1
        neighbors = set()
        for w, count in enumerate(counts):
            if count == 1 and w not in frozen:
                neighbor = frozenset((frozen - {unique[w]}) | {w})
                assert neighbor in known
                neighbors.add(neighbor)
        adjacency[frozen] = neighbors
    reached = {next(iter(known))}
    todo = list(reached)
    while todo:
        for neighbor in adjacency[todo.pop()] - reached:
            reached.add(neighbor)
            todo.append(neighbor)
    assert reached == known
    assert sum(map(len, adjacency.values())) == 150
    return dict(passed=True, states=36, witness_pair_checks=36*67161,
                directed_exchanges=150, core=360, union=374, free_domain=14,
                subsets_checked=16384, free_optimum=7, fixed_core_optimum=367,
                scope='The exact fixed core and this one-for-one component; no global bound.')


if __name__ == '__main__':
    if not __debug__:
        raise RuntimeError('Run without -O')
    print(json.dumps(verify(Path(__file__).resolve().parents[1]/'data/family-f.json'), indent=2))
