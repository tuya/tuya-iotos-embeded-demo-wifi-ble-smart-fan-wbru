/**
 * @file tuya_fan_dp_process.c
 * @author lifan
 * @brief DP信息处理模块
 * @version 1.0
 * @date 2021-07-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_fan_dp_process.h"
#include "tuya_fan_app.h"
#include "tuya_iot_wifi_api.h"
#include "uni_log.h"
#include "gw_intf.h"
#include <string.h>

/***********************************************************
************************micro define************************
***********************************************************/
/* DP ID */
#define DP_ID_SWITCH            1
#define DP_ID_MODE              2
#define DP_ID_FAN_SPEED         3
#define DP_ID_BRIGHT_MODE       101
#define DP_ID_COUNTDOWN_SET     22
/* DP amount */
#define FAN_DP_AMOUNT           5

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
 * @brief   上报所有dp点
 * @param   none
 * @return  none
 */
VOID_T fan_report_all_dp_status(VOID_T)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* 没有连接到路由器，退出 */
    GW_WIFI_NW_STAT_E wifi_state = STAT_LOW_POWER;
    get_wf_gw_nw_status(&wifi_state);
    if (wifi_state <= STAT_AP_STA_DISC || wifi_state == STAT_STA_DISC) {
        return;
    }
    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(FAN_DP_AMOUNT*SIZEOF(TY_OBJ_DP_S));

    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    memset(dp_arr, 0, FAN_DP_AMOUNT*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DP_ID_SWITCH;
    dp_arr[0].type = PROP_BOOL;
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = g_fan_mag.power;

    dp_arr[1].dpid = DP_ID_MODE;
    dp_arr[1].type = PROP_ENUM;
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_enum = g_fan_mag.mode;

    dp_arr[2].dpid = DP_ID_FAN_SPEED;
    dp_arr[2].type = PROP_VALUE;
    dp_arr[2].time_stamp = 0;
    dp_arr[2].value.dp_value = g_fan_mag.speed;

    dp_arr[3].dpid = DP_ID_BRIGHT_MODE;
    dp_arr[3].type = PROP_ENUM;
    dp_arr[3].time_stamp = 0;
    dp_arr[3].value.dp_enum = g_fan_mag.bright;

    dp_arr[4].dpid = DP_ID_COUNTDOWN_SET;
    dp_arr[4].type = PROP_ENUM;
    dp_arr[4].time_stamp = 0;
    dp_arr[4].value.dp_enum = g_fan_mag.timing;

    op_ret = dev_report_dp_json_async(NULL, dp_arr, FAN_DP_AMOUNT);
    Free(dp_arr);
    if (OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error, err_num: %d", op_ret);
    }

    PR_DEBUG("dp_query report_all_dp_data");
}

/**
 * @brief   处理dp信息
 * @param   root: dp信息
 * @return  none
 */
VOID_T fan_deal_dp_proc(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;

    dpid = root->dpid;
    PR_NOTICE("dpid:%d", dpid);

    switch(dpid) {
    case DP_ID_SWITCH:
        PR_NOTICE("root->value.dp_bool:%d", root->value.dp_bool);
        fan_set_cloud_power(root->value.dp_bool);
        break;
    case DP_ID_MODE:
        PR_NOTICE("root->value.dp_enum:%d", root->value.dp_enum);
        fan_set_cloud_mode((FAN_MODE_E)root->value.dp_enum);
        break;
    case DP_ID_FAN_SPEED:
        PR_NOTICE("root->value.dp_value:%d", root->value.dp_value);
        fan_set_cloud_speed((UCHAR_T)root->value.dp_value);
        break;
    case DP_ID_BRIGHT_MODE:
        PR_NOTICE("root->value.dp_enum:%d", root->value.dp_enum);
        fan_set_cloud_bright((FAN_BRIGHT_E)root->value.dp_enum);
        break;
    case DP_ID_COUNTDOWN_SET:
        PR_NOTICE("root->value.dp_enum:%d", root->value.dp_enum);
        fan_set_cloud_timing((UCHAR_T)root->value.dp_enum);
        break;
    default:
        break;
    }
}
