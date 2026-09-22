# Problem and proof ideas

## Words, conflicts and the target

Let `V = {0,…,6}^5`. Distinct vertices `x,y` are adjacent when
`(xi − yi) mod 7 ∈ {0,1,6}` for every coordinate. This is the strong graph power
`C₇ ⊠ C₇ ⊠ C₇ ⊠ C₇ ⊠ C₇`.

An independent set is a collection of mutually distinguishable words. For
example, `00000` conflicts with `16601`, but not with `20000`: the first
coordinate of the latter pair separates them.

We encode a word as

```
id = 2401*x0 + 343*x1 + 49*x2 + 7*x3 + x4
```

The target 368 is the first integer above the known construction of size 367.
It is a question, not an assumption that such a construction exists. A larger
independent set would contain a 368-word subset, so excluding 368 globally
would establish the exact independence number 367.

## Why boxes prove upper bounds

For an origin `a`, form the 32-word box

```
B(a) = {a0,a0+1} × … × {a4,a4+1}       (all coordinates modulo seven).
```

Every pair in a box conflicts. An independent set therefore uses at most one
word from each box. Give boxes nonnegative weights so that every word in a
domain receives total weight at least one. Summing those weights bounds the
size of every independent set in that domain.

In the U₂ certificate, integer millionths give total weight
`367,999,845 / 1,000,000 < 368`. This implies an integer upper bound of 367.
A directly checked 367-word set in the same domain gives equality.

## What U₁, U₂ and U₃ mean

Fix the exact 308-word core embedded in the standalone checkers. Count, for
each word, how many core words lie in its closed conflict neighborhood. A
core word counts itself. Define Uₖ to contain the words with at most `k` such
blockers. These are particular induced subgraphs of the 16,807-word graph.

The U₂ result does not force a hypothetical 368-word set to lie in U₂.
Extending the same method to U₃ meets a provable obstacle: the shipped dual
weights total `23,696/64 = 370.25`, while every box has load at most one.
Every clique in this graph is contained in a box, so even arbitrary fractional
clique covers cannot get below 368 on U₃. This says something about the proof
method; it does not produce an independent set of size 368.

## Core omissions and exchanges

Retaining a specified independent core excludes all words that conflict with
it. The remaining compatible universe can be reconstructed exactly. A cover
of that whole universe gives a conditional upper bound on any completion.

For the aligned C/F parents in `data/core-overlap.json.gz`, the intersection
contains 315 words. Checking every three-word omission proves that retaining
any 312 of them cannot reach 368. Consequently every 368-word independent set
must omit at least four of these particular 315 words. It may omit many more.

For a parent `S`, a deletion radius `r` restricts a candidate to
`|S \ T| ≤ r`. Closing such a region does not close regions farther away or
those omitting one of its fixed core words.

## Global pair relaxation

For an independent set `S`, its averaged autocorrelation is
`f(d) = |S ∩ (S−d)| / |S|`. It has `f(0)=1`, nonnegative values, zero on
nonzero conflict differences, nonnegative Fourier transform, and
`sum_d f(d)=|S|`. Boolean indicators also imply
`f(a)+f(b)−f(a−b) ≤ 1`. Bounds for cylinders constrain sums over products of
adjacent coordinate pairs and unrestricted coordinates.

Coordinate permutations and reflections reduce the symmetric description to
56 distance profiles. The shipped rational point satisfies these constraints
with sum exactly 368. The verifier checks every anchored pair placement
modulo first-word symmetry, every translated cylinder in each representative
coordinate set, and all 56 Fourier classes. Fourier coefficients are checked
exactly in `Q(t)`, where `t=2 cos(2π/7)` satisfies `t³+t²−2t−1=0`.

Cylinder ranks `[1,3,10,33,115]` use the smaller-dimensional bounds in the
Polak–Schrijver paper. The checker treats those values as stated mathematical
premises; it does not re-prove that paper's results. A feasible pair function
need not come from a set. The obstruction is specific to these implemented
constraints, not to every possible global method.
