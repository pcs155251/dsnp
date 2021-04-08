# FRAIG(Functionally Reduced And-Inverter Graph
## Disclaimer
This is final project of NTU dsnp course instructed by Prof. Chung-Yang (Ric) Huang. The code is based on Ric's architecture.

## Introdcution
FRAIG is a tool to simplify an And-Inverter Graph. It reduced the gate count by:
* Unused gate sweeping
* Trivial optimization
* Simplification by structural hash
* Equivalence gate merging

## Commands
* Sweep: Remove unsed gates
* Optimize: Trivial optimization
* Strash: Structural hash
* Simulate: Input binary patterns to simulate the circuit and detect FEC groups
* Fraig: Use SAT solver to prove equivalene of gate pairs
