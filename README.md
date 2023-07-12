# LoFy Interpreter

A simple Python-like language interpreter written in pure C.

## Features

- **Types**: Integers, Floats, Strings
- **Variables**: Dynamic typing, assignment
- **Arithmetic**: `+`, `-`, `*`, `/`
- **Statements**: `print()`, assignments
- **REPL**: Interactive shell

## Build

You need GCC and Make (or MinGW on Windows).

```bash
mingw32-make
# or
make
```

## Run

Start the REPL:

```bash
./lofy.exe
```

## Example Usage

```python
>>> x = 10
>>> y = 2.5
>>> print(x * y)
25.000000
>>> print("Hello")
Hello
```
