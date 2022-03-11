# Collision Avoidance System


## Table of Contents
1. **[Introduction](#introduction)**  
2. **[Project Development](#project-development)**  
    1. **[Environment](#environment)**  
    2. **[Agent](#agent)**  
    3. **[State](#state)**  
    4. **[Policy](#policy)**  
    5. **[Actions](#actions)**  
    6. **[Models Used](#models-used)**  
    7. **[Reward Function](#reward-function)**  
    8. **[Implementation](#implementation)**
3. **[Results](#results)**
    1. **[Deep Q-network](#deep-q-network)**  
    2. **[Double Deep Q-network](#double-deep-q-network)**  
    3. **[Duelling Double Deep Q-network](#duelling-double-deep-q-network)**  
    4. **[Comparison between models](#comparison-between-models)**  
4. **[Installation](#installation)**



## Introduction
This report is drawn up after the development of the thesis project at Tongji University (同济大学) for the Double Degree project Politong.  
The thesis project aims to develop a machine learning algorithm that allows an autonomous vehicle to learn to drive and avoid obstacles. 
The research will focus on Reinforcement Learning methods.

## Project Development
The project is developed in Unreal Engine 4 (4.27.0) for the graphic and environment part, and Python for the Machine Learning part.

As it is common in a Reinforcement Learning context, The agent is in an environment it does not know and it will learn a good policy in order to avoid the walls using a trial and error approach. The agent will be rewarded with positive or negative rewards depending on the actions it performs.

### Environment
There are 2 main environments: a simple track and a complex one.
#### Simple Track & Complex Track
The simple track is used for training and validation. Since this is used for training, it does not have long straightaways or sharp turns (Left image)  
The complex track is used for testing. This will have more long straightaways or sharp turns. (Right image)  
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Environment/Easy/top_mod.png" width="400" height="400">
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Environment/Med/top_w_line_mod.png" width="400" height="400">

### Agent
The agent is a car available on the Vehicle Variety pack asset of Unreal Engine 4.
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Agent/car.png" width="490" height="325">

### State
The agent senses the environment around it using some Light Detection and Ranging sensors (LiDAR). The number of LiDAR sensors to use is an important hyper-parameter: in this case, good results were found using 32 LiDAR sensors. These 32 sensors are equally spaced in an angle range of 170◦, this is another important hyper-parameter.  
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/State/carSensors.png" width="490" height="320">  
The beam's colour depends on whether the ray has hit an obstacle or not: red object is near enough, green no hit. There are also two other beams (yellow and blue) they have for visual purposes, in particular, to make humans understand how the agent learns; these will be used in the Reward Function.

### Policy
The policy, as mentioned early, is a mapping from state to action: how the agent chooses an action in a given state. The goal is to improve the policy over time in order to increase the cumulative reward.

### Actions
There are 2 possible actions the agent is allowed to perform: throttle and steer.
These are discretised into 5 actions:
- a<sub>1</sub>, steer to the left:
- a<sub>2</sub>, slightly steer to the left:
- a<sub>3</sub>, go fully forward:
- a<sub>4</sub>, slightly steering to the right:
- a<sub>5</sub>, steer to the right:

### Models Used
- Deep Q-Network
- Double Deep Q-Network
- Duelling Double Deep Q-Network

### Reward Function
The reward function is an important function that tells the agent what is correct and what is wrong using rewards and punishments.
Popular methods need some external information, for example where the middle of the road is, where the next checkpoint is and so on.
The proposed reward function in this project tries only to get information from the car sensors.  
Basically, the reward is proportional to the direction of the car and its speed (more information in the [report](https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Report.pdf); section 3.6)

### Implementation
The whole system can be divided into 2 parts: the environment part and the machine learning part.

- The environment part is developed in Unreal Engine 4 and it includes the environment itself and the physical agent; from this part, the agent senses the world performs actions and gets the rewards.  
The machine learning part is developed in Python and it is where the neural networks run.

- The 2 parts communicate through HTTP requests: Unreal Engine is the client and Python is the server. The 2 parts communicate through HTTP requests: Unreal Engine is the client and Python is the server. The main reason why there are two parts and that they communicate through HTTP requests is that: using Python tools (TensorFlow, etc.) is convenient when dealing with machine learning and write everything in C++ would be time-consuming and prone to errors; Unreal Engine does not allow native communication with Python scripts.

## Results
### Deep Q-network
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Results/D/plot0_reward.png" width="600" height="325">  

### Double Deep Q-network
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Results/DD/plot1_reward.png" width="600" height="325">  

### Duelling Double Deep Q-network
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Results/DDD/plot2_reward.png" width="600" height="325">  

### Comparison between models
<img src="https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Image/Results/Comparison/rewards.png" width="600" height="325">  
From these plots it is possible to see some differences between the learning paths of the models and that they were able to reach the goal and drive correctly.  

More details in the [report](https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Report/Report.pdf); chaper 4.


## Installation

> 1. Clone the repository  
> 2. Change the IP address of the server [here](https://github.com/MauriVass/CollisionAvoidanceSystem/blob/master/Source/Collis_Avoid_Sys/Client.cpp)  
> 3. Start the Unreal Engine client  
> 4. Start the Python server  
> 5. Start the simulation  
