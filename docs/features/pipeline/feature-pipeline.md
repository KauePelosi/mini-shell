# Pipeline Implementation in MiniShell

## Overview

This document details the implementation of **command pipelines** in MiniShell, a minimal Unix-like shell written in C++. Pipelines enable chaining multiple commands, where the standard output of one command serves as the standard input for the next. This fundamental shell feature significantly enhances command-line utility and flexibility.

**Example:**

```bash
ls -l | grep cpp | wc -l
```

## Motivation

Initially, MiniShell supported only the execution of a single command at a time. To evolve into a more functional and realistic shell, the introduction of command pipelines was crucial. This feature necessitates handling:

*   **Multiple processes:** Each command in a pipeline typically runs in its own process.
*   **Inter-process communication (IPC):** A mechanism to transfer data (output) from one process to another (input).
*   **Coordinated execution:** Managing the lifecycle and communication between these processes.

This implementation lays the groundwork for these advanced behaviors, moving beyond sequential single-command execution.

## Design Decisions

To integrate pipeline functionality without disrupting the existing command execution logic, a new architectural layer was introduced. The overall execution flow now incorporates a dedicated pipeline parsing step:

```text
tokens → pipeline parsing → execution
```

This modular approach ensures that the core `dispatcher` remains clean and existing functionalities are preserved, while allowing for robust and extensible pipeline management.

## Technical Implementation Details

### Parsing Strategy

The tokenizer largely remains unchanged, producing a flat list of tokens from the raw input. For instance, the input `ls -l | grep cpp | wc -l` is tokenized as:

```cpp
["ls", "-l", "|", "grep", "cpp", "|", "wc", "-l"]
```

A new `pipeParser` step then processes these tokens, splitting them into distinct command vectors based on the pipe (`|`) delimiter:

```cpp
[
  ["ls", "-l"],
  ["grep", "cpp"],
  ["wc", "-l"]
]
```

This structured representation facilitates the subsequent execution of individual commands within the pipeline.

### Execution Strategy

For a pipeline comprising `N` commands, the execution strategy involves:

1.  **Process Creation:** `N` child processes are created using `fork()`, one for each command.
2.  **Pipe Creation:** `N-1` pipes are established using `pipe()` to facilitate communication between adjacent commands.
3.  **File Descriptor Redirection:** Standard input (`STDIN_FILENO`) and standard output (`STDOUT_FILENO`) are redirected using `dup2()` within each child process.

Each process in the pipeline is configured as follows:

*   **Reads from previous command:** If not the first command, its standard input is redirected to the read end of the pipe connected to the preceding command.
*   **Writes to next command:** If not the last command, its standard output is redirected to the write end of the pipe connected to the subsequent command.

### Key System Calls

*   **`pipe()`:** Creates a unidirectional data channel (pipe) between two processes. It returns two file descriptors: `fd[0]` for the read end and `fd[1]` for the write end.
*   **`fork()`:** Creates a new process (child) that is an exact copy of the calling process (parent). It returns `0` in the child process and the child's Process ID (PID) in the parent process.
*   **`dup2()`:** Duplicates an existing file descriptor to a specified new file descriptor. This is crucial for redirecting `stdin` and `stdout` to the pipe ends.
*   **`exec*()`:** Replaces the current process image with a new program. In the context of a shell, `execvp()` is typically used to execute the command specified by the user.

## Integration with MiniShell

The pipeline execution logic is integrated into the main `shell()` loop. After tokenization, a check (`hasPipe()`) determines if the input contains a pipe operator. If a pipe is detected, the `pipeParser()` is invoked to segment the commands. The current implementation includes the parsing step, and the `pipelineCommands()` function is being developed to handle the actual execution:

```cpp
auto tokens = tokenize(input);

if (hasPipe(tokens)) {
  std::vector<std::vector<std::string>> pipeCommands = pipeParser(tokens);
  // executePipeline(pipeCommands, ctx); // Placeholder for future execution
} else if (!tokens.empty()) {
  ctx.lastExitStatus = dispatcher(tokens, ctx);
}
```

This structure ensures that single commands are dispatched as before, while piped commands are routed through the new pipeline handling mechanism.

## Current Status

*   **Basic pipeline parsing implemented:** The `pipeParser` successfully splits tokenized input into individual commands for pipeline processing.
*   **Initial execution logic under development:** The framework for creating processes and pipes is in place, but the full coordination and execution of piped commands are still being refined within `pipelineCommands()`.
*   **Edge cases not yet handled:** Scenarios such as invalid syntax, broken pipes, or complex redirections within pipelines are not fully addressed.

## Future Improvements

*   **Robust error handling:** Implement comprehensive error management for invalid pipeline configurations and execution failures.
*   **Integration with redirections:** Extend pipeline functionality to correctly handle input (`<`) and output (`>`, `>>`) redirections in conjunction with pipes.
*   **Built-in command compatibility:** Ensure that MiniShell's built-in commands (e.g., `cd`, `exit`, `history`) can participate correctly in pipelines.
*   **Refactor execution logic:** Optimize and streamline the `pipelineCommands()` function to reduce code duplication and improve clarity.
*   **Job control:** Implement basic job control features (`fg`, `bg`, `jobs`) for managing background processes created by pipelines.

## Notes

This implementation follows an incremental development approach, prioritizing a deep understanding of process communication and building a correct mental model for shell operations. The architecture is designed to be extensible, allowing for future refinements and the addition of more complex shell features.

## References

No external references were used for this document, as it is based on internal project documentation and code. All information is derived from the provided context.
