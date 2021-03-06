#ifndef __CRC_H__
#define __CRC_H__

#include "type.h"
#include "state.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *                Constants
 *******************************************************/

/*******************************************************
 *                Type Definitions
 *******************************************************/

/*******************************************************
 *                Structures
 *******************************************************/

/*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
type_uint8 crc_8541_get(type_uint8 *buf, type_uint16 len);
type_uint16 crc_161521_get(type_uint8 *buf, type_uint16 len);
#ifdef __cplusplus
}
#endif

#endif
