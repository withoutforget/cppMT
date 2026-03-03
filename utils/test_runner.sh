BINARY=$1
TEST_DIR=$2

TEST_IN_FILES=( $(ls $TEST_DIR | grep -E  '/*.in$' | sort) )
TEST_OUT_FILES=(  )

for FILE in $TEST_IN_FILES; do
    TEST_OUT_FILES+=( ${FILE/in/out} )
done

TEST_FILE_COUNT=${#TEST_IN_FILES[@]}

for ((i = 0; i < $TEST_FILE_COUNT; i++)) do
    IN=${TEST_IN_FILES[$i]}
    OUT=${TEST_IN_FILES[$i]}
    OUTPUT=$("$BINARY" < "$TEST_DIR/$IN" 2>&1 >/dev/null)
    if [ $? -ne 0 ]; then
        echo "FAILED: $OUTPUT"
        break
    else
        echo "PASSED"
    fi
done


