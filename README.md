# **Project Observer**

**A real-time system monitoring tool for Linux terminals (CLI-based)**

## **Brief project explanation**

- A lightweight system monitoring tool displaying real-time system metrics directly in the terminal
- Design: Display of the information by using ASCII characters via terminal/CLI
- Targeted peripherals: CPU, RAM and storage
- Each peripheral has two observation modes: minimum and maximum
- High-performance codebase primarily written in C++

## **Architecture & Design Principles**

- **Modular architecture:** Each subsystem (CPU, RAM, Storage) is implemented as an independent module.
- **Reusable components:** Monitoring logic and UI rendering are decoupled.
- **Maintainability:** Each module is split into dedicated source and header files.
- **Documentation:** All public interfaces are documented using Doxygen-style comments.
- **Code style:** Enforced via clang-format (Google C++ Style Guide).

## **Module Overview**

- Monitoring & preprocessing: cpu.cpp, ram.cpp, storage.cpp, utilities.cpp, logging.cpp, graph.cpp
- UI & application logic: monitoring.cpp, app.cpp (FTXUI-based)
- Execution: main.cpp

## **Real-Time Characteristics & Sampling**

- Real-Time: A process or thread that executes and completes its task within a predefined deadline.
- Sampling: A defined number of measurements collected within a specified period to balance power efficiency and reliability.
- **Disclaimer:** In this project, the refresh rate ranges between 1 Hz and 2 Hz, as higher rates would not provide additional benefits due 
to the soft time constraints. By contrast, fields such as medicine or aerospace require hard real-time constraints, since meeting strict deadlines 
is essential to ensure human safety and system integrity.

## **Set-Up**
**Disclaimer:** Before starting the installation make sure that you are in the root directory of the project.
- Installation of project dependencies via CLI/terminal: ./installation.sh
- System-wide project execution via CLI/terminal: Enter observer 
- Uninstallation of project dependencies via CLI/terminal: ./uninstallation.sh

## **Contribution**

- **FTXUI:** The UI within the CLI/Terminal is based on FTXUI which was created and is maintained by [ArthurSonzogni](https://github.com/ArthurSonzogni/FTXUI)

## **Testing & Quality Assurance**

- The project was extensively tested on Linux-based systems.
- Tests focus on correctness, robustness, and boundary conditions of system data acquisition.
- Tests achieve 100% code coverage.
- Due to confidentiality and security reasons, test cases are not part of the public repository.
- This decision prevents exposure of sensitive system data and environment-specific information.

## **Confidentiality notice**
- Project is governed by the MIT License. See the LICENSE file for details.
- This project is intended for educational purposes only. It is crucial to respect user privacy and data confidentiality when handling system information.
- Ensure that any data collected or displayed does not compromise user security or violate privacy regulations.
- Only non-sensitive system information should be accessed and displayed.
