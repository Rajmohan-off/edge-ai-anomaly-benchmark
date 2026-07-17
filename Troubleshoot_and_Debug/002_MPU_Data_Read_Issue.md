**Date:** 17 Jul 2026

**Issue:** The STM32 incorrectly interprets negative accelerometer data as massive positive spikes, limiting the graph to a 3.99g max cap instead of showing true directional orientation.

**Issue Location:** Driver >> inc >> MPU6050_drv.h

**Diagnosis method:** 
- Used STM32CubeMonitor
- Import .elf file of the project into my variables
- Deployed and started the trace
- Observed that the analog sensor data looked like a digital square wave, artificially capped at ~3.99.

**Mitigation 1:** Changes the datatype of MPU6050 accelerometer variables from uint16 to int16 
- Outcomes: The bug resolved

**Root cause:** The MPU6050 transmits raw data as two's complement signed integers to represent both positive and negative physical forces. Because the STM32 memory was allocated as uint16, any negative sensor reading caused an integer underflow, consider as massive positive number. This caused the final floating-point calculation to spike to ~3.99g instead of dipping below zero.

**Learning:** Kinematic sensors output signed directional data. Memory allocation and bit-shift casting must explicitly use signed variables to accurately process negative physical movements. 

**Before the resolve:**
<img width="1519" height="957" alt="image" src="https://github.com/user-attachments/assets/8e7378b3-9819-4a19-b8c9-424c99e43939" />

**After the resolve:** 
<img width="1516" height="957" alt="image" src="https://github.com/user-attachments/assets/ecd85d3b-03a9-479a-9e5d-1dfdd195c426" />
