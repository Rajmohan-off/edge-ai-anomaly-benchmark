**Date:** 23 Jul 2026

**Issue:** Real-time TinyML inference produced unstable predictions due to transient motor vibrations and high-frequency sensor noise.

**Issue Location:** Application Layer >> STM32 Main Program >> TinyML Inference Logic

**Diagnosis Method:**

- Monitored live inference output through SWV.
- Observed rapid switching between prediction classes despite a mechanically stable operating condition.
- Compared instantaneous predictions against physical motor behaviour.

**Mitigation 1:**
Retrain the model with new additional dataset and deploy the lite model into the existing project.
- Outcome:
Predictions fluctuated significantly and generated false detections.

**Mitigation 2:**
Introduce a constant 100ms delay in between reading each value of the sensor data, for stable predicition
- Outcome:
The issue still persist but the rapid prediction transistion got reduced.

**Mitigation 3:**
Implemented a Prediction Majority Voting (Tumbling Window) algorithm.
Buffered 100 consecutive AI predictions.
Calculated the statistical mode.
Published only the majority class as the final prediction.
- Outcome:
Prediction stability improved considerably, eliminating transient false alarms while maintaining classification accuracy.

**Hypothetical Root Cause:**
Single-sample inference is highly susceptible to mechanical vibration, electrical noise, and instantaneous sensor disturbances. Majority voting effectively performs temporal filtering on the classifier output.

**Learning:**
Temporal voting can substantially improve embedded AI robustness without modifying the trained model. However, for research reproducibility, temporal windowing should preferably be incorporated during dataset preparation and model training rather than only at deployment.
