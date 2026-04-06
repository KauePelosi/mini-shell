cmake_minimum_required(VERSION 3.20)
project(MiniShell LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(MiniShell)

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

target_include_directories(MiniShell PRIVATE include)

target_compile_options(MiniShell PRIVATE
    -Wall
    -Wextra
    -pedantic
)
set_target_properties(MiniShell PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
