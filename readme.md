# Jix Interpreter
![Ubuntu build status](https://github.com/harsath/Jix-Interpreter/actions/workflows/jix_tests_ubuntu.yml/badge.svg)

This project implements an interpreter for Jix, a [dynamically typed](https://en.wikipedia.org/wiki/Dynamic_programming_language) programming language. This repository is under active development.

See `examples` and `test_files` directories for example programs in Jix.

## Features: 
- Dynamic types (int, bool, and string)
- Arrays
    - `add()`
    - `len()`
- Functions and function pointers
- Builtin functions
    - Print()
- Statements
    - If-else
    - While
    - For
    - Return
    - Break
    - Variable declaration and assignment
- Nested blocks / statements

## Building
To build this project:
```bash
mkdir build && cd build
cmake .. && make
./jix your_file.jix
```

### Todo features:
- Builtin Hashtables
- User-defined datatypes
- Import statement
