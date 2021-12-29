/**
 * @file tuya_rotary.h
 * @author lifan
 * @brief rotary encoder driver
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_ROTARY_H__
#define __TUYA_ROTARY_H__

#include <stdint.h>
#include "tuya_gpio.h"
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __TUYA_ROTARY_GLOBALS
    #define __TUYA_ROTARY_EXT
#else
    #define __TUYA_ROTARY_EXT extern
#endif

/***********************************************************
************************micro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef VOID(* ROTARY_CALLBACK)(BOOL_T dir);
/* User define */
typedef struct {
    TY_GPIO_PORT_E port_a;
    TY_GPIO_PORT_E port_b;
    ROTARY_CALLBACK call_back;
} ROTARY_USER_DEF_S;

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief   rotary init
 * @param   p_tbl: rotary table
 * @param   cnt: rotary count
 * @return  OPERATE_RET
 */
OPERATE_RET rotary_init(IN CONST ROTARY_USER_DEF_S *p_tbl, IN CONST INT_T cnt);

/**
 * @brief   register rotary
 * @param   rotary_ud: rotary user define
 * @return  OPERATE_RET
 */
OPERATE_RET reg_proc_rotary(IN CONST ROTARY_USER_DEF_S *rotary_ud);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_ROTARY_H__ */
