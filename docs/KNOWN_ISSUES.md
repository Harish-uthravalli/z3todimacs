# Known issues

Ordered by severity. None of these has been fixed yet.

## 1. The root formula is never asserted (critical)

**Where:** `src/mainProj.cpp`, `main()`

The Tseitin clauses define `root ↔ formula` but never require `root` to be true. The CNF can
always be satisfied by setting `root = false`, so every formula comes out **SAT**, even
`x ∧ ¬x`.

**Fix:** after `int root = fun(e);`, append the unit clause `root 0` to the clause file and
increment `Conversion::clause_count`.

## 2. The IMPLIES encoding is wrong

**Where:** `src/convert.cpp`, the `"=>"` branch

Current clauses: `(¬t ∨ ¬a)`, `(¬t ∨ b)`, `(¬t ∨ a ∨ ¬b)`. These together force `t = false`.

**Fix:** for `t ↔ (a → b)` write

```
t a 0
t -b 0
-t -a b 0
```

## 3. The variable count in the header is off by one

**Where:** `src/mainProj.cpp`: `static int number = 1` with `++number`, and
`var_count = mapping.size()`

Numbering starts at 2, so the largest variable is `mapping.size() + 1`, but the header uses
`mapping.size()`. The sample output says `p cnf 6 9` but uses variable 7. Strict solvers
reject this.

**Fix:** start at `static int number = 0`, so numbering starts at 1.

## 4. Output files are appended to, never truncated

**Where:** `src/convert.cpp` (`ios::app` on `contents.txt`) and `src/final_processing.cpp`
(`ios::app` on the output file)

Re-running adds clauses from earlier runs, and adds a second `p cnf` header to
`harish.qdimacs`. `myfile.clear()` only clears stream error flags; it does not empty the file.

**Fix:** truncate both files once at program start, or keep the clauses in memory and write
the output once.

## 5. Limited operator support

**Where:** `src/convert.cpp`

- Only `and`, `or`, `not`, `=>` are handled. `xor`, `=` (iff), `ite`, `true` and `false`
  are silently ignored, which produces a wrong CNF. See the table in
  [TSEITIN_AND_DIMACS.md](TSEITIN_AND_DIMACS.md) for their encodings.
- Z3 often builds **n-ary** `and`/`or` (e.g. `a && b && c` can become one 3-argument `and`).
  Those nodes emit nothing. The n-ary encoding for `t ↔ (a1 ∧ … ∧ an)` is
  `(¬t ∨ ai)` for each i, plus `(¬a1 ∨ … ∨ ¬an ∨ t)`.
- `int id[50]` in `fun()` overflows for nodes with more than 50 arguments. Use
  `std::vector<int>`.

## 6. The clause count can disagree with the clauses written

**Where:** `src/convert.cpp`

`clause_count` is incremented based on the argument count, before checking that the
operator is supported. An unsupported 2-argument operator adds 3 to the count but writes
no clauses.

## 7. Minor / cleanup

- The global `clause_count` in `mainProj.cpp` is updated but never used.
- The formula is hardcoded in `main()`. It could instead be read from an SMT-LIB2 file with
  `c.parse_file(...)`.
- `.qdimacs` extension and "PNF" wording: the output has no quantifiers, so it is plain
  DIMACS (`.cnf`).
- `getch()` is called without `initscr()`, and the ncurses dependency exists only for that
  call. `std::cin.get()` does the same job without ncurses and would work on Windows and
  macOS.
- `.cpp` files are `#include`d instead of using headers.
- `a.out` (a build artifact) is committed. Add a `.gitignore` and remove it from the repo.
- `fun()` prints a lot of debug output on every call. Put it behind a verbose flag.
