/**
 * @file tuya_tools.h
 * @author lifan
 * @brief common tools
 * @version 1.0
 * @date 2021-07-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#ifndef __TUYA_TOOLS_H__
#define __TUYA_TOOLS_H__

#include "tuya_cloud_types.h"

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
/**
 * @brief: change number to str
 * @param[in]: mode -> change format( 0 -> %d, 4-> %4x)}
 * @param[in]: num -> number(unsigned int)}
 * @param[in]: len -> buf len(sizeof)}
 * @param[out]: str -> string}
 * @return: string
 */
VOID tuya_num_to_str(IN CHAR_T mode, IN UINT_T num, IN UCHAR_T len, OUT CHAR_T *str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TUYA_TOOLS_H__ */
