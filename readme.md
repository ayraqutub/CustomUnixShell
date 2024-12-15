# Custom Interactive UNIX Shell Implementation
## Overview

Dragon Shell is a custom command-line interface that implements basic shell functionalities, including the following commands and features:

- **`pwd`**: Print the current working directory.
- **`cd [directory]`**: Change the current working directory.
- **`exit`**:  Gracefully terminates the shell and all forked processes, and prints out the total user and
system time for all processes spawned by the shell since it was invoked.
- **Pipes (`|`)**: Redirect the output of one command as the input to another command.
- **Input (`<`) and Output (`>`) Redirection**: Redirect the input or output of a command to a file.
- **Background Execution (`&`)**: Execute a command in the background.
- **Signal Handling**: Handle `Ctrl-C` and `Ctrl-Z` signals to terminate or suspend child processes while keeping the shell running.

## Design Choices

1. **Command Execution**: 
   - Used `execv()` for executing commands, allowing flexibility in specifying the command path and its arguments.
   - Commands like `pwd`, `cd`, and `exit` are implemented as separate functions (`execPwd()`, `execCd()`, `execExit()`), while other commands are handled by child processes using `fork()` and `execv()`.

2. **Pipes**: 
   - Implemented pipe handling using the `pipe()` system call and the `dup2()` function to redirect file descriptors for inter-process communication.
   - The `execPipe()` function splits the command on the pipe symbol (`|`) and executes both commands in separate child processes.

3. **Signal Handling**:
   - Used `sigaction()` to handle `SIGINT` (Ctrl-C) and `SIGTSTP` (Ctrl-Z) signals. The signal handler ensures that only child processes are terminated or suspended, while the shell itself remains unaffected.

4. **Timing**:
   - Used `times()` and `sysconf()` to measure and report user and system CPU time upon exiting the shell.

## System Calls Used

1. **`fork()`**: Used to create child processes for executing commands.
2. **`execv()`**: Executes the specified command within a child process.
3. **`getcwd()`**: Retrieves the current working directory for the `pwd` command.
4. **`chdir()`**: Changes the current working directory for the `cd` command.
5. **`pipe()`**: Creates a pipe for inter-process communication, allowing for command chaining with `|`.
6. **`dup2()`**: Redirects file descriptors for input and output redirection and pipe implementation.
7. **`times()` and `sysconf(_SC_CLK_TCK)`**: Measures the user and system CPU time for the shell.
8. **`sigaction()`**: Sets up custom signal handlers for `SIGINT` and `SIGTSTP`.
9. **`waitpid()`**: Waits for child processes to complete, both in normal and background execution.
10. **`kill()`**: Sends signal to process; used to terminates background processes.

## Testing

The Dragon Shell was tested using the following scenarios:

1. **Basic Command Execution**:
   - Commands such as `pwd`, `cd`, and external commands like `ls` were tested to verify correct execution.

2. **Pipes**:
   - Commands with pipes (e.g., `/usr/bin/find ./ | /usr/bin/sort`) were tested to ensure correct redirection of output from one command to the input of another.

3. **Redirection**
    - Ensured one way redirection works (e.g., `/usr/bin/echo "test" > test.out`)
    - Ensured two way redirection works (e.g., `/usr/bin/tee < test.out > test2.out`)

4. **Background Execution**:
   - Commands executed in the background (e.g., `sleep 5 &`) were verified to ensure the shell remains interactive while the command runs in the background.

5. **Signal Handling**:
   - The `Ctrl-C` and `Ctrl-Z` signals were tested during command execution to ensure child processes are terminated or suspended without affecting the shell itself.

6. **Error Handling**:
   - Invalid commands, directories, and file paths were tested to ensure appropriate error messages are displayed.

## Sources

1. **Signal Handling**: Information on using `sigaction()` for signal handling was referenced from the [The Open Group](https://pubs.opengroup.org/onlinepubs/007904975/functions/sigaction.html).
2. **Pipe Implementation**: Concepts of pipes and `dup2()` were inspired by tutorials from [GeeksforGeeks](https://www.geeksforgeeks.org/pipe-system-call/) and videos from [Code Vault](https://www.youtube.com/channel/UC6qj_bPq6tQ6hLwOBpBQ42Q).
3. **Redirection Handling**: Techniques for input and output redirection were learned from the online slides from [Florida SU](https://www.cs.fsu.edu/~cop4610t/lectures/project1/io_redirect/io_redirection.pdf).
