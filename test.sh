#! /bin/zsh

# Getting the valid and invalid directories
if [ $# -ne 1 ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

MAIN=$1
DIRNAME=$(dirname "$MAIN")
FILENAME=${$(basename "$MAIN")%.*}

valid_dir=$MAIN/valid
invalid_dir=$MAIN/invalid


# Build the compiler
echo "building the compiler.."
make &>/dev/null

# Testing all inputs
echo "Testing for valid tests.\n"

for input in "$valid_dir"/*; do
    expected_result=${input##*return_}
    expected_result=${expected_result%%.*}
    output=$(./build.sh "$input")
    result=$(echo "$output" | tail -n 1 | sed 's/^[ \t]*//;s/[ \t]*$//')
    # result=output | tail -n 1
    echo "Testing $input" 
    if [ "$result" = "$expected_result" ]; then
        echo "PASS: $input"
    else
        echo "FAIL: $input"
    fi
    echo "Expected output: $expected_result"
    echo "Actual output: $result \n"
done

echo "\nTesting for invalid tests.\n"

for input in "$invalid_dir"/*; do
    output=$(./build.sh "$input") &>/dev/null
    echo "Testing $input" 
    if echo "$output" | grep -q "error"; then
        echo "PASS: $input \n"
    else
        echo "FAIL: $input"
        echo "Expected Error message: $output\n"
    fi
done

# Clean up
echo "\ncleaning up.."
make clean &>/dev/null
