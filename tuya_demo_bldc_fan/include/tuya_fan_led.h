/**
 * @file tuya_fan_led.h
 * @author lifan
 * @brief 指示灯控制模块
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_FAN_LED_H__
#define __TUYA_FAN_LED_H__

#include "tuya_cloud_types.h"
#include "tuya_iot_wifi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/
#define LED_DIMMER_PWM_DUTY_NORMAL  900     /* PWM占空比: 90%(正常亮度) */
#define LED_DIMMER_PWM_DUTY_DARKER  100     /* PWM占空比: 10%(较暗亮度) */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
VOID_T fan_set_gear_led(IN CONST UCHAR_T cur_gear);
VOID_T fan_set_mode_led(IN CONST UCHAR_T cur_mode);
VOID_T fan_set_wifi_led(IN CONST GW_WIFI_NW_STAT_E cur_stat);
VOID_T fan_set_timing_led(IN CONST UCHAR_T hour);
VOID_T fan_set_led_dimmer(IN CONST USHORT_T light);
VOID_T fan_close_all_led(VOID_T);
VOID_T fan_led_init(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_FAN_LED_H__ */
