# Arduino + LabVIEW Signal Analyzer

## Description
A collaborative embedded systems project that generates, transmits, and visualizes signals across three platforms: an FPGA (VHDL), an Arduino Mega (C), and a LabVIEW host application.

This repository contains **my contribution**: the Arduino (C) translator and the LabVIEW host application.

## System Overview
- **FPGA (Basys 2, VHDL)** — generates the signal (implemented by a teammate)
- **Arduino Mega (C)** — acts as a translator between the FPGA and the PC, handling SPI, UART, timers, and ADC
- **LabVIEW (host)** — sends signal parameters (frequency, signal type) to the Arduino, reads the returned data, and plots the waveform

## My Contribution
- **Arduino (C)**: SPI and UART communication between the FPGA and the PC, timer configuration, ADC handling, and the main control loop
- **LabVIEW**: host application that configures the signal, communicates over the serial port, and visualizes the received waveform, including checksum handling

## How to Run
### Arduino
1. Open `arduino/Oscillscope Mega.cproj` in Microchip Studio (Atmel Studio 7).
2. Build and flash to the Arduino Mega.

### LabVIEW
1. Open `labview/LabviewOscilloscope.vi` in LabVIEW.
2. Run the VI and use the interface to configure the signal.

> Note: The project requires the LabVIEW runtime and the appropriate serial port drivers.

## Concepts
- Bare-metal C on AVR (SPI, UART, timers, ADC)
- LabVIEW host application and serial communication
- Cross-platform system integration (FPGA + MCU + PC)
- Checksum-based data integrity

## Project Structure
- `arduino/` — Arduino Mega firmware (C)
- `labview/` — LabVIEW host application (.vi files)
- `docs/` — project report and reference manuals

## Note
This was a group project. The VHDL signal generator on the Basys 2 FPGA was implemented by a teammate. The Arduino and LabVIEW parts are my work.