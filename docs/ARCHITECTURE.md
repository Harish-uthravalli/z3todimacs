# Architecture

The program is a single compilation unit. `src/mainProj.cpp` `#include`s the other two
`.cpp` files, so all code ends up in one translation unit.

## Data flow

```
main()                                   src/mainProj.cpp
 │  build Z3 expr e
 ▼
fun(e)  ── recursive, post-order ──►  Conversion::dimacs(...)     src/convert.cpp
 │   assigns a DIMACS variable            appends Tseitin clauses for one gate
 │   number to every subterm              to contents.txt, counts clauses
 ▼
process(vars, clauses, "harish.qdimacs")  src/final_processing.cpp
     writes "p cnf <vars> <clauses>", then copies contents.txt below it
```

## `src/mainProj.cpp`

### Global state

| Name | Type | Purpose |
|---|---|---|
| `mapping` | `map<unsigned int, int>` | Z3 expression id → DIMACS variable number |
| `var_count` | `int` | set to `mapping.size()`, passed to `dimacs()` |
| `clause_count` | `int` | incremented during traversal but **never used** (the real count is `Conversion::clause_count`) |

### `int fun(z3::expr exp)`

Recursively assigns a variable number to `exp` and emits its clauses. It returns that number.

1. **Already seen?** If `exp.id()` is in `mapping`, return the stored number. Z3
   *hash-conses* expressions: structurally identical subterms are the same object with
   the same id. A shared subterm such as `z` in `(x∨z)∧(y∨z)` therefore gets one
   variable and its clauses are written only once.
2. **New node:** give it the next number. A function-local `static int number = 1` is
   pre-incremented, so numbering **starts at 2**.
3. **Leaf** (`is_const()` and `Z3_OP_UNINTERPRETED`, i.e. a user variable such as `x`):
   return the number. A leaf produces no clauses.
4. **Operator node:** recurse into each argument and collect the child numbers in `id[]`,
   then call `Conversion::dimacs(num, op_name, num_args, id, var_count)`.

Numbers are handed out **pre-order**: a parent is numbered before its children. Clauses are
written **post-order**: a gate's clauses are written after its children's.

The function prints a lot of debug output to stdout, which shows the traversal step by step.

### `int main()`

Builds a hardcoded formula, calls `fun(e)`, then
`process(Conversion::var, Conversion::clause_count, "harish.qdimacs")`, and finally prints the
`mapping` table (Z3 id → DIMACS number).

## `src/convert.cpp`

### `class Conversion`

Static-only helper class.

| Member | Purpose |
|---|---|
| `static int clause_count` | total number of clauses emitted (used in the header) |
| `static int var` | last `variables` value seen (used in the header) |
| `static void dimacs(int t, string oprt, int arg_num, int element[], int variables)` | append clauses for `t ↔ oprt(element...)` to `contents.txt` |

The operator is matched on Z3's declaration name:

| `oprt` | args | Clauses written | Meaning |
|---|---|---|---|
| `"and"` | 2 | `-t a`, `-t b`, `-a -b t` | `t ↔ (a ∧ b)` |
| `"or"` | 2 | `t -a`, `t -b`, `-t a b` | `t ↔ (a ∨ b)` |
| `"=>"` | 2 | `-t -a`, `-t b`, `-b -t a` | intended `t ↔ (a → b)`, **incorrect**, see KNOWN_ISSUES |
| `"not"` | 1 | `-t -a`, `t a` | `t ↔ ¬a` |

With any other operator name or argument count, nothing is written. For a 2- or
1-argument node `clause_count` is still incremented, even though no clauses were written.

## `src/final_processing.cpp`

### `void process(int cvar, int cclause, string fnm)`

1. Opens `contents.txt` for reading and `fnm` for **appending**.
2. Writes the header `p cnf <cvar> <cclause>`.
3. Copies `contents.txt` into the output line by line (up to 255 characters per line).
4. Calls ncurses `getch()` as a "press a key" pause. `initscr()` is never called, so this
   probably returns straight away. ncurses is only a dependency because of this call.

## Design notes

- **`#include`-ing `.cpp` files** avoids a build system but doesn't scale. The usual
  structure is `.h` declarations plus separately compiled `.cpp` files.
- **The intermediate file:** clauses go to `contents.txt` first because the DIMACS header
  (which holds the counts) must come first, and the counts are only known after the
  traversal. Keeping clauses in a `std::vector<std::string>` in memory would remove the
  temp file.
