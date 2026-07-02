# STM32 EV Control Module

Educational Embedded C implementation of an Electric Vehicle control module using **STM32CubeIDE**, **STM32 HAL**, and **UART telemetry**.

This project is part of my Embedded Systems learning practice. It simulates core EV control calculations such as vehicle speed, motor torque, battery State of Charge, estimated range, regenerative braking, drive modes, motor power, and motor temperature.

> Note: This is an educational simulation project, not a production EV controller or a safety-critical automotive system.

---

## Short Description

This STM32 project implements an EV control module using modular Embedded C files:

* `ev_control.c`
* `ev_control.h`

The firmware runs a simple EV simulation loop and prints EV state data over UART.

---

## Project Overview

The goal of this project is to practice writing a clean Embedded C module for EV-related calculations and integrating it into an STM32CubeIDE project.

The EV control model includes:

* Speed computation using a simplified inertia model
* Motor torque calculation
* Drive mode torque scaling
* Regenerative braking behavior
* SOC estimation using energy integration
* Estimated driving range
* Motor power calculation
* Motor temperature warm-up / cool-down model
* UART telemetry output for monitoring

---

## Features

* Modular EV control logic in separate `.c` and `.h` files
* Supports three drive modes:

  * ECO
  * NORMAL
  * SPORT
* Simulates regenerative braking when brake percentage exceeds a threshold
* Computes vehicle speed in km/h
* Estimates battery State of Charge, SOC
* Estimates remaining range in km
* Computes motor power in kW
* Simulates motor temperature
* Sends formatted telemetry over USART1 at 115200 baud
* Toggles PC13 LED as a simple heartbeat indicator

---

## Hardware / Simulation Target

| Item      | Description                                 |
| --------- | ------------------------------------------- |
| MCU       | STM32F103C8T6                               |
| Board     | STM32 Blue Pill or compatible STM32F1 board |
| IDE       | STM32CubeIDE                                |
| Framework | STM32 HAL                                   |
| Simulator | PICSimLab, optional                         |
| UART      | USART1 at 115200 baud                       |
| LED       | PC13 heartbeat LED                          |

---

## Tools Used

* STM32CubeIDE
* STM32CubeMX configuration through `.ioc`
* STM32 HAL drivers
* Git and GitHub
* Git Bash on Windows
* PICSimLab, optional for simulation
* UART virtual terminal / serial monitor

---

## How It Works

The project initializes the STM32 system clock, GPIO, and USART1.

Inside the main loop, the firmware simulates different driving phases:

1. NORMAL mode acceleration
2. Regenerative braking
3. SPORT mode acceleration
4. ECO mode acceleration

At each update step, the application calls:

```c
EV_Update(&ev, accel_pct, brake_pct, 0.01f);
```

The EV control module updates the internal EV state, including:

* speed
* torque
* SOC
* range
* power
* regenerative braking status
* motor temperature
* active drive mode

Telemetry is printed over UART in a readable format.

---

## EV Control Module Explanation

### Main Data Structure

The EV state is stored in `EV_State_t`, which contains:

* `speed_kmh`
* `torque_Nm`
* `soc_pct`
* `range_km`
* `power_kW`
* `motor_temp_C`
* `regen_level`
* `drive_mode`
* `regen_active`

---

### Drive Modes

| Mode   | Torque Scaling | Behavior                           |
| ------ | -------------: | ---------------------------------- |
| ECO    |           0.6x | Lower torque, better range         |
| NORMAL |           1.0x | Balanced behavior                  |
| SPORT  |  1.3x, clamped | Higher response, higher energy use |

---

### Speed Model

The speed model uses a simplified inertia equation:

```text
acceleration = (motor_torque - drag) / mass_factor
```

Then speed is updated using Euler integration:

```text
speed = speed + acceleration × dt
```

The speed is clamped between 0 and 200 km/h.

---

### SOC Estimation

SOC is updated based on estimated power consumption:

```text
delta_soc = power × dt / battery_capacity
```

A simulation scale factor is used to make SOC changes visible during short demos.

---

### Regenerative Braking

If brake percentage is above the regen threshold, the module applies negative torque and enables regenerative braking mode.

This makes `regen_active = 1` and allows SOC recovery behavior to be represented in the simulation.

---

### Motor Temperature Model

The motor temperature increases based on motor power and cools down toward ambient temperature using a simple first-order thermal model.

---

## Folder Structure

```text
stm32-ev-control-module/
│
├── Core/
│   ├── Inc/
│   │   ├── ev_control.h
│   │   └── main.h
│   ├── Src/
│   │   ├── ev_control.c
│   │   └── main.c
│   └── Startup/
│
├── Drivers/
│
├── EV_Control.ioc
├── STM32F103C8TX_FLASH.ld
├── .project
├── .cproject
├── .mxproject
├── .gitignore
├── README.md
└── LICENSE
```

---

## Important Files

| File / Folder                         | Purpose                                                 |
| ------------------------------------- | ------------------------------------------------------- |
| `Core/Src/main.c`                     | Main application loop, UART printing, simulation phases |
| `Core/Src/ev_control.c`               | EV model implementation                                 |
| `Core/Inc/ev_control.h`               | Public EV control API and data structures               |
| `EV_Control.ioc`                      | STM32CubeIDE / CubeMX configuration                     |
| `Drivers/`                            | STM32 HAL and CMSIS drivers                             |
| `STM32F103C8TX_FLASH.ld`              | Linker script                                           |
| `.project`, `.cproject`, `.mxproject` | STM32CubeIDE project metadata                           |

---

## How to Build in STM32CubeIDE

1. Open STM32CubeIDE.
2. Go to:

```text
File → Import → Existing Projects into Workspace
```

3. Select the project folder.
4. Open the project.
5. Build the project using:

```text
Project → Build Project
```

or press:

```text
Ctrl + B
```

6. Make sure the build finishes with no errors.

---

## How to Run / Test

### Option 1: On STM32F103C8T6 Blue Pill

1. Connect the Blue Pill board using ST-Link.
2. Build the project in STM32CubeIDE.
3. Flash the firmware.
4. Open a UART serial monitor at:

```text
Baud Rate: 115200
Data Bits: 8
Stop Bits: 1
Parity: None
```

5. Observe EV telemetry printed periodically.

---

### Option 2: Using PICSimLab

PICSimLab can be used as a simulation environment if configured with a compatible STM32 Blue Pill setup.

General steps:

1. Build the project in STM32CubeIDE.
2. Generate the `.hex` or `.bin` output if needed.
3. Open PICSimLab.
4. Select the STM32 Blue Pill board.
5. Load the generated firmware file.
6. Open the virtual UART terminal.
7. Observe the telemetry output.

> Exact PICSimLab wiring and configuration may vary depending on the simulator version and board setup.

---

## Example UART Output

Example output format:

```text
EV_Control demo started
MODE=NORMAL | ACCEL= 50% | BRAKE=  0% | SPD=  2.31 km/h | TRQ= 100.00 Nm | SOC= 99.98% | RNG= 109.98 km | PWR=  1.25 kW | TEMP= 25.02 C | REGEN=0
MODE=NORMAL | ACCEL=  0% | BRAKE= 80% | SPD=  4.10 km/h | TRQ= -44.80 Nm | SOC= 99.99% | RNG= 109.99 km | PWR= -0.35 kW | TEMP= 25.03 C | REGEN=1
```

---

## Demo / Screenshots

Demo media can be added later.

Suggested files:

```text
images/uart-output.png
images/picsimlab-setup.png
demo/ev-control-demo.mp4
```

---

## What I Learned

Through this project, I practiced:

* Writing modular Embedded C code
* Creating `.c` and `.h` files for a reusable module
* Using structs and enums in C
* Simulating EV control logic
* Implementing drive modes
* Modeling regenerative braking
* Estimating SOC and range
* Printing telemetry over UART using STM32 HAL
* Organizing an STM32CubeIDE project for GitHub

---

## Future Improvements

* Replace fixed simulation phases with real ADC inputs
* Read accelerator and brake from potentiometers
* Add UART commands to change drive mode manually
* Add unit-test-like test cases for EV calculations
* Add dashboard integration using Python
* Add PICSimLab screenshots and demo video
* Add clearer documentation for the calculation formulas
* Integrate this module into the full EV ADAS Dashboard project

---

## Credits / Course Attribution

This project was implemented as part of my Embedded Systems learning practice during the Emertxe internship / EV Dashboard workshop.

The repository contains my own STM32CubeIDE project implementation and documentation. Course slides, PDFs, and proprietary training materials are not included.

---

## Disclaimer

This project is for educational purposes only.

It is not intended for use in a real vehicle, commercial EV controller, production automotive ECU, or safety-critical system.

---

## Author

Mohamed Ashraf
Mechatronics Student
Interested in Embedded Systems and AIoT

GitHub: https://github.com/mohamed-ashraf-eng

---

## License

This project can be released under the MIT License if all included code is your own implementation or generated STM32CubeIDE project code.

If any code is directly copied from proprietary course material, do not license it as fully original without permission.
