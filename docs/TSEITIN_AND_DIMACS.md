# Background: CNF, DIMACS and the Tseitin transformation

## CNF

A formula is in **Conjunctive Normal Form** when it is an AND of *clauses*, and each clause
is an OR of *literals* (a variable or its negation):

```
(a ∨ ¬b) ∧ (¬a ∨ c ∨ d) ∧ (b)
```

SAT solvers take CNF as input.

## DIMACS CNF format

```
c optional comment lines start with 'c'
p cnf <num_variables> <num_clauses>
1 -2 0
-1 3 4 0
2 0
```

- Variables are positive integers `1..num_variables`. A negative number is a negated literal.
- Each clause ends with `0`.
- `num_variables` must be at least the largest variable number used.

**QDIMACS** extends DIMACS with quantifier lines (`e 1 2 0`, `a 3 0`) for quantified Boolean
formulas in prenex normal form (PNF). This project writes a `.qdimacs` file but no quantifier
lines, so the output is plain DIMACS CNF.

## Why not just distribute?

Converting to CNF with the distributive law can blow up exponentially. For example,
`(a1∧b1) ∨ (a2∧b2) ∨ … ∨ (an∧bn)` becomes 2ⁿ clauses.

## The Tseitin transformation

Introduce a **fresh variable for every subformula** and add clauses stating that the variable
is *equivalent* to its subformula. Each gate needs only a constant number of clauses, so the
output is **linear** in the size of the formula.

| Gate | Definition | Clauses |
|---|---|---|
| AND | `t ↔ (a ∧ b)` | `(¬t ∨ a)`, `(¬t ∨ b)`, `(¬a ∨ ¬b ∨ t)` |
| OR | `t ↔ (a ∨ b)` | `(t ∨ ¬a)`, `(t ∨ ¬b)`, `(¬t ∨ a ∨ b)` |
| NOT | `t ↔ ¬a` | `(¬t ∨ ¬a)`, `(t ∨ a)` |
| IMPLIES | `t ↔ (a → b)` | `(t ∨ a)`, `(t ∨ ¬b)`, `(¬t ∨ ¬a ∨ b)` |
| XOR | `t ↔ (a ⊕ b)` | `(¬t ∨ a ∨ b)`, `(¬t ∨ ¬a ∨ ¬b)`, `(t ∨ ¬a ∨ b)`, `(t ∨ a ∨ ¬b)` |
| IFF | `t ↔ (a ↔ b)` | `(¬t ∨ ¬a ∨ b)`, `(¬t ∨ a ∨ ¬b)`, `(t ∨ a ∨ b)`, `(t ∨ ¬a ∨ ¬b)` |

Finally, **assert the root**: add the unit clause `(r)`, where `r` is the root formula's
variable.

The result is **equisatisfiable** with the original formula, not logically equivalent: it is
satisfiable exactly when the original is, and any model restricted to the original variables
satisfies the original formula.

## Worked example (the formula in `main()`)

Formula: `(x ∨ z) ∧ (y ∨ z)`

Variable numbers assigned by `fun()` (pre-order, starting at 2):

| Var | Subterm |
|---|---|
| 2 | `(x ∨ z) ∧ (y ∨ z)` (root) |
| 3 | `x ∨ z` |
| 4 | `x` |
| 5 | `z` (shared by both ORs) |
| 6 | `y ∨ z` |
| 7 | `y` |

Clauses emitted (see `examples/harish.qdimacs`):

```
p cnf 6 9
3 -4 0        ┐
3 -5 0        │ 3 ↔ (4 ∨ 5)
-3 4 5 0      ┘
6 -7 0        ┐
6 -5 0        │ 6 ↔ (7 ∨ 5)
-6 7 5 0      ┘
-2 3 0        ┐
-2 6 0        │ 2 ↔ (3 ∧ 6)
-3 -6 2 0     ┘
```

A fully correct output would have `p cnf 7 10` and an extra line `2 0` asserting the root.
See [KNOWN_ISSUES.md](KNOWN_ISSUES.md).

## Checking the output

Run any DIMACS solver on the file, for example:

```sh
minisat harish.qdimacs result.txt
```
