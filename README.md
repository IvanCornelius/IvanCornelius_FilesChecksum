# IvanCornelius FilesChecksum

**English** | [Русский](README.ru.md)

A C++ console application built with the Qt framework that calculates MD5 checksums for all files in a specified directory and its subdirectories. The application is designed to run on both Windows and Linux operating systems.

## Features

*   **Multi-threaded Processing:** Calculates checksums for multiple files simultaneously using separate threads.
*   **Two Synchronization Implementations:** Provides two distinct versions of the application demonstrating different thread synchronization techniques:
    *   **Mutex-based:** Uses `QMutex` to protect shared data.
    *   **Signals/Slots-based:** Uses Qt's signal/slot mechanism for thread-safe communication.
*   **Recursive Directory Traversal:** Scans all files within the specified starting directory and its subdirectories.
*   **Alphabetical Output:** Prints the full file path and its corresponding MD5 checksum in alphabetical order.
*   **Error Handling:** Gracefully handles files and directories that cannot be accessed due to permission issues.
*   **Cross-platform:** Compatible with Windows and Linux.

## Example

![Example of program execution](docs/Example.png)

## Usage

Run the application from the command line, providing the path to the starting directory as an argument.

./IvanCornelius_FilesChecksum /path/to/your/directory

**Example Output:**
/path/to/your/directory/file1.txt  d41d8cd98f00b204e9800998ecf8427e
/path/to/your/directory/subdir/file2.pdf  a1b2c3d4e5f678901234567890abcdef
...

## Key Implementation Details

*   **Mutex Version:** Source code can be found in ([src/IvanCornelius_FC_Mutex](src/IvanCornelius_FC_Mutex)). Worker threads inherit from `QThread`. They directly access a shared `QMap` of results, protected by a `QMutex`. The main thread manages a pool of worker threads.
*   **Signals/Slots Version:** Source code can be found in ([src/IvanCornelius_FC_SignalsSlots](src/IvanCornelius_FC_SignalsSlots)). Worker objects inherit from `QObject` and live in separate threads (`QThread`). Communication (sending results, errors, finished signals) is done exclusively through Qt's signals and slots, which are thread-safe when connected with `Qt::QueuedConnection` (the default for cross-thread connections).

## Pre-built Executables

Ready-to-use Windows executables are available for both versions:
*   Mutex-based version: ([releases/IvanCornelius_FC_Mutex_Windows](releases/IvanCornelius_FC_Mutex_Windows))
*   Signals/Slots-based version: ([releases/IvanCornelius_FC_SignalsSlots_Windows](releases/IvanCornelius_FC_SignalsSlots_Windows))

## Technical Specification

You can review the original technical requirements in ([docs/technical_specification.ru.txt](docs/technical_specification.ru.txt)) (in Russian).

## License

This project is licensed under the MIT License. See the license in ([LICENSE](LICENSE)) or in Russian ([LICENSE.ru.md](LICENSE.ru.md)).