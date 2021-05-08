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
********************     Workfile:      lin_slave_task.h **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All defines for lin_slave_task.c                     *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 43          $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2015-10-21#$                                 *
*                                                                             *
******************************************************************************/
/**
@file lin_slave_task.h
@brief Header file of lin_slave_task.c
*/

#ifndef LIN_SLAVE_TASK_H /* to interpret header file only once */
#define LIN_SLAVE_TASK_H

#include "lin_type.h"

/* ===========================================================================
 *  Constants
 * ==========================================================================*/

#if ((defined LIN_ENABLE_ASSIGN_FRAME_ID) || (defined LIN_ENABLE_RBI_RD_MSG_ID_PID))
   #define L_CTRL_PID_OFFSET               2u
   #define L_CTRL_TYPE_OFFSET              3u
   #define L_CTRL_LENGTH_OFFSET            4u
   #define L_CTRL_NEXT_OFFSET              5u
#else
   #define L_CTRL_PID_OFFSET               0u
   #define L_CTRL_TYPE_OFFSET              1u
   #define L_CTRL_LENGTH_OFFSET            2u
   #define L_CTRL_NEXT_OFFSET              3u
#endif /* end #ifdef LIN_ENABLE_ASSIGN_FRAME_ID */

/* ===========================================================================
 *  Structures
 * ==========================================================================*/

/* ===========================================================================
 *  Function Prototypes
 * ==========================================================================*/

void lin_slave_task_rx_pid(l_u8 rx_pid_uc);
void lin_slave_task_rx_data(void);
void lin_slave_task_tx_data(void);

/* ===========================================================================
 *  Variables
 * ==========================================================================*/

#endif /* end #ifndef LIN_SLAVE_TASK_H */
