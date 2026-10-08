#!/bin/sh

PORT=6389
SERVER_PID=0
PASSED=0
FAILED=0

finish_test()
{
    if [ "$SERVER_PID" -ne 0 ]; then
        kill "$SERVER_PID" 2>/dev/null
        wait "$SERVER_PID" 2>/dev/null
    fi
}

trap finish_test EXIT INT TERM

./miniredis-server -p "$PORT" >/tmp/miniredis-test.log 2>&1 &
SERVER_PID=$!

ready=0
for i in 1 2 3 4 5 6 7 8 9 10; do
    if printf 'PING\n' | ./miniredis-cli -p "$PORT" >/dev/null 2>&1; then
        ready=1
        break
    fi
    sleep 1
done

if [ "$ready" -eq 0 ]; then
    echo "The server did not start."
    exit 1
fi

check_command()
{
    expected=$1
    command=$2
    result=$(printf '%s\n' "$command" | ./miniredis-cli -p "$PORT")

    if [ "$result" = "$expected" ]; then
        echo "[OK] $command"
        PASSED=$((PASSED + 1))
    else
        echo "[FAIL] $command"
        echo "  expected: $expected"
        echo "  received: $result"
        FAILED=$((FAILED + 1))
    fi
}

check_command "PONG" "PING"
check_command "OK" "SET name Cyril"
check_command "Cyril" "GET name"
check_command "1" "EXISTS name"
check_command "1" "DEL name"
check_command "(nil)" "GET name"
check_command "1" "INCR counter"
check_command "2" "INCR counter"
check_command "OK" "SET greeting hello world"
check_command "hello world" "GET greeting"
check_command "0" "EXISTS missing"
check_command "0" "DEL missing"
check_command "ERR value is not an integer" "INCR greeting"
check_command "ERR key is missing" "GET"
check_command "ERR SET needs a key and a value" "SET onlykey"
check_command "ERR unknown command" "UNKNOWN"

echo ""
echo "Passed: $PASSED"
echo "Failed: $FAILED"

if [ "$FAILED" -ne 0 ]; then
    exit 1
fi
