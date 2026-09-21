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
| Metrics            | INA-only  | MPU-only  | Heterogeneous  |
| ------------------ | --------- | --------- | -------------- |
| Accuracy           | 36.82%    | 92.47%    | 92.89%         |
| Macro F1           | 32.94%    | 95.81%    | 96.13%         |
| Weighted F1        | 50.20%    | 92.50%    | 92.92%         |
| Trainable Params   | 1,223     | 1,255     | 1,271          |
| FP32 Params size   | 4.78 KB   | 4.90 KB   | 4.96 KB        |
| TFLite model size  | TBD       | TBD       | TBD            |
| Flash usage        | TBD       | TBD       | TBD            |
| RAM usage          | TBD       | TBD       | TBD            |
| Tensor arena       | TBD       | TBD       | TBD            |
| Inference latency  | TBD       | TBD       | TBD            |

*TBD - To be derive 



