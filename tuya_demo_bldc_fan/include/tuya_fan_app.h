/**
 * @file tuya_fan_app.h
 * @author lifan
 * @brief BLDC风扇应用程序
 * @version 1.0
 * @date 2021-07-27
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_FAN_APP_H__
#define __TUYA_FAN_APP_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
/* 工作模式 */
typedef BYTE_T FAN_MODE_E;
#define FAN_MODE_NORMAL     0x00    /* 直吹风 */
#define FAN_MODE_NATURE     0x01    /* 自然风 */
#define FAN_MODE_SLEEP      0x02    /* 睡眠风 */
/* 亮度设置 */
typedef BYTE_T FAN_BRIGHT_E;
#define FAN_BRIGHT_NORMAL   0x00    /* 正常亮度 */
#define FAN_BRIGHT_DARKER   0x01    /* 较暗亮度 */

/* 风扇数据管理 */
typedef struct {
    BOOL_T power;
    FAN_MODE_E mode;
    UCHAR_T speed;
    FAN_BRIGHT_E bright;
    UCHAR_T timing;
    UCHAR_T timing_set;
    BOOL_T timing_display_flag;
    UCHAR_T gear;
} FAN_MANAGE_T;

/***********************************************************
***********************variable define**********************
***********************************************************/
extern FAN_MANAGE_T g_fan_mag;
extern FAN_MANAGE_T g_fan_mag_default;

/***********************************************************
***********************function define**********************
***********************************************************/
BOOL_T fan_switch_power(VOID_T);
VOID_T fan_set_power(IN CONST BOOL_T power);
UCHAR_T fan_switch_mode(VOID_T);
VOID_T fan_set_mode(IN CONST UCHAR_T mode);
VOID_T fan_set_local_timing(VOID_T);
VOID_T fan_quit_timing_setting(VOID_T);

VOID_T fan_nature_mode_timer_cb(VOID_T);
VOID_T fan_sleep_mode_timer_cb(VOID_T);
VOID_T fan_confirm_timer_cb(VOID_T);
VOID_T fan_shutdown_timer_cb(VOID_T);

VOID_T fan_set_cloud_power(IN CONST BOOL_T power);
VOID_T fan_set_cloud_mode(IN CONST FAN_MODE_E mode);
VOID_T fan_set_cloud_speed(IN CONST UCHAR_T speed);
VOID_T fan_set_cloud_bright(IN CONST FAN_BRIGHT_E bright);
VOID_T fan_set_cloud_timing(IN CONST UCHAR_T timing);

VOID_T fan_reset(VOID_T);
VOID_T fan_app_init(VOID_T);
VOID_T fan_device_init(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_FAN_APP_H__ */
