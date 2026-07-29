# Architecture Specification: Edge-Based Condition Monitoring Framework

**Project:** Autonomous Condition Monitoring & Predictive Maintenance Framework

**Target Hardware:** STM32F407G-DISC1 Microcontroller

**Primary Sensor Payload:** MPU6050 (6-Axis IMU / Vibration Telemetry), INA219 (Bi-Directional Current & Power Telemetry)

**Execution Engine:** ST X-CUBE-AI TinyML Runtime

---

## Executive Summary & Version Matrix

This document outlines the architectural evolution of the embedded condition monitoring framework from a lightweight, single-sensor bare-metal baseline (**Version 0**) to a fully integrated, multimodal TinyML inference platform (**Version 1**).

The primary driver for architectural refactoring was firmware compatibility and multimodal feature fusion: migrating from bare-metal C to the **STM32 Hardware Abstraction Layer (HAL)** was mandatory to natively support the **X-CUBE-AI** neural network runtime, manage memory alignment for tensor buffers, integrate the **INA219** current sensor on a shared I²C bus, and expand classification resolution from a 3-class load estimator to a 7-class electromechanical anomaly detection system.

| Architectural Dimension | Version 0 (v0: Bare-Metal Baseline) | Version 1 (v1: Multimodal TinyML Framework) |
| --- | --- | --- |
| **Firmware Abstraction** | Bare-Metal C (Direct Register Access) | STM32 HAL (Hardware Abstraction Layer) |
| **Sensor Payload** | **Single-Modal:** MPU6050 (IMU only) | **Multimodal:** MPU6050 (IMU) + INA219 (Current/Power) |
| **I²C Driver Implementation** | Custom Register-Level Polling / Bit-Banging | Shared Hardware `HAL_I2C` Bus + APB1 RCC Resets |
| **Analytical Engine** | Classical Threshold / Rule-Based Monitoring | On-Device Neural Network Inference (X-CUBE-AI) |
| **Machine States Monitored** | **3 Classes:** Ideal, Minor Load, Major Load | **7 Classes:** Motor Off, Ideal State, Ideal State on Flatten Position, Minor Obstacle, Heavy Obstacle, Minor Load, Major Load |
| **Data Processing Pipeline** | Raw 16-bit Integer Register Polling | Scaled Float Pipeline (g-force + Current_LSB scaling) |
| **Primary Architectural Bottleneck** | Incompatible with embedded AI runtime libraries | Requires strict I²C bus sharing & power-domain management |

---

## Architectural Rationale: Why Migrate from Bare-Metal to HAL?

While bare-metal firmware offers minimum memory overhead and absolute execution speed, it presents an engineering barrier when deploying modern multimodal edge machine learning models:

1. **X-CUBE-AI Runtime Dependency:** STMicroelectronics' X-CUBE-AI engine generates optimized C-model wrappers (`app_x-cube-ai.c`) that natively hook into STM32Cube HAL hardware descriptors and DMA structures. Integrating auto-generated neural network libraries into a bare-metal environment requires manually writing API shims and custom memory allocators, introducing significant stability risks.
2. **Multimodal Feature Fusion & Bus Arbitration:** Version 1 fuses mechanical vibration data (MPU6050) with electrical current telemetry (INA219) over a shared I²C1 bus. HAL provides thread-safe peripheral locking and standardized timeout structures (`HAL_I2C_Mem_Read`), preventing bus collisions when polling two separate slave addresses sequentially.
3. **Buffer Alignment & DMA Readiness:** Neural network inference on ARM Cortex-M4F processors requires strict memory alignment for tensor input/output buffers to exploit SIMD and FPU instructions. HAL provides standardized abstractions for buffer handling and clock configuration (`RCC`) that ensure stable execution during heavy mathematical computation.

---

## Architecture Specification: Version 0 (v0)

### Bare-Metal Data Collection & Basic Monitoring

Version 0 was designed as a proof-of-concept data acquisition pipeline to establish baseline vibration telemetry and validate I²C communication with the MPU6050 IMU.

```
+-----------------------------------------------------------------+
|                       V0 BARE-METAL ARCHITECTURE                |
|                                                                 |
|  +----------------+      Raw I2C      +----------------------+  |
|  |    MPU6050     |------------------>| STM32F407 (Bare Metal) |  |
|  |  6-Axis IMU    |  Register Reads   |  - Custom I2C Driver |  |
|  +----------------+                   |  - Integer Math Only |  |
|                                       +----------------------+  |
|                                                  |              |
|                                                  v              |
|                                       +----------------------+  |
|                                       | Condition Classifier |  |
|                                       | (3 Machine States)   |  |
|                                       +----------------------+  |
+-----------------------------------------------------------------+

```

### 1. Firmware & Driver Layer

* **Driver Abstraction:** Written entirely in bare-metal C by directly manipulating peripheral registers (`I2C_CR1`, `I2C_SR1`, `I2C_DR`, and `RCC_APB1ENR`).
* **Math Processing:** Used 16-bit signed integer arithmetic (`int16_t`) directly pulled from high/low data registers (`0x3B` through `0x40`). No floating-point normalization was performed on-chip to conserve CPU cycles.
* **Execution Flow:** Blocking, synchronous polling loop. The microcontroller continuously queried the MPU6050 registers, verified the `ADDR` and `RXNE` hardware flags, and parsed raw data frames.

### 2. Condition Monitoring Classification Scheme (3 Classes)

Version 0 utilized basic threshold-banding to categorize the physical stress of the machine into three operational states:

* **Ideal State:** Normal operational vibration baseline; motor running under no-load or nominal mechanical resistance.
* **Minor Load:** Slight dampening or frequency shift in vibration signatures, indicating modest mechanical strain or payload addition.
* **Major Load:** Pronounced amplitude surge or severe damping in IMU axes, indicating high mechanical torque, binding, or maximum operational capacity.

### 3. Architectural Limitations

* **Single-Modal Blindness:** Without electrical telemetry, the system could not differentiate between a motor binding due to mechanical load versus an external impact.
* **No Edge AI Integration:** Could not host pre-trained TensorFlow/Keras models converted via X-CUBE-AI due to missing HAL system hooks and memory abstraction layers.

---

## Architecture Specification: Version 1 (v1)

### Multimodal HAL-Integrated TinyML Condition Monitoring Framework

Version 1 represents an embedded, multimodal edge-AI architecture. By standardizing on the STM32 HAL and integrating the **INA219** sensor alongside the **MPU6050** on shared I²C pins (`PB6`/`PB7`), the framework fuses mechanical vibration and electrical torque signatures into a unified 7-class condition monitoring system.

```
+-----------------------------------------------------------------------------------+
|                     V1 MULTIMODAL TINYML EDGE-AI ARCHITECTURE                     |
|                                                                                   |
|  +----------------+      HAL_I2C_Mem_Read    +---------------------------------+  |
|  |    MPU6050     |------------------------->|       STM32F407 (HAL Layer)     |  |
|  |  6-Axis IMU    |    Float Cast (/16384.0f)| - Shared I2C1 Bus Arbitration   |  |
|  +----------------+                          | - APB1 RCC Hardware Bus Reset   |  |
|  +----------------+      HAL_I2C_Mem_Read    | - Global Buffer Memory Mapping  |  |
|  |    INA219      |------------------------->| - Multimodal Feature Fusion   |  |
|  | Current Sensor |    Current_LSB Scaling   +---------------------------------+  |
|  +----------------+                                           |                   |
|                                                               v                   |
|                                              +---------------------------------+  |
|                                              |      X-CUBE-AI Runtime Engine   |  |
|                                              |  - Single-Shot Non-Blocking Call|  |
|                                              |  - Stripped Validation Do-While |  |
|                                              +---------------------------------+  |
|                                                               |                   |
|                                                               v                   |
|                                              +---------------------------------+  |
|                                              |   7-Class Inference Output      |  |
|                                              |  (Motor Off -> Major Load)      |  |
|                                              +---------------------------------+  |
+-----------------------------------------------------------------------------------+

```

### 1. Firmware & Multimodal Acquisition Layer

* **Shared I²C Driver Abstraction:** Configured via STM32Cube HAL (`HAL_I2C_Mem_Read`). Both sensors share the same physical bus, requiring sequential polling with pre-boot APB1 RCC register resets to clear trapped `BUSY` flags and low GPIO slew rates (`GPIO_SPEED_FREQ_LOW`) to prevent clock-edge ringing.
* **Dual-Pipeline Data Normalization:**
* **MPU6050 (Vibration):** Raw 16-bit integer readings are explicitly cast to floating-point values and scaled by the sensor's sensitivity range (`/ 16384.0f` for ±2g resolution).
* **INA219 (Electrical Load):** Raw 16-bit digital register counts from `INA219_REG_CURRENT` (`0x04`) are multiplied by the calibrated `Current_LSB` conversion metric to generate true float Amperes (`current_A`), preventing math overflows.


* **Neural Network Runtime:** Uses an **X-CUBE-AI** generated model wrapper. The default ST validation architecture was refactored by removing the trapping `do { ... } while();` loop, converting inference execution into a deterministic, single-shot function called inside `main.c`.

### 2. Expanded Multiclass Monitoring Scheme (7 Classes)

By combining IMU g-force data with INA219 motor current telemetry, the TinyML model correlates mechanical shock with electrical strain to classify 7 distinct operational conditions:

1. **Motor Off:** Static environmental baseline; zero mechanical vibration detected and INA219 current reads ~0.00A.
2. **Ideal State:** Motor active on nominal surface; clean harmonic vibration baseline with nominal, stable current draw.
3. **Ideal State on Flatten Position:** Motor active while the platform is oriented on a completely horizontal, level plane (reference baseline for tilt/pitch separation).
4. **Minor Obstacle:** High-frequency transient shock spike on IMU Z/X axes with no sustained surge in INA219 motor current (surface irregularity or small debris).
5. **Heavy Obstacle:** Severe multi-axis impact impulse paired with a brief, sharp INA219 current spike as the motor encounters sudden physical resistance.
6. **Minor Load:** Sustained elevation in INA219 current draw paired with minor dampening of high-frequency motor harmonics, indicating moderate working strain.
7. **Major Load:** Maximum sustained INA219 current draw (approaching stall torque limits) combined with pronounced vibration dampening and torsional IMU shifts, indicating peak mechanical stress or heavy payload.

### 3. Key Architectural Enhancements over v0

* **Electromechanical Correlation:** Adding the INA219 eliminates "false positives" in anomaly detection—the AI model can now distinguish whether a vibration drop is caused by physical surface damping or an electrical load increase.
* **Decoupled Acquisition and Inference:** Data collection arrays for both sensors are globally declared, allowing real-time multi-signal debugging via **STM32CubeMonitor** without halting or starving the neural network inference pipeline.
* **Resilient Bus & Power Hardening:** Integrates Split-Power Domain wiring (3.3V external supply for INA219/MPU6050, 2.9V USB rail for MCU) and power-sequencing delays to ensure both peripheral sensors wake up and stabilize before I²C enumeration begins.
