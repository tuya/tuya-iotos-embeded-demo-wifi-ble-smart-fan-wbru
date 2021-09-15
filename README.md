# Tuya IoTOS Embedded Wi-Fi & Bluetooth LE Smart Fan (WBRU)

[English](./README.md) | [中文](./README_zh.md)

<br>

## Introduction

In this demo, we will show you how to retrofit an ordinary fan and make it IoT-enabled. Based on the [Tuya IoT Platform](https://iot.tuya.com/), we use Tuya's Wi-Fi and Bluetooth LE combo module, SDK, and the Tuya Smart app to connect the fan to the cloud.

<br>

## Get started

### Compile and flash

- Create a product on the [Tuya IoT Platform](https://iot.tuya.com/) and download the SDK in the third step of **Hardware Development**.

- Clone this demo to the `demos` folder in the downloaded SDK.

  ```bash
  $ cd demos
  $ git clone https://github.com/Tuya-Community/tuya-iotos-embeded-demo-wifi-ble-smart-fan-wbru.git
  ```

   Note that the `build.sh` file is only used for SDK earlier than v2.0.3.

- Run the following command in the SDK root directory to start compiling.

  ```bash
  sh build_app.sh demos/tuya_demo_bldc_fan tuya_demo_bldc_fan 1.0.0
  ```

- For more information about flashing and authorization, see [Flash Firmware to and Authorize WBR Modules](https://developer.tuya.com/en/docs/iot/burn-and-authorize-wbr-series-modules?id=Ka78imt8pf85p).

<br>

### File introduction
```
├── src         /* Source code files */
|    ├── common
|    |    └── tuya_tools.c        /* Common tools */
|    ├── driver
|    |    └── tuya_rotary.c       /* Rotary encoder driver */
|    ├── soc
|    |    ├── soc_flash.c         /* SoC flash */
|    |    └── soc_timer.c         /* SoC timer */
|    ├── tuya_device.c            /* Entry file of application layer */
|    ├── tuya_fan_app.c           /* Fan application */
|    ├── tuya_fan_dp_process.c    /* DP data processing */
|    ├── tuya_fan_flash.c         /* Read/write flash memory */
|    ├── tuya_fan_key.c           /* Key press detection */
|    ├── tuya_fan_led.c           /* LED indicator control */
|    ├── tuya_fan_motor.c         /* Motor control */
|    ├── tuya_fan_rotary.c        /* Rotary encoder application */
|    └── tuya_fan_timer.c         /* Timer control */
|
└── include     /* Header files */
     ├── common
     |    └── tuya_tools.h        /* Common tools */
     ├── driver
     |    └── tuya_rotary.h       /* Rotary encoder driver */
     ├── soc
     |    ├── soc_flash.h         /* SoC flash */
     |    └── soc_timer.h         /* SoC timer */
     ├── tuya_device.h            /* Entry file of application layer */
     ├── tuya_fan_app.h           /* Fan application */
     ├── tuya_fan_dp_process.h    /* DP data processing */
     ├── tuya_fan_flash.h         /* Read/write flash memory */
     ├── tuya_fan_key.h           /* Key detection */
     ├── tuya_fan_led.h           /* LED indicator control */
     ├── tuya_fan_motor.h         /* Motor control */
     ├── tuya_fan_rotary.h        /* Rotary encoder application */
     └── tuya_fan_timer.h         /* Timer control */
```

<br>

### Entry to application
Entry file: `/demos/src/tuya_device.c`

Main function: `device_init()`

+ Call `tuya_iot_wf_soc_dev_init_param()` for SDK initialization to configure working mode and pairing mode, register callbacks, and save the firmware key and PID.
+ Call `tuya_iot_reg_get_wf_nw_stat_cb()` to register callback of device network status.
+ Call `fan_device_init()` for application initialization.

<br>

### Data point (DP)

- Report DP data: `dev_report_dp_json_async()`

  | Function | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
  | ------- | ------------------------------------------------------------ |
  | devid | For gateways and devices built with the MCU or SoC, the `devid` is NULL. For sub-devices, the `devid` is `sub-device_id`. |
  | dp_data | The name of DP struct array |
  | cnt | The number of elements in the DP struct array |
  | Return | `OPRT_OK`: success. Other values: failure. |

<br>

### Pin configuration

| Peripherals | I/O | Peripherals | I/O | Peripherals | I/O | Peripherals | I/O |
| ------------ | ---- | -------------- | ---- | ----------- | ---- | ------------ | ---- |
| Power key | PA7 | Rotary encoder A | PA18 | Level indicator 1 | PA9 | Standard mode indicator | TX |
| Timer key | PA8 | Rotary encoder B | PA0 | Level indicator 2 | PA2 | Natural mode indicator | PA10 |
| Device pairing key | PA19 | Brushless DC motor | PA11 | Level indicator 3 | PA3 | Sleep mode indicator | PA20 |
| Rotary encoder C | PA17 | Light adjustment of indicators | PA12 | Level indicator 4 | PA4 | Network status indicator | RX |

<br>

## Reference

- [Wi-Fi SDK Guide](https://developer.tuya.com/en/docs/iot/tuya-common-wifi-sdk?id=K9glcmvw4u9ml)
- [Wi-Fi SDK Demo](https://developer.tuya.com/en/docs/iot/tuya-wifi-sdk-demo-instructions?id=K9oce5ayw5xem)
- [Tuya Project Hub](https://developer.tuya.com/demo)

<br>


## Technical support

You can get support from Tuya with the following methods:

- [Tuya Developer Platform](https://developer.tuya.com/en/)
- [Help Center](https://support.tuya.com/help)
- [Service & Support](https://service.console.tuya.com)

<br>
