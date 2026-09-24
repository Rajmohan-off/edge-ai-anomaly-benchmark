# STM32 TinyML Inference - Heterogeneous Sensor Fusion

This directory contains the embedded TinyML implementation of the **heterogeneous sensor-fusion condition-monitoring system** running on an **STM32F407**.

The system combines:

* **MPU6050** vibration/acceleration sensing
* **INA226** electrical current sensing
* A **heterogeneous TinyML model**
* STM32F407-based on-device inference
* **USART2** for reliable status communication
* A **USB-to-TTL converter** for displaying inference results on a PC

The embedded system classifies the observed machine/fan condition into **seven machine states** and reports the result through the serial interface.

---

# 1. System Overview

The complete embedded inference pipeline is:

```text
                  ┌──────────────────┐
                  │     MPU6050      │
                  │ Vibration / Acc. │
                  └────────┬─────────┘
                           │
                           │ I²C
                           │
                           ▼
                    ┌──────────────┐
                    │              │
                    │   STM32F407  │
                    │              │
                    │ Sensor Data  │
                    │      ↓       │
                    │ Preprocessing│
                    │      ↓       │
                    │ Heterogeneous│
                    │ TinyML Model │
                    │      ↓       │
                    │ 7-State      │
                    │ Classification│
                    │              │
                    └──────┬───────┘
                           │
                           │ I²C
                           │
                  ┌────────▼─────────┐
                  │      INA226      │
                  │ Current / Power  │
                  └──────────────────┘


                    STM32F407
                        │
                        │ USART2
                        │
                        ▼
                USB-to-TTL Converter
                        │
                        ▼
                       PC
```

The model uses heterogeneous information from both:

* **Mechanical domain:** MPU6050 acceleration
* **Electrical domain:** INA226 current measurement

This allows the system to evaluate machine state using complementary physical measurements.

---

# 2. Hardware

## Required Hardware

| Component            | Purpose                               |
| -------------------- | ------------------------------------- |
| STM32F407            | Main MCU and TinyML inference         |
| MPU6050              | 3-axis vibration/acceleration sensing |
| INA226               | Electrical current measurement        |
| USB-to-TTL converter | USART-to-PC communication             |
| ST-LINK              | Firmware programming/debugging        |
| Test fan/machine     | Condition-monitoring target           |

---

# 3. STM32CubeMX Configuration

The embedded project is configured using **STM32CubeMX / STM32CubeIDE**.

The following peripheral configuration is required for the current implementation.

---

## 3.1 I²C1 Configuration

I²C1 is configured in **Master mode** using:

```text
I²C Speed Mode:       Standard Mode
I²C Clock Speed:      100000 Hz
Primary Address:      7-bit
Clock No Stretch:     Disabled
Dual Address:         Disabled
General Call:         Disabled
```

Therefore, the sensor communication operates at:

```text
100 kHz I²C
```

### I²C1 Pins

| STM32F407 Pin | Function |
| ------------- | -------- |
| **PB6**       | I²C1_SCL |
| **PB7**       | I²C1_SDA |

The resulting connection is:

```text
STM32F407 PB6 (SCL) ───── SCL
STM32F407 PB7 (SDA) ───── SDA
                         │
                   I²C sensor bus
                         │
                  MPU6050 / INA226
```

Both sensors share the I²C bus and are addressed using their respective I²C addresses.

### CubeMX Configuration Summary

```text
Peripheral:        I2C1
Mode:              I2C
Speed Mode:        Standard Mode
Clock Speed:       100000 Hz
Address Length:    7-bit

SCL:               PB6
SDA:               PB7
```

---

# 4. USART2 Configuration

USART2 is used exclusively for **status and inference communication between the STM32F407 and the PC**.

It is configured in:

```text
Mode:              Asynchronous
Baud Rate:         115200
Word Length:       8 Bits
Parity:            None
Stop Bits:         1
Data Direction:    Receive and Transmit
Over Sampling:     16 Samples
```

This corresponds to the commonly used:

```text
115200 baud, 8-N-1
```

configuration.

---

## 4.1 USART2 Pins

| STM32F407 Pin | Function  |
| ------------- | --------- |
| **PA2**       | USART2_TX |
| **PA3**       | USART2_RX |

The physical communication path is:

```text
STM32F407 PA2 (TX)
        │
        ▼
USB-to-TTL RX


STM32F407 PA3 (RX)
        ▲
        │
USB-to-TTL TX


STM32F407 GND ───── USB-to-TTL GND
```

### Important

TX and RX must be crossed:

```text
STM32 TX → USB-TTL RX
STM32 RX ← USB-TTL TX
```

The USB-to-TTL converter should use **3.3 V-compatible logic levels** for the STM32 USART pins.

---

# 5. Complete Pin Configuration

For quick reference, the primary communication pins used by this implementation are:

| Peripheral | STM32 Pin | Function | Configuration         |
| ---------- | --------- | -------- | --------------------- |
| I²C1       | **PB6**   | SCL      | 100 kHz Standard Mode |
| I²C1       | **PB7**   | SDA      | 100 kHz Standard Mode |
| USART2     | **PA2**   | TX       | 115200 baud           |
| USART2     | **PA3**   | RX       | 115200 baud           |

Therefore:

```text
                STM32F407
             ┌─────────────┐
             │             │
 PB6 ────────┤ I²C1_SCL    │
 PB7 ────────┤ I²C1_SDA    │
             │             │
 PA2 ────────┤ USART2_TX   │──────► USB-TTL RX
 PA3 ────────┤ USART2_RX   │◄────── USB-TTL TX
             │             │
 GND ────────┤ GND         │─────── USB-TTL GND
             │             │
             └─────────────┘
```

---

# 6. Sensor Data Used by the TinyML Model

The heterogeneous model receives four input features:

```text
accel_g_x
accel_g_y
accel_g_z
current_mA
```

The first three features are obtained from the MPU6050:

```text
MPU6050
 ├── accel_g_x
 ├── accel_g_y
 └── accel_g_z
```

The fourth feature is obtained from the INA226:

```text
INA226
 └── current_mA
```

These are combined into the model input:

```text
[accel_g_x, accel_g_y, accel_g_z, current_mA]
```

The **order of these features must remain identical to the order used during model training**.

---

# 7. Heterogeneous TinyML Model

The embedded implementation uses the **heterogeneous sensor-fusion model** developed for Research 01.

The model architecture is:

```text
Input
4 features
    │
    ▼
Dense
16 neurons
    │
    ▼
Dense
32 neurons
    │
    ▼
Dense
16 neurons
    │
    ▼
Dense
7 neurons
    │
    ▼
Softmax
    │
    ▼
Machine-state prediction
```

The four input features combine:

```text
3 × vibration features
        +
1 × electrical-current feature
        ↓
Heterogeneous sensor fusion
```

The model contains **1,271 trainable parameters**.

---

# 8. Seven Machine States

The TinyML model classifies the machine/fan into seven states.

| Class | Reliable Status    |
| ----: | ------------------ |
| **0** | `Ideal off`        |
| **1** | `Ideal off fallen` |
| **2** | `Ideal run`        |
| **3** | `Ideal run fallen` |
| **4** | `Minor load`       |
| **5** | `Major load`       |
| **6** | `Major Obstacle`   |

The class ID is converted into a human-readable status before transmission through USART.

---

# 9. Reliable Status Output

The STM32 reports the final classification through USART using the following output format:

```text
Reliable status: <status>
```

For example:

```text
Reliable status: Ideal off
```

or:

```text
Reliable status: Ideal run
```

or:

```text
Reliable status: Minor load
```

The possible outputs are:

```text
Reliable status: Ideal off
Reliable status: Ideal off fallen
Reliable status: Ideal run
Reliable status: Ideal run fallen
Reliable status: Minor load
Reliable status: Major load
Reliable status: Major Obstacle
```

This output can be viewed on a PC using a serial terminal connected through the USB-to-TTL converter.

---

# 10. Running the Embedded System

## Step 1 — Open the project

Open the STM32 project in:

```text
STM32CubeIDE
```

The project was configured using STM32CubeMX.

---

## Step 2 — Verify CubeMX configuration

Before building, verify:

### I²C1

```text
I2C1
├── Mode: I2C
├── Speed: Standard Mode
├── Clock: 100000 Hz
├── Address: 7-bit
├── SCL: PB6
└── SDA: PB7
```

### USART2

```text
USART2
├── Mode: Asynchronous
├── Baud Rate: 115200
├── Word Length: 8 Bits
├── Parity: None
├── Stop Bits: 1
├── TX: PA2
└── RX: PA3
```

---

# 11. Connect the Sensors

Connect the I²C sensor bus:

```text
STM32F407 PB6 → SCL
STM32F407 PB7 → SDA
STM32F407 GND → Sensor GND
STM32F407 3.3V → Sensor VCC
```

Connect both the MPU6050 and INA226 to the appropriate I²C bus.

Verify that the sensor addresses do not conflict.

---

# 12. Connect the USB-to-TTL Converter

Connect:

```text
STM32 PA2 (USART2_TX) → USB-TTL RX
STM32 PA3 (USART2_RX) → USB-TTL TX
STM32 GND             → USB-TTL GND
```

Then connect the USB-to-TTL converter to the PC.

---

# 13. Build and Flash

In STM32CubeIDE:

```text
Project
   ↓
Build Project
```

Resolve any compilation errors before programming the board.

Connect the STM32 through ST-LINK and program the firmware:

```text
Run → Run
```

or use the Debug configuration if required.

---

# 14. Configure the Serial Terminal

Open a serial terminal such as:

* PuTTY
* Tera Term
* CoolTerm

Select the COM port assigned to the USB-to-TTL converter.

Configure:

```text
Baud rate:   115200
Data bits:   8
Parity:      None
Stop bits:   1
Flow control: None
```

These settings must match the USART2 configuration in STM32CubeMX.

---

# 15. Start the System

After programming the STM32:

1. Power the system.
2. Open the serial terminal.
3. Select the correct COM port.
4. Confirm the terminal is configured for `115200 8-N-1`.
5. Reset the STM32 if required.
6. Allow the sensors to initialize.
7. Place the machine/fan in the desired operating condition.
8. Observe the `Reliable status:` messages.

The overall runtime sequence is:

```text
STM32 startup
      ↓
Initialize I²C1
      ↓
Initialize MPU6050
      ↓
Initialize INA226
      ↓
Initialize TinyML model
      ↓
Acquire sensor measurements
      ↓
Prepare four model features
      ↓
Run TinyML inference
      ↓
Determine machine state
      ↓
Generate reliable status
      ↓
USART2 transmission
      ↓
USB-to-TTL
      ↓
PC serial terminal
```

---

# 16. Example Runtime

A successful system may produce output such as:

```text
Reliable status: Ideal off
Reliable status: Ideal off
Reliable status: Ideal run
Reliable status: Ideal run
Reliable status: Minor load
Reliable status: Major load
Reliable status: Major Obstacle
```

The status depends on the physical condition detected by the sensor-fusion model.

---

# 17. Troubleshooting

## No USART output

Check:

```text
✓ USB-to-TTL converter detected
✓ Correct COM port selected
✓ Baud rate = 115200
✓ Data bits = 8
✓ Parity = None
✓ Stop bits = 1
✓ STM32 TX → USB-TTL RX
✓ STM32 RX → USB-TTL TX
✓ Common GND
✓ STM32 is powered
```

---

## Garbled output

The most likely cause is a mismatch between the STM32 USART configuration and the serial terminal.

Verify:

```text
115200 baud
8 data bits
No parity
1 stop bit
```

Also verify that the STM32 clock configuration is correct.

---

## No sensor data

Check:

```text
PB6 → SCL
PB7 → SDA
```

Then verify:

* Sensor power
* Ground
* I²C addresses
* I²C pull-ups
* I²C1 initialization
* Sensor initialization routines

---

## Incorrect classification

Verify that the embedded preprocessing exactly matches the preprocessing used during model training.

In particular, check:

```text
Input order
Normalization
Scaling
Feature units
```

The heterogeneous model expects:

```text
[accel_g_x, accel_g_y, accel_g_z, current_mA]
```

Changing the order or numerical scaling can produce incorrect predictions even when the TinyML model itself is functioning correctly.

---

# 18. Research 01 Embedded Deployment

This embedded implementation forms the deployment stage of **Research 01 — Heterogeneous Sensor Fusion**.

The research compares:

```text
INA-only
    vs
MPU-only
    vs
MPU + INA heterogeneous
```

The heterogeneous deployment specifically evaluates whether combining vibration and electrical-current information provides useful machine-state classification while remaining practical for an STM32-class embedded platform.

The following deployment metrics are being evaluated:

```text
TFLite model size
Flash usage
RAM usage
Tensor arena
Inference latency
CPU cycles per inference
Energy per inference
```

These measurements complement the classification metrics:

```text
Accuracy
Macro F1
Weighted F1
Confusion matrix
```

Together, these measurements provide both **ML performance** and **embedded deployment cost** for the heterogeneous condition-monitoring system.

---

# 19. Repository

Main project:

https://github.com/Rajmohan-off/edge-ai-anomaly-benchmark

The repository contains the research documentation, dataset information, model development and embedded implementation for the Edge AI condition-monitoring benchmark.
