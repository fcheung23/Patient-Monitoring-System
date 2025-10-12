# Patient Monitoring System

![Project Image](images/project_photo.heic)  
*Photo of the Patient Monitoring Device in action*

## Description
This project is a real-time patient monitoring platform built on the **STM32 Nucleo-F446RE** (ARM Cortex-M4) using **FreeRTOS APIs**.  
It simultaneously collects and processes differing physiological data, including **pulse, temperature, and motion**, using the **KY-039, TMP36, and ADXL345 sensors**.  

I applied embedded engineering principles by developing **bare-metal sensor drivers**, implementing **FreeRTOS task management**, and integrating multiple sensors for real-time physiological monitoring.

## Features
- Low-level drivers for ADC, DMA, and I2C to reliably read multiple sensors.  
- Prioritized tasks and queue sets in FreeRTOS for synchronized, real-time data acquisition.  
- A UART logging module for serial output and performance debugging.  
- A data fusion layer that combines sensor readings into unified health metrics with abnormality alerts.

## Skills Learned
- **Bare-metal programming:** Bare-metal programming through developing drivers for ADC, DMA, and I2C  
- **RTOS integration:** FreeRTOS task scheduling, queues/queuesets, and synchronized data acquisition  
- **Sensor interfacing:** KY-039 (pulse), TMP36 (temperature), ADXL345 (motion)  
- **Data processing:** Combining multiple sensor readings into unified health metrics with alerts  
- **Development tools:** STM32CubeIDE, debugging, and flashing to STM32 boards

## Setup / Installation
1. Clone the repository:
```bash
git clone https://github.com/fcheung23/Patient-Monitoring-System.git
cd Patient-Monitoring-System
```
1. Open **STM32CubeIDE** and import the `Patient-Monitoring-System` folder (*File → Import → Existing Projects into Workspace*.
1. Build and flash to the STM32 board.
