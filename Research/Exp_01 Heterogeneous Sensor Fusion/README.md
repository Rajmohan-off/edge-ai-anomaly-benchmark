# Experiment 01 - Heterogeneous Sensor Fusion

## Research Question

Does heterogeneous vibration + current sensing improve
condition-monitoring performance compared with single-sensor
monitoring under a resource-constrained edge-AI architecture?

## Sensors

- MPU6050 - 3-axis acceleration
- INA226 - current measurement

## Models

1. MPU-only
2. INA-only
3. MPU + INA heterogeneous

## Controlled Variables

- Same network topology
- Same number of output classes
- Same training procedure
- Same test protocol
- Same evaluation metrics

## Evaluation

- Accuracy
- Macro F1
- Per-class precision/recall
- Confusion matrix
- Parameter count
- Flash
- RAM
- Inference latency
- CPU cycles
- Energy/inference

## Results
| Metric | INA-only | MPU-only | Heterogeneous (MPU + INA) |
|---|---:|---:|---:|
| Accuracy | 36.82%* | 92.47% | **92.89%** |
| Macro F1 | 32.94%* | 95.81% | **96.13%** |
| Weighted F1 | 50.20%* | 92.50% | **92.92%** |
| Classification Errors | 99,056* | 11,802 | **11,148** |
| Trainable Parameters | 1,223 | 1,255 | **1,271** |
| X-CUBE-AI Complexity | 1,392 MACC | 1,424 MACC | **1,440 MACC** |
| X-CUBE-AI Flash | 15.96 KiB | 16.09 KiB | **16.15 KiB** |
| X-CUBE-AI RAM | 2.61 KiB | 2.61 KiB | **2.61 KiB** |
| Compression | None | None | None |
| X-CUBE-AI Optimization | Balanced | Balanced | Balanced |
| Total Firmware Flash | 28.10 KB | 28.30 KB | **35.48 KB** |
| Flash Utilization | 2.74% | 2.76% | **3.47%** |
| Total Firmware RAM | 7.52 KB | 7.52 KB | **7.52 KB** |
| RAM Utilization | 6.71% | 6.71% | **6.71%** |




