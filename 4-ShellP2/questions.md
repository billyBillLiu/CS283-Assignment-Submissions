1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: We use fork to create a new process, which is a copy of the current process. The execvp function is then called in the child process to replace the child's memory space with a new program. This allows the parent process to continue running independently of the child process. Fork allows the parent process to manage multiple child processes, handle their termination, and perform other tasks at the same time.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If the `fork()` system call fails, it returns -1. In the provided implementation, if `fork()` fails, the `perror` function is called to print an error message indicating that the fork failed. This helps in diagnosing the issue.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: The `execvp()` function searches for the command to execute in the directories listed in the `PATH` environment variable. The `PATH` variable contains a colon-separated list of directories that the system searches for executable files.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: The purpose of calling `wait()` in the parent process is to wait for the child process to terminate and to retrieve its exit status. If we didn’t call `wait()`, the child process would become a zombie process after it terminates, as its exit status would not be collected by the parent. This can lead to resource leaks and an accumulation of zombie processes.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: The `WEXITSTATUS()` macro extracts the exit status of the child process from the status value returned by `wait()`. This information is important because it allows the parent process to determine whether the child process terminated successfully or if it encountered an error.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: The implementation of `build_cmd_buff()` handles quoted arguments by toggling an `in_quotes` flag whenever a quote character is encountered. This allows the function to treat quoted strings as single arguments, even if they contain spaces. This is necessary to correctly parse commands where arguments may include spaces, such as file paths or strings with spaces.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: Changes to the parsing logic include handling quoted arguments and trimming whitespace from the input. One unexpected challenge in refactoring the old code was ensuring that the `cmd_buff_t` structure was properly initialized and that memory management was handled correctly to avoid segmentation faults.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals in a Linux system are used to notify a process that a specific event has occurred. They are a form of asynchronous interprocess communication (IPC) that can be used to interrupt, terminate, or otherwise control processes. Unlike other forms of IPC, such as pipes or message queues, signals do not carry data but rather serve as notifications.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: 
    > - `SIGKILL` (signal 9): This signal is used to forcefully terminate a process. It cannot be caught or ignored by the process. Typical use case: Killing a process that is unresponsive or misbehaving.
    > - `SIGTERM` (signal 15): This signal requests a process to terminate gracefully. The process can catch this signal and perform cleanup operations before exiting. Typical use case: Politely asking a process to terminate.
    > - `SIGINT` (signal 2): This signal is sent when the user interrupts a process from the terminal, typically by pressing `Ctrl+C`. The process can catch this signal to perform cleanup operations before exiting. Typical use case: Interrupting a running process from the terminal.


- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: When a process receives `SIGSTOP`, it is stopped (paused) by the operating system. Unlike `SIGINT`, `SIGSTOP` cannot be caught or ignored by the process. This is because `SIGSTOP` is designed to allow the operating system or a user with appropriate permissions to unconditionally stop a process, ensuring that it cannot continue executing until it is explicitly resumed with `SIGCONT`.