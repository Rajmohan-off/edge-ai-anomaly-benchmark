# edge-ai-anomaly-benchmark
A research focused project aims to benchmark pre-trained Neural Networks (TensorFlow flite) with self-calibrating On-Device learning (NanoEdge AI) optimize the resilience and adaptability of Digital Twin sensor nodes on STM32 microcontrollers.

**#Research Objective:**
The goal of this project is to evaluate and compare two distinct strategtical method of deployment for predictive maintanence on resource constrained Cortex-M4 microcontroller
**1. Dynamic learning:** The raw data directly feeds into the microcontroller using NanoEdge AI library followed by run On-Device training through an optimal pre-defined model and deploy into the microcontroller 
**2. Static Interface:** The raw data cleaned and pre-processed then using TensorFlow keras built an optmize model in neural network and convert into an liter version to deploy into microcontroller

**Tech Stack & Tools**
**MCU:** STM32F407VGTx (Discovery Board)
**Sensors & peripherals:** MPU6050(6 axis IMU)
**IDE:** STM32 CUBE IDE
**Tools:** STM32cube MX, STM32cube monitor, NanoEdge AI, Anaconda(Jupyter Notebook)
**Programming Stack:** Embedded C, Python version: 3.13.9, TensorFlow version: 2.21.0

**Workflow**
**Static Interface:** 
- The microcontroller peripherals of GPIO and I2C get initiated using STM32cube MX, then with help of STM32cube IDE create a HAL instruction to read raw data from three axis ax, ay, az of MPU6050
- Using Anaconda(python) the raw data get processed and labelled for the anamoly detection followed by data split (80% Training set, 20% Test set)
- Create a neural network model using Tensflow keras and train the model with the Training set data, evaluate it by Test set data
- After trail and errors on the layers and neurons, find the optimal model with lesser in footprint then convert it into lite model using TensorFlow lite
- The Converted lite model then fetch into STM32 Hal layer and flash the code into the microcontroller
**Dynamic learning (On-Device training)**
- The microcontroller peripherals of GPIO and I2C get initiated using STM32cube MX, then with help of STM32cube IDE create a HAL instruction to read raw data from three axis ax, ay, az of MPU6050
- With help of TLL converter read the ideal condition and fault condition of the targeted device using ax, ay, az data with respect to time, on NanoEdge AI
- Find the optimal model by trail and error method with the existing models available in the NanoEdge library



