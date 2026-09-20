# Edge AI Anomaly Benchmark

### Resource-Constrained TinyML for Heterogeneous Sensor-Based Condition Monitoring

An embedded Edge AI research project investigating **heterogeneous sensor fusion, TinyML inference, and resource-constrained deployment** for machine condition monitoring.

The platform combines **vibration sensing from an MPU6050 accelerometer** with **electrical-current measurements from an INA219 sensor** and evaluates whether heterogeneous sensing provides measurable advantages over single-sensor monitoring while remaining suitable for deployment on an STM32 microcontroller.

The project is being developed as a practical research benchmark rather than only as a predictive-maintenance prototype, with emphasis on:

* Sensor-fusion effectiveness
* Classification performance
* Model size and parameter efficiency
* Flash and RAM consumption
* Inference latency and CPU cycles
* Energy per inference
* INT8 quantized deployment
* Edge-vs-cloud inference trade-offs

---

## Research Direction

The project is structured around two connected research studies.

### Research 01 — Heterogeneous Sensor Fusion

**Research question:**

> Does heterogeneous vibration + current sensing provide sufficient improvement in machine-state classification to justify the additional sensing and computational cost compared with single-sensor monitoring?

The benchmark evaluates three sensing configurations:

```text
MPU6050
   │
   └── MPU-only model

INA219
   │
   └── INA-only model

MPU6050 + INA219
   │
   └── Heterogeneous sensor-fusion model
```

The models use a controlled neural-network architecture so that the primary experimental variable is the available sensor information rather than a substantially different model design.

Evaluation includes:

* Accuracy
* Macro F1-score
* Per-class precision and recall
* Confusion matrices
* Parameter count
* Model size
* Flash usage
* RAM usage
* Tensor arena requirements
* Inference latency
* CPU cycles per inference
* Energy per inference

Detailed experimental artifacts are maintained in [`/research`](./research).

---

### Research 02 — Edge vs. Cloud Deployment

The second stage extends the heterogeneous sensor-fusion model into a deployment comparison.

Three configurations are evaluated:

| Configuration  | Location        | Precision |
| -------------- | --------------- | --------- |
| Edge           | STM32F407       | INT8      |
| Cloud          | Cloud inference | INT8      |
| Cloud baseline | Cloud inference | FP32      |

This enables two controlled comparisons:

**Edge INT8 vs Cloud INT8**

→ effect of **inference location**

**Cloud INT8 vs Cloud FP32**

→ effect of **quantization**

The deployment study will investigate:

* Classification performance
* Model inference latency
* End-to-end latency
* Communication/network overhead
* Bandwidth requirements
* Energy consumption
* Memory footprint
* Offline capability
* Computational requirements
* Quantization impact

The objective is to understand the practical trade-offs between **local intelligence and remote inference** rather than simply determining which architecture has higher accuracy.

---

# Project Evolution: Bare-Metal → Edge AI

The project originated as a **bare-metal STM32 implementation** focused on direct sensor acquisition and low-level I²C communication.

The original implementation is preserved under:

```text
/legacy_bare_metal
```

As the project evolved toward embedded AI, the main implementation was migrated to:

* STM32 HAL
* STM32CubeIDE
* ST X-CUBE-AI
* TensorFlow Lite / TensorFlow Lite for Microcontrollers
* Quantized neural-network inference

The evolution reflects the transition from:

```text
Low-level sensor acquisition
        ↓
Embedded signal/data processing
        ↓
Machine-learning inference
        ↓
Resource-constrained Edge AI
        ↓
Deployment benchmarking
```

---

# Core Architecture

## Heterogeneous Sensor Layer

### MPU6050 — Vibration

The MPU6050 provides three-axis acceleration measurements:

```text
accel_g_x
accel_g_y
accel_g_z
```

These features capture mechanical vibration characteristics associated with different machine operating conditions.

### INA219 — Electrical Current

The INA219 provides electrical-current measurements:

```text
current_mA
```

Current consumption provides a complementary electrical representation of machine behaviour.

The combination creates a heterogeneous sensing vector:

```text
[accel_x, accel_y, accel_z, current]
```

This allows the research to investigate whether **mechanical and electrical information together provide additional discriminative information** compared with either modality independently.

---

# Edge AI Pipeline

```text
        MPU6050
           │
           │ I²C
           ▼
      STM32F407
           ▲
           │
        INA219
           │
           ▼
   Sensor Data Acquisition
           │
           ▼
      Preprocessing
           │
           ▼
    Feature Preparation
           │
           ▼
   INT8 Quantized Model
           │
           ▼
    X-CUBE-AI / TFLM
           │
           ▼
     Machine State
           │
           ▼
   Temporal Decision Logic
           │
           ▼
       Alert / Output
```

---

# On-Device Inference

The trained neural network is deployed directly onto the STM32 microcontroller.

The current model architecture uses a compact fully connected network:

```text
Input
  ↓
Dense(16, ReLU)
  ↓
Dense(32, ReLU)
  ↓
Dense(16, ReLU)
  ↓
Output
```

For the current seven-class experiment:

```text
MPU-only:
3 inputs → 16 → 32 → 16 → 7 outputs

Heterogeneous:
4 inputs → 16 → 32 → 16 → 7 outputs
```

The same hidden-layer topology is retained between the MPU-only and heterogeneous models to provide a controlled comparison.

Model summaries and architecture evidence are available under:

```text
/research/01_sensor_fusion/
```

---

# Quantized Edge Deployment

The project uses **INT8 quantization** for microcontroller deployment.

Quantization is investigated as a deployment optimization rather than as the sole research contribution.

The objective is to determine how much model compression can be achieved while preserving useful classification performance and enabling practical deployment on resource-constrained hardware.

Deployment measurements include:

* Model size
* Flash consumption
* Static RAM
* Tensor arena
* Inference cycles
* Inference latency
* Energy per inference

---

# Temporal Decision Filtering

Raw neural-network predictions can fluctuate because of sensor noise, mechanical transients, or short-lived classification errors.

A temporal majority-voting mechanism is therefore used to stabilize the final machine-state decision.

```text
Prediction 1 ─┐
Prediction 2 ─┤
Prediction 3 ─┤
Prediction 4 ─┤ → Majority Vote → Final State
Prediction 5 ─┘
```

An anomaly or machine-state transition is reported only when a sufficient proportion of recent predictions agree.

This separates:

**ML classification**

from

**application-level decision logic**

and provides a more realistic representation of how an industrial monitoring system can consume noisy model predictions.

---

# Benchmarking Methodology

The research is designed around controlled comparisons rather than a single accuracy measurement.

### Model-level metrics

* Accuracy
* Macro F1
* Weighted F1
* Per-class precision
* Per-class recall
* Confusion matrix

### Embedded deployment metrics

* Parameter count
* Model size
* Flash usage
* RAM usage
* Tensor arena
* CPU cycles
* Inference latency
* Peak/average energy
* Energy per inference

### Deployment-level metrics

For the Edge vs Cloud study:

* End-to-end latency
* Network transfer time
* Bandwidth consumption
* Offline availability
* Communication overhead
* Cloud processing requirements
* Quantization impact

---

# Research Artifacts

Research documentation is maintained separately from the implementation:

```text
/research
│
├── 01_sensor_fusion
│   ├── README.md
│   ├── dataset.md
│   ├── experiment_design.md
│   ├── model_architecture.md
│   ├── model_summary_heterogeneous.txt
│   ├── model_summary_mpu.txt
│   ├── confusion_matrices/
│   └── results/
│
└── 02_edge_vs_cloud

```

The research directory contains experimental methodology, model summaries, visual results, benchmark measurements, and interpretation of findings.

The raw dataset is not included in the repository at this stage. Dataset structure, sensor configuration, collection methodology, class definitions, and preprocessing procedures are documented separately.

---

# Technology Stack

### Hardware

* STM32F407 development board
* MPU6050 3-axis accelerometer
* INA219 current sensor
* I²C interface
* Embedded power/inference measurement instrumentation

### Embedded Software

* C / Embedded C
* STM32CubeIDE
* STM32 HAL
* STM32CubeMX
* ST X-CUBE-AI
* TensorFlow Lite for Microcontrollers

### Machine Learning

* Python
* TensorFlow / Keras
* TensorFlow Lite
* INT8 quantization
* Scikit-learn
* NumPy
* Pandas

### Research & Analysis

* Jupyter Notebook
* Confusion-matrix analysis
* Classification reports
* Embedded resource benchmarking
* Edge-vs-cloud deployment analysis

---

# Current Status

### Completed

* [x] STM32 sensor acquisition
* [x] MPU6050 integration
* [x] INA219 integration
* [x] Sensor data collection
* [x] Dataset preparation
* [x] Neural-network training
* [x] MPU-only benchmark
* [x] Heterogeneous sensor-fusion benchmark
* [x] Confusion-matrix evaluation
* [x] Initial INT8 deployment work
* [x] Research documentation structure

### In Progress

* [ ] Finalize independent test-session evaluation
* [ ] Macro/weighted F1 analysis
* [ ] Embedded Flash/RAM benchmarking
* [ ] Inference-cycle measurement
* [ ] Energy-per-inference measurement
* [ ] Finalize Research 01 results
* [ ] Cloud INT8 deployment
* [ ] Cloud FP32 baseline
* [ ] Edge INT8 vs Cloud INT8 comparison
* [ ] Cloud INT8 vs Cloud FP32 comparison

---

# Research Objective

The broader objective of this project is to investigate how **heterogeneous sensing, model compression, and inference location interact under the constraints of embedded industrial AI systems**.

Rather than treating model accuracy as the only metric, the project evaluates the complete deployment trade-off:

```text
Sensor Information
        +
Model Accuracy
        +
Memory
        +
Latency
        +
Energy
        +
Communication
        +
Deployment Location
        ↓
Practical Edge AI System
```

The resulting benchmark is intended to provide a reproducible engineering and research foundation for **TinyML, industrial condition monitoring, sensor fusion, and Edge AI deployment**.
