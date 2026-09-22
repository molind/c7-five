"""Independently check the rational global target368 pair-relaxation witness.

No LP/model generator is imported. Enumerate all words, all anchored pair
placements modulo the first-word symmetry, and every translated cylinder.
Fourier positivity is checked with exact Q(t) interval arithmetic.
"""
import argparse
import hashlib
from fractions import Fraction as Q
from itertools import product
import json
import math
from pathlib import Path
import numpy as np


def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()

def verify(path):
    d=json.loads(Path(path).read_text());assert d['dimension']==5 and d['target']==368
    profiles=[tuple(p) for p in d['profiles']];values=[Q(x) for x in d['values']]
    assert len(profiles)==len(values)==56 and len(set(profiles))==56
    mapping=dict(zip(profiles,values));D=math.lcm(*(x.denominator for x in values))
    coords=np.array(list(product(range(7),repeat=5)),dtype=np.int64)
    dist=np.minimum(coords,7-coords)
    signatures=[tuple((r==i).sum() for i in range(4)) for r in dist]
    assert set(signatures)==set(profiles)
    nums=[int(mapping[p]*D) for p in signatures]
    assert nums[0]==D and sum(nums)==368*D
    for c,num in zip(dist,nums):
        assert 0<=num<=D
        if np.any(c) and np.max(c)<=1:assert num==0
    assert max(abs(v) for v in nums)*len(nums)<2**63
    arr=np.array(nums,dtype=np.int64);grid=arr.reshape((7,)*5)
    cylinders=0
    for k in range(1,6):
        plane=grid.sum(axis=tuple(range(k,5))) if k<5 else grid
        total=np.zeros_like(plane)
        for shift in product((0,1),repeat=k):total+=np.roll(plane,shift,axis=tuple(range(k)))
        assert int(total.max())<=[1,3,10,33,115][5-k]*D
        cylinders+=int(total.size)
    pairs=0
    for p in profiles:
        a=np.repeat(np.arange(4),p)
        delta=(a-coords)%7;delta=np.minimum(delta,7-delta)
        fc=np.array([int(mapping[tuple((row==i).sum() for i in range(4))]*D) for row in delta],dtype=np.int64)
        assert np.all(int(mapping[p]*D)+arr-fc<=D)
        pairs+=len(coords)
    lo,hi=map(Q,d['root_interval']);poly=lambda z:z**3+z**2-2*z-1
    assert 1<lo<hi<2 and poly(lo)<0<poly(hi)
    # The derivative3t^2+2t-2 is positive on[1,2], so this isolates the root.
    lower_bounds=[]
    for p,record in zip(profiles,d['eigenvalues']):
        freq=np.repeat(np.arange(4),p);phase=(coords@freq)%7
        sums=[sum(v for v,q in zip(nums,phase) if int(q)==j) for j in range(7)]
        assert sums[1]==sums[6] and sums[2]==sums[5] and sums[3]==sums[4]
        a,b,c=sums[0]-2*sums[2]+sums[3],sums[1]-sums[3],sums[2]-sums[3]
        assert [Q(a,D),Q(b,D),Q(c,D)]==[Q(x) for x in record['coefficients']]
        low=(a+b*(lo if b>=0 else hi)+c*(lo*lo if c>=0 else hi*hi))/D
        assert low==Q(record['lower']) and low>0
        lower_bounds.append(low)
    assert len(lower_bounds)==56
    return dict(passed=True,exact_objective=368,word_values=16807,anchored_pair_checks=pairs,translated_cylinder_checks=cylinders,spectral_classes=56,covered_characters=16807,minimum_eigenvalue_lower=str(min(lower_bounds)),minimum_eigenvalue_lower_approx=float(min(lower_bounds)),scope='These pair, Fourier, triangle and cylinder constraints admit target368. This is not an independent368-word set and proves no lower bound on alpha.')


def main():
    if not __debug__:
        raise RuntimeError('Run without -O')
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--input',type=Path,required=True);p.add_argument('--output',type=Path,required=True);args=p.parse_args();assert not args.output.exists()
    out=verify(args.input);out.update(checker_sha256=sha(__file__),dependencies={str(args.input):sha(args.input)})
    args.output.write_text(json.dumps(out,indent=2)+'\n');print(out)


if __name__=='__main__':main()
