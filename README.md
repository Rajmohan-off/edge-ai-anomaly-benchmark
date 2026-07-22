# Edge AI Anomaly Benchmark (TinyML Predictive Maintenance)

An end-to-end predictive maintenance prototype deploying a neural network directly onto an STM32 microcontroller. This project benchmarks on-device TinyML inference against traditional cloud AI architectures for industrial anomaly detection.

## Project Evolution: From Bare-Metal to Edge AI
This project originated as a **bare-metal C** implementation for direct I2C sensor acquisition (available in `/legacy_bare_metal`). 
To support complex neural network inference and hardware abstraction, the main branch has been upgraded to utilize **STM32 HAL, and the ST X-CUBE-AI framework**.

## Core Architecture & Features
* **Sensor Acquisition:** Real-time vibration data streaming from an MPU6050 accelerometer via I2C.
* **On-Device Inference:** A quantized Deep Learning model running locally on the STM32 to classify machine state (Normal vs. Anomalous).
* **AI Debouncing (Temporal Majority Voting):** A custom sliding-window algorithm that filters out mechanical noise and transient spikes, preventing false-positive anomaly triggers.
* **Benchmarking Platform:** Designed to evaluate the latency, power consumption, and reliability of Edge AI vs. Cloud AI in an Industrie 4.0 context.

## Tech Stack
* **Hardware:** STM32 Development Board, MPU6050 Sensor
* **Embedded Software:** C / Embedded C, STM32CubeIDE, Hardware Abstraction Layer (HAL)
* **AI & Machine Learning:** TensorFlow, TensorFlow Lite for Microcontrollers (TFLM), ST X-CUBE-AI
* **Data Processing:** Python (for model training & dataset generation)

## How It Works: The Data Pipeline
1. **Data Ingestion:** I2C reads raw X, Y, Z axis data from the MPU6050.
2. **Preprocessing:** Data is normalized and buffered into a sliding window.
3. **Inference:** X-CUBE-AI executes the neural network on the buffered data.
4. **Majority Voting:** The system aggregates the last *N* predictions. An anomaly is only flagged if the majority of recent inferences agree, ensuring robust, noise-resistant industrial alerts.
