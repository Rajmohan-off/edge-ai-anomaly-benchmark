# Dataset Specification

## 1. Dataset Overview

This research uses a heterogeneous sensor dataset collected from an STM32F407-based rotating-machine test platform.

The dataset combines:

* **MPU6050** three-axis acceleration measurements for mechanical/vibration information.
* **INA226** current measurements for electrical-load information.
* A categorical machine-state label representing the experimental operating condition.

The dataset is used to evaluate whether combining mechanically and electrically derived sensor information provides measurable benefits over single-sensor condition monitoring under resource-constrained Edge AI deployment.

Three sensor configurations are evaluated:

1. **MPU6050-only**
2. **INA226-only**
3. **MPU6050 + INA226 heterogeneous sensor fusion**

---

## 2. Data Acquisition Hardware

### 2.1 Microcontroller

| Component               | Specification    |
| ----------------------- | ---------------- |
| MCU                     | STM32F407        |
| Sensor communication    | I²C              |
| I²C mode                | Standard Mode    |
| I²C clock frequency     | 100 kHz          |
| Data monitoring/logging | STM32CubeMonitor |

The STM32F407 acts as the primary data-acquisition and embedded-processing platform.

### 2.2 MPU6050

The MPU6050 provides three-axis acceleration measurements.

| Feature     | Unit | Description         |
| ----------- | ---- | ------------------- |
| `accel_g_x` | g    | X-axis acceleration |
| `accel_g_y` | g    | Y-axis acceleration |
| `accel_g_z` | g    | Z-axis acceleration |

The three acceleration axes are retained independently to preserve directional vibration information.

### 2.3 INA226

The INA226 is used to measure the electrical current associated with the machine.

| Feature      | Unit | Description              |
| ------------ | ---- | ------------------------ |
| `current_mA` | mA   | Measured machine current |

The electrical measurement provides a complementary sensing modality to the mechanical measurements obtained from the MPU6050.

The INA226 replaces the previously used INA219 configuration and should be referred to as **INA226 throughout the research documentation**.

---

## 3. Communication Configuration

Both sensor interfaces are operated through the STM32F407 I²C peripheral.

The I²C configuration used for the experimental platform is:

```text
I²C mode       : Standard Mode
Clock          : 100 kHz
```

100 kHz corresponds to the Standard Mode I²C configuration. ST's STM32 documentation provides the same Standard Mode / 100 kHz configuration.

---

## 4. Data Acquisition Software

Sensor and machine-state variables are monitored through **STM32CubeMonitor**.

The acquired data are recorded directly into **CSV files**.

STM32CubeMonitor supports CSV logging from its processing node and provides both single-column and multiple-column CSV formats. The multiple-column format stores the acquisition timestamp followed by the monitored variables.

For this research, the exported CSV files constitute the raw data acquisition artifacts.

The raw acquisition workflow is:

```text
MPU6050 ─────┐
             │
             ├──> STM32F407 ──> STM32CubeMonitor ──> CSV
             │
INA226 ──────┘
```

---

## 5. Dataset Schema

After preprocessing and synchronization, the principal dataset contains the following columns:

| Column       | Type    | Description                 |
| ------------ | ------- | --------------------------- |
| `accel_g_x`  | float   | MPU6050 X-axis acceleration |
| `accel_g_y`  | float   | MPU6050 Y-axis acceleration |
| `accel_g_z`  | float   | MPU6050 Z-axis acceleration |
| `current_mA` | float   | INA226 current measurement  |
| `Output`     | integer | Machine-state class label   |

The heterogeneous model therefore receives the following input vector:

```text
[accel_g_x, accel_g_y, accel_g_z, current_mA]
```

The single-modality experiments use subsets of these features.

---

## 6. Sensor Configurations

### 6.1 MPU6050-only

The vibration-only configuration uses:

```text
[accel_g_x,
 accel_g_y,
 accel_g_z]
```

Input dimension:

```text
3
```

This configuration evaluates the information available from mechanical vibration measurements alone.

---

### 6.2 INA226-only

The electrical-only configuration uses:

```text
[current_mA]
```

Input dimension:

```text
1
```

This configuration evaluates the ability of electrical current information to distinguish the experimental machine states independently.

---

### 6.3 Heterogeneous Sensor Fusion

The heterogeneous configuration combines both sensor modalities:

```text
[accel_g_x,
 accel_g_y,
 accel_g_z,
 current_mA]
```

Input dimension:

```text
4
```

This configuration evaluates whether complementary mechanical and electrical information improves machine-state classification.

---

## 7. Experimental Recording Protocol

For each machine condition, **five independent recordings** are collected.

Each recording has a duration between:

```text
10–60 seconds
```

The recording protocol can therefore be represented as:

```text
Machine condition
       │
       ├── Recording 1 ── 10–60 s
       ├── Recording 2 ── 10–60 s
       ├── Recording 3 ── 10–60 s
       ├── Recording 4 ── 10–60 s
       └── Recording 5 ── 10–60 s
```

The five recordings are treated as independent acquisition sessions for dataset documentation and reproducibility.

The exact number of samples obtained from each recording depends on the effective acquisition rate and the final CSV logging configuration.

Therefore, **sample count should be reported from the finalized CSV files rather than estimated from recording duration alone**.

---

## 8. Machine-State Classes

The current experiment contains seven machine-state classes represented by labels:

```text
0, 1, 2, 3, 4, 5, 6
```

Example structure:

| Label | Physical condition              |
| ----: | ------------------------------- |
|     0 | Ideal_off_fallen                |
|     1 | Ideal_off                       |
|     2 | Ideal_run                       |
|     3 | Ideal_run_fallen                |
|     4 | Minor_load                      |
|     5 | Major_load                      |
|     6 | Major_obstacle                  |

The numerical class identifiers are intentionally retained separately from the physical condition names so that the dataset-processing pipeline remains independent of the final naming convention.

---

## 10. Raw Data Format

The raw acquisition data are stored as CSV files generated by STM32CubeMonitor.

STM32CubeMonitor's CSV logging supports acquisition timestamps and monitored variables. Depending on the STM32CubeMonitor version and selected export format, the CSV may use comma or semicolon separators according to the computer's locale configuration.

Therefore, the preprocessing pipeline should not assume a fixed delimiter without checking the acquisition file.

A representative processed row is:

```text
timestamp,accel_g_x,accel_g_y,accel_g_z,current_mA,Output
...
```

The `timestamp` column is retained during preprocessing when temporal analysis or recording/session reconstruction is required.

---

## 11. Data Preprocessing

The raw CSV files are processed before machine-learning experiments.

The preprocessing pipeline is:

```text
STM32CubeMonitor CSV
        ↓
CSV parsing
        ↓
Column identification
        ↓
Timestamp ordering
        ↓
Sensor synchronization
        ↓
Data cleaning
        ↓
Label assignment
        ↓
Feature extraction / selection
        ↓
Train / test split
        ↓
Machine-learning dataset
```

STM32CubeMonitor documentation notes that CSV data may require chronological sorting depending on the logging format because data can be stored in packets.

Any sorting, filtering, synchronization, scaling, or normalization applied to the dataset should be documented in the corresponding preprocessing code.

---

## 12. Train-Test Split

The current experimental protocol uses a:

```text
70% Training
30% Testing
```

split.

No separate validation dataset is used.

Therefore:

```text
100% dataset
     │
     ├── 70% → Training
     │
     └── 30% → Testing
```

The test subset is reserved for final performance evaluation and should not be used for model selection or hyperparameter tuning.

If model selection is required during future experiments, the training portion can be further subdivided internally, but such a subdivision should be explicitly documented.

---

## 13. Dataset Consistency Across Sensor Experiments

Research 01 requires the three sensor configurations to be evaluated under the same experimental population.

The intended comparison is:

```text
                    Same experimental dataset
                              │
              ┌───────────────┼───────────────┐
              ↓               ↓               ↓
          MPU-only        INA226-only     MPU + INA226
           3 inputs         1 input          4 inputs
              │               │               │
              └───────────────┼───────────────┘
                              ↓
                       Same test population
```

This ensures that observed performance differences are attributable primarily to the available sensor information and model configuration rather than different evaluation populations.

---

## 21. Summary

The final Research 01 dataset consists of synchronized mechanical and electrical sensor observations collected using an STM32F407-based acquisition platform.

The dataset uses:

```text
MPU6050
   +
INA226
   ↓
STM32F407
   ↓
I²C Standard Mode / 100 kHz
   ↓
STM32CubeMonitor
   ↓
CSV recordings
```

For each machine condition, five independent recordings are collected, with each recording lasting approximately 10–60 seconds.

The prepared dataset contains three-axis acceleration, INA226 current measurements, and a seven-class machine-state label.

A 70:30 training/test split is used, with no separate validation set.

The dataset supports the controlled comparison of single-sensor and heterogeneous sensor configurations for resource-constrained Edge AI condition monitoring, followed by deployment experiments comparing on-device INT8 inference with cloud INT8 and cloud FP32 inference.
