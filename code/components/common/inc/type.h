#ifndef __TYPE_H__
#define __TYPE_H__

#include "hal_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************
 *                Constants
 *******************************************************/
#ifdef  NULL
#define TYPE_NULL                       NULL
#else
#define TYPE_NULL                       ((void *)0)
#endif

/*******************************************************
 *                Type Definitions
 *******************************************************/
#if HAL_CFG_BIT_WIDTH==8
    #define type_bool                   unsigned char

    #define type_uchar                  unsigned char
    #define type_char                   char
    #define type_string                 char *

    #define type_byte                   unsigned char

    #define type_uint8                  unsigned char
    #define type_int8                   signed char
    #define type_uint16                 unsigned short int
    #define type_int16                  signed short int
    #define type_uint32                 unsigned long int
    #define type_int32                  signed long int
    #define type_uint64
    #define type_int64

    #define type_float                  float
    #define type_double                 double

    #define type_handle                 void *

    #define type_addr                   unsigned short int
#elif HAL_CFG_BIT_WIDTH==16
    #define type_bool                   unsigned char

    #define type_uchar                  unsigned char
    #define type_char                   char
    #define type_string                 char *

    #define type_byte                   unsigned char

    #define type_uint8                  unsigned char
    #define type_int8                   signed char
    #define type_uint16                 unsigned short
    #define type_int16                  signed short
    #define type_uint32                 unsigned long
    #define type_int32                  signed long
    #define type_uint64                 unsigned long long
    #define type_int64                  long long

    #define type_float                  float
    #define type_double                 double

    #define type_handle                 void *

    #define type_addr                   unsigned short
#elif HAL_CFG_BIT_WIDTH==32
    #define type_bool                   unsigned char

    #define type_uchar                  unsigned char
    #define type_char                   char
    #define type_string                 char *

    #define type_byte                   unsigned char

    #define type_uint8                  unsigned char
    #define type_int8                   signed char
    #define type_uint16                 unsigned short
    #define type_int16                  signed short
    #define type_uint32                 unsigned long
    #define type_int32                  signed long
    #define type_uint64                 unsigned long long
    #define type_int64                  long long

    #define type_float                  float
    #define type_double                 double

    #define type_handle                 void *

    #define type_addr                   unsigned long
#elif HAL_CFG_BIT_WIDTH==64
    #define type_bool                   unsigned char

    #define type_uchar                  unsigned char
    #define type_char                   char
    #define type_string                 char *

    #define type_byte                   unsigned char

    #define type_uint8                  unsigned char
    #define type_int8                   signed char
    #define type_uint16                 unsigned short
    #define type_int16                  signed short
    #define type_uint32                 unsigned int
    #define type_int32                  signed int
    #define type_uint64                 unsigned long
    #define type_int64                  signed long

    #define type_float                  float
    #define type_double                 double

    #define type_handle                 void *

    #define type_addr                   unsigned long
#endif

    #define type_err                    type_int16
 /*******************************************************
 *                Structures
 *******************************************************/

 /*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
#ifdef __cplusplus
}
#endif

#endif