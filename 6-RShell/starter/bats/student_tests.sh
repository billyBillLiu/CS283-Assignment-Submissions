#!/usr/bin/env bats

setup() {
    # Start the server in the background
    ./dsh -s &
    sleep 1 # Give the server time to start
}

teardown() {
    # Stop the server after tests
    echo "stop-server" | ./dsh -c
    sleep 1
}

@test "Check ls command" {
    run bash -c "echo 'ls' | ./dsh -c"
    [[ "$output" =~ "dsh" ]]  # Check if "dsh" binary appears in the output
}

@test "Check pwd command" {
    run bash -c "echo 'pwd' | ./dsh -c"
    [[ "$output" =~ "/home" ]]  # Assuming running from home directory
}

@test "Change directory and verify" {
    run bash -c "echo 'cd ..' | ./dsh -c"
    [[ "$output" =~ "Changed directory" ]]
}

@test "Invalid command handling" {
    run bash -c "echo 'invalidcommand' | ./dsh -c"
    [[ "$output" =~ "execvp failed" ]]
}

@test "Pipe commands" {
    run bash -c "echo 'ls | grep .c' | ./dsh -c"
    [[ "$output" =~ "rsh_cli.c" ]]
}

@test "Exit command" {
    run bash -c "echo 'exit' | ./dsh -c"
    [[ "$status" -eq 0 ]]
}

@test "Verify server stops on stop-server command" {
    run bash -c "echo 'stop-server' | ./dsh -c"
    [[ "$output" == *"cmd loop returned 0"* ]]
}



@test "Ensure multiple clients can connect sequentially" {
    run bash -c "echo 'ls' | ./dsh -c"
    [[ "$output" =~ "dsh" ]]
    run bash -c "echo 'ls' | ./dsh -c"
    [[ "$output" =~ "dsh" ]]
}

@test "Test handling of long commands" {
    run bash -c "echo 'echo This is a very long test command to check buffer handling' | ./dsh -c"
    [[ "$output" =~ "This is a very long test command to check buffer handling" ]]
}

