# Sources and attribution

## Published starting point

- Sven Polak and Alexander Schrijver,
  [New lower bound on the Shannon capacity of C7 from circular graphs](https://arxiv.org/abs/1808.07438).
  Source of the 367-word construction and the dimension-five bounds 367–401.
  Its smaller-dimensional bounds also supply the cylinder premises used here.
- [arXiv:2607.21517v2](https://arxiv.org/html/2607.21517v2), Itty et al.
  Consulted during the research for later computational constructions. Its
  higher-dimensional C₇ result does not supply a 368-word construction in five
  dimensions. This repository does not claim an exhaustive literature survey.

The finite quantity α(C₇⁵) is not the Shannon capacity itself. A construction in
one power gives a capacity lower bound; it does not determine every finite power.

## Contributor attribution

- **fabius-cunctator:** an equivalent radius-five traversal and detailed replay
  reports. The C++ kernel credits the contributed branch order.
- **bidmart-agent:** reported an independently authored U₁ verifier that parsed
  numeric data and rebuilt the graph without importing the supplied checker.
- **glitchfox:** reported execution of the U₂ and U₃ checkers. These were
  external executions, not independently authored verifiers.

These reports are historical attribution. Current local replay outputs are in
`results/`. Agreement, execution of the same code, and independent mathematical
verification are distinct forms of evidence.

## Authorship, scope and licensing

The investigation and packaging were carried out by an AI coding agent working
with the repository owner. The original 367 construction is credited above;
no literature priority is claimed for the locally named families.

No third-party repository or environment is vendored here. Historical local
scripts retain their attribution comments and may reference external material.
No blanket license has been selected for this initial research snapshot, and
this repository does not grant new rights over cited or third-party work.
