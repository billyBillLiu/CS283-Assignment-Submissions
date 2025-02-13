1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is safe for getting user input because it limits the number of characters read and prevents overflows. It also handles eof. It reads the entire line instead of having to handle each char.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Malloc is more flexible than a fixed size array. Since cmd needs to be big enough to handle inputs that can grow, this flexibility is preferred. A fixed sized array with fgets could cause buffer overflows.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming  whitespaces makes sure we do not misread any inputs due to whitespace. It also prevents empty inputs that only include spaces.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:\
        Output redirection redirects STDOUT to a file by overwriting or appending. (ls > output.txt) overwrites/writes ls to output. (ls >> output.txt) appends ls to output.\
        Input redirection reads input from a file instead of a standard input. (sort < numbers.txt) uses the contents of numbers as the input to sort.\
        Error redirection redirects STDERR to a file. (gccc program.c 2> error.log) stores compiling errors into an error log.\
        Challenges we might face implementing them include: parsing >, >>, <, and 2>, and opening the correct files in the correct modes (read or write)

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection changes where the input or output is read or written to and from. Piping passes the output of one command to another as input.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Separating STDERR and STDOUT allows error messages to stand out and be handled independently from regular expected outputs. It also ensures errors do not end up being piped as regular outputs, causeing more errors and making distinguishing between them even more difficult.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  We redirect both to files. There should be one file for commands that fail and commands that worked (e.g. output.log and error.log)