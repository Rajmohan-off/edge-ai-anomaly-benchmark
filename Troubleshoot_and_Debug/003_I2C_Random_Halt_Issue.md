**Date:** 18 Jul 2026

**Issue:** I2C communication randomly halted, causing sensor data acquisition to freeze or fail to start, especially after a soft reset or reprogramming.

**Issue Location:** Firmware initialization sequence / RCC (Reset and Clock Control) configuration for the I2C peripheral.

**Diagnosis method:**
- Used the STM32CubeIDE debugger to step through the initialization code and monitor the I2C registers (I2C_SR1, I2C_SR2).
- Observed that the I2C peripheral was occasionally booting up in a "busy" state (BUSY flag set) without any actual traffic on the wires.
- Verified that physical pull-up resistors and GPIO configurations were correct.

**Mitigation 1:** Performed a hard power cycle (unplugging and replugging the USB power) to completely drain the hardware.
- Outcome: The bug temporarily resolved on a cold boot, but immediately returned upon soft-resetting the board or launching a new debug session, making the TinyML data pipeline highly unreliable.

**Mitigation 2:** Modified the initialization firmware to explicitly assert and then release the I2C peripheral reset flag (via the RCC APB1 reset register) immediately before executing the standard I2C configuration code.
- Outcome: The bug resolved permanently. The sensor readings initialized reliably on every single boot and soft reset.

**Hypothetical Root cause:** If the STM32 is paused, reprogrammed, or soft-reset in the middle of an active I2C transaction, the I2C hardware module is suddenly interrupted while waiting for a clock cycle or an Acknowledge (ACK) bit. Without an explicit hardware reset command during the next boot, the internal silicon remains deadlocked in that incomplete state. Applying the peripheral reset clears all internal state machines, forcing the I2C module to drop any phantom transactions and start with a clean slate.

**Learning:** Always explicitly force a hardware reset on communication peripherals (I2C, SPI, UART) before configuring their parameters in firmware. Relying solely on the default boot state leaves the conditional monitor framework vulnerable to bus lockups caused by previous execution cycles.
