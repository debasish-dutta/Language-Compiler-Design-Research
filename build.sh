#!/bin/zsh

BIN=bin
BUILD=build
MAIN=$1
DIRNAME=$(dirname "$MAIN")
FILENAME=${$(basename "$MAIN")%.*}

echo "Compiling $FILENAME..."

mkdir -p bin
cp $BUILD/dd $BIN/$FILENAME

echo "Building $FILENAME"

./$BIN/$FILENAME $MAIN

mv $BUILD/$FILENAME.asm $BIN/$FILENAME.asm
# mv dd.asm $BIN/$FILENAME.asm

as $BIN/$FILENAME.asm -o $BIN/$FILENAME.o

ld $BIN/$FILENAME.o -o $BIN/$FILENAME -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`

echo "Running binary $FILENAME"
./$BIN/$FILENAME
echo $?