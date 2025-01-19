VHD Prediction Using Modified librosacpp Library
Overview
This repository contains code and resources for predicting Valvular Heart Disease (VHD) using a customized version of the librosacpp library. The library has been modified to work with the ADI MAX78000FTHR development board to enable efficient real-time processing of valvular heart disease data.

Custom Modifications
The librosacpp library has been modified to accommodate the specific needs of the ADI MAX78000FTHR hardware, optimizing it for embedded use.
These changes include performance improvements, adjustments to the memory footprint, and adaptations to the MAX78000FTHR's specific hardware capabilities (such as sensor interfacing, real-time data processing, etc.).
Purpose
The primary goal of this project is to use a multimodal approach (ECG, Doppler Ultrasound, and Phonocardiogram data) to predict Valvular Heart Disease (VHD) in real-time. This is achieved by implementing machine learning models, utilizing the modified librosacpp library for efficient data handling and inference on the MAX78000FTHR.

Hardware Used
Development Board: ADI MAX78000FTHR
Sensors: ECG, Doppler Ultrasound, and Phonocardiogram sensors
Embedded Platform: MAX78000F microcontroller
