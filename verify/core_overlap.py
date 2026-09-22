"""Exact replay of all 5,159,805 omissions of three words from one fixed core.

Ported from check_common312_covers.py. The archive hashes are in provenance.json;
this entry point verifies mathematical data directly, without the old filesystem.
"""
import argparse
import gzip
from itertools import combinations, product
import json
from pathlib import Path

def decode(v):
    return tuple(v // 7**i % 7 for i in range(4, -1, -1))

def encode(w):
    return sum(v * 7**(4-i) for i, v in enumerate(w))

def box_words(origin):
    assert type(origin) is int and 0<=origin<16807
    return {encode(w) for w in product(*[(x,(x+1)%7) for x in decode(origin)])}

def verify_cover(pool,cover,denominator):
    coverage={v:0 for v in pool};total=0
    for c in cover:
        units=c['units'];assert type(units) is int and units>0
        for v in box_words(c['origin'])&pool:coverage[v]+=units
        total+=units
    assert min(coverage.values())>=denominator
    return total

def verify(path):
    with gzip.open(path, 'rt') as f:
        bundle = json.load(f)
    d, pair, single = (bundle[k] for k in ('triples','pairs','singles'))
    base = dict(fixed=bundle['core'], witness=bundle['witness'])
    parents = bundle['parents']
    assert d['status']=='complete' and d['open_domains']==[]
    assert len(parents)==2
    for words in parents:
        assert len(words)==len(set(words))==367
        assert all(type(v) is int and 0<=v<16807 for v in words)
        assert all(any((x-y)%7 in (2,3,4,5) for x,y in zip(decode(a),decode(b)))
                   for a,b in combinations(words,2))
    core=sorted(set(parents[0]) & set(parents[1]))
    assert core==sorted(base['fixed']) and len(core)==315
    blockers=[set() for _ in range(16807)]
    for v in core:
        for w in product(*[((x-1)%7,x,(x+1)%7) for x in decode(v)]):
            blockers[encode(w)].add(v)
    buckets={}
    for v,bk in enumerate(blockers):
        if len(bk)<=3:buckets.setdefault(tuple(sorted(bk)),set()).add(v)
    D=d['denominator'];assert type(D) is int and D==pair['denominator']==single['denominator']>0
    empty=buckets[()]
    single_totals={}
    assert [r['omitted'] for r in single['rows']]==core
    for r in single['rows']:
        i=r['omitted'];pool=empty|buckets[(i,)]
        assert pool==set(r['pool']) and len(pool)==len(r['pool'])
        total=verify_cover(pool,r['cover'],D);assert total==r['total_units']<54*D
        single_totals[i]=total
    pair_totals={}
    assert [tuple(r['omitted']) for r in pair['rows']]==list(combinations(core,2))
    for r in pair['rows']:
        i,j=r['omitted'];joint=buckets.get((i,j),set())
        if r['kind']=='extension':
            parent=r['parent_omission'];assert parent in (i,j)
            new=j if parent==i else i
            assert buckets[(new,)]|joint<=box_words(r['extra_box_origin'])
            total=single_totals[parent]+D
        else:
            assert r['kind']=='lp';pool=empty|buckets[(i,)]|buckets[(j,)]|joint
            assert pool==set(r['pool']) and len(pool)==len(r['pool'])
            total=verify_cover(pool,r['cover'],D)
        assert total==r['total_units']<55*D
        pair_totals[(i,j)]=total
    assert [r['word'] for r in d['singleton_boxes']]==core
    good=set()
    for r in d['singleton_boxes']:
        if r['origin'] is not None:
            assert buckets[(r['word'],)]<=box_words(r['origin'])
            good.add(r['word'])
    neighbors={i:set() for i in core}
    for key in buckets:
        if len(key)==2:
            i,j=key;neighbors[i].add(j);neighbors[j].add(i)
    hard=[];implicit=cases=0
    for i,j,k in combinations(core,3):
        cases+=1
        singleton_extension=(i in good and j not in neighbors[i] and k not in neighbors[i]) or \
            (j in good and i not in neighbors[j] and k not in neighbors[j]) or \
            (k in good and i not in neighbors[k] and j not in neighbors[k])
        if singleton_extension and (i,j,k) not in buckets:implicit+=1
        else:hard.append((i,j,k))
    assert cases==d['total_cases'] and implicit==d['implicit_cases']
    assert [tuple(r['omitted']) for r in d['rows']]==hard
    maximum=max(pair_totals.values())+D;extensions=lps=0;opened=[]
    for r in d['rows']:
        triple=tuple(r['omitted'])
        if r['kind']=='extension':
            parent=tuple(r['parent_omission'])
            assert parent in list(combinations(triple,2))
            new=(set(triple)-set(parent)).pop()
            admitted=set().union(*(buckets.get(key,set()) for size in range(1,4)
                for key in combinations(triple,size) if new in key))
            assert admitted<=box_words(r['extra_box_origin'])
            total=pair_totals[parent]+D;extensions+=1
        else:
            assert r['kind']=='lp'
            pool=set().union(*(buckets.get(key,set()) for size in range(4)
                for key in combinations(triple,size)))
            assert pool==set(r['pool']) and len(pool)==len(r['pool'])
            total=verify_cover(pool,r['cover'],D);lps+=1
        assert total==r['total_units']
        maximum=max(maximum,total)
        if total>=56*D:opened.append(triple)
    assert opened==[tuple(r['omitted']) for r in d['open_domains']]
    witness=base['witness'];assert len(witness)==len(set(witness))==367 and set(core)<=set(witness)
    assert all(type(v) is int and 0<=v<16807 for v in witness)
    pair_checks=0
    for a,b in combinations(witness,2):
        assert any((x-y)%7 in (2,3,4,5) for x,y in zip(decode(a),decode(b)))
        pair_checks+=1
    return dict(passed=True, cases=cases, implicit=implicit, explicit_extensions=extensions,
                lp_covers=lps, maximum_total_units=maximum, denominator=D,
                witness_pair_checks=pair_checks, minimum_core_omissions_for368=4)

if __name__=='__main__':
    if not __debug__: raise RuntimeError('Run without -O')
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input',type=Path,default=Path(__file__).resolve().parents[1]/'data/core-overlap.json.gz')
    args=parser.parse_args()
    print(json.dumps(verify(args.input),indent=2))
