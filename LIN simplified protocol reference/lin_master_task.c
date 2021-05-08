/******************************************************************************
*                _  _               ___             _                         *
*               (_)| |__   _ __    / _ \ _ __ ___  | |__    /\  /\            *
*               | || '_ \ | '__|  / /_\/| '_ ` _ \ | '_ \  / /_/ /            *
*               | || | | || |    / /_\\ | | | | | || |_) |/ __  /             *
*               |_||_| |_||_|    \____/ |_| |_| |_||_.__/ \/ /_/              *
*                                                                             *
*    ihr GmbH                                                                 *
*    Airport Boulevard B210                                                   *
*    77836 Rheinm�nster - Germany                                             *
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
********************     Workfile:     lin_master_task.c **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All routines for LIN header transmission             *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 101         $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2016-03-08#$                                 *
*                                                                             *
******************************************************************************/
/**
@file lin_master_task.c
@brief Implementation of the LIN Master Task inside the LIN Driver
*/

/* ===========================================================================
 *  Header files
 * ==========================================================================*/

#include "genLinConfig.h"
#if ((defined LIN_MASTER)||(defined J2602_MASTER))

#include "lin_hal.h"
#include "lin_main.h"
#include "lin_driver_api.h"

#include "lin_master_task.h"

/* ===========================================================================
 *  Global Variables
 * ==========================================================================*/

#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
l_u8 g_lin_coll_res_active;
l_u16 g_lin_coll_schedidx_save;
l_u8 g_lin_coll_frameidx_save;
l_u8 g_lin_coll_tbl_entry;
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */

/* ===========================================================================
 *  Functions
 * ==========================================================================*/

/* ---------------------------------------------------------------------------
 *  void lin_master_task_init(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Initialization for master parts like header transmission and - if necessary -
           event triggered frame collision resolving
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_master_task_init(void)
{
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
   g_lin_coll_res_active = 0u;
   g_lin_coll_schedidx_save = 0u;
   g_lin_coll_frameidx_save = 0u;
   g_lin_coll_tbl_entry = 0u;
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
}

/* ---------------------------------------------------------------------------
 *  void lin_master_task_tx_header(l_u8 pid_uc)
 * --------------------------------------------------------------------------*/
/**
   @brief  Starts transmission of a frame header according to the running
           schedule table
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_master_task_tx_header(l_u8 pid_uc)
{
   /* transmit PID field */
   lin_ hal_tx_header(pid_uc);
}

#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
/* ---------------------------------------------------------------------------
 *  void lin_master_check_etf(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Collision resolving of event triggered frames
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_master_check_etf(void)
{
   l_u8 loc_i;

   for (loc_i = 0u; loc_i < L_RESOLVER_TBL_SIZE; loc_i++)
   {
      if (g_lin_active_pid == l_resolver_tbl[loc_i].ETF_id)
      {
         g_lin_coll_schedidx_save = g_ScheduleIdx;
         g_lin_coll_frameidx_save = g_ScheduleFrmIdx;
         g_lin_coll_tbl_entry = loc_i;
         l_sch_set((l_u16)l_resolver_tbl[loc_i].resolver_sched_id, 0u);
      }
   }
}
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
