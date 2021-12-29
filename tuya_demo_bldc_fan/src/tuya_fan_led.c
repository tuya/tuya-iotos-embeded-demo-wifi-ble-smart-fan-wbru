/**
 * @file tuya_fan_led.c
 * @author lifan
 * @brief 指示灯控制模块
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_led.h"
#include "tuya_fan_app.h"
#include "tuya_gpio.h"
#include "tuya_led.h"
#include "tuya_iot_wifi_api.h"
#include "pwmout_api.h"
#include "uni_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* LED句柄 */
#define LED_HANDLE_D1               0
#define LED_HANDLE_D2               1
#define LED_HANDLE_D3               2
#define LED_HANDLE_D4               3
#define LED_HANDLE_MODE_WIND        4
#define LED_HANDLE_MODE_NATURE      5
#define LED_HANDLE_MODE_SLEEP       6
#define LED_HANDLE_WIFI             7

#define LED_GEAR_NUM                4       /* 挡位指示灯数量 */
#define LED_WIFI_FAST_FLASH_MS      300     /* 快闪 300ms */
#define LED_WIFI_SLOW_FLASH_MS      500     /* 慢闪 500ms */

/* LED亮度调节PWM */
#define P_LED_DIMMER                TY_GPIOA_12
#define LED_DIMMER_PWM_PERIOD       1000    /* PWM周期: 1ms(1kHz) */


/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/* LED端口 */
STATIC UCHAR_T sg_fan_led_port[] = {
    TY_GPIOA_9,     /* D1 */
    TY_GPIOA_2,     /* D2 */
    TY_GPIOA_3,     /* D3 */
    TY_GPIOA_4,     /* D4 */
    TY_GPIOA_14,    /* MODE_WIND */
    TY_GPIOA_10,    /* MODE_NATURE */
    TY_GPIOA_20,    /* MODE_SLEEP */
    TY_GPIOA_13     /* WIFI */
};

/* LED句柄 */
LED_HANDLE fan_led_handle[(SIZEOF(sg_fan_led_port) / SIZEOF(sg_fan_led_port[0]))];

/* LED亮度调节PWM */
pwmout_t g_led_dimmer_pwmout;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   设置挡位指示灯
 * @param   cur_gear: 当前挡位
 * @return  none
 */
VOID_T fan_set_gear_led(IN CONST UCHAR_T cur_gear)
{
    UCHAR_T i;

    if (g_fan_mag.timing_display_flag) {
        return;
    }

    for (i = 0; i < LED_GEAR_NUM; i++) {
        if (i == (cur_gear / 2)) {
            if (cur_gear % 2) {
                /* LED闪烁 */
                tuya_set_led_light_type(fan_led_handle[i], OL_FLASH_HIGH, 0, 0xffff);
            } else {
                /* LED常亮 */
                tuya_set_led_light_type(fan_led_handle[i], OL_LOW, 0, 0xffff);
            }
        } else if (i < (cur_gear/2)) {
            /* LED常亮 */
            tuya_set_led_light_type(fan_led_handle[i], OL_LOW, 0, 0xffff);
        } else {
            /* LED常灭 */
            tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff);
        }
    }
}

/**
 * @brief   设置模式指示灯
 * @param   cur_mode: 当前工作模式
 * @return  none
 */
VOID_T fan_set_mode_led(IN CONST UCHAR_T cur_mode)
{
    /* 关闭所有模式指示灯 */
    tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_WIND], OL_HIGH, 0, 0xffff);
    tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_NATURE], OL_HIGH, 0, 0xffff);
    tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_SLEEP], OL_HIGH, 0, 0xffff);
    /* 根据当前模式打开对应的指示灯 */
    if (cur_mode == FAN_MODE_NORMAL) {          /* 直吹风 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_WIND], OL_LOW, 0, 0xffff);
    } else if (cur_mode == FAN_MODE_NATURE) {   /* 自然风 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_NATURE], OL_LOW, 0, 0xffff);
    } else {                                    /* 睡眠风 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_MODE_SLEEP], OL_LOW, 0, 0xffff);
    }
}

/**
 * @brief   设置配网指示灯
 * @param   cur_stat: 当前wifi状态
 * @return  none
 */
VOID_T fan_set_wifi_led(IN CONST GW_WIFI_NW_STAT_E cur_stat)
{
    switch (cur_stat) {
    case STAT_LOW_POWER:    /* wifi 连接超时，进入低功耗模式 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_WIFI], OL_HIGH, 0, 0);/* 关闭指示灯 */
        break;
    case STAT_UNPROVISION:  /* SamrtConfig 配网模式，等待连接 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_WIFI], OL_FLASH_HIGH, LED_WIFI_FAST_FLASH_MS, 0xFFFF);/* 指示灯快闪 */
        break;
    case STAT_AP_STA_UNCFG: /* ap 配网模式，等待连接 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_WIFI], OL_FLASH_HIGH, LED_WIFI_SLOW_FLASH_MS, 0xFFFF);/* 指示灯慢闪 */
        break;
    case STAT_AP_STA_DISC:
    case STAT_STA_DISC:     /* SamrtConfig/ap 正在连接中 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_WIFI], OL_HIGH, 0, 0);/* 关闭指示灯 */
        break;
    case STAT_CLOUD_CONN:
    case STAT_AP_CLOUD_CONN:/* 连接到涂鸦云 */
        tuya_set_led_light_type(fan_led_handle[LED_HANDLE_WIFI], OL_LOW, 0, 0); /* 指示灯常亮 */
        break;
    default:
        break;
    }
}

/**
 * @brief   设置定时指示灯(与挡位指示灯共用)
 * @param   hour: 定时时间
 * @return  none
 */
VOID_T fan_set_timing_led(IN CONST UCHAR_T hour)
{
    UCHAR_T i;

    if (!g_fan_mag.timing_display_flag) {
        return;
    }

    /* 无定时，关闭所有指示灯 */
    if (hour > LED_GEAR_NUM) {
        for (i = 0; i < LED_GEAR_NUM; i++) {
            tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff);
        }
        return;
    }
    /* 点亮对应指示灯 */
    for (i = 0; i < LED_GEAR_NUM; i++) {
        if (i < hour) {
            tuya_set_led_light_type(fan_led_handle[i], OL_LOW, 0, 0xffff);
        } else {
            tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff);
        }
    }
}

/**
 * @brief   设置指示灯亮度
 * @param   brightness: 10-1000, 越大灯越亮
 * @return  none
 */
VOID_T fan_set_led_dimmer(IN CONST USHORT_T brightness)
{
    pwmout_pulsewidth_us(&g_led_dimmer_pwmout, (LED_DIMMER_PWM_PERIOD - brightness));
}

/**
 * @brief   关闭所有指示灯(除Wi-Fi指示灯)
 * @param   none
 * @return  none
 */
VOID_T fan_close_all_led(VOID_T)
{
    INT_T i;
    for (i = 0; i < (SIZEOF(sg_fan_led_port) / SIZEOF(sg_fan_led_port[0]) - 1); i++) {
        tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff); /* 关闭LED */
    }
}

/**
 * @brief   指示灯亮度调节PWM初始化
 * @param   none
 * @return  none
 */
STATIC VOID_T fan_led_dimmer_init(VOID_T)
{
    pwmout_init(&g_led_dimmer_pwmout, P_LED_DIMMER);
    pwmout_period_us(&g_led_dimmer_pwmout, LED_DIMMER_PWM_PERIOD);
    pwmout_pulsewidth_us(&g_led_dimmer_pwmout, (LED_DIMMER_PWM_PERIOD - LED_DIMMER_PWM_DUTY_NORMAL));
}

/**
 * @brief   指示灯初始化
 * @param   none
 * @return  none
 */
VOID_T fan_led_init(VOID_T)
{
    INT_T i;
    OPERATE_RET op_ret = OPRT_OK;
    /* 指示灯初始化 */
    for (i = 0; i < (SIZEOF(sg_fan_led_port) / SIZEOF(sg_fan_led_port[0])); i++) {
        op_ret = tuya_create_led_handle(sg_fan_led_port[i], FALSE, &fan_led_handle[i]);
        if (op_ret != OPRT_OK) {
            PR_ERR("led init err:%d", op_ret);
            return;
        }
        tuya_set_led_light_type(fan_led_handle[i], OL_HIGH, 0, 0xffff); /* 关闭LED */
    }
    /* 指示灯亮度初始化 */
    fan_led_dimmer_init();
}
