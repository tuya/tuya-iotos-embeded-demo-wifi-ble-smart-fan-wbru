/**
 * @file tuya_device.c
 * @author lifan
 * @brief 设备初始化流程处理模块
 * @version 1.0
 * @date 2021-07-30
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_device.h"
#include "tuya_fan_app.h"
#include "tuya_fan_led.h"
#include "tuya_fan_dp_process.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_uart.h"
#include "uni_log.h"
#include "sys_api.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* 工作模式选择 */
#define WIFI_WORK_MODE_SEL  GWCM_LOW_POWER

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
 * @brief   SDK内部状态变化时回调
 * @param   status: 当前状态
 * @return  none
 */
VOID gw_status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_NOTICE("gateway status changed to:%d", status);

    if (GW_NORMAL == status) {
        fan_report_all_dp_status();
    } else if (GW_RESET == status) {
        PR_NOTICE("status is GW_RESET");
    } else {
        ;
    }
}

/**
 * @brief   firmware download content storage callback
 * @param   fw: firmware info
 * @param   total_len: firmware total size
 * @param   offset: offset of this download package
 * @param   data && len: this download package
 * @param   pri_data: private data
 * @param   remain_len: the size left to process in next cb
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                             IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    PR_DEBUG("Rev File Data");
    PR_DEBUG("Total_len:%d", total_len);
    PR_DEBUG("Offset:%d Len:%d", offset, len);

    return OPRT_OK;
}

/**
 * @brief   firmware download finish callback
 * @param   fw: firmware info
 * @param   download_result: 0-success other-fail
 * @param   pri_data: private data
 * @return  none
 */
VOID upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    PR_DEBUG("download finished");
    PR_DEBUG("download_result:%d", download_result);
}

/**
 * @brief   固件更新时回调
 * @param   fw: 固件信息
 * @return  OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_RET gw_ug_inform_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev GW Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%d", fw->file_size);

    return tuya_iot_upgrade_gw(fw, get_file_data_cb, upgrade_notify_cb, NULL);
}

/**
 * @brief   SDK进行设备重置时回调
 * @param   type: 复位类型
 * @return  none
 */
VOID gw_reset_ifm_cb(IN CONST GW_RESET_TYPE_E type)
{
    PR_DEBUG("gw_reset_ifm_cb type:%d", type);
    if (GW_REMOTE_RESET_FACTORY != type) {
        PR_DEBUG("type is GW_REMOTE_RESET_FACTORY");
        return;
    }
}

/**
 * @brief   SDK收到DP时回调
 * @param   dp: obj dp info
 * @return  none
 */
VOID dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_DEBUG("dp->cid: %s dp->dps_cnt: %d", dp->cid, dp->dps_cnt);
    UCHAR_T i = 0;

    for (i = 0; i < dp->dps_cnt; i++) {
        fan_deal_dp_proc(&(dp->dps[i]));
    }
    fan_report_all_dp_status();
}

/**
 * @brief   SDK收到RAW DP时回调
 * @param   dp: raw dp info
 * @return  none
 */
VOID dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_DEBUG("raw data dpid:%d", dp->dpid);
    PR_DEBUG("recv len:%d", dp->len);
#if 1
    INT_T i = 0;
    for (i = 0; i < dp->len; i++) {
        PR_DEBUG_RAW("%02X ", dp->data[i]);
    }
#endif
    PR_DEBUG_RAW("\n");
    PR_DEBUG("end");
    return;
}

/**
 * @brief   SDK收到DP查询命令时回调
 * @param   dp_qry: query info
 * @return  none
 */
STATIC VOID dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry) 
{
    PR_NOTICE("Recv DP Query Cmd");
    fan_report_all_dp_status();
}

/**
 * @brief   涂鸦SDK网络检测回调
 * @param   stat: 网络状态
 * @return  none
 */
VOID wf_nw_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_NOTICE("wf_nw_status_cb, wifi_status: %d", stat);
    fan_set_wifi_led(stat);

    if (stat == STAT_AP_STA_CONN || stat >= STAT_STA_CONN) {
        fan_report_all_dp_status();
    }
}

/**
 * @brief   应用初始化前置准备工作, 被user_main调用
 * @param   none
 * @return  none
 */
VOID pre_tuya_iot_init(VOID)
{
    /* 应用初始化前置准备工作 */
    return;
}

/**
 * @brief   设备初始化前置准备工作, 被user_main调用
 * @param   none
 * @return  none
 */
VOID pre_device_init(VOID)
{
    PR_DEBUG("%s", tuya_iot_get_sdk_info());
    PR_DEBUG("%s:%s", APP_BIN_NAME, DEV_SW_VERSION);
    PR_NOTICE("firmware compiled at %s %s", __DATE__, __TIME__);
    PR_NOTICE("system reset reason:[%s]", tuya_hal_system_get_rst_info());
    /* 设置日志级别 */
    SetLogManageAttr(TY_LOG_LEVEL_INFO);

    return;
}

/**
 * @brief   应用初始化程序, 被user_main调用
 * @param   none
 * @return  none
 */
VOID app_init(VOID)
{
    fan_app_init();

    return;
}

/**
 * @brief   设备初始化, 被user_main调用
 * @param   none
 * @return  OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_RET device_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;
    /* SDK用户回调接口 */
    TY_IOT_CBS_S wf_cbs = {
        gw_status_changed_cb,\
        gw_ug_inform_cb,\
        gw_reset_ifm_cb,\
        dev_obj_dp_cmd_cb,\
        dev_raw_dp_cmd_cb,\
        dev_dp_query_cb,\
        NULL,
    };
    /* Wi-Fi SOC设备初始化 */
    op_ret = tuya_iot_wf_soc_dev_init_param(WIFI_WORK_MODE_SEL, WF_START_SMART_FIRST, &wf_cbs, NULL, PRODUCT_KEY, DEV_SW_VERSION);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init_param error, err_num:%d", op_ret);
        return op_ret;
    }
    /* 获取Wi-Fi状态 */
    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(wf_nw_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb is error, err_num:%d", op_ret);
        return op_ret;
    }

    fan_device_init();

    return OPRT_OK;
}

/**
 * @brief   授权时擦除用户数据的回调函数, 被user_main调用
 * @param   none
 * @return  none
 */
VOID mf_user_callback(VOID)
{
    /* 授权时擦除用户数据 */
    return;
}
