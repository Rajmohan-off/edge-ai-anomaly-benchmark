**Date:** 23 Jul 2026

**Issue:** INA219 current reading (current_A) displayed wildly exaggerated values, spiking to ~65 in STM32CubeMonitor, corrupting the feature matrix for the TinyML model.

**Issue Location:** Firmware signal processing / INA219 Current Register scaling conversion math.

**Diagnosis method:**
- Observed the current_A plot in STM32CubeMonitor bouncing erratically between ~0 and ~65, while the physical load current was negligible.
- Cross-referenced the raw 16-bit register readings from INA219_REG_CURRENT (0x04) against the converted floating-point variable current_A.
- Identified that raw digital values from the register were being assigned directly to the current output variable without applying the mandatory datasheet scaling metric.

**Mitigation 1:** Applied the exact Current_LSB division constant per the INA219 datasheet (e.g., measuring the actual current in amperes by taking the raw register reading and multiplying it by the Current_LSB value).
- Outcome: The bug resolved. current_A normalized to true physical values, yielding a clean baseline telemetry trace suitable for the predictive maintenance feature extraction.

**Hypothetical Root cause:** The INA219's internal Current Register (0x04) stores raw 16-bit values that represent digital counts. To translate these raw digital measurements into actual Amperes, the software must process the raw data by multiplying it by the specific Current_LSB value used during the sensor's calibration calculation. Skipping this explicit scaling metric caused the unscaled raw integer counts to be directly interpreted as full Amperes, flooding the data pipeline with math overflows and arbitrary spikes that rendered the TinyML conditional monitor framework unusable.

**Learning:** Sensor data sheets must be thoroughly reviewed for registers requiring custom software scaling factors (like the INA219 Current_LSB). Never pipe raw register counts directly into a TinyML pipeline without verifying the explicit conversion mathematics against the hardware's datasheet metrics.

**Before the resolve**
<img width="1216" height="805" alt="image" src="https://github.com/user-attachments/assets/11b4d75c-5749-4016-9406-a53dab752484" />

**After the resolve**
<img width="1210" height="801" alt="Good_run_1" src="https://github.com/user-attachments/assets/35f1e489-6bc8-4176-b1d9-15287293627b" />
