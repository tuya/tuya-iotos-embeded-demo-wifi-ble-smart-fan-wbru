/**
 * @file tuya_tools.c
 * @author lifan
 * @brief common tools
 * @version 1.0
 * @date 2021-07-28
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_tools.h"
#include <string.h>

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
VOID tuya_num_to_str(IN CHAR_T mode, IN UINT_T num, IN UCHAR_T len, OUT CHAR_T *str)
{
    memset(str, 0, len);

    switch (mode) {
        case 0:
            snprintf(str, len, "%d", num);
            break;
        case 4:
            snprintf(str, len, "%04x", num);
            break;
        default:
            break;
    }
}
