# MiniShell

A **minimal Unix-like shell written in C++**, developed as a **personal portfolio project**. MiniShell focuses on demonstrating strong fundamentals in **operating systems**, **process management**, **command execution**, and **clean, extensible C++ design**.

This project is intentionally scoped: it is not meant to replace Bash or Zsh, but to show **how a shell works internally**, with emphasis on clarity and correctness.

---

## Features

### Implemented

- Interactive prompt displaying the **current working directory (cwd)**
- Command input via `std::getline`
- Custom **tokenizer** with support for single and double quotes
- Execution of external commands using:
  - `fork()`
  - `execvp()`
  - `waitpid()`
- Built-in command system with dynamic dispatch
- Implemented built-ins:
  - `cd` (defaults to `$HOME`)
  - `exit [status]`
  - `history` (in-memory)
- Return status of the last executed command (`$?`)
- Pipe support (`|`)
- Clear separation between:
  - internal commands (built-ins)
  - external system commands
  - pipe commands
- Modular project structure (`src/` / `include/`)
- Fully functional compiled binary

---

## Architecture Overview

The project is designed to be **simple to follow**, while remaining **easy to extend**.

### Execution Flow

```
User Input
↓
Tokenize
↓
hasPipe(tokens)?
│
├─ yes → pipeParser(tokens)
│              ↓
│         pipelineCommands()
│         [fork + pipe + dup2 + exec × N]
│
└─ no  → dispatcher(tokens)
├─ Built-in  (cd, exit, history)
└─ External  (fork + exec)
```

### Built-in Dispatching

Built-ins are registered in an `unordered_map`, allowing new commands to be added **without modifying the dispatcher logic**.

---

## Pipeline Implementation Details

### Overview

This update introduces initial support for **command pipelines** using the pipe operator (`|`). A pipeline allows multiple commands to be connected so that the output of one becomes the input of the next.

**Example:**

```bash
ls -l | grep cpp | wc -l
```

### Motivation

Previously, the shell only supported executing a **single command at a time**. However, real shells support chaining commands through pipes, which requires:

- **Multiple processes:** Each command in a pipeline typically runs in its own process.
- **Inter-process communication (IPC):** A mechanism to transfer data (output) from one process to another (input).
- **Coordinated execution:** Managing the lifecycle and communication between these processes.

This change introduces the foundation for that behavior.

### Design Decision

Instead of modifying the existing execution logic, a new layer was introduced:

```text
tokens → pipeline parsing → execution
```

This keeps the architecture clean and avoids breaking existing functionality.

### Parsing Strategy

The tokenizer remains mostly unchanged. Input like `ls -l | grep cpp | wc -l` is tokenized as:

```cpp
["ls", "-l", "|", "grep", "cpp", "|", "wc", "-l"]
```

A new step, `pipeParser`, splits these tokens into commands:

```cpp
[
["ls", "-l"],
["grep", "cpp"],
["wc", "-l"]
]
```

### Execution Strategy

For a pipeline with `N` commands:

- `N` processes are created (`fork()`)
- `N-1` pipes are created (`pipe()`)
- File descriptors are redirected using `dup2()`

Each process:

- Reads from the previous command (if not first)
- Writes to the next command (if not last)

### Key System Calls

- **`pipe()`:** Creates a unidirectional communication channel (`fd[0]` → read end, `fd[1]` → write end).
- **`fork()`:** Creates a child process (returns `0` in child, child PID in parent).
- **`dup2()`:** Redirects standard streams (`STDIN_FILENO`, `STDOUT_FILENO`).
- **`exec*()`:** Replaces the current process with a new program.

### Integration

The `shell` function now checks for pipes after tokenization:

```cpp
auto tokens = tokenize(input);

if (hasPipe(tokens)) {
  std::vector<std::vector<std::string>> pipeCommands = pipeParser(tokens);
  if (!pipeCommands.empty() && pipeCommands.size() > 1) {
    ctx.lastExitStatus = pipelineCommands(pipeCommands);
  } else if (!pipeCommands.empty()) {
    ctx.lastExitStatus = dispatcher(pipeCommands[0], ctx);
  }
} else {
  if (!tokens.empty()) {
    ctx.lastExitStatus = dispatcher(tokens, ctx);
  }
}
```

---

## Build System (CMake)

MiniShell utilizes **CMake** as its build system, adhering to modern, target-based practices.

CMake is primarily responsible for **generating build files** (such as `Makefiles` or `Ninja files`), rather than directly compiling the source code.

### Build Flow (Mental Model)

```
CMakeLists.txt
↓
cmake -B build
↓
Build System Files (Makefile / Ninja)
↓
cmake --build build
↓
MiniShell Executable
```

### Basic Concepts

#### Project

```cmake
project(MiniShell LANGUAGES CXX)
```

This command defines the project's name and specifies the programming languages it uses (e.g., CXX for C++). It's a foundational declaration and does not, by itself, initiate any compilation or linking.

#### Targets

In CMake, a target is any artifact that CMake is configured to build. This can include executables, libraries, or other custom outputs.

**Examples:**

- **Executable Target:** `add_executable(MiniShell)`
- **Library Target:** `add_library(minishell_lib)`

All build configurations, such as source files, compile options, and link libraries, are associated with specific targets.

#### Source Files

Source files are explicitly linked to a target, informing CMake which files are part of a particular build artifact:

```cmake
set(CORE_SRC 
  src/core/dispatcher.cpp
  src/core/shell.cpp
)

set(EXEC_SRC
  src/execution/pipelineCommands.cpp
  src/execution/externalCommands.cpp
)

set(BUILTIN_SRC
    src/execution/built-ins/builtCd.cpp
    src/execution/built-ins/builtExit.cpp
    src/execution/built-ins/builtHistory.cpp
)

set(PARSER_SRC
    src/parser/tokenize.cpp
    src/parser/pipeParser.cpp
)

set(UTILS_SRC
  src/utils/getInternMap.cpp
  src/utils/history.cpp
  src/utils/historyGlobal.cpp
  src/utils/printCwd.cpp
  src/utils/hasPipe.cpp
)

target_sources(MiniShell PRIVATE
    src/main.cpp
    ${CORE_SRC}
    ${EXEC_SRC}
    ${BUILTIN_SRC}
    ${PARSER_SRC}
    ${UTILS_SRC}
)
```

This declaration ensures that the specified source files are compiled and linked as part of the `MiniShell` executable target.

#### Include Directories

Include directories specify the locations where the compiler should search for header files (`.hpp`, `.h`).

```cmake
target_include_directories(MiniShell PRIVATE include)
```

By setting this, you can use clean, absolute includes relative to the specified directory, such as:

```cpp
#include "core/shell.hpp"
```

This approach is preferred over using relative paths (e.g., `../`) within source files, which can lead to brittle build configurations.

#### Compile Options

Compiler warnings and flags are applied to specific targets to enforce coding standards and enable diagnostic checks:

```cmake
target_compile_options(MiniShell PRIVATE
-Wall
-Wextra
-pedantic
)
```

These options help maintain code quality and catch potential issues during compilation.

#### Build Types

CMake supports different build types, which control optimization levels and the inclusion of debugging symbols. The build type is selected during the configuration step, not within the `CMakeLists.txt` file itself.

**Common Build Types:**

- `Debug`: Optimized for debugging, with minimal or no optimizations and full debug symbols.
- `Release`: Optimized for performance, with maximum optimizations and no debug symbols.
- `RelWithDebInfo`: Optimized for performance, but includes debug symbols for easier post-mortem debugging.
- `MinSizeRel`: Optimized for the smallest possible binary size.

To specify a build type during configuration:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

### Common CMake Commands

#### 1. Configure the Project

This command should be executed when setting up the project for the first time or after any modifications to the `CMakeLists.txt` files.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

This step performs the following actions:

- Creates the `build/` directory if it doesn't already exist.
- Generates the native build system files (e.g., `Makefile` for Make, or `build.ninja` for Ninja).
- Generates the `compile_commands.json` file, which is crucial for various development tools.

#### 2. Build the Project

After configuring, this command compiles all defined targets within the project:

```sh
cmake --build build
```

#### 3. Clean Rebuild

To perform a clean rebuild, you can remove the `build` directory and then re-configure and build:

```sh
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

---

## Roadmap

The following roadmap is organized by **impact and technical depth**, with portfolio value in mind.

### Core Shell Features

- [x] Pipe support (`|`)
- [x] Input/output redirection (`>`)
- [ ] Proper signal handling (`SIGINT`, Ctrl+C)
- [x] Return status of the last executed command (`$?`)
- [x] Command history (in-memory)

---

## Goals of the Project

MiniShell demonstrates, in its current state:

### Complete pipeline execution

Supports chaining multiple commands with `|` using `fork()`, `pipe()`, `dup2()`, and `waitpid()`. Each command runs in its own process, with standard input/output correctly redirected between stages.

### Clean built-in command dispatch

Built-ins (`cd`, `exit`, `history`) are registered in an `unordered_map`, allowing new commands to be added without touching the dispatcher logic.

### Tokenization with quote handling

Properly respects single and double quotes, treating quoted strings as single tokens (e.g., `echo "hello world"`).

### Command history

In-memory history with persistent storage (`~/.minishell_history`) and a `history` built-in to view it.

### Exit status tracking

`$?`-like behavior via `ctx.lastExitStatus`, displayed in the prompt with coloured ✓ (success) / ✗ (failure).

### Modular C++ design

Clear separation of parsing, execution, built-ins, and utilities. Modern CMake build system with target-based configuration, explicit source lists, and strict compiler warnings (`-Wall -Wextra -pedantic`).


### Unix system programming fluency

Direct use of low-level syscalls (`fork`, `execvp`, `pipe`, `dup2`, `chdir`, `waitpid`) without higher-level wrappers, proving solid understanding of process creation, IPC, and file descriptor manipulation.

---

## How to Run

After building the project using CMake, you can run MiniShell from the `bin` directory:

```bash
./bin/MiniShell
```

---
## License

Personal project developed for educational and portfolio purposes, using the MIT license.

---

## Technologies and Concepts

- Modern C++ (C++17 / C++20)
- Unix/Linux system programming
- Process creation and management
- Inter-process communication (IPC) via pipes
- File system navigation
- Low-level system calls (`fork`, `exec`, `pipe`, `dup2`, `waitpid`, `chdir`)
- STL containers and algorithms
- Modular and extensible design
