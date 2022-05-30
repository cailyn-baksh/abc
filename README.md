# abc
A Brainfuck Compiler.

## Usage

```
bin/abc FILE [options]

General Options:
  --arch arg             The target architecture to generate code for.
  -f arg                 Options to be passed to the code generator
  -h [ --help ]          Show this help message
  -o [ --output ] arg    Place primary output in the specified file
  -v [ --verbose ]       Show verbose output
  --version              Print version string
```

## Build Instructions

To build, run `make`. abc requires `libboost_program_options`.
