# Tuya IoTOS Embeded Demo WiFi & BLE Smart Fan (WBRU)

[English](./README.md) | [中文](./README_zh.md) 

<br>

## Introduction 

This demo is based on [Tuya IoT Platform](https://iot.tuya.com/), Tuya Smart APP, IoTOS Embeded WiFi&Ble SDK, using Tuya WiFi/WiFi+Ble series modules quickly build a smart fan application.

<br>


## Quick start 

### Compile & Burn

- After creating a product on [Tuya IoT Platform](https://iot.tuya.com/), download the SDK from the development materials of hardware development.

- Download the demo to the demos directory of the SDK:

  ```bash
  $ cd demos
  $ git clone https://github.com/Tuya-Community/tuya-iotos-embeded-demo-wifi-ble-smart-fan-wbru.git
  ```

  (Note: The build.sh in the Demo is only required to be used in the SDK below version 2.0.3.)

- Execute the following command in the SDK root directory to start compiling:

  ```bash
  sh build_app.sh demos/tuya_demo_bldc_fan tuya_demo_bldc_fan 1.0.0
  ```

- Firmware burn-in license information please refer to: [WBR series module programming authorization](https://developer.tuya.com/cn/docs/iot/burn-and-authorize-wbr-series-modules?id=Ka78imt8pf85p)  

<br>

### File description 

```
├── src         /* Source code files */
|    ├── common
|    |    └── tuya_tools.c        /* Common tools */
|    ├── driver
|    |    └── tuya_rotary.c       /* Rotary encoder detection driver */
|    ├── soc
|    |    ├── soc_flash.c         /* SoC flash */
|    |    └── soc_timer.c         /* SoC timer */
|    ├── tuya_device.c            /* Entry file of application layer */
|    ├── tuya_fan_app.c           /* Application code for smart fan */
|    ├── tuya_fan_dp_process.c    /* DP process */
|    ├── tuya_fan_flash.c         /* Flash operation */
|    ├── tuya_fan_key.c           /* KEY detection */
|    ├── tuya_fan_led.c           /* LED control */
|    ├── tuya_fan_motor.c         /* Motor control */
|    ├── tuya_fan_rotary.c        /* Rotary encoder application */
|    └── tuya_fan_timer.c         /* Timer control */
|
└── include     /* Header files */
     ├── common
     |    └── tuya_tools.h        /* Common tools */
     ├── driver
     |    └── tuya_rotary.h       /* Rotary encoder detection driver */
     ├── soc
     |    ├── soc_flash.h         /* SoC flash */
     |    └── soc_timer.h         /* SoC timer */
     ├── tuya_device.h            /* Entry file of application layer */
     ├── tuya_fan_app.h           /* Application code for smart fan */
     ├── tuya_fan_dp_process.h    /* DP process */
     ├── tuya_fan_flash.h         /* Flash operation */
     ├── tuya_fan_key.h           /* KEY detection */
     ├── tuya_fan_led.h           /* LED control */
     ├── tuya_fan_motor.h         /* Motor control */
     ├── tuya_fan_rotary.h        /* Rotary encoder application */
     └── tuya_fan_timer.h         /* Timer control */
```

<br>

### Entry to application

Entry file: /demos/src/tuya_device.c

Important function：`device_init()`

- Call the `tuya_iot_wf_soc_dev_init_param()` interface to initialize the SDK, configure the working mode, network distribution mode, and register various callback functions and save the firmware key and PID.
- Call the `tuya_iot_reg_get_wf_nw_stat_cb()` interface to register the device network status callback function.
- Call the application layer initialization function `fan_device_init()`.

<br>

### Data point (DP)

- Report dp point interface: dev_report_dp_json_async()

| Function name | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------------- | ------------------------------------------------------------ |
| devid         | device id (if gateway, MCU, SOC class device then devid = NULL; if sub-device, then devid = sub-device_id) |
| dp_data       | dp structure array name                                      |
| cnt           | Number of elements of the dp structure array                 |
| Return        | OPRT_OK: Success Other: Failure                              |

<br>

### I/O list

| 外设             | I/O  | 外设             | I/O  | 外设      | I/O  | 外设       | I/O  |
| ---------------- | ---- | ---------------- | ---- | --------- | ---- | ---------- | ---- |
| Power key        | PA7  | Rotary encoder-A | PA18 | Gear led1 | PA9  | Wind led   | TX   |
| Timer key        | PA8  | Rotary encoder-B | PA0  | Gear led2 | PA2  | Nature led | PA10 |
| WiFi key         | PA19 | BLDC motor pwm   | PA11 | Gear led3 | PA3  | Sleep led  | PA20 |
| Rotary encoder-N | PA17 | LED dimmer       | PA12 | Gear led4 | PA4  | Net led    | RX   |

<br>

## Reference

- [General Wi-Fi SDK Instruction](https://developer.tuya.com/en/docs/iot/tuya-common-wifi-sdk?id=K9glcmvw4u9ml) 
- [General Wi-Fi SDK Demo Instruction](https://developer.tuya.com/en/docs/iot/tuya-wifi-sdk-demo-instructions?id=K9oce5ayw5xem) 
- [Tuya Project Hub](https://developer.tuya.com/demo)

<br>

## Technical Support

You can get support from Tuya with the following methods:

- [Tuya IoT Developer Platform](https://developer.tuya.com/en/)
- [Help Center](https://support.tuya.com/en/help)
- [Service & Support](https://service.console.tuya.com)

<br>

