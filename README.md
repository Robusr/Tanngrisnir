# Tanngrisnir

[![Platform](https://img.shields.io/badge/platform-STM32F103C8T6-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html)
[![IDE](https://img.shields.io/badge/IDE-Keil%20MDK%205-brightgreen)](https://www.keil.com/mdk5/)
[![HAL](https://img.shields.io/badge/HAL-STM32F1xx-orange)](https://github.com/STMicroelectronics/stm32f1xx_hal_driver)
[![License](https://img.shields.io/badge/license-BSD--3--Clause-green)](LICENSE)

A two-wheeled self-balancing vehicle based on the STM32F103C8T6 microcontroller, featuring cascaded PID control, MPU6050 DMP attitude sensing, and real-time OLED telemetry.

> **Tanngrisnir** (Old Norse: "teeth-barer") is one of the two goats that pull Thor's chariot in Norse mythology — a fitting name for a balancing vehicle.

---

## Features

- **Self-balancing control** — cascaded PD (balance) + PI (velocity) controller running at 100 Hz
- **MPU6050 DMP** — onboard digital motion processor for accurate pitch/roll/yaw attitude data
- **Encoder feedback** — dual quadrature encoders for precise wheel velocity measurement
- **OLED display** — real-time telemetry showing pitch angle, angular velocity, and battery voltage
- **Virtual oscilloscope** — USART-based DataScope protocol for wireless waveform monitoring
- **Safety protection** — low-voltage shutdown (< 7.0 V), excessive tilt shutdown (> 30°), and manual stop
- **Battery monitoring** — ADC voltage sensing with 11:1 resistor divider

---

## Hardware

### Bill of Materials

| Component | Part / Model | Notes |
|---|---|---|
| MCU | STM32F103C8T6 (Cortex-M3, 72 MHz) | 64 KB Flash, 20 KB SRAM |
| IMU | ATK-MS6050 (MPU6050-based) | I2C address 0x68, DMP enabled |
| Motor Driver | TB6612 / L298N compatible | H-bridge with direction + PWM |
| Motors | DC geared motors with encoders | Quadrature encoder, 2-channel per motor |
| Display | 0.96" OLED (SPI 4-wire) | 128×64, SSD1306-compatible |
| Battery | 3S LiPo (11.1 V nominal) | Measured via ADC through 11:1 divider |
| Debug | USB-TTL serial adapter | 115200 bps for DataScope / printf |

### Pin Mapping

<details>
<summary>Click to expand full pin map</summary>

| Function | Pin | GPIO | Timer / Peripheral |
|---|---|---|---|
| **Motor A PWM** | PA8 | TIM1_CH1 | TIM1 |
| **Motor B PWM** | PA11 | TIM1_CH4 | TIM1 |
| Motor A IN1 | PB14 | GPIO | — |
| Motor A IN2 | PB15 | GPIO | — |
| Motor B IN1 | PB13 | GPIO | — |
| Motor B IN2 | PB12 | GPIO | — |
| **Left Encoder A** | PA6 | TIM3_CH1 | TIM3 (Encoder mode) |
| **Left Encoder B** | PA7 | TIM3_CH2 | TIM3 |
| **Right Encoder A** | PB6 | TIM4_CH1 | TIM4 (Encoder mode) |
| **Right Encoder B** | PB7 | TIM4_CH2 | TIM4 |
| MPU6050 I2C SCL | PB10 | I2C2_SCL | I2C2 |
| MPU6050 I2C SDA | PB11 | I2C2_SDA | I2C2 |
| MPU6050 INT | PA12 | EXTI12 | 200 Hz interrupt |
| OLED RST | PB3 | GPIO | — |
| OLED CS | PA1 | GPIO | — |
| OLED RS (D/C) | PA15 | GPIO | — |
| OLED SCLK | PB5 | GPIO | SPI (software) |
| OLED SDIN | PB4 | GPIO | SPI (software) |
| KEY0 (stop) | PA5 | EXTI5 | Falling edge |
| LED | PA4 | GPIO | Status indicator |
| Battery ADC | PB0 | ADC1_CH8 | ADC1 |
| USART1 TX | PA9 | USART1_TX | 115200 bps |

</details>

---

## Software Architecture

### Control Algorithm

The balancing controller uses a **cascaded PID** structure:

```
                          +-----------+
Target velocity = 0 ──→  │  Velocity  │ ──→ Target angle ──→  +         + ──→ PWM ──→ Motor
                          │  PI Loop   │                        │         │
                          +-----------+                        +-----------+
                                ↑                             │  Balance  │
                          Encoder feedback                    │  PD Loop  │
                                                              +-----------+
                                                                   ↑
                                                             MPU6050 attitude
```

#### Parameters

| Loop | Type | Kp | Ki / Kd | Input | Output | Period |
|---|---|---|---|---|---|---|
| Balance | PD | 400 | Kd = 2.0 | Pitch angle + Gyro rate | PWM component | 10 ms |
| Velocity | PI | 160 | Ki = 0.8 | Encoder pulse count | Angle offset | 10 ms |

#### PWM Calculation

```c
Balance_Pwm  = Balance(pitch_angle, gyro_rate);           // PD control
Velocity_Pwm = Velocity(encoder_left, encoder_right);      // PI control
Motor_Pwm    = Balance_Pwm + Velocity_Pwm;                 // Superposition
Motor_Pwm    = PWM_Limit(Motor_Pwm, 6900, -6900);          // Clamp to ±6900
```

### Interrupt Timing

```
MPU6050 INT (200 Hz)
  │
  ├── 5 ms  →  read encoder velocity + read DMP attitude
  │            set tenMsFlag
  │
  └── 10 ms →  run PID controller
               sample battery voltage
               update motor PWM output
```

### Directory Structure

```
Tanngrisnir/
├── Startup/
│   └── startup_stm32f103xb.s        # Vector table & reset handler
├── Drivers/
│   ├── CMSIS/                        # ARM Cortex-M CMSIS
│   ├── STM32F1xx_HAL_Driver/         # STM32 HAL library
│   ├── BSP/
│   │   ├── MOTOR/                    # PWM motor driver (TIM1)
│   │   ├── ENCODER/                  # Encoder reader (TIM3/TIM4)
│   │   ├── EXTI/                     # External interrupts → main control loop
│   │   ├── ATK_MS6050/               # MPU6050 driver + DMP firmware
│   │   ├── ADC/                      # Battery voltage ADC
│   │   ├── OLED/                     # OLED display driver
│   │   ├── LED/                      # Status LED
│   │   ├── KEY/                      # Push button
│   │   └── TIMER/                    # General-purpose timers
│   └── SYSTEM/                       # sys / delay / usart utilities
├── User/
│   ├── main.c                        # Entry point & main loop
│   ├── exti.c                        # ISR callbacks → PID controller
│   ├── curveShow.c                   # DataScope virtual oscilloscope output
│   ├── oledShow.c                    # OLED real-time telemetry
│   ├── oledfont.h                    # OLED font bitmap
│   └── utilities.c                   # Helper functions & DataScope protocol
├── Projects/
│   └── MDK-ARM/
│       └── test_f103.uvprojx         # Keil MDK project file
└── Output/                           # Build artifacts (.hex, etc.)
```

---

## Getting Started

### Prerequisites

- **Keil MDK 5** with ARM Compiler 5 (tested with v5.06 update 7)
- **STM32F1xx Device Family Pack** (Keil.STM32F1xx_DFP.2.4.1 or later)
- **ST-Link / J-Link** debugger for flashing

### Build

1. Open `Projects/MDK-ARM/test_f103.uvprojx` in Keil MDK
2. Verify the include paths and preprocessor defines:
   - `USE_HAL_DRIVER`
   - `STM32F103xB`
3. Build (<kbd>F7</kbd>) — output hex file will be generated at `Output/test_f103.hex`

### Flash

Connect your debugger and press <kbd>F8</kbd> in Keil MDK, or use STM32CubeProgrammer:

```bash
STM32_Programmer_CLI -c port=SWD -w Output/test_f103.hex -v -s
```

### Usage

1. Power on the vehicle on a **flat surface** — it will initialize and attempt to balance
2. Press **KEY0** to toggle motor stop/start
3. OLED display shows real-time data:
   - **pitAngle**: pitch angle in degrees
   - **pitVel**: angular velocity from gyroscope
   - **batVol**: battery voltage in mV
4. Connect to USART1 (115200 bps) to view DataScope virtual oscilloscope output

---

## Safety

| Protection | Threshold | Action |
|---|---|---|
| Low battery | < 7.0 V | Motor shutdown |
| Excessive tilt | pitch or roll > 30° | Motor shutdown |
| Manual stop | KEY0 pressed (stopFlag = 1) | Motor shutdown |
| Low voltage warning | < 10.0 V | Console alert (no shutdown) |

---

## Acknowledgements

- **ALIENTEK (正点原子)** — Hardware drivers and HAL adaptation (MPU6050, OLED, ADC, EXTI)
- **InvenSense** — MPU6050 DMP firmware (`eMPL/`)
- **STMicroelectronics** — STM32F1xx HAL Driver and CMSIS
- **ARM / Keil** — MDK-ARM toolchain

## License

This project uses code components under the BSD 3-Clause License (STMicroelectronics) and code from ALIENTEK. See individual file headers for specific license terms.

---

*"Tanngrisnir" — pulling Thor's chariot across the sky, one balanced step at a time.* ⚡🐐
