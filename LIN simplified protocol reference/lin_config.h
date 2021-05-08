/******************************************************************************
*                _  _               ___             _                         *
*               (_)| |__   _ __    / _ \ _ __ ___  | |__    /\  /\            *
*               | || '_ \ | '__|  / /_\/| '_ ` _ \ | '_ \  / /_/ /            *
*               | || | | || |    / /_\\ | | | | | || |_) |/ __  /             *
*               |_||_| |_||_|    \____/ |_| |_| |_||_.__/ \/ /_/              *
*                                                                             *
*    ihr GmbH                                                                 *
*    Airport Boulevard B210                                                   *
*    77836 Rheinmünster - Germany                                             *
*    http://www.ihr.de                                                        *
*    Phone +49(0) 7229-18475-0                                                *
*    Fax   +49(0) 7229-18475-11                                               *
*                                                                             *
*******************************************************************************
*                                                                             *
* (c) Alle Rechte bleiben bei IHR GmbH, auch fuer den Fall von Schutzrechts-  *
* anmeldungen. Jede Verfuegungsbefugnis, wie Kopier- und Weitergaberecht      *
* bleibt bei IHR GmbH.                                                        *
*                                                                             *
* (c) All rights reserved by IHR GmbH including the right to file             *
* industrial property rights. IHR GmbH retains the sole power of              *
* disposition such as reproduction or distribution.                           *
*                                                                             *
********************     Workfile:      lin_config.h     **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All defines for lin_config.c                         *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 82          $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2015-12-16#$                                 *
*                                                                             *
******************************************************************************/
/**
@file lin_config.h
@brief Header file of lin_config.c

   Defines and declaration for lin_config.c
*/

#ifndef LIN_CONFIG_H /* to interpret header file only once */
#define LIN_CONFIG_H

/* ===========================================================================
 *  Constants
 * ==========================================================================*/

#define LIN_BROADCAST_NAD                 ((l_u8)0x7F)
#define LIN_SUPPLIER_ID_WILDCARD_HI       ((l_u8)0x7F)
#define LIN_SUPPLIER_ID_WILDCARD_LO       ((l_u8)0xFF)
#define LIN_FUNCTION_ID_WILDCARD_HI       ((l_u8)0xFF)
#define LIN_FUNCTION_ID_WILDCARD_LO       ((l_u8)0xFF)

#ifdef LIN_PROTOCOL_VERSION_2_1
   #define LIN_FUNCTIONAL_NAD   126U
#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 */

#ifndef LIN_COOKED_API
   #define LIN_TP_TIMEOUT_MS               500u
   #define LIN_TP_COUNTER_MAX              (LIN_TP_TIMEOUT_MS / LIN_TASK_CYCLE_MS)
#endif /* ifndef LIN_COOKED_API */

/* ===========================================================================
 *  Structures
 * ==========================================================================*/

/* ===========================================================================
 *  Function Prototypes
 * ==========================================================================*/

#if ((defined(J2602_SLAVE)) || (defined(LIN_SLAVE)))
#if (!defined(LIN_PROTOCOL_VERSION_1_3)) && ((defined (LIN_DIAG_ENABLE)) || (defined(LIN_USER_DEF_DIAG_MANDSERV_ENABLE)))
#if ((defined (LIN_COOKED_API)) || (defined (LIN_RAW_API)))
void lin_handle_config(void);
#else
static void lin_handle_config(void);
#endif /* #if ((defined LIN_COOKED_API) || (defined LIN_RAW_API)) */
#endif /* #if (!defined(LIN_PROTOCOL_VERSION_1_3)) && ((defined (LIN_DIAG_ENABLE)) || (defined(LIN_USER_DEF_DIAG_MANDSERV_ENABLE))) */
#endif /* #if ((defined(J2602_SLAVE)) || (defined(LIN_SLAVE))) */

#ifndef LIN_PROTOCOL_VERSION_1_3
#if (((!defined(LIN_USER_DEF_DIAG_ENABLE)) && (!defined(LIN_DIAG_ENABLE))) || (defined(LIN_USER_DEF_DIAG_MANDSERV_ENABLE)))
void ld_config_task(void);
#endif /* #if ((!defined(LIN_PROTOCOL_VERSION_1_3)) && (!defined(LIN_COOKED_API)) && (!defined(LIN_RAW_API))) */
#endif /* #ifndef LIN_PROTOCOL_VERSION_1_3 */

#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
extern void lin_diag_rx_chksum_error(void);
#endif /* #if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1)) */

#if (defined(LIN_PROTOCOL_VERSION_2_1)) && (((!defined(LIN_USER_DEF_DIAG_ENABLE)) && (!defined(LIN_DIAG_ENABLE))))
   static void lin_handle_FunctionalNAD(void);
#endif /* end #if (defined(LIN_PROTOCOL_VERSION_2_1)) && (((!defined(LIN_USER_DEF_DIAG_ENABLE)) && (!defined(LIN_DIAG_ENABLE))) */
#ifdef  LIN_ENABLE_COND_CHG_NAD
   static void l_handle_ConditionalChangeNAD (const l_u8 ConditionalData[], l_bool* ConditionResult);
#endif /* end #ifdef  LIN_ENABLE_COND_CHG_NAD */

/* ===========================================================================
 *  Variables
 * ==========================================================================*/

#endif /* end #ifndef LIN_CONFIG_H */
