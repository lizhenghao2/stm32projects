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
********************     Workfile:     lin_master_task.h **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All defines for lin_master_task.c                    *
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
@file lin_master_task.h
@brief Header file of lin_master_task.c
*/

#ifndef LIN_MASTER_TASK_H /* to interpret header file only once */
#define LIN_MASTER_TASK_H

#include "lin_type.h"

/* ===========================================================================
 *  Constants
 * ==========================================================================*/

/* ===========================================================================
 *  Structures
 * ==========================================================================*/

/* ===========================================================================
 *  Prototypes
 * ==========================================================================*/

void lin_master_task_tx_header(l_u8 pid_uc);
void lin_master_task_init(void);
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
void lin_master_check_etf(void);
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */

/* ===========================================================================
 *  Variables
 * ==========================================================================*/

#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
extern l_u8 g_lin_coll_res_active;
extern l_u16 g_lin_coll_schedidx_save;
extern l_u8 g_lin_coll_frameidx_save;
extern l_u8 g_lin_coll_tbl_entry;
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */

#endif /* end #ifndef LIN_MASTER_TASK_H */
