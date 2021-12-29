/**
 * @file tuya_fan_dp_process.h
 * @author lifan
 * @brief DP信息处理模块
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_FAN_DP_PROCESS_H__
#define __TUYA_FAN_DP_PROCESS_H__

#include "tuya_cloud_com_defs.h"

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
VOID_T fan_report_all_dp_status(VOID_T);
VOID_T fan_deal_dp_proc(IN CONST TY_OBJ_DP_S *root);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_FAN_DP_PROCESS_H__ */
