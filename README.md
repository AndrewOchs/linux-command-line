Custom Command Line Interface:

A Unix-like shell implementation with support for pipes, redirection, and background processes, built in C.
Overview
This project is a custom shell implementation that replicates core functionalities of standard Unix/Linux shells. It provides a command-line interface where users can execute commands with arguments, chain commands using pipes, redirect input/output, and run processes in the background.


Features:

 * Command Execution: Execute programs with arguments
 * Pipe Support: Chain multiple commands using the pipe operator (|)
 * I/O Redirection: Redirect input (<) and output (>, >>) to and from files
 * Background Processing: Run commands in the background using the & operator
 * Built-in Commands: Support for built-in shell commands
 * Signal Handling: Properly handle keyboard interrupts and other signals

Implementation Details

This shell was built using C and implements system calls such as fork(), exec(), pipe(), and dup2() to handle process creation and communication. 
Key components include:

* Command Parser: Tokenizes and interprets user input
* Process Management: Creates and manages child processes
* Pipe Handler: Establishes communication between processes
* Redirection Logic: Manages file descriptors for input/output redirection
* Signal Handlers: Properly responds to user-generated signals

Usage Examples
bashCopy# Basic command execution
$ ls -la

# Pipe example
$ ls -l | grep ".txt"

# Redirection example
$ echo "Hello, World!" > output.txt

# Background process
$ sleep 10 &

# Combining features
$ cat input.txt | sort | uniq > output.txt &
Building and Running
To compile and run this shell:
bashCopy# Compile
gcc -o myshell main.c

# Run
./myshell
Technical Challenges
This project addresses several complex aspects of systems programming:

* Process Creation and Management: Handling the lifecycle of child processes
* Inter-Process Communication: Establishing pipes for data flow between processes
* File Descriptor Manipulation: Managing I/O redirection
* Signal Handling: Properly responding to keyboard interrupts and other signals
* Memory Management: Avoiding leaks when allocating memory for commands and arguments

Future Enhancements
Potential improvements that could be added:

* Command history with up/down arrow navigation
* Tab completion for filenames and commands
* Custom environment variable support
* More sophisticated job control
* Scripting capabilities

Academic Context
This project was developed as part of coursework in Operating Systems at Temple University, applying concepts of process management, inter-process communication, and system calls in a practical implementation.

Note: This shell is intended for educational purposes and may not have all the safety features and robustness of production shells like Bash or Zsh.
