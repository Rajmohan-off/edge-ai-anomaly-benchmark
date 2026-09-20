# Experiment 02 - Edge vs Cloud Inference

## Research Question

Does on-device **INT8 inference** provide comparable condition-monitoring performance to cloud-based inference while reducing inference latency, communication dependency, and deployment overhead?

## Deployment Configurations

- **Edge INT8** - quantized model deployed on STM32F407
- **Cloud INT8** - same quantized model executed in the cloud
- **Cloud FP32** - full-precision model executed in the cloud

## Models

1. **Edge INT8**
2. **Cloud INT8**
3. **Cloud FP32**

## Controlled Variables

- Same heterogeneous sensor inputs
- Same **MPU6050 + INA226** data
- Same number of output classes
- Same model architecture
- Same trained model weights where applicable
- Same preprocessing and normalization
- Same test dataset
- Same evaluation metrics
- Same classification task

## Evaluation

- **Accuracy**
- **Macro F1**
- **Per-class precision/recall**
- **Confusion matrix**
- **Model size**
- **Flash/RAM usage**
- **Inference latency**
- **End-to-end latency**
- **CPU cycles**
- **Communication overhead**
- **Energy/inference**
- **INT8 vs FP32 performance difference**

## Results

| Metric | Edge INT8 | Cloud INT8 | Cloud FP32 |
|---|---:|---:|---:|
| Accuracy | — | — | — |
| Macro F1 | — | — | — |
| Model size | — | — | — |
| Inference latency | — | — | — |
| End-to-end latency | — | — | — |
| CPU cycles | — | — | N/A |
| Communication overhead | — | — | — |
| Energy/inference | — | N/A | N/A |
| Flash | — | N/A | N/A |
| RAM | — | N/A | N/A |

## Interpretation

[What the experiment actually demonstrated]

The experiment evaluates two separate deployment questions:

- **Edge INT8 vs Cloud INT8** — evaluates the effect of moving inference from the cloud to the STM32F407 while keeping model precision constant.
- **Cloud INT8 vs Cloud FP32** — evaluates the effect of INT8 quantization on model performance while keeping the deployment location constant.
