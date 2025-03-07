#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

# Test basic command execution
@test "Basic: check ls runs without errors" {
    run ./dsh <<EOF
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test built-in command 'exit'
@test "Built-in: check exit command" {
    run ./dsh <<EOF
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test built-in command 'cd'
@test "Built-in: cd with valid directory" {
    mkdir -p test_dir
    run $test_shell <<EOF
cd test_dir
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"test_dir"* ]]
    rmdir test_dir
}


# Test pipeline command 'ls | cat -A'
@test "Pipeline: check ls | cat -A" {
    run ./dsh <<EOF
ls | cat -A
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

# Test empty command
@test "Error: check empty command" {
    run ./dsh <<EOF

EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "warning: no commands provided" ]
}

# Test too many commands in pipeline
@test "Error: check too many commands in pipeline" {
    run ./dsh <<EOF
cmd1 | cmd2 | cmd3 | cmd4 | cmd5 | cmd6 | cmd7 | cmd8 | cmd9
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "error: piping limited to 8 commands" ]
}

# Test invalid command
@test "Error: check invalid command" {
    run ./dsh <<EOF
invalidcommand
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "error: failed to execute command" ]
}

# Test command with arguments
@test "Basic: check echo with arguments" {
    run ./dsh <<EOF
echo Hello, World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, World!" ]
}

# Test command with quotes
@test "Basic: check echo with quoted arguments" {
    run ./dsh <<EOF
echo "Hello, World!"
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, World!" ]
}

# Test command with single quotes
@test "Basic: check echo with single quoted arguments" {
    run ./dsh <<EOF
echo 'Hello, World!'
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, World!" ]
}

# Test command with mixed quotes
@test "Basic: check echo with mixed quotes" {
    run ./dsh <<EOF
echo "Hello, 'World!'"
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, 'World!'" ]
}

# Test command with multiple arguments
@test "Basic: check echo with multiple arguments" {
    run ./dsh <<EOF
echo Hello, World! How are you?
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, World! How are you?" ]
}

# Test command with multiple spaces
@test "Basic: check echo with multiple spaces" {
    run ./dsh <<EOF
echo    Hello,    World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, World!" ]
}

# Test command with tabs
@test "Basic: check echo with tabs" {
    run ./dsh <<EOF
echo    Hello,    World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, World!" ]
}

# Test command with newlines
@test "Basic: check echo with newlines" {
    run ./dsh <<EOF
echo Hello,
World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello," ]
    [ "${lines[1]}" = "World!" ]
}

# Test command with backslashes
@test "Basic: check echo with backslashes" {
    run ./dsh <<EOF
echo Hello, \\World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, \\World!" ]
}

# Test command with special characters
@test "Basic: check echo with special characters" {
    run ./dsh <<EOF
echo Hello, \$World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, \$World!" ]
}

# Test command with environment variables
@test "Basic: check echo with environment variables" {
    run ./dsh <<EOF
echo Hello, \$USER!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, $USER!" ]
}

# Test command with subshell
@test "Basic: check echo with subshell" {
    run ./dsh <<EOF
echo Hello, \$(whoami)!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, $(whoami)!" ]
}

# Test command with command substitution
@test "Basic: check echo with command substitution" {
    run ./dsh <<EOF
echo Hello, \`whoami\`!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Hello, $(whoami)!" ]
}