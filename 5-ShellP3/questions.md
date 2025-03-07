1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

It ensures that all child processes complete before the shell continues accepting input by using waitpid() in the parent process. After forking the child processes for each command in the pipeline, the parent waits for each child to finish waitpid(pids[1], &status, 0). This makes sure that the parent does not continue until all child prcesses have finished executing their commands
If waitpid() is not called on all child processes, it could cause Zombie Processes (child processes remain on the process table wasing resources), race conditions (the shell continunes accepting input before the previous child process finishes), unpredictable outputs (the shell will assume the previous commands were successful, leading to incorrect shell behavior).

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

dup2 can prevent resource leaks (unused pipe files are left open to waste resources), deadlock (a child process waiting on a pipe that was never closed, causing it to be stuck indefinitely), or unintended behavior including pipe file descriptors being left open, causing data to flow incorrectly between processes.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in command because it modifies the process's current working directory, which is a property of the shell process itself. When the shell runs an external command, that command executes in a separate process, and any changes to the working directory would be confined to the child process. Once the child process terminates, the working directory changes would be lost, and the shell's working directory would remain unchanged.
If cd were implemented as an external process, 
the change in the working directory would only affect the child process and not the parent shell process.
The shell would not reflect the directory change, leading to an inconsistent and misleading user experience.
Implementing cd externally would require complicated inter-process communication (IPC) to inform the shell of directory changes, which adds unnecessary complexity.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow an arbitrary number of piped commands while still handling memory allocation efficiently, I would modify my implementation to dynamically allocate memory based on the number of commands entered by the user.

Dynamic Memory Allocation for Commands: Instead of limiting the number of commands to CMD_MAX,allocate memory for the commands based on the actual input size. This could be done by reallocating memory as needed when parsing the input.
Flexible Pipeline Handling: The implementation of pipes could be adjusted to handle any number of pipes, dynamically creating and closing them based on the number of commands in the pipeline.
Trade-offs to consider:
Complexity: Dynamically allocating and managing memory for a variable number of commands introduces additional complexity. Proper error handling and memory deallocation would become more critical to avoid memory leaks or fragmentation.
Performance: Dynamic memory allocation is more flexible but, it could cost more resources if reallocations happen frequently.
Buffer Overflows: The program needs to meticulously track each string and buffer size.
