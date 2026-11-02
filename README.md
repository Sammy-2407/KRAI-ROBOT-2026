# KRAI Robot 2026 — Kung Fu Quest

A two-robot robotic system developed for **Kontes Robot ABU Indonesia (KRAI) 2026**, based on the official **ABU Robocon 2026 — Kung Fu Quest** challenge.

The system consists of two coordinated robots with different operational roles:

- **Robot 1 — Manual Robot:** controlled using a PS3 controller and responsible for collecting the spear, collecting KFS boxes, and arranging them on the rack.
- **Robot 2 — Autonomous Robot:** operates autonomously using Raspberry Pi-based computer vision to identify and collect the spearhead, then collaborates with Robot 1 to complete the spear assembly.

The project combines embedded systems, wireless communication, motor control, robotic mechanisms, and computer vision into a coordinated two-robot system.

> **Competition:** Kontes Robot ABU Indonesia (KRAI) 2026  
> **Theme:** Kung Fu Quest  
> **Robot Configuration:** Robot 1 + Robot 2  
> **Control:** ESP32, Arduino, Raspberry Pi, PS3 Controller  
> **Vision:** Raspberry Pi Computer Vision

---

## Competition

KRAI 2026 is based on the **Kung Fu Quest** theme from ABU Robocon 2026. The competition challenges teams to develop two robots that cooperate to complete a sequence of missions inspired by Kung Fu training.

The game emphasizes:

- Robot collaboration
- Weapon assembly
- Autonomous operation
- Precision and stability
- Strategic movement
- Interaction between the two robots

This repository contains the software developed for our KRAI 2026 robot system.

---

# Robot System

The robot system consists of two robots with complementary roles.

```text
                    KRAI 2026 ROBOT SYSTEM
                            │
              ┌─────────────┴─────────────┐
              │                           │
       ROBOT 1 — MANUAL             ROBOT 2 — AUTONOMOUS
              │                           │
       PS3 Controller               Raspberry Pi
              │                     Computer Vision
            ESP32                         │
              │                          ESP
           Arduino                      Arduino
              │                           │
       Manual Mechanisms          Autonomous Mechanisms
              │                           │
              └─────────────┬─────────────┘
                            │
                    ROBOT COLLABORATION
```

---

# Robot 1 — Manual Robot
<img width="738" height="1600" alt="WhatsApp Image 2026-11-02 at 6 30 02 PM" src="https://github.com/user-attachments/assets/b489d720-3972-48ab-91ab-8b8cf1c60c1a" />

Robot 1 is the manually controlled robot.

The robot is operated using a **PS3 controller**, with an ESP32 acting as the main controller and an Arduino acting as the slave controller for the low-level actuators.

### Main Responsibilities

Robot 1 is responsible for:

1. Moving through the competition field using manual control.
2. Collecting the spear.
3. Collecting KFS boxes.
4. Carrying and arranging KFS boxes on the rack.
5. Carrying the spear toward the assembly area.
6. Coordinating with Robot 2 during the spear assembly process.

### Control Architecture

```text
PS3 Controller
      │
      ▼
    ESP32
   Master
      │
      │ Communication
      ▼
   Arduino
   Slave
      │
      ├── Drive Motors
      ├── Manipulator
      ├── Gripper
      └── Other Actuators
```

The ESP32 handles high-level control and communication, while the Arduino is responsible for actuator-level control.

### Main Features

- PS3-based manual control
- Differential drive
- Motor control
- Spear manipulation
- KFS box handling
- Rack placement mechanism
- ESP32–Arduino communication

---

# Robot 2 — Autonomous Robot
<img width="1179" height="690" alt="WhatsApp Image 2026-11-02 at 6 30 25 PM" src="https://github.com/user-attachments/assets/6f2db0db-6e32-4e15-9990-19e2ff8cb499" />

Robot 2 is the autonomous robot responsible for the spearhead assembly task.

The robot uses a **Raspberry Pi-based computer vision system** together with an ESP32 and Arduino control architecture.

### Main Responsibilities

Robot 2 is responsible for:

1. Operating autonomously after the match begins.
2. Detecting and locating the spearhead.
3. Navigating toward the target.
4. Picking up the spearhead.
5. Moving toward the spear carried by Robot 1.
6. Aligning the spearhead with the spear.
7. Completing the spear assembly.

### Control Architecture

```text
              Raspberry Pi
                   │
          Computer Vision
                   │
                   ▼
                 ESP32
                 Master
                   │
                   │ Communication
                   ▼
                Arduino
                 Slave
                   │
          ┌────────┼────────┐
          │        │        │
       Motors   Sensors   Mechanisms
```

### Computer Vision

The Raspberry Pi is responsible for the high-level perception and decision-making required for autonomous operation.

The vision system is used to:

- Detect the target object.
- Estimate the target position.
- Determine the robot's movement direction.
- Assist with alignment during the pickup and assembly process.

---

# Robot-to-Robot Collaboration

One of the main aspects of this project is the coordination between Robot 1 and Robot 2.

The robots are designed with complementary responsibilities rather than operating as two independent systems.

### Spear Assembly Sequence

```text
Robot 1
   │
   │ Carries Spear
   ▼
[ Spear ]
   │
   │
   │                Robot 2
   │                   │
   │                   │ Detect
   │                   ▼
   │              [ Spearhead ]
   │                   │
   │                   │ Pick
   │                   ▼
   │              Move & Align
   │                   │
   └───────────────►───┘
                    │
                    ▼
              Spear Assembly
```

Robot 1 provides and positions the spear, while Robot 2 autonomously retrieves and positions the spearhead.

This requires coordination between mechanical positioning, autonomous navigation, computer vision, and actuator control.

---

# Software Architecture

The project separates high-level control from low-level actuator control.

### Robot 1

```text
PS3 Controller
      ↓
ESP32 Master
      ↓
Command Processing
      ↓
Arduino Slave
      ↓
Motor & Mechanism Control
```

### Robot 2

```text
Raspberry Pi
      ↓
Computer Vision
      ↓
Autonomous Decision
      ↓
ESP32 Master
      ↓
Arduino Slave
      ↓
Motor & Mechanism Control
```

This architecture allows the computationally intensive vision system to remain on the Raspberry Pi while the ESP32 and Arduino handle communication and real-time actuator control.

---

# Repository Structure

```text
KRAI-ROBOT-2026/
│
├── Robot1 (Manual)/
│   ├── ESP32/
│   └── Arduino/
│
├── Robot2 (Autonomus)/
│   ├── RaspberryPi/
│   ├── ESP32/
│   └── Arduino/
│
├── README.md
└── .gitattributes
```

> The directory structure may evolve as the robot software is modularized and additional subsystems are added.

---

# Mission Sequence

The overall operation of the robot system can be summarized as:

```text
START
  │
  ▼
Robot Initialization
  │
  ├─────────────────────┐
  ▼                     ▼
Robot 1              Robot 2
Manual Operation     Autonomous Operation
  │                     │
  ▼                     ▼
Collect Spear        Detect Spearhead
  │                     │
  ▼                     ▼
Collect KFS          Pick Spearhead
  │                     │
  ▼                     ▼
Place KFS            Navigate to Spear
on Rack                 │
  │                     ▼
  │                  Align
  │                     │
  └──────────────► Spear Assembly
                        │
                        ▼
                      DONE
```

---

# Hardware

## Robot 1

| Component | Function |
|---|---|
| ESP32 | Main controller |
| Arduino | Actuator controller |
| PS3 Controller | Manual control |
| DC Motors | Robot movement |
| Motor Driver | Motor control |
| Servo Motors | Manipulator mechanisms |
| Mechanical Gripper | Object handling |
| Power System | Robot power |

## Robot 2

| Component | Function |
|---|---|
| Raspberry Pi | Computer vision & autonomous processing |
| ESP32 | Main communication/controller |
| Arduino | Low-level actuator control |
| Camera | Object detection |
| DC Motors | Robot movement |
| Motor Driver | Motor control |
| Manipulator | Spearhead handling |
| Power System | Robot power |

---

# Development

The robot system was developed through iterative mechanical, electrical, and software testing.

Development areas include:

- Embedded programming
- Motor control
- Servo control
- Robot communication
- Autonomous navigation
- Computer vision
- Object detection
- Mechanical mechanism development
- Robot-to-robot coordination
- Competition strategy

---

# Competition Context

This project was developed for **KRAI 2026**, which adopts the ABU Robocon 2026 **Kung Fu Quest** theme.

The competition requires teams to develop two robots capable of working together to complete a sequence of challenges. The official rules define Robot 1 as a manual or autonomous robot, while Robot 2 is required to operate autonomously during the game.

---

# Team

Developed for:

**Kontes Robot ABU Indonesia (KRAI) 2026**

Theme:

**Kung Fu Quest**

---

# References

- ABU Robocon 2026 — *Kung Fu Quest* Official Rulebook
- KRAI 2026 Official Competition Guidelines
- Directorate General of Higher Education, Ministry of Higher Education, Science and Technology, Republic of Indonesia
