/**
 * @file tuya_device.h
 * @author lifan
 * @brief 应用设备管理模块，用于填写固件key、产品key等
 * @version 1.0
 * @date 2021-07-19
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_DEVICE_H__
#define __TUYA_DEVICE_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT
#else
    #define _TUYA_DEVICE_EXT extern
#endif

/***********************************************************
************************micro define************************
***********************************************************/
/* 固件信息 */
#define PRODUCT_KEY     "xxxxxxxxxxxxxxxx"  /* 产品ID */
#define DEV_SW_VERSION  USER_SW_VER

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_DEVICE_H__ */
