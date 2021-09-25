#ifndef __STATE_H__
#define __STATE_H__

#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *                Constants
 *******************************************************/
#define STATE_OK                        0
#define STATE_FAIL                      -1

#ifdef TRUE
#define STATE_TRUE                      TRUE
#else
#define STATE_TRUE                      1
#endif

#ifdef FALSE
#define STATE_FALSE                     FALSE
#else
#define STATE_FALSE                     0
#endif
/*******************************************************
 *                Type Definitions
 *******************************************************/

#ifdef HAL_CFG_STATE_W_BASE
    #define STATE_W_BASE                HAL_CFG_STATE_W_BASE
#else
    #define STATE_W_BASE                0x100
#endif
#define STATE_W_INV_ARG                 (STATE_W_BASE+1)
#define STATE_W_INV_OBJECT              (STATE_W_BASE+2)
#define STATE_W_INV_STATE               (STATE_W_BASE+3)
#define STATE_W_INV_SIZE                (STATE_W_BASE+4)
#define STATE_W_NOT_FOUND               (STATE_W_BASE+5)
#define STATE_W_NOT_SUPPORTED           (STATE_W_BASE+6)

#ifdef HAL_CFG_STATE_E_BASE
    #define STATE_E_BASE                HAL_CFG_STATE_E_BASE
#else
    #define STATE_E_BASE                0x200
#endif
#define STATE_E_INV_ARG                 (STATE_E_BASE+1)
#define STATE_E_INV_OBJECT              (STATE_E_BASE+2)
#define STATE_E_INV_STATE               (STATE_E_BASE+3)
#define STATE_E_INV_SIZE                (STATE_E_BASE+4)
#define STATE_E_NOT_FOUND               (STATE_E_BASE+5)
#define STATE_E_NOT_SUPPORTED           (STATE_E_BASE+6)

#ifdef HAL_CFG_STATE_F_BASE
    #define STATE_F_BASE                HAL_CFG_STATE_F_BASE
#else
    #define STATE_F_BASE                0x300
#endif
#define STATE_F_INV_ARG                 (STATE_F_BASE+1)

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