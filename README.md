# z3todimacs

Converts a Boolean formula built with the [Z3](https://github.com/Z3Prover/z3) C++ API into
**CNF in DIMACS format**, the plain-text input format understood by SAT solvers
(MiniSat, CaDiCaL, Glucose, ...). The conversion uses the **Tseitin transformation**, so
the output grows linearly with the size of the formula.

```
Z3 expression  ──►  recursive traversal  ──►  Tseitin clauses  ──►  DIMACS file
(x ∨ z) ∧ (y ∨ z)    (number each subterm)    (contents.txt)       (harish.qdimacs)
```

> **Status:** prototype / first internship project. Supports AND, OR, NOT and IMPLIES.
> There are known correctness issues. Read [docs/KNOWN_ISSUES.md](docs/KNOWN_ISSUES.md)
> before relying on the output.

## Repository layout

```
z3todimacs/
├── src/
│   ├── mainProj.cpp          # entry point: builds the formula, traverses it, numbers variables
│   ├── convert.cpp           # Conversion::dimacs(): emits Tseitin clauses per gate
│   └── final_processing.cpp  # process(): writes the "p cnf" header + clauses to the output file
├── examples/
│   ├── contents.txt          # intermediate clause list from a sample run
│   └── harish.qdimacs        # final DIMACS output from a sample run
├── docs/
│   ├── ARCHITECTURE.md       # how the code works, function by function
│   ├── TSEITIN_AND_DIMACS.md # background: CNF, DIMACS format, Tseitin encoding, worked example
│   └── KNOWN_ISSUES.md       # bugs and limitations, with suggested fixes
└── a.out                     # prebuilt Linux binary (build artifact)
```

## Requirements

- Linux (the code uses ncurses)
- `g++`
- Z3 with its C++ headers (`z3++.h`), e.g. `sudo apt install libz3-dev`
- ncurses, e.g. `sudo apt install libncurses-dev`

## Build and run

```sh
g++ src/mainProj.cpp -o z3todimacs -lz3 -lncurses
./z3todimacs
```

Only `mainProj.cpp` is compiled. It pulls in the other two source files with `#include`.

The program writes two files to the **current working directory**:

| File | Contents |
|---|---|
| `contents.txt` | clauses only (intermediate) |
| `harish.qdimacs` | `p cnf <vars> <clauses>` header followed by the clauses |

Both are opened in append mode, so **delete them before re-running**. Otherwise the output
from earlier runs piles up.

## Changing the input formula

The formula is hardcoded in `main()` in [src/mainProj.cpp](src/mainProj.cpp):

```cpp
expr x = c.bool_const("x");
expr y = c.bool_const("y");
expr z = c.bool_const("z");
expr e = (( x || z ) && ( y || z ));
```

Edit this expression and rebuild. Use only `&&`, `||`, `!` and `implies(a, b)`, with at most
two operands per operator (see [KNOWN_ISSUES.md](docs/KNOWN_ISSUES.md)).

## Further reading

- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md): code walkthrough
- [docs/TSEITIN_AND_DIMACS.md](docs/TSEITIN_AND_DIMACS.md): the theory behind the conversion
- [docs/KNOWN_ISSUES.md](docs/KNOWN_ISSUES.md): what's broken and how to fix it
