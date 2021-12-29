/**
 * @file tuya_fan_timer.c
 * @author lifan
 * @brief 定时模块
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_timer.h"
#include "tuya_fan_app.h"
#include "uni_log.h"
#include "soc_timer.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* TIMER ID */
#define TIMER_ID_NATURE_MODE    0           /* 自然模式 */
#define TIMER_ID_SLEEP_MODE     1           /* 睡眠模式 */
#define TIMER_ID_TIMING_CONFIRM 2           /* 定时功能确认 */
#define TIMER_ID_SHUTDOWN       3           /* 定时关机 */
/* 定时时间设置 */
#define CYCLE_MS_NATURE_MODE    (5*1000)    /* 5s */
#define CYCLE_MS_SLEEP_MODE     (60*60*1000)/* 1h */
#define CYCLE_MS_TIMING_CONFIRM (2*1000)    /* 2s */
#define CYCLE_MS_SHUTDOWN       (60*60*1000)/* 1h */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   自然模式定时器启动
 * @param   none
 * @return  none
 */
VOID_T fan_nature_mode_timer_start(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_start(TIMER_ID_NATURE_MODE, CYCLE_MS_NATURE_MODE, fan_nature_mode_timer_cb);
    if (op_ret != OPRT_OK) {
        PR_ERR("Start nature mode timer fails");
    }
}

/**
 * @brief   自然模式定时器关闭
 * @param   none
 * @return  none
 */
VOID_T fan_nature_mode_timer_stop(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_stop(TIMER_ID_NATURE_MODE);
    if (op_ret != OPRT_OK) {
        PR_ERR("Stop nature mode timer fails");
    }
}

/**
 * @brief   睡眠模式定时器启动
 * @param   none
 * @return  none
 */
VOID_T fan_sleep_mode_timer_start(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_start(TIMER_ID_SLEEP_MODE, CYCLE_MS_SLEEP_MODE, fan_sleep_mode_timer_cb);
    if (op_ret != OPRT_OK) {
        PR_ERR("Start sleep mode timer fails");
    }
}

/**
 * @brief   睡眠模式定时器关闭
 * @param   none
 * @return  none
 */
VOID_T fan_sleep_mode_timer_stop(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_stop(TIMER_ID_SLEEP_MODE);
    if (op_ret != OPRT_OK) {
        PR_ERR("Stop sleep mode timer fails");
    }
}

/**
 * @brief   定时关机定时器启动
 * @param   none
 * @return  none
 */
VOID_T fan_shutdown_timer_start(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_start(TIMER_ID_SHUTDOWN, CYCLE_MS_SHUTDOWN, fan_shutdown_timer_cb);
    if (op_ret != OPRT_OK) {
        PR_ERR("Start shutdown timer fails");
    }
}

/**
 * @brief   定时关机定时器关闭
 * @param   none
 * @return  none
 */
VOID_T fan_shutdown_timer_stop(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_stop(TIMER_ID_SHUTDOWN);
    if (op_ret != OPRT_OK) {
        PR_ERR("Stop shutdown timer fails");
    }
}

/**
 * @brief   定时设置确认定时器启动
 * @param   none
 * @return  none
 */
VOID_T fan_timing_confirm_timer_start(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_start(TIMER_ID_TIMING_CONFIRM, CYCLE_MS_TIMING_CONFIRM, fan_confirm_timer_cb);
    if (op_ret != OPRT_OK) {
        PR_ERR("Start confirm timer fails");
    }
}

/**
 * @brief   定时确认定时器关闭
 * @param   none
 * @return  none
 */
VOID_T fan_timing_confirm_timer_stop(VOID_T)
{
    OPERATE_RET op_ret;
    op_ret = tuya_soc_software_timer_stop(TIMER_ID_TIMING_CONFIRM);
    if (op_ret != OPRT_OK) {
        PR_ERR("Stop confirm timer fails");
    }
}
