**Date:** 16 Jul 2026

**Issue:** Code was permanently stuck in a while loop waiting for the ADDR (Address) flag to set.

**Issue Location:** Driver >> src >> stm32f407xx_drv_I2C.c >> I2C_MasterSendData.

**Diagnosis method:** 
- Used STM32CubeIDE debugger.
- Single-stepped through I2CMasterStartCondition().
- Monitored I2C_SR1 and I2C_CR1 using the SFR view.
- Observed that ADDR flag never asserted.

**Mitigation 1:** Disconnect and reassemble all the wiring then run the program again to confirm the bug isn't arraise due to loose connections, or parasitic capacitance
- Outcome: The bug still persist

**Mitigation 2:** Revise the GPIO_PinSpeed from HIGH to LOW, which lower the slew rate of STM32 and matches the baud rate of MPU6050
- Outcome: The bug resolved

**Hypothetical Root cause:** The STM32 GPIO output speed for the I2C pins was configured to HIGH (50 MHz output speed setting) while the I2C peripheral operated at 100 kHz Standard Mode. After changing the GPIO output speed from HIGH to LOW, the communication became stable and the ADDR flag was successfully asserted.
A possible explanation is that the faster GPIO edge transitions degraded signal integrity, causing the STM32 to fail to detect the ACK from the MPU6050.

**Learning:** When debugging I²C communication, GPIO configuration, bus timing, and signal integrity all should be verified alongside peripheral configuration.
