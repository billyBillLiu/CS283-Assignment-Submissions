#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup() {
    TEST_SHELL="./dsh"  # Adjust this if your shell has a different name
}

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# zero inputs
@test "Empty command handling" {
    run $TEST_SHELL <<< ""
    [ "$status" -eq 0 ]
}

# one command
@test "Basic command execution" {
    run $TEST_SHELL <<< "echo Hello, World!"
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, World!"* ]]
}

# multiple commands
@test "Multiple commands execution" {
    run $TEST_SHELL <<< "echo Hello; echo World"
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello"* ]]
    [[ "$output" == *"World"* ]]
}

# boundaries
@test "Long command input" {
    long_input=$(head -c 1024 < /dev/zero | tr '\0' 'A')
    run $TEST_SHELL <<< "echo $long_input"
    [ "$status" -eq 0 ]
}

# interface (built-in commands)
@test "Exit command" {
    run $TEST_SHELL <<< "exit"
    [ "$status" -eq 0 ]
}

@test "Change directory" {
    run $TEST_SHELL <<< "cd /tmp; pwd"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]  # Use regex matching
}

# errors
@test "Invalid command handling" {
    run $TEST_SHELL <<< "nonexistentcommand"
    [ "$status" -ne -6 ]
}

@test "Invalid command with arguments handling" {
    run $TEST_SHELL <<< "nonexistentcommand arg1 arg2"
    [ "$status" -ne -6 ]
}



# stress
@test "Single pipe execution" {
    run $TEST_SHELL <<< "echo Hello | grep Hello"
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello"* ]]
}

@test "Multiple pipes execution" {
    run $TEST_SHELL <<< "echo Hello | tr 'H' 'h' | grep hello"
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello"* ]]
}

@test "Stress test with many commands" {
    run $TEST_SHELL <<< "seq 1 1000 | wc -l"
    [ "$status" -eq 0 ]
    [[ "$output" == *"1000"* ]] 
}

# Test multiple piped commands with different transformations
@test "Complex pipeline with multiple transformations" {
    run $TEST_SHELL <<< "echo 'Hello World' | tr ' ' '\n' | sort | tr -d '\n'"
    [ "$status" -eq 0 ]
    [[ "$output" == *"HelloWorld"* ]]
}


# Test piping with a large amount of data
@test "Large data through pipeline" {
    run $TEST_SHELL <<< "seq 1 10000 | awk '{sum+=\$1} END {print sum}'"
    [ "$status" -eq 0 ]
    [[ "$output" == *"50005000"* ]]
}

# Test maximum number of pipes allowed
@test "Maximum number of pipes" {
    run $TEST_SHELL <<< "echo start $(for i in $(seq 1 $((CMD_MAX - 1))); do echo "| cat"; done)"
    [ "$status" -eq 0 ]
    [[ "$output" == *"start"* ]]
}


# Test chained pipes modifying text
@test "Chained text modification in pipes" {
    run $TEST_SHELL <<< "echo 'TeStInG' | tr '[:upper:]' '[:lower:]' | tr '[:lower:]' '[:upper:]'"
    [ "$status" -eq 0 ]
    [[ "$output" == *"TESTING"* ]]
}
