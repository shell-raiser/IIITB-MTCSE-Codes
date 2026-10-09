## Overview

# VehicleVision: Robust Vehicle Classification

## Overview

Can you build a machine learning model that can reliably identify a vehicle type from its specifications and performance measurements?

In this competition, you are given a collection of vehicle measurements such as engine displacement, horsepower, vehicle weight, wheelbase, fuel efficiency, cargo capacity, acceleration, braking distance, tire width, and aerodynamic characteristics.

Your task is to classify each vehicle into one of three categories:

- **0 — Sedan**
- **1 — SUV**
- **2 — Sports Car**

## The Challenge

The dataset has been designed to resemble a real-world vehicle analytics problem.

The training data may contain data-quality issues such as duplicate vehicle measurements. The `vehicle_id` identifies a record, but it should not be treated as a predictive vehicle characteristic.

Participants are encouraged to perform exploratory data analysis and appropriate preprocessing before training their models.

The hidden evaluation data also introduces measurement uncertainty. The final evaluation samples are collected under a different measurement environment, where the recorded vehicle characteristics may contain additional random measurement error.

Therefore, a model that performs well on the training data is not necessarily the model that will perform best on the final evaluation.

## Features

The dataset contains 20 numerical vehicle attributes:

- Engine displacement
- Horsepower
- Vehicle weight
- Wheelbase
- Vehicle length
- Vehicle width
- Vehicle height
- Fuel tank capacity
- City fuel efficiency
- Highway fuel efficiency
- Number of engine cylinders
- Torque
- Ground clearance
- Turning radius
- Cargo capacity
- 0–100 km/h acceleration time
- Braking distance
- Tire width
- Drag coefficient
- Battery capacity

## Files

### `vehicle_train.csv`

Contains the training examples and their vehicle classifications.

The target column is:

`vehicle_class`

### `vehicle_public_test.csv`

Contains vehicle measurements without the target. Participants must predict the vehicle class for each row.

### `vehicle_sample_submission.csv`

Shows the required submission format.

## Target

The target is `vehicle_class`:

|Value|Vehicle Type|
|---|---|
|0|Sedan|
|1|SUV|
|2|Sports Car|

## Evaluation

Submissions are evaluated using **classification accuracy**.

Accuracy is the fraction of predictions that exactly match the hidden vehicle classes.

## Submission

Your submission should contain:

```autohotkey
vehicle_id,vehicle_class
```

For every vehicle in the test set, provide one predicted class.