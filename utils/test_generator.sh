TEST_DIR=$1
TEST_COUNT=$2

MIN_ITER=500
MAX_ITER=5000
MIN_THREADS=2
MAX_THREADS=12

random_iter() {
    local min=$MIN_ITER
    local max=$MAX_ITER
    local range=$((max - min + 1))
    
    local rand=$(od -An -tu4 -N4 /dev/urandom | tr -d ' ')
    echo $(( rand % range + min ))
}

random_thread() {
    local min=$MIN_THREADS
    local max=$MAX_THREADS
    local range=$((max - min + 1))
    
    local rand=$(od -An -tu4 -N4 /dev/urandom | tr -d ' ')
    echo $(( rand % range + min ))
}

for (( i=1;i<=$TEST_COUNT;i++ )); do
    IN=$(printf "%05d.in" "$i")
    OUT=$(printf "%05d.out" "$i")
    ITERS=$(random_iter)
    THREADS=$(random_thread)
    echo "$ITERS $THREADS" > $TEST_DIR/$IN
    echo $(($ITERS * $THREADS)) > $TEST_DIR/$OUT
done