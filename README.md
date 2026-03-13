# Regex Engine

A lightweight regular expression engine implemented in modern C++ (C++20). This project demonstrates the complete pipeline of regex processing: lexical analysis, parsing, NFA construction, and matching using Thompson's algorithm.

## About This Project (Why I Built This)

This is a personal practice project built strictly for educational purposes. The goal wasn't to create a production-ready regex library or replace existing tools like `std::regex` or `RE2`. Instead, the motivation was to "reinvent the wheel" to deeply understand the magic of how regular expression engines actually work under the hood.

Through building this small engine from scratch, I explored:
- The classic compiler pipeline: Tokenization (Lexer) -> Abstract Syntax Tree (Parser) -> NFA Graph (Compiler) -> Execution (VM).
- The theory behind Thompson's Construction algorithm for translating regex into automata.
- How to implement a parallel-state virtual machine to avoid the exponential backtracking trap (ReDoS) found in many standard regex engines.
- Applying modern C++20 features (`std::variant`, `std::visit`, `std::span`) in a practical, system-level programming context.

It's a small, imperfect project, but it successfully demystifies the complex machinery behind the text-processing tools we use every day.

## Features

- **Lexical Analysis**: Tokenizes regex patterns into tokens (characters, operators, etc.)
- **Parsing**: Builds an Abstract Syntax Tree (AST) from token stream
- **NFA Construction**: Converts AST to Nondeterministic Finite Automaton using Thompson's algorithm
- **Matching**: Supports both exact matching (`is_match`) and partial matching (`search`)
- **CLI Interface**: Command-line tool to interact with each stage of the pipeline
- **Comprehensive Testing**: Unit tests for all components

### Supported Regex Syntax

| Operator | Description | Example |
|----------|-------------|---------|
| `.` | Matches any single character | `a.b` |
| `*` | Kleene star (zero or more) | `a*` |
| `+` | Kleene plus (one or more) | `a+` |
| `?` | Optional (zero or one) | `a?` |
| `|` | Alternation (OR) | `a|b` |
| `()` | Grouping for precedence | `(ab)*` |
| Literals | Regular characters | `abc` |

**Note**: Character classes (`[a-z]`), escapes (`\d`, `\w`), anchors (`^`, `$`), and quantifiers (`{m,n}`) are not yet implemented.

## Build Instructions

### Prerequisites

- `g++` (version supporting C++20)
- `make`
- `ar` (static library archiver)

### Building

```bash
# Build the main executable
make

# Build and run tests
make test

# Clean build artifacts
make clean
```

The build process creates:
- `bin/re` - Main regex CLI tool
- `bin/re_test` - Test runner
- `build/libre.a` - Core regex library

## Usage

The `re` CLI tool provides several subcommands to inspect different stages of regex processing:

```bash
# Show help
re --help

# Show version
re --version

# Tokenize a regex pattern
re --lex "a*b"

# Parse and display AST
re --ast "a*b"

# Compile to NFA and display state machine
re --nfa "a*b"

# Exact match (full string must match)
re --match "a*b" "aaaab"

# Partial match (search for pattern in text)
re --search "a*b" "xxxaaaabxxx"
```

### Examples

```bash
# Tokenization example
$ re --lex "a(b|c)*"
0: Char('a')
1: LParen
2: Char('b')
3: Or
4: Char('c')
5: RParen
6: Star

# AST example
$ re --ast "a(b|c)*"
Concat:
  Literal('a')
  Repeat(*):
    Alt:
      Literal('b')
      Literal('c')

# Matching examples
$ re --match "a*b" "aaaab"
Match: TRUE

$ re --match "a*b" "aaac"
Match: FALSE

$ re --search "a*b" "xxxaaaabxxx"
Search: FOUND
```

## Project Structure

```
regex/
├── include/re/          # Public headers
│   ├── ast.hpp          # AST data structures
│   ├── error.hpp        # Error handling utilities
│   ├── lexer.hpp        # Lexical analyzer interface
│   ├── match.hpp        # Matching algorithms
│   ├── nfa.hpp          # NFA data structures
│   └── parser.hpp       # Parser interface
├── src/re/              # Core implementation
│   ├── ast.cpp          # AST construction and printing
│   ├── lexer.cpp        # Tokenization implementation
│   ├── match.cpp        # NFA matching implementation
│   ├── nfa.cpp          # Thompson's NFA construction
│   └── parser.cpp       # Recursive descent parser
├── src/app/             # Application code
│   ├── cli.hpp          # Command-line interface definitions
│   ├── cli.cpp          # CLI argument parsing
│   └── main.cpp         # Entry point
├── tests/               # Unit tests
│   ├── test_main.cpp    # Test runner
│   ├── test_lexer.cpp   # Lexer tests
│   ├── test_parser.cpp  # Parser tests
│   ├── test_nfa.cpp     # NFA tests
│   └── test_match.cpp   # Matching tests
├── Makefile             # Build system
└── README.md            # This file
```


## Testing

Run the test suite:

```bash
make test
```

Tests cover:
- Lexical analysis of valid and invalid patterns
- Parser correctness and error handling
- NFA construction for all operators
- Matching behavior with various patterns

## License

[MIT License](LICENSE)

## Acknowledgments

- Ken Thompson for the NFA construction algorithm
- Modern C++ best practices and idioms
- Educational resources on automata theory and compiler construction
- Gemini

---

Built with ❤️ using C++20. Perfect for educational purposes and understanding how regex engines work under the hood.
