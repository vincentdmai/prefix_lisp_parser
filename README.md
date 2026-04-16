# prefix_lisp_parser

A prefix-notation Lisp interpreter with support for arithmetic, logic, math functions, and 2D graphics drawing — implemented in C++11 with a Qt5 GUI frontend.

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Dependencies](#dependencies)
- [Building](#building)
- [Programs](#programs)
  - [vtscript — Command-line interpreter](#vtscript--command-line-interpreter)
  - [vtdraw — GUI drawing application](#vtdraw--gui-drawing-application)
- [Language Reference](#language-reference)
  - [Literals](#literals)
  - [Special Forms](#special-forms)
  - [Arithmetic Operators](#arithmetic-operators)
  - [Logical Operators](#logical-operators)
  - [Math Functions](#math-functions)
  - [Graphics Primitives](#graphics-primitives)
  - [Draw Command](#draw-command)
- [Testing](#testing)

---

## Overview

`prefix_lisp_parser` is a small Lisp-style expression evaluator that uses **prefix notation** — operators come before their operands, and every expression is wrapped in parentheses.

```
(+ 1 2)          ; evaluates to 3
(define x 42)    ; binds x to 42
(* x 2)          ; evaluates to 84
```

The project provides two executables:
- **`vtscript`** — a command-line interpreter with REPL, file evaluation, and single-expression evaluation modes.
- **`vtdraw`** — a Qt5 GUI application that evaluates expressions and renders 2D graphics (points, lines, arcs).

---

## Architecture

The interpreter pipeline consists of the following layers:

| Component | Files | Responsibility |
|-----------|-------|----------------|
| Tokenizer | `tokenize.hpp / .cpp` | Splits raw input text into a token stream, stripping whitespace and comments |
| Expression / AST | `expression.hpp / .cpp` | Defines the `Expression` tree (head atom + tail children), type system, evaluation, and output formatting |
| Environment | `environment.hpp / .cpp` | Maintains a symbol table mapping names to values or built-in procedures; initialises all built-ins on construction |
| Interpreter | `interpreter.hpp / .cpp` | Orchestrates parsing (token stream → AST) and evaluation (AST + Environment → result); also collects graphics atoms |
| Qt Interpreter | `qt_interpreter.hpp / .cpp` | Wraps `Interpreter` as a `QObject`; emits Qt signals for results, errors, and drawable graphics items |
| GUI Widgets | `main_window`, `canvas_widget`, `repl_widget`, `message_widget`, `qgraphics_arc_item` | Qt5 window, drawing canvas, REPL input bar, status message area, and custom arc graphics item |

---

## Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| C++ compiler | C++11 or later | Language standard |
| CMake | ≥ 3.5 | Build system |
| Qt5 | any recent 5.x | GUI (Widgets, Core, Test modules) |
| Catch | included (`catch.hpp`) | Unit test framework |

---

## Building

```bash
mkdir build
cd build
cmake ..
make
```

This produces the following targets in the build directory:

| Target | Description |
|--------|-------------|
| `vtscript` | Command-line interpreter |
| `vtdraw` | Qt5 GUI drawing application |
| `unittests` | Interpreter unit test suite |
| `test_gui` | Qt5 GUI integration tests |
| `test_message` | Message widget tests |

### Optional build flags

| Flag | Effect |
|------|--------|
| `-DCOVERAGE=TRUE` | Enable GCC code coverage instrumentation (Linux/GCC only) |
| `-DMEMORY=TRUE` | Enable Valgrind memory check target (`make memtest`) |
| `-DTIDY=TRUE` | Enable clang-tidy static analysis target (`make tidy`) |

---

## Programs

### vtscript — Command-line interpreter

`vtscript` runs the interpreter in one of three modes:

#### REPL mode (no arguments)

```bash
./vtscript
```

Starts an interactive read-eval-print loop. Type expressions and press Enter to evaluate them.

```
vtscript> (+ 1 2)
(3)
vtscript> (define x 10)
(10)
vtscript> (* x 3)
(30)
```

Use `Ctrl-D` (EOF) to exit.

#### Evaluate a single expression (`-e`)

```bash
./vtscript -e "(+ 1 2)"
```

Prints the result to stdout and exits. Returns exit code `0` on success, `1` on error.

#### Evaluate a script file

```bash
./vtscript path/to/script.vts
```

Reads the file, parses and evaluates its single top-level expression, and prints the result.

---

### vtdraw — GUI drawing application

```bash
./vtdraw [filename]
```

Opens an 800×600 Qt5 window. The REPL input bar at the bottom accepts expressions. Results appear in the message area; graphics (points, lines, arcs) are rendered on the canvas.

An optional script filename can be provided to pre-load and evaluate a drawing on startup.

---

## Language Reference

### Literals

| Syntax | Type | Example |
|--------|------|---------|
| Integer or decimal number | Number | `42`, `3.14`, `-1.5e2` |
| `True` / `False` | Boolean | `True` |
| `pi` | Number (≈ 3.14159…) | `(* 2 pi)` |

### Special Forms

These keywords have special evaluation rules and cannot be redefined.

#### `define`

Binds a symbol to a value for use in subsequent expressions.

```
(define <symbol> <expression>)
```

```
(define radius 5)
(* 2 pi radius)     ; => (31.4159...)
```

#### `begin`

Evaluates a sequence of expressions in order; returns the value of the last one.

```
(begin <expr1> <expr2> ... <exprN>)
```

```
(begin (define a 1) (define b 2) (+ a b))   ; => (3)
```

#### `if`

Conditional evaluation.

```
(if <condition> <then-expr> <else-expr>)
```

```
(if (< 1 2) True False)   ; => (True)
```

#### `draw`

Renders one or more graphics primitives on the canvas (vtdraw only).

```
(draw <graphic-expr> ...)
```

```
(draw (point 0 0) (line (point 0 0) (point 10 10)))
```

---

### Arithmetic Operators

All arithmetic operators work on `Number` types.

| Operator | Arity | Description | Example |
|----------|-------|-------------|---------|
| `+` | M-ary (≥ 1) | Sum | `(+ 1 2 3)` → `(6)` |
| `-` | Unary or Binary | Negate or subtract | `(- 5 3)` → `(2)`, `(- 3)` → `(-3)` |
| `*` | M-ary (≥ 1) | Product | `(* 2 3 4)` → `(24)` |
| `/` | Binary | Division | `(/ 10 2)` → `(5)` |

---

### Logical Operators

All logical operators work on `Boolean` types unless noted.

| Operator | Arity | Description | Example |
|----------|-------|-------------|---------|
| `not` | Unary | Boolean negation | `(not True)` → `(False)` |
| `and` | M-ary | Logical AND | `(and True False)` → `(False)` |
| `or` | M-ary | Logical OR | `(or True False)` → `(True)` |
| `<` | Binary | Less than | `(< 1 2)` → `(True)` |
| `<=` | Binary | Less than or equal | `(<= 2 2)` → `(True)` |
| `>` | Binary | Greater than | `(> 3 1)` → `(True)` |
| `>=` | Binary | Greater than or equal | `(>= 3 3)` → `(True)` |
| `=` | Binary | Equal | `(= 4 4)` → `(True)` |

---

### Math Functions

| Function | Arity | Description | Example |
|----------|-------|-------------|---------|
| `log10` | Unary | Base-10 logarithm | `(log10 100)` → `(2)` |
| `pow` | Binary | Exponentiation (base, exp) | `(pow 2 10)` → `(1024)` |
| `sin` | Unary | Sine (radians) | `(sin 0)` → `(0)` |
| `cos` | Unary | Cosine (radians) | `(cos 0)` → `(1)` |
| `arctan` | Binary | atan2(y, x) (radians) | `(arctan 1 1)` → `(0.785398...)` |

---

### Graphics Primitives

These produce typed atoms used with `draw`.

| Function | Arguments | Description | Example |
|----------|-----------|-------------|---------|
| `point` | `x y` (Numbers) | A 2D point | `(point 10 20)` |
| `line` | `point point` | A line segment between two points | `(line (point 0 0) (point 100 100))` |
| `arc` | `center start angle` | Arc with given center point, start point, and span angle in radians | `(arc (point 0 0) (point 10 0) pi)` |

### Draw Command

`draw` accepts any combination of `point`, `line`, `arc` expressions or symbols previously defined as those types.

```
(define p1 (point 0 0))
(define p2 (point 50 50))
(draw p1 p2 (line p1 p2))
```

---

## Testing

Run the full test suite from the build directory:

```bash
ctest
```

Or run individual test executables:

```bash
./unittests          # interpreter unit tests (tokenizer, types, expression, interpreter)
./test_message       # message widget tests
./test_gui           # GUI integration tests
```

Integration tests for the `vtscript` REPL require Python and the `pexpect` package:

```bash
pip install pexpect
python integration_test.py
```

> **Note:** The integration tests reference `/vagrant/tests/test3.vts` for file-evaluation tests. Adjust the path if running outside a Vagrant environment.
