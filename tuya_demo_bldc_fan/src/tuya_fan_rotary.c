/**
 * @file tuya_fan_rotary.c
 * @author lifan
 * @brief 旋钮编码器应用模块
 * @version 1.0
 * @date 2021-07-30
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_rotary.h"
#include "tuya_fan_app.h"
#include "tuya_fan_timer.h"
#include "tuya_gpio.h"
#include "tuya_rotary.h"
#include "uni_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* Pin */
#define P_ROTARY_A  TY_GPIOA_18   /* 旋钮编码器-A */
#define P_ROTARY_B  TY_GPIOA_0    /* 旋钮编码器-B */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
VOID_T fan_rotary_cb(BOOL_T dir);

ROTARY_USER_DEF_S g_rotary_def = {
    .port_a = P_ROTARY_A,
    .port_b = P_ROTARY_B,
    .call_back = fan_rotary_cb
};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   旋钮编码器回调函数
 * @param   dir: 旋转方向 TRUE-顺时针 FALSE-逆时针
 * @return  none
 */
VOID_T fan_rotary_cb(BOOL_T dir)
{
    /* 关机时旋钮无效 */
    if (g_fan_mag.power == FALSE) {
        return;
    }
    /* 旋转旋钮时可退出定时设置 */
    if (g_fan_mag.timing_display_flag) {
        fan_timing_confirm_timer_stop();
        fan_quit_timing_setting();
    }
    /* 根据转向加大或减小挡位 */
    if (dir) {
        PR_NOTICE("The direction of rotation is CW.");
        g_fan_mag.gear = (g_fan_mag.gear >= (FAN_GEAR_NUM-1)) ? (FAN_GEAR_NUM-1) : (g_fan_mag.gear+1);
    } else {
        PR_NOTICE("The direction of rotation is CCW.");
        g_fan_mag.gear = (g_fan_mag.gear <= 0) ? (0) : (g_fan_mag.gear-1);
    }
    PR_NOTICE("fan current gear is : %d", g_fan_mag.gear);
    /* 更新风速输出 (自然风模式下挡位变化时默认切换到直吹风模式) */
    if (g_fan_mag.mode == FAN_MODE_SLEEP) {
        fan_set_mode(FAN_MODE_SLEEP);
    } else {
        fan_set_mode(FAN_MODE_NORMAL);
    }
}

/**
 * @brief   旋钮编码器初始化
 * @param   none
 * @return  none
 */
VOID_T fan_rotary_init(VOID_T)
{
    OPERATE_RET opRet;
    /* 初始化 */
    opRet = rotary_init(NULL, 0);
    if (opRet != OPRT_OK) {
        PR_ERR("rotary_init err:%d", opRet);
        return;
    }
    opRet = reg_proc_rotary(&g_rotary_def);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_rotary err:%d", opRet);
    }
}
