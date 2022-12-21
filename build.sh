#!/bin/zsh

BIN=bin
MAIN=$1
FILENAME=${$(basename "$MAIN")%.*}

echo "Compiling $FILENAME..."

mkdir -p bin
cp build/dd $BIN/$FILENAME

echo "Building $FILENAME"

./$BIN/$FILENAME $MAIN

mv dd.asm $BIN/$FILENAME.asm

as $BIN/$FILENAME.asm -o $BIN/$FILENAME.o

ld $BIN/$FILENAME.o -o $BIN/$FILENAME -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`

echo "Running binary $FILENAME"
./$BIN/$FILENAME
echo $?