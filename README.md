# C-Shell

## Table of Contents
- [Introduction](#introduction)
- [Detailed Features](#feature-details)
- [Usage](#usage)
- [Assumptions](#assumptions--deviation-from-the-instruction)
- [It's a Feature, not a bug](#its-a-feature-not-a-bug)
- [Acknowledgements](#acknowledgements)

## Introduction
C-Shell is a custom shell implementation written in C. It provides a command-line interface for users to interact with the operating system. Apart from the usual bash commands, it contains various specialised commands curated specially to enhance the user experience. It can execute built-in commands, run external programs, manage processes, log commands, and provide manual pages for commands.

## Feature Details

### 1. User Prompt
- When awaiting user input, a `shell prompt` is displayed.
- Contains information like username, system name, current working directory and the last executed process.

### 2. Multi-Commanding
- Allowing `multiple ";" seperated commands` to be passed in as input.
- Sequentally runs the all the commands.
- An error in one not affecting subsequent runs (unless it is dependent on previous command's result).

### 3. hop
- Custom command to `change directory`.
- Similar to cd but allows multiple arguments at a time.
- Supports all sorts of pathing (relative, absolute, none, special signs like "-", "~", "." and "..").

### 4. reveal
- Custom command to `show contents of a directory`.
- Similar to ls, with functionalities of flags like -a, -l and all their combinations.
- Supports all sorts of pathing (relative, absolute, none, special signs like "~", "." and "..").
- Provides colour-coded and structured output for enhanced user experience.
- Reveals cwd in absence of any path; and doesnt support multiple paths (yet!)

### 5. log
- Maintains a `log file` (at systemFiles/cmndlogs.txt) of all the commands executed; which is shared across all instances of the shell.
- Maintains a cap (15 lines, for demo purposes) on the file size.
- When run with the `purge` argument, it clears the log file.
- Also supports `execute N` command to run the Nth command (fromt the end) from the log file.

### 6. System Commands
- Supports all the `system commands` like ls, cat, echo, pwd, etc; along with their respective flags and arguments.
- Supports running of background processes using "&" at the end of the command.
- Displays the process id of the background process, and the process name when it completes; keeping the user informed.

### 7. proclore
- Custom command to `show process details`.
- Takes process id as input and shows details like process ID, name, state, memory usage, etc.
- Shows C-shell's process details in absence of any pid; and doesnt support multiple paths (yet!).

### 8. seek
- Custom command to `search for a file/directory`.
- Takes a filename as input and searches for it in the current directory; supports all sorts of pathing (relative, absolute, none, special signs like "~", "." and "..").
- Takes upto two arguments; first (compulsary) being the file/directory to look for and the second (optional) being the directory to search in.

### 9. Custom Commands
- Allow users to set `custom aliases and/or functions`.
- Supports passing arguments to the custom commands.

### 10/11/12. I/O Redirection and Pipiing
- Similar to bash, supports `I/O redirection` using > (writing), < (reading), >> (appending).
- Supports `piping` using | to pass the output of one command as input to another.
- Supports multiple redirections and pipings in a single command.

### 13. activities
- Prints the `list of processes` spawned by our shell.
- Shows details like process ID, name and status.
- Only shows the background processes spawned by the shell.

### 14. ping
- Takes a pid and `pings the process` with the provided signal.

### 15. Foreground and Background Handling
- Allows the user to `bring a background process to the foreground` using the fg command.
- Allows the user to `run a stopped background process` using the bg command.

### 16. neonate
- Displays the `process id of the most recently created process` on the system.
- Prints the PID every N seconds, until the key 'x' is pressed.

### 17. iMan
- Custom command to `display manual pages` for the commands.
- Takes a command name as input and extracts the manual from the Internet.

### X. Easter Eggs
- Well, no spoilers here! You gotta find them yourself!
- Hey, don't go looking for them in the code; that's cheating!

#### Note : For more details, refer to [this](https://web.archive.org/web/20240906104046/https://karthikv1392.github.io/cs3301_osn/mini-projects/mp1).

## Usage
To start the C-Shell, follow these steps:

1. Build and Run the Shell executable using `make`:
    ```sh
    make
    ```

2. (Optional, but Recommended) Clean up the build files:
    ```sh
    make clean
    ```

## Assumptions / Deviation from the Instruction
- Firstly, I have made every effort to address the doubts document, but it has become too lengthy and complex to follow effectively. Therefore, I assure that I have tried my best to stick to the provided requirements and do not use any prohibited calls or functions; but in the unlikely case of any mistakes, please consider the very tedious nature of keeping up with the doubts doc. Thank You!

- Format / content of custom print statements like error messages may not exactly match with the requirements. But, they do contain all (if not more) information that the requirements have asked for.

- Running multiple ";" seperated commands are treated as multiple commands, and not as a single command. So, if one of the commands fails (or is a log command), the other commands will still run; and thus get logged into the log file, also. This is a deviation from the requirements, but I believe it is a more user-friendly approach.

- The `activities` command only shows the background processes spawned by the shell, and not all the processes spawned (the latter actually fills up the screen, in certain cases, and makes it very user-unfriendly). Also, the print is not in lexographically sorted order; but in the order of the process creation (seemed like a more user-friendly approach; and also makes it less memory-intensive for the program). Since, we are doing only background processes, a stopped status refers to a zombie process; and running otherwise.

- `Ctrl+D` quits the shell gracefully, ie, it waits for all foreground processes to complete before exiting. This is a deviation from the requirements, but I believe it is a more user-friendly approach.

## It's a Feature, not a bug
- `vim &` may run into some problems, as the vim window appears but the keyboard control is with the shell. Leading to the inability of the user to interact with the vim smoothly, and hence raising significant annoyance. While it may seem a very hypothetical command call to run vim in background, but just in case.

- `Ctrl+Z` puts thw whole shell in the background. This may be a sub-optimally designed feature, one can easily bring the shell back to the foreground using the 'fg' command.

- As for any other bugs that you may encounter, do let us know! The code monkeys at our heaquarters are trying their best in making this shell better and better!

## Acknowledgements
- [CS3.301 Operating Systems and Networks (OSN) Course](https://karthikv1392.github.io/cs3301_osn) at IIIT-Hyderabad by [Prof. Karthik Vaidhyanathan](https://karthikvaidhyanathan.com/) and Team.
- [ChatGPT](https://chatgpt.com/share/bee66dd4-3c31-4cd5-92a4-d40adad1bd6f) and [Co-Pilot](.)

<hr>
<hr>
<hr>
