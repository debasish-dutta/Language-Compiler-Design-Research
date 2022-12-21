# Compiler & Language Design

This project is to design a compiler on a custom designed language loosly based on The mother of all languages -> `C`.

The original methodology is to designed the compiler based on incremental stages.
Each stage will compile the source language into the required assemble. And the stages will increase in difficulty down the order.

Each stage comprises of mainly 3 stages:

    1. The scanner -> Converts the source language into tokens
    2. The parser -> Takes the tokens as input an outputs an AST.
    3. The assembler -> Takes the AST and generates the required assembly.

## Designing the Language

## The Scanner

## The Parser

### Structure of a flex/bison program

![structure of a flex/bison program](assets/struct.png)

There are two main parts to a parser:

    1. The scanner -> Lex/Flex
    2. The parser -> Yacc/Bison

### Lex

### Yacc
