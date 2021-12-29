/**
 * @file tuya_fan_motor.c
 * @author lifan
 * @brief BLDC电机控制模块
 * @version 1.0
 * @date 2021-07-27
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_motor.h"
#include "tuya_gpio.h"
#include "pwmout_api.h"
#include "uni_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* Pin */
#define P_BLDC_MOTOR            TY_GPIOA_11
/* PWM周期 */
#define BLDC_MOTOR_PWM_PERIOD   1000
/* PWM占空比 */
#define BLDC_MOTOR_PWM_DUTY_OFF 5
#define BLDC_MOTOR_PWM_DUTY_MIN 30
#define BLDC_MOTOR_PWM_DUTY_MAX 99

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
pwmout_t g_bldc_motor_pwmout;

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   电机初始化
 * @param   none
 * @return  none
 */
VOID_T fan_motor_init(VOID_T)
{
    pwmout_init(&g_bldc_motor_pwmout, P_BLDC_MOTOR);
    pwmout_period_us(&g_bldc_motor_pwmout, BLDC_MOTOR_PWM_PERIOD);
    pwmout_pulsewidth_us(&g_bldc_motor_pwmout, (BLDC_MOTOR_PWM_PERIOD * BLDC_MOTOR_PWM_DUTY_OFF / 100));
}

/**
 * @brief   设置风速
 * @param   speed: 风速
 * @return  none
 */
VOID_T fan_set_speed(IN CONST UCHAR_T speed)
{
    UCHAR_T pwm_duty;

    if (speed <= 0) {
        pwmout_pulsewidth_us(&g_bldc_motor_pwmout, (BLDC_MOTOR_PWM_PERIOD * BLDC_MOTOR_PWM_DUTY_OFF / 100));
        return;
    }

    /* 由于电机在30%以下工作时间过长会出现异常，这里对 PWM 输出进行一些处理，使输出的 PWM 在 30%-99% 之间 */
    pwm_duty = (UCHAR_T)(BLDC_MOTOR_PWM_DUTY_MIN + ((BLDC_MOTOR_PWM_DUTY_MAX - BLDC_MOTOR_PWM_DUTY_MIN) * (speed / 100.0)));
    pwmout_pulsewidth_us(&g_bldc_motor_pwmout, (BLDC_MOTOR_PWM_PERIOD * pwm_duty / 100));
}
