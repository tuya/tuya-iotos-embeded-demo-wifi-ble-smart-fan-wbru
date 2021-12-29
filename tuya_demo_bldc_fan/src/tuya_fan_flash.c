/**
 * @file tuya_fan_flash.c
 * @author lifan
 * @brief Flash读写模块
 * @version 1.0
 * @date 2021-07-19
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_flash.h"
#include "tuya_fan_app.h"
#include "soc_flash.h"
#include "uni_log.h"
#include "crc_8.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* 风扇数据存储偏移量 */
#define FAN_DATA_STORAGE_OFFSET 0x00
/* 风扇数据存储长度：数据头-风扇开关-风扇模式-风扇转速-定时时间-CRC_8校验 */
#define FAN_DATA_STORAGE_LEN    6
/* 数据帧头 */
#define FAN_DATA_HEADER         0xFF
/* 风扇状态数据存放位置 */
#define FAN_STATE_POWER         1
#define FAN_STATE_MODE          2
#define FAN_STATE_SPEED         3
#define FAN_STATE_TIMING        4

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
 * @brief   读取flash存储的风扇数据
 * @param   none
 * @return  none
 */
VOID_T fan_read_data_from_flash(VOID_T)
{
    INT_T read_cnt;
    UCHAR_T fan_state_data_crc;
    UCHAR_T fan_state_buffer[FAN_DATA_STORAGE_LEN];

    /* 读取flash数据 */
    read_cnt = tuya_soc_flash_read(SAVE_TYP1, FAN_DATA_STORAGE_OFFSET, FAN_DATA_STORAGE_LEN, (UCHAR_T *)fan_state_buffer);
    if (read_cnt <= 0) {
        PR_ERR("Reset cnt read error!");
        return;
    }

    /* 检查数据头 */
    if (fan_state_buffer[0] != FAN_DATA_HEADER) {
        PR_ERR("data head error");
        return;
    }
    /* 检查校验数据 */
    fan_state_data_crc = get_crc_8(fan_state_buffer, (FAN_DATA_STORAGE_LEN - 1) * SIZEOF(UCHAR_T));
    if (fan_state_data_crc != fan_state_buffer[FAN_DATA_STORAGE_LEN - 1]) { 
        PR_ERR("crc error, before_fan_power_off_state[%d] = %02x, crc data = %02x.", FAN_DATA_STORAGE_LEN - 1, fan_state_buffer[FAN_DATA_STORAGE_LEN - 1], fan_state_data_crc);
        return;
    }

    /* 更新风扇数据 */
    g_fan_mag.power = fan_state_buffer[FAN_STATE_POWER];
    g_fan_mag.mode = fan_state_buffer[FAN_STATE_MODE];
    g_fan_mag.speed = fan_state_buffer[FAN_STATE_SPEED];
    g_fan_mag.timing = fan_state_buffer[FAN_STATE_TIMING];
}

/**
 * @brief   将风扇数据写入flash
 * @param   none
 * @return  none
 */
VOID_T fan_write_data_to_flash(VOID_T)
{
    INT_T i;
    OPERATE_RET opRet;
    UCHAR_T fan_data_buffer[FAN_DATA_STORAGE_LEN];

    /* 更新风扇数据到buffer */
    fan_data_buffer[0] = FAN_DATA_HEADER;
    fan_data_buffer[FAN_STATE_POWER] = g_fan_mag.power;
    fan_data_buffer[FAN_STATE_MODE] = g_fan_mag.mode;
    fan_data_buffer[FAN_STATE_SPEED] = g_fan_mag.speed;
    fan_data_buffer[FAN_STATE_TIMING] = g_fan_mag.timing;
    fan_data_buffer[FAN_DATA_STORAGE_LEN-1] = get_crc_8(fan_data_buffer, (FAN_DATA_STORAGE_LEN - 1) * SIZEOF(UCHAR_T));
    /* 打印buffer */
    for (i = 0; i < FAN_DATA_STORAGE_LEN; i++) {
        PR_NOTICE(" +++ fan_data_buffer is [%d] : %02x", i, fan_data_buffer[i]);
    }

    /* 写入flash数据 */
    opRet = tuya_soc_flash_write(SAVE_TYP1, FAN_DATA_STORAGE_OFFSET, fan_data_buffer, FAN_DATA_STORAGE_LEN); 
    if (opRet != OPRT_OK) {
        PR_ERR("write flash error");
    }

    return;
}
