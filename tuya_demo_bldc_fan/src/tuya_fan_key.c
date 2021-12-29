/**
 * @file tuya_fan_key.c
 * @author lifan
 * @brief 按键检测模块
 * @version 1.0
 * @date 2021-07-29
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_key.h"
#include "tuya_fan_app.h"
#include "tuya_fan_timer.h"
#include "tuya_gpio.h"
#include "tuya_key.h"
#include "uni_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* 按键端口 */
#define P_KEY_POWER                 TY_GPIOA_7
#define P_KEY_TIMER                 TY_GPIOA_8
#define P_KEY_WIFI                  TY_GPIOA_19
#define P_KEY_ROTARY_N              TY_GPIOA_17
/* 按键配置 */
#define KEY_SEQ_PRESS_TIME_MS       400     /* 连按时间: 400ms */
#define KEY_MODE_LONG_PRESS_TIME_MS 10000   /* 模式键(复位)长按时间: 10s */
#define KEY_WIFI_LONG_PRESS_TIME_MS 3000    /* 配网键长按时间: 3s */
#define KEY_LONG_PRESS_TIME_MS      10000   /* 默认长按时间: 10s */

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/* 按键定义 */
KEY_USER_DEF_S g_key_def;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   开关键处理函数
 * @param   type: 触发类型
 * @param   cnt: 连击次数
 * @return  none
 */
STATIC VOID_T fan_key_power_handler(IN CONST PUSH_KEY_TYPE_E type, IN CONST INT_T cnt)
{
    switch (type) {
    case NORMAL_KEY:
        PR_DEBUG("power press.");
        fan_set_power(fan_switch_power());  /* 设置开/关机 */
        break;
    case LONG_KEY:
        PR_DEBUG("power long press.");
        break;
    case SEQ_KEY:
        PR_DEBUG("power SEQ press, the count is %d.", cnt);
        break;
    default:
        break;
    }
}

/**
 * @brief   定时键处理函数
 * @param   type: 触发类型
 * @param   cnt: 连击次数
 * @return  none
 */
STATIC VOID_T fan_key_timer_handler(IN CONST PUSH_KEY_TYPE_E type, IN CONST INT_T cnt)
{
    switch (type) {
    case NORMAL_KEY:
        PR_DEBUG("timer press.");
        if (g_fan_mag.power == TRUE) {  /* 开机时 */
            fan_set_local_timing();     /* 设置本地定时 */
        }
        break;
    case LONG_KEY:
        PR_DEBUG("timer long press.");
        break;
    case SEQ_KEY:
        PR_DEBUG("timer SEQ press, the count is %d.", cnt);
        break;
    default:
        break;
    }
}

/**
 * @brief   配网键处理函数
 * @param   type: 触发类型
 * @param   cnt: 连击次数
 * @return  none
 */
STATIC VOID_T fan_key_wifi_handler(IN CONST PUSH_KEY_TYPE_E type, IN CONST INT_T cnt)
{
    switch (type) {
    case NORMAL_KEY:
        PR_DEBUG("wifi press.");
        break;
    case LONG_KEY:
        PR_DEBUG("wifi long press.");
        tuya_iot_wf_gw_unactive();  /* 手动移除设备 */
        break;
    case SEQ_KEY:
        PR_DEBUG("wifi SEQ press, the count is %d.", cnt);
        break;
    default:
        break;
    }
}

/**
 * @brief   模式键处理函数
 * @param   type: 触发类型
 * @param   cnt: 连击次数
 * @return  none
 */
STATIC VOID_T fan_key_mode_handler(IN CONST PUSH_KEY_TYPE_E type, IN CONST INT_T cnt)
{
    switch (type) {
    case NORMAL_KEY:
        PR_DEBUG("mode press.");
        if (g_fan_mag.power == TRUE) {          /* 开机时 */
            fan_set_mode(fan_switch_mode());    /* 设置工作模式 */
        }
        break;
    case LONG_KEY:
        PR_DEBUG("mode long press.");
        fan_reset();                            /* 复位 */
        break;
    case SEQ_KEY:
        PR_DEBUG("mode SEQ press, the count is %d.", cnt);
        break;
    default:
        break;
    }
}

/**
 * @brief   按键事件回调函数
 * @param   port: 按键端口
 * @param   type: 按键触发类型
 * @param   cnt: 连击次数
 * @return  none
 */
STATIC VOID_T fan_key_press_cb(IN CONST TY_GPIO_PORT_E port, IN CONST PUSH_KEY_TYPE_E type, IN CONST INT_T cnt)
{
    PR_DEBUG("port: %d, type: %d, cnt: %d", port, type, cnt);
    /* 按其他键退出定时设置 */
    if (g_fan_mag.timing_display_flag) {
        fan_timing_confirm_timer_stop();
        if (port != P_KEY_TIMER) {
            fan_quit_timing_setting();
        }
    }

    switch (port) {
    case P_KEY_POWER:
        fan_key_power_handler(type, cnt);
        break;
    case P_KEY_TIMER:
        fan_key_timer_handler(type, cnt);
        break;
    case P_KEY_WIFI:
        fan_key_wifi_handler(type, cnt);
        break;
    case P_KEY_ROTARY_N:
        fan_key_mode_handler(type, cnt);
        break;
    default:
        break;
    }
}

/**
 * @brief   按键初始化
 * @param   none
 * @return  none
 */
VOID_T fan_key_init(VOID_T)
{
    OPERATE_RET opRet;
    /* 初始化 */
    opRet = key_init(NULL, 0, 0);
    if (opRet != OPRT_OK) {
        PR_ERR("key_init err:%d", opRet);
        return;
    }
    /* 用户定义初始化 */
    memset(&g_key_def, 0, SIZEOF(KEY_USER_DEF_S));
    g_key_def.low_level_detect = TRUE;
    g_key_def.lp_tp = LP_ONCE_TRIG;
    g_key_def.seq_key_detect_time = KEY_SEQ_PRESS_TIME_MS;
    g_key_def.call_back = fan_key_press_cb;
    /* 开关键 */
    g_key_def.port = P_KEY_POWER;
    g_key_def.long_key_time = KEY_LONG_PRESS_TIME_MS;
    opRet = reg_proc_key(&g_key_def);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }
    /* 定时键 */
    g_key_def.port = P_KEY_TIMER;
    g_key_def.long_key_time = KEY_LONG_PRESS_TIME_MS;
    opRet = reg_proc_key(&g_key_def);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }
    /* 配网键 */
    g_key_def.port = P_KEY_WIFI;
    g_key_def.long_key_time = KEY_WIFI_LONG_PRESS_TIME_MS;
    opRet = reg_proc_key(&g_key_def);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }
    /* 模式键/复位键 */
    g_key_def.port = P_KEY_ROTARY_N;
    g_key_def.long_key_time = KEY_MODE_LONG_PRESS_TIME_MS;
    opRet = reg_proc_key(&g_key_def);
    if (opRet != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", opRet);
        return;
    }
}
