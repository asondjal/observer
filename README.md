# **Projtect Observer: Monitoring-Tool for Linux via Terminal/CLI**

## **Brief project explaination**

- Creation of a monitoring tool showing basic data and real-time data
- Design: Display of the information by using ASCII characters from FTXUI, shoutout to [ArthurSonzogni](https://github.com/ArthurSonzogni/FTXUI)
- Focus on periphals: CPU, RAM, storage
- High Performance: Code will be written completely in C++

## **Digital signal processing**

- Real-Time: A process or thread that executes and completes its task within a predefined deadline.
- Sampling: A defined number of measurements collected within a specified period to balance power efficiency and reliability.
- **Disclaimer:** In this project, the refresh rate ranges between 1 Hz and 2 Hz, as higher rates would not provide additional benefits due 
to the soft time constraints. By contrast, fields such as medicine or aerospace require hard real-time constraints, since meeting strict deadlines 
is essential to ensure human safety and system integrity.

## **Set-Up**
**Disclaimer:** Before starting the installation make sure that you are in the root directory of the project.
- Installation of project dependencies via CLI/terminal: ./setup.sh
- Project execution via CLI/terminal: ./build/observer
