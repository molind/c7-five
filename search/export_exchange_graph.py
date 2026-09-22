"""Export the pinned 367-word C7^5 input to exchange_dfs.cpp's text format."""
import hashlib
from itertools import product
from pathlib import Path
import sys

if len(sys.argv) not in (3, 4):
    raise SystemExit('Usage: export_exchange_graph.py WORDS_FILE SHA256 [RADIUS]')
radius = int(sys.argv[3]) if len(sys.argv) == 4 else 5
if not 1 <= radius <= 8:
    raise SystemExit('Radius must be between 1 and 8')
tokens = Path(sys.argv[1]).read_text().split()
assert hashlib.sha256(' '.join(tokens).encode('ascii')).hexdigest() == sys.argv[2]
assert len(tokens) == len(set(tokens)) == 367
assert all(len(t) == 5 and set(t) <= set('0123456') for t in tokens)
S = [tuple(map(int, t)) for t in tokens]
index = {v: i for i, v in enumerate(S)}


def neighbors(v):
    return product(*[((x-1) % 7, x, (x+1) % 7) for x in v])


assert all(w == v or w not in index for v in S for w in neighbors(v))
outside = [v for v in product(range(7), repeat=5) if v not in index]
blockers = {v: sum(1 << index[w] for w in neighbors(v) if w in index) for v in outside}
pool = sorted((v for v in outside if blockers[v].bit_count() <= radius),
              key=lambda v: blockers[v].bit_count())
pi = {v: i for i, v in enumerate(pool)}
n = len(pool)
cw = (n + 63) // 64
full = (1 << n) - 1
mask = (1 << 64) - 1
print(n, 6, radius)
for v in pool:
    compatible = full ^ sum(1 << pi[w] for w in neighbors(v) if w in pi)
    values = [(blockers[v] >> (64*j)) & mask for j in range(6)]
    values += [(compatible >> (64*j)) & mask for j in range(cw)]
    print(' '.join(f'{x:x}' for x in values))
