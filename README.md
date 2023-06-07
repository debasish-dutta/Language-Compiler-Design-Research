# Compiler Construction

![Lines of code](https://img.shields.io/tokei/lines/github/debasish-dutta/Language-Compiler-Design-Research?style=flat-square)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/48411ec1d14a46febced145298378773)](https://app.codacy.com/gh/debasish-dutta/Language-Compiler-Design-Research/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
![GitHub last commit](https://img.shields.io/github/last-commit/debasish-dutta/Language-Compiler-Design-Research?style=flat-square)
[![wakatime](https://wakatime.com/badge/user/12ef7e17-3042-439c-a828-61d087c8b93e/project/04af9e50-f83a-4754-ae52-c8b9f36df22b.svg?style=flat-square)](https://wakatime.com/badge/user/12ef7e17-3042-439c-a828-61d087c8b93e/project/04af9e50-f83a-4754-ae52-c8b9f36df22b)

This is a compiler designed to support a custom programming language loosely based on **`C`**  a 64-bit ARMv8 CPU ( mainly on m1, but also can be expanded on such as on the Raspberry Pi 3 and such). It aims to provide an efficient and reliable compilation process for your language, enabling developers to translate their code into executable binaries. It is also designed to be a easy-to-get-started with language with minimal hardware requirements.

## Features

 - Lexical analysis: The compiler performs lexical analysis on the source code to tokenize it into meaningful units.
 - Syntax analysis: It parses the tokenized code using a specified grammar to build an abstract syntax tree (AST).
 - Semantic analysis: The compiler performs semantic analysis on the AST to check for language-specific rules and enforce correct usage.
 - Code generation: It generates target machine code or intermediate code from the processed AST.
 - Optimization: The compiler may perform various optimization techniques to improve the efficiency of the generated code.

## Requirements

**Compiler Tools**: Tools required to build and run the compiler.

  - Lexical analyzer generator `Flex`
  - Parser generator `Bison`
  - Compiler backend tools `LLVM ` toolchain


## Build and Run

1. Clone the repository: git clone https://github.com/debasish-dutta/Language-Compiler-Design-Research/
2. Navigate to the project directory: `cd  Language-Compiler-Design-Research`
3. Build the compiler: 
    - `make` to build the base compiler.
    - Ensure that the required compiler tools are installed and available in the system's PATH.
7. Compile and run a Language program:
    - Run `build.sh <input-file>` to compile a `.dd` source file.
    - Specify the path to the input file that contains the Custom Language code. 
    - Optionally, provide additional flags or options for the compiler.

## Limitations

As with any compiler, there may be limitations and areas that require further improvement. It's important to thoroughly test the compiler and identify any issues or constraints that may impact the development experience or the execution of specific language constructs.

## Future Development

### TODO








