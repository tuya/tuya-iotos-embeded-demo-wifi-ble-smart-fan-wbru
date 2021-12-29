/**
 * @file tuya_rotary.c
 * @author lifan
 * @brief rotary encoder driver
 * @version 1.0
 * @date 2021-07-26
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_rotary.h"
#include "tuya_hal_mutex.h"
#include "tuya_hal_semaphore.h"
#include "uni_log.h"
#include "uni_thread.h"
#include "mem_pool.h"
#include "sys_timer.h"
#include "freertos_pmu.h"
#include "wf_basic_intf.h"
#include <string.h>

/***********************************************************
************************micro define************************
***********************************************************/
#define __TUYA_ROTARY_GLOBALS

#define TIMER_SPACE 1   /* 1ms */

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef enum {
    ROTARY_NO_ACTION = 0,
    ROTARY_LOW_CHECK,
    ROTARY_A_LOW_CONFIRM,
    ROTARY_A_LOW_B_HIGH_CONFIRM,
    ROTARY_A_LOW_B_LOW_CONFIRM,
    ROTARY_B_LOW_CONFIRM,
    ROTARY_B_LOW_A_HIGH_CONFIRM,
    ROTARY_B_LOW_A_LOW_CONFIRM
} ROTARY_STAT_E;

typedef struct {
    ROTARY_USER_DEF_S rud;
    ROTARY_STAT_E status;
    UCHAR_T counter;
    BOOL_T dir;
} ROTARY_ENTITY_S;

typedef struct rotary_en_lst {
    struct rotary_en_lst *nxt;
    ROTARY_ENTITY_S rotary_ent;
} ROTARY_EN_LST_S;

typedef struct {
    ROTARY_ENTITY_S *p_tbl;
    INT_T tbl_cnt;
    ROTARY_EN_LST_S *lst;
    MUTEX_HANDLE mutex;
} ROTARY_MANAGE_S;

/***********************************************************
***********************variable define**********************
***********************************************************/
STATIC ROTARY_MANAGE_S *rotary_mag = NULL;
STATIC THRD_HANDLE rotary_handle = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/
STATIC VOID rotary_handle_thrd(PVOID_T arg);
STATIC VOID __rotary_handle(VOID);
STATIC VOID __rotary_ent_proc(INOUT ROTARY_ENTITY_S *rotary_ent);

/**
 * @brief   rotary init
 * @param   p_tbl: rotary table
 * @param   cnt: rotary count
 * @return  OPERATE_RET
 */
OPERATE_RET rotary_init(IN CONST ROTARY_USER_DEF_S *p_tbl, IN CONST INT_T cnt)
{
    if (rotary_mag) {
        return OPRT_OK;
    }
    rotary_mag = (ROTARY_MANAGE_S *)Malloc(SIZEOF(ROTARY_MANAGE_S));
    if (NULL == rotary_mag) {
        PR_ERR("Malloc err");
        return OPRT_MALLOC_FAILED;
    }
    memset(rotary_mag, 0, sizeof(rotary_mag));
    OPERATE_RET op_ret = OPRT_OK;

    if (cnt != 0) {
        rotary_mag->p_tbl = (ROTARY_ENTITY_S *)Malloc(SIZEOF(ROTARY_ENTITY_S) * cnt);
        if (NULL == rotary_mag->p_tbl) {
            PR_ERR("Malloc err");
            op_ret = OPRT_MALLOC_FAILED;
            goto ERR_EXIT;
        }
        memset(rotary_mag->p_tbl, 0, SIZEOF(ROTARY_ENTITY_S) * cnt);
    } else {
        rotary_mag->p_tbl = NULL;
    }
    rotary_mag->tbl_cnt = cnt;
    rotary_mag->lst = NULL;

    INT_T i = 0;
    for (i = 0; i < cnt; i++) {
        memcpy(&(rotary_mag->p_tbl[i].rud), &p_tbl[i], SIZEOF(ROTARY_USER_DEF_S));
        op_ret = tuya_gpio_inout_set(rotary_mag->p_tbl[i].rud.port_a, TRUE);
        if (OPRT_OK != op_ret) {
            PR_ERR("tuya_gpio_inout_set port_a err:%d", op_ret);
            goto ERR_EXIT;
        }
        op_ret = tuya_gpio_inout_set(rotary_mag->p_tbl[i].rud.port_b, TRUE);
        if (OPRT_OK != op_ret) {
            PR_ERR("tuya_gpio_inout_set port_b err:%d", op_ret);
            goto ERR_EXIT;
        }
    }

    op_ret = tuya_hal_mutex_create_init(&rotary_mag->mutex);
    if (OPRT_OK != op_ret) {
        goto ERR_EXIT;
    }

    THRD_PARAM_S thrd_param;
    thrd_param.priority = TRD_PRIO_0;
    thrd_param.stackDepth = 1024 + 1024;
    thrd_param.thrdname = "rotary_handle";
    OPERATE_RET opRet = CreateAndStart(&rotary_handle, NULL, NULL, rotary_handle_thrd, NULL, &thrd_param);
    if (OPRT_OK != opRet) {
        rotary_handle = NULL;
        PR_ERR("create rotary_handle thread fails");
        goto ERR_EXIT;
    }

    return OPRT_OK;

ERR_EXIT:
    if (rotary_mag) {
        Free(rotary_mag->p_tbl);
        Free(rotary_mag);
        rotary_mag = NULL;
    }

    return op_ret;
}

/**
 * @brief   register rotary
 * @param   rotary_ud: rotary user define
 * @return  OPERATE_RET
 */
OPERATE_RET reg_proc_rotary(IN CONST ROTARY_USER_DEF_S *rotary_ud)
{
    if (NULL == rotary_ud) {
        return OPRT_INVALID_PARM;
    }
    if (NULL == rotary_ud->call_back){
        return OPRT_INVALID_PARM;
    }

    OPERATE_RET op_ret = OPRT_OK;

    /* is registered? */
    ROTARY_EN_LST_S *tmp_rotary_ent_lst = rotary_mag->lst;
    tuya_hal_mutex_lock(rotary_mag->mutex);
    while(tmp_rotary_ent_lst) {
        if (tmp_rotary_ent_lst->rotary_ent.rud.port_a == rotary_ud->port_a) {
            op_ret = tuya_gpio_inout_set(rotary_ud->port_a, TRUE);
            if (OPRT_OK != op_ret) {
                tuya_hal_mutex_unlock(rotary_mag->mutex);
                return op_ret;
            }
            op_ret = tuya_gpio_inout_set(rotary_ud->port_b, TRUE);
            if (OPRT_OK != op_ret) {
                tuya_hal_mutex_unlock(rotary_mag->mutex);
                return op_ret;
            }
            memcpy(&(tmp_rotary_ent_lst->rotary_ent.rud), rotary_ud, SIZEOF(ROTARY_USER_DEF_S));
            tuya_hal_mutex_unlock(rotary_mag->mutex);
            return OPRT_OK;
        }
        tmp_rotary_ent_lst = tmp_rotary_ent_lst->nxt;
    }
    tuya_hal_mutex_unlock(rotary_mag->mutex);

    ROTARY_EN_LST_S *rotary_ent_lst = (ROTARY_EN_LST_S *)Malloc(SIZEOF(ROTARY_EN_LST_S));
    if (NULL == rotary_ent_lst) {
        return OPRT_MALLOC_FAILED;
    }
    memset(rotary_ent_lst, 0, SIZEOF(ROTARY_EN_LST_S));
    memcpy(&(rotary_ent_lst->rotary_ent.rud), rotary_ud, SIZEOF(ROTARY_USER_DEF_S));

    op_ret = tuya_gpio_inout_set(rotary_ud->port_a, TRUE);
    if (OPRT_OK != op_ret) {
        tuya_hal_mutex_unlock(rotary_mag->mutex);
        return op_ret;
    }
    op_ret = tuya_gpio_inout_set(rotary_ud->port_b, TRUE);
    if (OPRT_OK != op_ret) {
        tuya_hal_mutex_unlock(rotary_mag->mutex);
        return op_ret;
    }

    tuya_hal_mutex_lock(rotary_mag->mutex);
    rotary_ent_lst->nxt = rotary_mag->lst;
    rotary_mag->lst = rotary_ent_lst;
    tuya_hal_mutex_unlock(rotary_mag->mutex);

    return OPRT_OK;
}

/**
 * @brief   rotary handle
 * @param   none
 * @return  none
 */
STATIC VOID __rotary_handle(VOID)
{
    INT_T i = 0;

    for (i = 0; i < rotary_mag->tbl_cnt; i++) {
        __rotary_ent_proc(&rotary_mag->p_tbl[i]);
    }

    tuya_hal_mutex_lock(rotary_mag->mutex);
    ROTARY_EN_LST_S *rotary_ent_lst = rotary_mag->lst;
    while (rotary_ent_lst) {
        __rotary_ent_proc(&rotary_ent_lst->rotary_ent);
        rotary_ent_lst = rotary_ent_lst->nxt;
    }
    tuya_hal_mutex_unlock(rotary_mag->mutex);
}

/**
 * @brief   rotary entity process
 * @param   rotary_ent: rotary entity
 * @return  none
 */
STATIC VOID __rotary_ent_proc(INOUT ROTARY_ENTITY_S *rotary_ent)
{
    switch(rotary_ent->status) {
        case ROTARY_NO_ACTION:
            if ((TRUE == tuya_gpio_read(rotary_ent->rud.port_a)) && (TRUE == tuya_gpio_read(rotary_ent->rud.port_b))) {
                rotary_ent->counter++;
                if (rotary_ent->counter >= 20){
                    rotary_ent->status = ROTARY_LOW_CHECK;
                }
            } else {
                rotary_ent->counter = 0;
            }
            break;
        case ROTARY_LOW_CHECK:
            if (FALSE == tuya_gpio_read(rotary_ent->rud.port_a)) {
                rotary_ent->status = ROTARY_A_LOW_CONFIRM;
            } else {
                if (FALSE == tuya_gpio_read(rotary_ent->rud.port_b)) {
                    rotary_ent->status = ROTARY_B_LOW_CONFIRM;
                }
            }
            break;
        case ROTARY_A_LOW_CONFIRM:
            if (FALSE == tuya_gpio_read(rotary_ent->rud.port_a)) {
                if (TRUE == tuya_gpio_read(rotary_ent->rud.port_b)) {
                    rotary_ent->status = ROTARY_A_LOW_B_HIGH_CONFIRM;
                } else {
                    rotary_ent->status = ROTARY_A_LOW_B_LOW_CONFIRM;
                }
            } else {
                rotary_ent->status = ROTARY_LOW_CHECK;
            }
            break;
        case ROTARY_A_LOW_B_HIGH_CONFIRM:
            if (TRUE == tuya_gpio_read(rotary_ent->rud.port_b)) {
                rotary_ent->dir = TRUE;
                rotary_ent->rud.call_back(rotary_ent->dir);
            }
            rotary_ent->status = ROTARY_NO_ACTION;
            rotary_ent->counter = 0;
            break;
        case ROTARY_A_LOW_B_LOW_CONFIRM:
            if (FALSE == tuya_gpio_read(rotary_ent->rud.port_b)) {
                rotary_ent->dir = FALSE;
                rotary_ent->rud.call_back(rotary_ent->dir);
            }
            rotary_ent->status = ROTARY_NO_ACTION;
            rotary_ent->counter = 0;
            break;
        case ROTARY_B_LOW_CONFIRM:
            if (FALSE == tuya_gpio_read(rotary_ent->rud.port_b)) {
                if (TRUE == tuya_gpio_read(rotary_ent->rud.port_a)) {
                    rotary_ent->status = ROTARY_B_LOW_A_HIGH_CONFIRM;
                } else {
                    rotary_ent->status = ROTARY_B_LOW_A_LOW_CONFIRM;
                }
            } else {
                rotary_ent->status = ROTARY_LOW_CHECK;
            }
            break;
        case ROTARY_B_LOW_A_HIGH_CONFIRM:
            if (TRUE == tuya_gpio_read(rotary_ent->rud.port_a)) {
                rotary_ent->dir = FALSE;
                rotary_ent->rud.call_back(rotary_ent->dir);
            }
            rotary_ent->status = ROTARY_NO_ACTION;
            rotary_ent->counter = 0;
            break;
        case ROTARY_B_LOW_A_LOW_CONFIRM:
            if (FALSE == tuya_gpio_read(rotary_ent->rud.port_a)) {
                rotary_ent->dir = TRUE;
                rotary_ent->rud.call_back(rotary_ent->dir);
            }
            rotary_ent->status = ROTARY_NO_ACTION;
            rotary_ent->counter = 0;
            break;
        default:
            break;
    }
}

/**
 * @brief   rotary handle thread
 * @param   arg
 * @return  none
 */
STATIC VOID rotary_handle_thrd(PVOID_T arg)
{
    while(1) {
        __rotary_handle();
        tuya_hal_system_sleep(TIMER_SPACE);
    }
}
