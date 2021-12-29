/**
 * @file tuya_fan_timer.h
 * @author lifan
 * @brief 定时模块
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_FAN_TIMER_H__
#define __TUYA_FAN_TIMER_H__

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

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
VOID_T fan_nature_mode_timer_start(VOID_T);
VOID_T fan_nature_mode_timer_stop(VOID_T);
VOID_T fan_sleep_mode_timer_start(VOID_T);
VOID_T fan_sleep_mode_timer_stop(VOID_T);
VOID_T fan_timing_confirm_timer_start(VOID_T);
VOID_T fan_timing_confirm_timer_stop(VOID_T);
VOID_T fan_shutdown_timer_start(VOID_T);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_FAN_TIMER_H__ */
