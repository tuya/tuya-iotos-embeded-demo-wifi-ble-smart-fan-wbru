# Tuya IoTOS Embeded Demo WiFi & BLE Smart Fan (WBRU)

[English](./README.md) | [中文](./README_zh.md)

<br>

## 简介 

本 demo 基于 [涂鸦IoT平台](https://iot.tuya.com/) 、涂鸦智能APP、IoTOS Embeded WiFi & Ble SDK，使用涂鸦WiFi/WiFi+BLE系列模组快速组建一个智能电风扇应用。

<br>

## 快速上手

### 编译与烧录

-  在 [涂鸦IoT平台](https://iot.tuya.com/) 上创建产品后，在硬件开发的开发资料中下载 SDK。

- 下载Demo至SDK目录的demos目录下：

  ```bash
  $ cd demos
  $ git clone https://github.com/Tuya-Community/tuya-iotos-embeded-demo-wifi-ble-smart-fan-wbru.git
  ```

  （注：Demo中的build.sh仅在版本2.0.3以下的SDK中需要使用。）

- 在SDK根目录下执行以下命令开始编译：

  ```bash
  sh build_app.sh demos/tuya_demo_bldc_fan tuya_demo_bldc_fan 1.0.0
  ```

- 固件烧录授权相关信息请参考：[WBR 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/burn-and-authorize-wbr-series-modules?id=Ka78imt8pf85p) 

<br>

### 文件介绍
```
├── src         /* 源文件目录 */
|    ├── common
|    |    └── tuya_tools.c        /* 通用工具 */
|    ├── driver
|    |    └── tuya_rotary.c       /* 旋钮编码器检测驱动 */
|    ├── soc
|    |    ├── soc_flash.c         /* SoC flash */
|    |    └── soc_timer.c         /* SoC 定时器 */
|    ├── tuya_device.c            /* 应用层入口文件 */
|    ├── tuya_fan_app.c           /* 风扇应用程序 */
|    ├── tuya_fan_dp_process.c    /* dp数据处理 */
|    ├── tuya_fan_flash.c         /* flash数据读写 */
|    ├── tuya_fan_key.c           /* 按键检测 */
|    ├── tuya_fan_led.c           /* 指示灯控制 */
|    ├── tuya_fan_motor.c         /* 电机控制 */
|    ├── tuya_fan_rotary.c        /* 旋钮编码器应用 */
|    └── tuya_fan_timer.c         /* 定时器控制 */
|
└── include     /* 头文件目录 */
     ├── common
     |    └── tuya_tools.h        /* 通用工具 */
     ├── driver
     |    └── tuya_rotary.h       /* 旋钮编码器检测驱动 */
     ├── soc
     |    ├── soc_flash.h         /* SoC flash */
     |    └── soc_timer.h         /* SoC 定时器 */
     ├── tuya_device.h            /* 应用层入口文件 */
     ├── tuya_fan_app.h           /* 风扇应用程序 */
     ├── tuya_fan_dp_process.h    /* dp数据处理 */
     ├── tuya_fan_flash.h         /* flash数据读写 */
     ├── tuya_fan_key.h           /* 按键检测 */
     ├── tuya_fan_led.h           /* 指示灯控制 */
     ├── tuya_fan_motor.h         /* 电机控制 */
     ├── tuya_fan_rotary.h        /* 旋钮编码器应用 */
     └── tuya_fan_timer.h         /* 定时器控制 */
```

<br>

### 应用入口
入口文件：`/demos/src/tuya_device.c`

重要函数：`device_init()`

+ 调用 `tuya_iot_wf_soc_dev_init_param()` 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。
+ 调用 `tuya_iot_reg_get_wf_nw_stat_cb()`接口注册设备网络状态回调函数。
+ 调用应用层初始化函数 `fan_device_init()`。

<br>

### DP点相关

- 上报dp点接口：`dev_report_dp_json_async()`

| 函数名  | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------- | ------------------------------------------------------------ |
| devid   | 设备id（若为网关、MCU、SOC类设备则devid = NULL；若为子设备，则devid = sub-device_id) |
| dp_data | dp结构体数组名                                               |
| cnt     | dp结构体数组的元素个数                                       |
| Return  | OPRT_OK：成功  Other：失败                                   |

<br>

### I/O 列表

| 外设         | I/O  | 外设           | I/O  | 外设        | I/O  | 外设         | I/O  |
| ------------ | ---- | -------------- | ---- | ----------- | ---- | ------------ | ---- |
| 电源键       | PA7  | 旋钮编码器-A   | PA18 | 挡位指示灯1 | PA9  | 直吹风指示灯 | TX   |
| 定时键       | PA8  | 旋钮编码器-B   | PA0  | 挡位指示灯2 | PA2  | 自然风指示灯 | PA10 |
| 配网键       | PA19 | 无刷直流电机   | PA11 | 挡位指示灯3 | PA3  | 睡眠风指示灯 | PA20 |
| 旋钮编码器-N | PA17 | 指示灯亮度调节 | PA12 | 挡位指示灯4 | PA4  | 网络指示灯   | RX   |

<br>

## 相关文档

- [通用 Wi-Fi SDK 说明](https://developer.tuya.com/cn/docs/iot/tuya-common-wifi-sdk?id=K9glcmvw4u9ml) 
- [通用 Wi-Fi SDK Demo 说明](https://developer.tuya.com/cn/docs/iot/tuya-wifi-sdk-demo-instructions?id=K9oce5ayw5xem) 
- [涂鸦 Demo 中心](https://developer.tuya.com/demo) 

<br>


## 技术支持

您可以通过以下方法获得涂鸦的支持:

- [涂鸦 AI+IoT 开发者平台](https://developer.tuya.com)
- [帮助中心](https://support.tuya.com/help)
- [服务与支持](https://service.console.tuya.com)

<br>

