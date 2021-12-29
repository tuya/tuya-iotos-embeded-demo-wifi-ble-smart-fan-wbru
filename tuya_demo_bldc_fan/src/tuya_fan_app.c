/**
 * @file tuya_fan_app.c
 * @author lifan
 * @brief BLDC风扇应用程序
 * @version 1.0
 * @date 2021-07-29
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_app.h"
#include "tuya_fan_dp_process.h"
#include "tuya_fan_key.h"
#include "tuya_fan_led.h"
#include "tuya_fan_motor.h"
#include "tuya_fan_rotary.h"
#include "tuya_fan_timer.h"
#include "tuya_fan_flash.h"
#include "uni_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* 自然风模式挡位设置 */
#define NATURE_MODE_SPEED_SLOW_GEAR 1
#define NATURE_MODE_SPEED_FAST_GEAR 6
/* 定时设置 */
#define FAN_TIMING_NONE             0
#define FAN_TIMING_MAX              4
/* 速度设置 */
#define FAN_SPEED_DEFAULT           10

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/* 风扇速度挡位设置 */
CONST UCHAR_T g_fan_speed_gear[FAN_GEAR_NUM] = {10, 20, 35, 55, 60, 75, 85, 100};

/* 风扇数据管理 */
FAN_MANAGE_T g_fan_mag;
FAN_MANAGE_T g_fan_mag_default = {
    .power = FALSE,
    .mode = FAN_MODE_NORMAL,
    .speed = FAN_SPEED_DEFAULT,
    .bright = FAN_BRIGHT_NORMAL,
    .timing = FAN_TIMING_NONE,
    .timing_set = FAN_TIMING_NONE,
    .timing_display_flag = FALSE,
    .gear = FAN_GEAR_DEFAULT
};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   风扇开关切换
 * @param   none
 * @return  TRUE-开, FALSE-关
 */
BOOL_T fan_switch_power(VOID_T)
{
    if (g_fan_mag.power == FALSE) {
        g_fan_mag.power = TRUE;
    } else {
        g_fan_mag.power = FALSE;
    }
    return g_fan_mag.power;
}

/**
 * @brief   开/关机设置
 * @param   power: 开/关
 * @return  none
 */
VOID_T fan_set_power(IN CONST BOOL_T power)
{
    if (power) {
        PR_NOTICE("Power on.");
        /* 有定时设置时，启动定时关机定时器 */
        if (g_fan_mag.timing > FAN_TIMING_NONE) {
            PR_NOTICE("The fan will shutdown after: %dh", g_fan_mag.timing);
            fan_shutdown_timer_start();
        }
        /* 从当前模式启动 */
        fan_set_mode(g_fan_mag.mode);
    } else {
        PR_NOTICE("Power off.");
        /* 停止所有定时器 */
        fan_nature_mode_timer_stop();
        fan_sleep_mode_timer_stop();
        fan_timing_confirm_timer_stop();
        fan_shutdown_timer_stop();
        /* 关闭电机和指示灯 */
        fan_set_speed(0);
        fan_close_all_led();
        g_fan_mag.timing_display_flag = FALSE;
    }
    /* 上报/存储数据更新 */
    fan_report_all_dp_status();
    fan_write_data_to_flash();
}

/**
 * @brief   本地定时设置
 * @param   none
 * @return  none
 */
VOID_T fan_set_local_timing(VOID_T)
{
    if (g_fan_mag.timing_display_flag == FALSE) {
        /* 进入定时设置，获取当前剩余定时时间 */
        g_fan_mag.timing_set = g_fan_mag.timing;
        g_fan_mag.timing_display_flag = TRUE;
    } else {
        /* 定时时间切换：无定时->1h->2h->3h->4h->无定时 */
        if (g_fan_mag.timing_set >= FAN_TIMING_MAX) {
            g_fan_mag.timing_set = FAN_TIMING_NONE;
        } else {
            g_fan_mag.timing_set++;
        }
    }
    /* 定时设置时间显示并启动计时 */
    fan_set_timing_led(g_fan_mag.timing_set);
    fan_timing_confirm_timer_start();
}

/**
 * @brief   退出定时设置
 * @param   none
 * @return  none
 */
VOID_T fan_quit_timing_setting(VOID_T)
{
    /* 回到挡位显示 */
    g_fan_mag.timing_display_flag = FALSE;
    fan_set_gear_led(g_fan_mag.gear);
}

/**
 * @brief   定时确认回调函数
 * @param   none
 * @return  none
 */
VOID_T fan_confirm_timer_cb(VOID_T)
{
    if (g_fan_mag.timing != g_fan_mag.timing_set) {
        /* 确认定时设置 */
        g_fan_mag.timing = g_fan_mag.timing_set;
        fan_report_all_dp_status();
        /* 关闭定时 */
        fan_shutdown_timer_stop();
        /* 定时设置不是“无定时”时, 开启定时关机 */
        if (g_fan_mag.timing > FAN_TIMING_NONE) {
            fan_shutdown_timer_start();
            PR_NOTICE("The fan will shutdown after: %dh", g_fan_mag.timing);
        }
    }
    /* 退出定时设置 */
    fan_quit_timing_setting();
}

/**
 * @brief   定时关机回调函数
 * @param   none
 * @return  none
 */
VOID_T fan_shutdown_timer_cb(VOID_T)
{
    g_fan_mag.timing--;
    g_fan_mag.timing_set = g_fan_mag.timing;
    if (g_fan_mag.timing == 0) {
        fan_shutdown_timer_stop();
        fan_set_power(FALSE);
    } else {
        PR_NOTICE("The fan will shutdown after: %dh", g_fan_mag.timing);
        fan_report_all_dp_status();
        fan_write_data_to_flash();
        fan_shutdown_timer_start();
    }
}

/**
 * @brief   根据当前速度得到当前挡位
 * @param   cur_speed: 当前速度
 * @return  cur_gear: 当前挡位
 */
STATIC UCHAR_T fan_get_cur_gear_according_to_speed(IN CONST UCHAR_T cur_speed)
{
    UCHAR_T i, cur_gear;
    for (i = 0; i < FAN_GEAR_NUM; i++) {
        if (cur_speed <= g_fan_speed_gear[i]) {
            cur_gear = i;
            break;
        }
    }
    return cur_gear;
}

/**
 * @brief   直吹风模式
 * @param   none
 * @return  none
 */
STATIC VOID_T fan_mode_normal(VOID_T)
{
    /* 停止其他模式使用的定时器 */
    fan_nature_mode_timer_stop();
    fan_sleep_mode_timer_stop();
    /* 根据当前挡位设置风速并输出 */
    g_fan_mag.speed = g_fan_speed_gear[g_fan_mag.gear];
    fan_set_speed(g_fan_mag.speed);
    PR_NOTICE("fan current speed is : %d", g_fan_mag.speed);
}

/**
 * @brief   自然风模式风速设置
 * @param   start_flag: 初始化标志
 * @return  none
 */
STATIC VOID_T fan_set_nature_mode_speed(IN CONST BOOL_T start_flag)
{
    if (start_flag) {
        g_fan_mag.gear = NATURE_MODE_SPEED_FAST_GEAR;
        PR_NOTICE("natural mode fast speed");
    } else {
        if (g_fan_mag.gear == NATURE_MODE_SPEED_SLOW_GEAR) {
            g_fan_mag.gear = NATURE_MODE_SPEED_FAST_GEAR;
            PR_NOTICE("natural mode fast speed");
        } else {
            g_fan_mag.gear = NATURE_MODE_SPEED_SLOW_GEAR;
            PR_NOTICE("natural mode low speed");
        }
    }
    g_fan_mag.speed = g_fan_speed_gear[g_fan_mag.gear];
    fan_set_speed(g_fan_mag.speed);
    PR_NOTICE("fan current speed is : %d", g_fan_mag.speed);
    fan_set_gear_led(g_fan_mag.gear);
}

/**
 * @brief   自然风模式回调函数
 * @param   none
 * @return  none
 */
VOID_T fan_nature_mode_timer_cb(VOID_T)
{
    /* 风速更新，再次启动定时器 */
    fan_set_nature_mode_speed(FALSE);
    fan_nature_mode_timer_start();
    /* 上报/存储数据更新 */
    fan_report_all_dp_status();
    fan_write_data_to_flash();
}

/**
 * @brief   自然风模式
 * @param   none
 * @return  none
 */
STATIC VOID_T fan_mode_nature(VOID_T)
{
	/* 停止其他模式使用的定时器 */
    fan_sleep_mode_timer_stop();
    /* 风速设置并启动定时器 */
    fan_set_nature_mode_speed(TRUE);
    fan_nature_mode_timer_start();
}

/**
 * @brief   睡眠风模式风速设置
 * @param   start_flag: 初始化标志
 * @return  none
 */
STATIC VOID_T fan_set_sleep_mode_speed(IN CONST BOOL_T start_flag)
{
    if (!start_flag) {
        if (g_fan_mag.gear > 0) {
            g_fan_mag.gear--;
        }
    }
    PR_NOTICE("current gear is: %d", g_fan_mag.gear);
    g_fan_mag.speed = g_fan_speed_gear[g_fan_mag.gear];
    fan_set_speed(g_fan_mag.speed);
    PR_NOTICE("fan current speed is : %d", g_fan_mag.speed);
    fan_set_gear_led(g_fan_mag.gear);
}

/**
 * @brief   睡眠风模式回调函数
 * @param   none
 * @return  none
 */
VOID_T fan_sleep_mode_timer_cb(VOID_T)
{
    /* 风速更新，再次启动定时器 */
    fan_set_sleep_mode_speed(FALSE);
    if (g_fan_mag.gear != 0) {
        fan_sleep_mode_timer_start();
    }
    /* 上报/存储数据更新 */
    fan_report_all_dp_status();
    fan_write_data_to_flash();
}

/**
 * @brief   睡眠风模式
 * @param   none
 * @return  none
 */
STATIC VOID_T fan_mode_sleep(VOID_T)
{
    /* 停止其他模式使用的定时器 */
    fan_nature_mode_timer_stop();
    /* 风速设置并启动定时器 */
    fan_set_sleep_mode_speed(TRUE);
    if (g_fan_mag.gear != 0) {
        fan_sleep_mode_timer_start();
    }
}

/**
 * @brief   切换模式
 * @param   none
 * @return  工作模式
 */
UCHAR_T fan_switch_mode(VOID_T)
{
    /* 直吹风->自然风->睡眠风->直吹风 */
    if (g_fan_mag.mode == FAN_MODE_SLEEP) {
        g_fan_mag.mode = FAN_MODE_NORMAL;
    } else {
        g_fan_mag.mode++;
    }
    PR_NOTICE("fan mode changed to: %d", g_fan_mag.mode);
    return g_fan_mag.mode;
}

/**
 * @brief   设置工作模式
 * @param   mode: 工作模式
 * @return  none
 */
VOID_T fan_set_mode(IN CONST FAN_MODE_E mode)
{
    /* 各模式入口 */
    switch (mode) {
    case FAN_MODE_NORMAL:
        fan_mode_normal();
        break;
    case FAN_MODE_NATURE:
        fan_mode_nature();
        break;
    case FAN_MODE_SLEEP:
        fan_mode_sleep();
        break;
    default:
        break;
    }
    /* 指示灯状态更新 */
    fan_set_mode_led(mode);
    fan_set_gear_led(g_fan_mag.gear);
    /* 上报/存储数据更新 */
    fan_report_all_dp_status();
    fan_write_data_to_flash();
}

/**
 * @brief   云端开关设置
 * @param   power: 开关
 * @return  none
 */
VOID_T fan_set_cloud_power(IN CONST BOOL_T power)
{
    if (power != g_fan_mag.power) {
        g_fan_mag.power = power;
        fan_set_power(g_fan_mag.power);
    }
}

/**
 * @brief   云端模式设置
 * @param   mode: 模式
 * @return  none
 */
VOID_T fan_set_cloud_mode(IN CONST FAN_MODE_E mode)
{
    if (!g_fan_mag.power) {
        return;
    }
    if (mode != g_fan_mag.mode) {
        g_fan_mag.mode = mode;
        fan_set_mode(g_fan_mag.mode);
    }
}

/**
 * @brief   云端速度设置
 * @param   speed: 速度值
 * @return  none
 */
VOID_T fan_set_cloud_speed(IN CONST UCHAR_T speed)
{
    /* 关机时设置无效 */
    if (!g_fan_mag.power) {
        return;
    }
    /* 速度和挡位更新 */
    fan_set_speed(speed);
    g_fan_mag.speed = speed;
    g_fan_mag.gear = fan_get_cur_gear_according_to_speed(speed);
    fan_set_gear_led(g_fan_mag.gear);
    PR_NOTICE("fan current speed is : %d", g_fan_mag.speed);
    PR_NOTICE("fan current gear is : %d", g_fan_mag.gear);
    /* 模式如果是自然风则更新为直吹风 */
    if (g_fan_mag.mode == FAN_MODE_NATURE) {
        g_fan_mag.mode = FAN_MODE_NORMAL;
        fan_set_mode_led(g_fan_mag.mode);
        fan_nature_mode_timer_stop();
    }
    /* 模式如果是睡眠风则更新重新计时 */
    if (g_fan_mag.mode == FAN_MODE_SLEEP) {
        fan_sleep_mode_timer_stop();
        fan_sleep_mode_timer_start();
    }
    /* 更新记忆值 */
    fan_write_data_to_flash();
}

/**
 * @brief   亮度设置
 * @param   bright: 亮度
 * @return  none
 */
STATIC VOID_T fan_set_bright(IN CONST FAN_BRIGHT_E bright)
{
    switch (bright) {
    case FAN_BRIGHT_NORMAL:
        fan_set_led_dimmer(LED_DIMMER_PWM_DUTY_NORMAL);
        break;
    case FAN_BRIGHT_DARKER:
        fan_set_led_dimmer(LED_DIMMER_PWM_DUTY_DARKER);
        break;
    default:
        break;
    }
}

/**
 * @brief   云端亮度设置
 * @param   bright: 亮度
 * @return  none
 */
VOID_T fan_set_cloud_bright(IN CONST FAN_BRIGHT_E bright)
{
    if (bright != g_fan_mag.bright) {
        g_fan_mag.bright = bright;
        fan_set_bright(bright);
    }
}

/**
 * @brief   云端定时设置
 * @param   timing: 定时值
 * @return  none
 */
VOID_T fan_set_cloud_timing(IN CONST UCHAR_T timing)
{
    if ((!g_fan_mag.power) || (timing == g_fan_mag.timing)) {
        return;
    }
    if (timing > FAN_TIMING_MAX) {
        g_fan_mag.timing = FAN_TIMING_MAX;
    } else {
        g_fan_mag.timing = timing;
    }
    fan_shutdown_timer_stop();
    if (g_fan_mag.timing > 0) {
        fan_shutdown_timer_start();
    }
}

/**
 * @brief   复位
 * @param   none
 * @return  none
 */
VOID_T fan_reset(VOID_T)
{
    g_fan_mag = g_fan_mag_default;
    fan_set_bright(g_fan_mag.bright);
    fan_set_power(g_fan_mag.power);
    PR_INFO("fan reset.");
}

/**
 * @brief   BLDC风扇应用初始化程序, 被app_init调用
 * @param   none
 * @return  none
 */
VOID_T fan_app_init(VOID_T)
{
    /* 初始化 g_fan_mag */
    g_fan_mag = g_fan_mag_default;
    /* 读取flash数据 */
    fan_read_data_from_flash();
    /* 首次上电时，更新默认值 */
    if (g_fan_mag.power == 0xFF) {
        g_fan_mag = g_fan_mag_default;
        fan_write_data_to_flash();
    } else {
        g_fan_mag.power = FALSE;
    }
}

/**
 * @brief   BLDC风扇设备初始化, 被device_init调用
 * @param   none
 * @return  none
 */
VOID_T fan_device_init(VOID_T)
{
    fan_key_init();
    fan_rotary_init();
    fan_led_init();
    fan_motor_init();
}
