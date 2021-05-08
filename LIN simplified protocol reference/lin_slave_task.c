/******************************************************************************
*                _  _               ___             _                         *
*               (_)| |__   _ __    / _ \ _ __ ___  | |__    /\  /\            *
*               | || '_ \ | '__|  / /_\/| '_ ` _ \ | '_ \  / /_/ /            *
*               | || | | || |    / /_\\ | | | | | || |_) |/ __  /             *
*               |_||_| |_||_|    \____/ |_| |_| |_||_.__/ \/ /_/              *
*                                                                             *
*    ihr GmbH                                                                 *
*    Airport Boulevard B210                                                   *
*    77836 Rheinm黱ster - Germany                                             *
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
********************     Workfile:      lin_slave_task.c **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All Routines for LIN data receive/transmit           *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 110         $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2016-04-05#$                                 *
*                                                                             *
******************************************************************************/
/**
@file lin_slave_task.c
@brief Implementation of the LIN Slave Task inside the LIN Driver
*/

/* ===========================================================================
 *  Header files
 * ==========================================================================*/

#include "genLinConfig.h"
#include "lin_main.h"
#include "lin_driver_api.h"
#include "lin_hal.h"
#include "lin_slave_task.h"
#include "can.h"

/* ===========================================================================
 *  Global Variables
 * ==========================================================================*/

/* ===========================================================================
 *  Functions
 * ==========================================================================*/

/* ---------------------------------------------------------------------------
 *  void lin_slave_task_tx_data(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Cleans up all states after a successful frame transmit
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_slave_task_tx_data(void)
{
#if ((defined LIN_RAW_API) && !(defined LIN_SLAVE))
   l_u8 lin_queue_status;
#endif /* #if ((defined LIN_RAW_API) && !(defined LIN_SLAVE)) */
   /* tx successful finished */
   if(g_lin_status_word.flag.last_pid != 0x0u)
   {
#ifdef J2602_SLAVE
      /* Save the error for transmission */
      g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate | 0x02u;
#endif /* end #ifdef J2602_SLAVE */
      g_lin_status_word.flag.overrun = L_SET;
   }
   g_lin_status_word.flag.last_pid = g_lin_active_pid;
   g_lin_status_word.flag.successful_transfer = L_SET;
#if ((defined LIN_SLAVE)||(defined J2602_SLAVE))
#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
   if(g_lin_active_pid == LIN_Response_Error_Frame_PID) /* PID of status frame */
   {
      /* Reset Response Error Flag */
      l_Reset_Response_Error_Flag();
   }
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
#endif /* #if ((defined LIN_SLAVE)||(defined J2602_SLAVE)) */
   /* check if tp has another frame to transmit */
#if ((defined LIN_MASTER)||(defined J2602_MASTER))
   /* Master Request Frame */
   if((g_lin_active_pid == 0x3Cu))
#else
   /* Slave Response Frame */
   if((g_lin_active_pid == 0x7Du))
#endif /*#if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
   {
#ifdef LIN_COOKED_API
      lin_tp_tx_part_complete();
#endif /* end #ifdef LIN_COOKED_API */
#if ((defined LIN_MASTER)||(defined J2602_MASTER))
#ifdef LIN_COOKED_API
      /* Last master request was a node configuration service, update status. */
      if (LD_SERVICE_BUSY == g_lin_config_status)
      {
         g_lin_config_status = LD_REQUEST_FINISHED;
      }
#endif /* end #ifdef LIN_COOKED_API */
#ifdef LIN_RAW_API
      /* Last master request was a node configuration service, update status. */
      lin_queue_status = ld_raw_tx_status();
      if(lin_queue_status == LD_QUEUE_EMPTY)
      {
         g_lin_config_status = LD_REQUEST_FINISHED;
      }
#endif /* end #ifdef LIN_RAW_API */
      /* Last master request was a goto sleep command. */
      if (LIN_DIAG_SEND_FRAME.byte[0] == 0x0u)
      {
         g_lin_status_word.flag.goto_sleep = L_SET;
         /* Try to switch of LIN communication*/
         l_sch_set(0xFFu, 0u);
         /* Prevent an accidentally second recognition of the sleep command */
         LIN_DIAG_SEND_FRAME.byte[0] = 0xFFu;
      }
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
   }

#ifdef J2602_SLAVE
#ifdef LIN_RAW_API
   lin_queue_status = ld_raw_tx_status();
   if(lin_queue_status == LD_QUEUE_EMPTY)
   {
      g_j2602_status.flags.sresp = 0u;
      g_j2602_status.flags.tx_resp = 0u;
   }
#endif /* end #ifdef LIN_RAW_API */
#ifndef LIN_DIAG_ENABLE
   g_j2602_status.flags.sresp = 0u;
   g_j2602_status.flags.tx_resp = 0u;
#endif /* end #ifndef LIN_DIAG_ENABLE */
#endif /* end #ifdef J2602_SLAVE */
   /* Update Frame Flag */
   if (g_lin_frame_index < BGMsgNum_LIN)
   {
      g_lin_frame_ctrl[g_lin_frame_index].frame.frame_type.update_flag = 0u;
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
      if ((g_lin_frame_ctrl[g_lin_frame_index].frame.frame_type.transfer_type & 0x01u) != 0u)
      {
         g_lin_frame_ctrl[l_LinData.Frame[g_lin_frame_index].frame_data[0]].frame.frame_type.update_flag = 0u;
      }
      g_lin_rx_ctrl.flag.evttrgfrm = L_RESET;
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
   }
#ifdef J2602_SLAVE
#ifndef GM_SPEC_ADAPTION
   /* Check if statusbyte was transmitted */
   if((g_j2602_status.flags.status_byte_transm == 1u) && LIN_STATUS_BYTE_TRANSM())
   {
      g_j2602_status.flags.status_byte_transm = 0u;
      if (g_j2602_status.flags.errorstate != 0u)
      {
         /* Check if still errors are stored for transmission, if so, delete just tranmitted
          * error (highest priority) and continue the next statusbyte with the next error */
         if ((g_j2602_status.flags.errorstate & 0x10u))
         {
            g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate & 0x0Fu;
         }
         else
         {
            if ((g_j2602_status.flags.errorstate & 0x08u))
            {
               g_j2602_status.flags.errorstate &= 0x07u;
            }
            else
            {
               if ((g_j2602_status.flags.errorstate & 0x04u))
               {
                  g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate & 0x03u;
               }
               else
               {
                  if ((g_j2602_status.flags.errorstate & 0x02u))
                  {
                     g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate & 0x01u;
                  }
                  else
                  {
                     if ((g_j2602_status.flags.errorstate & 0x01u))
                     {
                        g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate & 0x00u;
                     }
                  }
               }
            }
         }
      }
   }
#else
   /* GM specific adaption requires no statusword only a error flag. The flag should behave like
    * the response error flag from LIN, so erase the memory, each time the flag was transmitted
    * successfully. */
   if(J2602_Response_Error_Frame_PID(g_lin_active_pid)) /* PID of status frame */
   {
      LIN_STATUS_BYTE(0u);
      g_j2602_status.flags.errorstate = 0u;
   }
#endif /* #ifndef GM_SPEC_ADAPTION */
#endif /* end #ifdef J2602_SLAVE */
}

/* ---------------------------------------------------------------------------
 *  void lin_slave_task_rx_pid(l_u8 rx_pid_uc)
 * --------------------------------------------------------------------------*/
/**
   @brief  PID recognition
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
//该段代码注释了大量的ID判断，为了去除一些诊断的影响，只是判断PID是否在g_lin_frame_ctrl[loc_i].frame.pid中，来执行响应的任务
void lin_slave_task_rx_pid(l_u8 rx_pid_uc)
{
   l_u8 lin_frame_found = 0u;
   l_u8 loc_i;
#ifdef LIN_CYCLIC_TASK_PERFORMANCE
   l_u8 *pointer = 0;
#endif /* #ifdef LIN_CYCLIC_TASK_PERFORMANCE */
#ifdef J2602_SLAVE
   l_u8 l_x0 = 0u, l_x1 = 0u;
#endif /* end #ifdef J2602_SLAVE */
#ifndef J2602_SLAVE
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
   l_u8 frm_idx;
#endif /* end #ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
#endif /* end #ifndef J2602_SLAVE */
#ifdef LIN_RAW_API
   l_u8 lin_queue_status;
#endif /* ifdef LIN_RAW_API */
#ifdef SUPPORT_TWO_BAUDRATES
   const l_u8 VALID_PID_LIST [64] = {0x80u,0xC1u,0x42u,0x03u,0xC4u,0x85u,0x06u,0x47u,0x08u,0x49u,
                                     0xCAu,0x8Bu,0x4Cu,0x0Du,0x8Eu,0xCFu,0x50u,0x11u,0x92u,0xD3u,
                                     0x14u,0x55u,0xD6u,0x97u,0xD8u,0x99u,0x1Au,0x5Bu,0x9Cu,0xDDu,
                                     0x5Eu,0x1Fu,0x20u,0x61u,0xE2u,0xA3u,0x64u,0x25u,0xA6u,0xE7u,
                                     0xA8u,0xE9u,0x6Au,0x2Bu,0xECu,0xADu,0x2Eu,0x6Fu,0xF0u,0xB1u,
                                     0x32u,0x73u,0xB4u,0xF5u,0x76u,0x37u,0x78u,0x39u,0xBAu,0xFBu,
                                     0x3Cu,0x7Du,0xFEu,0xBFu};
#endif /*#ifdef SUPPORT_TWO_BAUDRATES*/
   g_lin_active_pid = rx_pid_uc;
#ifdef SUPPORT_TWO_BAUDRATES
   if (L_RESET == lin_autobaud_flags_g.baudrate_is_identified )
   {
      /* check if PID is valid */
      if ( VALID_PID_LIST[(l_u8)(rx_pid_uc & 0x3Fu)] == rx_pid_uc )
      {
         /* check if two successive frames with same baudrate are received*/
         if ( (0x2u == lin_autobaud_flags_g.low_baudrate_frames )
           || (0x2u == lin_autobaud_flags_g.high_baudrate_frames ))
         {
            /* valid baudrate is identified and will be set until next reset*/
            lin_autobaud_flags_g.baudrate_is_identified = L_SET;
         }
         /* reinitialize baudrate register values only if the lower baudrate have to be set,
            baudrate register already initialized to the higher one*/
         if ( 0x2u == lin_autobaud_flags_g.low_baudrate_frames )
         {
            /* initialize to the lower baudrate */
            baudrate_reg_value = BRGVAL_LOW_BR;
            baudrate_reg_value_H = BRGH_VAL_LOW_BR;
            baudrate_reg_value_L = BRGL_VAL_LOW_BR;
         }
      }
      else
      {
         /* PID not correct reset number of valid frames*/
         lin_autobaud_flags_g.low_baudrate_frames = 0u;
         lin_autobaud_flags_g.high_baudrate_frames = 0u;
      }
   }
#endif /* #ifdef SUPPORT_TWO_BAUDRATES */
   switch(rx_pid_uc)
   {
		 //we don't need the following fuction to default
//#if ((defined LIN_MASTER)||(defined J2602_MASTER))
//      /* Slave Response Frame */
//      case(0x7Du):
//#else
//      /* Master Request Frame */
//      case(0x3Cu):
//#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
//         g_lin_frame_index = 0xFBu;
//         lin_hal_set_rx_pid(rx_pid_uc);
//         lin_hal_rx_response(LIN_1_3,DIAG_FRAME_LENGTH);
//         break;

//#if ((defined LIN_MASTER)||(defined J2602_MASTER))
//      /* Master Request Frame */
//      case(0x3Cu):
//#else
//      /* Slave Response Frame */
//      case(0x7Du):
//#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
//         /* check if transmission from TP is requested if so transmit */
//#ifdef LIN_COOKED_API
//         if(g_lin_tp_tx_rx_ctrl.tx_on)
//         {
//            lin_hal_set_tx_pid(rx_pid_uc);
//#ifdef J2602_SLAVE
//            if (g_j2602_status.flags.sresp)
//            {
//               lin_hal_tx_response(LIN_1_3, LIN_DIAG_SEND_FRAME.byte,DIAG_FRAME_LENGTH);
//               g_j2602_status.flags.sresp = 0u;
//            }
//#else
//            lin_hal_tx_response(LIN_1_3, LIN_DIAG_SEND_FRAME.byte,DIAG_FRAME_LENGTH);
//#endif /* end #ifdef J2602_SLAVE */
//         }
//#endif /* end #ifdef LIN_COOKED_API */
//#ifdef LIN_RAW_API
//         lin_queue_status = ld_raw_tx_status();
//         if((lin_queue_status == LD_DATA_AVAILABLE) || (lin_queue_status == LD_QUEUE_FULL))
//         {
//            lin_hal_set_tx_pid(rx_pid_uc);
//#ifdef J2602_SLAVE
//            if (g_j2602_status.flags.sresp)
//            {
//               lin_hal_tx_response(LIN_1_3, lin_diag_raw_pop_tx_queue(),DIAG_FRAME_LENGTH);
//               g_j2602_status.flags.sresp = 0u;
//            }
//#else
//            lin_hal_tx_response(LIN_1_3, lin_diag_raw_pop_tx_queue(),DIAG_FRAME_LENGTH);
//#endif /* end #ifdef J2602_SLAVE */
//         }
//#endif /* end #ifdef LIN_RAW_API */
//#if ((!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_USER_DEF_DIAG_ENABLE)))
//         if(g_lin_diag_config_active)
//         {
//            g_lin_diag_config_active = 0x0u;
//            lin_hal_set_tx_pid(rx_pid_uc);
//            lin_hal_tx_response(LIN_1_3, LIN_DIAG_SEND_FRAME.byte,DIAG_FRAME_LENGTH);
//#ifdef J2602_SLAVE
//            g_j2602_status.flags.sresp = 0u;
//#endif /* end #ifdef J2602_SLAVE */
//         }
//#endif /* end #if (!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_USER_DEF_DIAG_ENABLE)) */
//#ifdef LIN_USER_DEF_DIAG_ENABLE
//         if(g_lin_user_diag_active)
//         {
//            g_lin_user_diag_active = 0x0u;
//            lin_hal_set_tx_pid(rx_pid_uc);
//            lin_hal_tx_response(LIN_1_3, LIN_DIAG_SEND_FRAME.byte,DIAG_FRAME_LENGTH);
//#ifdef J2602_SLAVE
//            g_j2602_status.flags.sresp = 0u;
//#endif /* end #ifdef J2602_SLAVE */
//         }
//#endif /* end #ifdef LIN_USER_DEF_DIAG_ENABLE */
//#ifdef J2602_SLAVE
//         /* Slave Response to J2602 reset (targeted and broadcast) and node configuration */
//         if (g_j2602_status.flags.tx_resp)
//         {
//            switch (l_callback_Reset_Response())
//            {
//               case (1):
//                  /* Reset was made, corresponding positive response has to be sent
//                   * Errorbits of the statusbyte has to be deleted and also the error state has
//                   * to be deleted and set remember, that a reset was made, and the next transmitted
//                   * frame was a reset response */
//                  LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
//                  LIN_DIAG_SEND_FRAME.frame.PCI = 0x06u;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.SID = 0xF5u;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = g_lin_prod_id.Supplier_lo; /* Supplier ID LSB */
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = g_lin_prod_id.Supplier_hi; /* Supplier ID MSB */
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = g_lin_prod_id.Function_lo; /* Function ID LSB */
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = g_lin_prod_id.Function_hi; /* Function ID MSB */
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = g_lin_prod_id.Variant;     /* Variant ID */
//                  lin_hal_tx_response(LIN_1_3, &LIN_DIAG_SEND_FRAME.frame.NAD,DIAG_FRAME_LENGTH);
//#ifdef GM_SPEC_ADAPTION
//                  g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate | 0x40u;
//#else
//                  g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate | 0x41u;
//#endif /* end #ifdef GM_SPEC_ADAPTION */
//                  g_j2602_status.flags.tx_resp = 0u;
//                  g_j2602_status.flags.sresp = 0u;
//                  break;
//               case (2):
//                  /* Application chose to not reset, corresponding negative response has to be sent
//                   * Statusbyte doesn't change, but it must be remembered, that an reset response was sent */
//                  LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
//                  LIN_DIAG_SEND_FRAME.frame.PCI = 0x06u;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.SID = 0x7Fu;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = g_lin_prod_id.Supplier_lo;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = g_lin_prod_id.Supplier_hi;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = g_lin_prod_id.Function_lo;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = g_lin_prod_id.Function_hi;
//                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = g_lin_prod_id.Variant;
//                  lin_hal_tx_response(LIN_1_3, &LIN_DIAG_SEND_FRAME.frame.NAD,DIAG_FRAME_LENGTH);
//                  g_j2602_status.flags.tx_resp = 0u;
//                  g_j2602_status.flags.sresp = 0u;
//                  g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate | 0x40u;
//                  break;
//               default:
//                  break;
//            }
//         }
//         /* If there is no slave response to be sent, leave */
//         if (!g_j2602_status.flags.sresp)
//         {
//            break;
//         }
//#endif /* end #ifdef J2602_SLAVE */
//         break;

//#ifdef J2602_SLAVE
//      /* Broadcast messages from the master - dependent on the NAD (DNN) and number of message IDs
//       * the slave has to pick certain bytes of these messages for the use through the application */
//      case(0x78):
//      case(0xBA):
//         /* Only Slaves with NAD between 60 and 67 need bytes from these two frames. */
//         if (((g_lin_prod_id.NAD & 0x08u) == 0u) && (g_lin_prod_id.NAD <= 0x6Du))
//         {
//            g_lin_frame_index = 0xFCu;
//            lin_hal_set_rx_pid(rx_pid_uc);
//            lin_hal_rx_response(LIN_PROTOCOL_TYPE, DIAG_FRAME_LENGTH);
//         }
//         else
//         {
//            lin_slave_state_g = idle;
//         }
//         break;
//      case(0x39):
//      case(0xFB):
//         /* Only Slaves with NAD between 68 and 6D need bytes from these two frames. */
//         if (((g_lin_prod_id.NAD & 0x08u) != 0u) && (g_lin_prod_id.NAD <= 0x6Du))
//         {
//            g_lin_frame_index = 0xFCu;
//            lin_hal_set_rx_pid(rx_pid_uc);
//            lin_hal_rx_response(LIN_PROTOCOL_TYPE, DIAG_FRAME_LENGTH);
//         }
//         else
//         {
//            lin_slave_state_g = idle;
//         }
//         break;
//#endif /* end #ifdef J2602_SLAVE */

//      /* User Defined Frame */
//      case(0xFEu):
//#ifdef J2602_SLAVE
//         g_lin_frame_index = 0xFAu;
//         lin_hal_set_rx_pid(rx_pid_uc);
//         lin_hal_rx_response(LIN_1_3,DIAG_FRAME_LENGTH);
//#endif /* end #ifdef J2602_SLAVE */
//         break;

//      /* Reserved PID */
//      case(0xBFu):
//         break;

//      /* Application Frame Handling */
      default:
#ifdef LIN_CYCLIC_TASK_PERFORMANCE
         pointer = g_lin_frame_ctrl[0].reg;
#endif /* #ifdef LIN_CYCLIC_TASK_PERFORMANCE */
         /* find frame according to pid */
         for(loc_i = 0u; loc_i < BGMsgNum_LIN; loc_i++)
         {
#ifdef LIN_CYCLIC_TASK_PERFORMANCE
            if(*(pointer+L_CTRL_PID_OFFSET) == rx_pid_uc)
#else
            if(g_lin_frame_ctrl[loc_i].frame.pid == rx_pid_uc)   //这个判断用于判断PID是否是我们需要的
#endif /* #ifdef LIN_CYCLIC_TASK_PERFORMANCE */
            {
#ifdef LIN_PID_RECOGNITION_CALLBACK
               l_callback_PIDreceived (rx_pid_uc);
#endif /* #ifdef LIN_PID_RECOGNITION_CALLBACK */
               g_lin_frame_index = loc_i;
#ifndef J2602_SLAVE
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
               /* event triggered frames */
               if(g_lin_frame_ctrl[loc_i].reg[L_CTRL_TYPE_OFFSET] & EVENT_TRIGGERED)
               {
                  frm_idx = l_LinData.Frame[loc_i].frame_data[0];
                  if(g_lin_frame_ctrl[frm_idx].reg[L_CTRL_TYPE_OFFSET] & PUBLISH)
                  {
                     if(g_lin_frame_ctrl[frm_idx].reg[L_CTRL_TYPE_OFFSET] & UPDATED_SIGNALS)
                     {
                        /* config LIN for event_triggered tx data */
                        lin_hal_set_tx_pid(g_lin_active_pid);
                        l_LinData.Frame[frm_idx].frame_data[0] = g_lin_frame_ctrl[frm_idx].frame.pid;
                        lin_hal_tx_response(LIN_PROTOCOL_TYPE, l_LinData.Frame[frm_idx].frame_data, g_lin_frame_ctrl[frm_idx].reg[L_CTRL_LENGTH_OFFSET]);
                        /* Keep information that current frame is an event triggered frame */
                        g_lin_rx_ctrl.flag.evttrgfrm = L_SET;
                     }
                  }
                  else
                  {
                     /* config LIN for rx data */
                     lin_hal_set_rx_pid(g_lin_active_pid);
                     lin_hal_rx_response(LIN_PROTOCOL_TYPE, g_lin_frame_ctrl[frm_idx].reg[L_CTRL_LENGTH_OFFSET]);
                     /* Keep information that current frame is an event triggered frame */
                     g_lin_rx_ctrl.flag.evttrgfrm = L_SET;
                  }
                  lin_frame_found = 1u;
               }
               else
#endif /* end #ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
#endif /* end #ifndef J2602_SLAVE */
               {
                  /* unconditional frames */
                  /* check if publish or subscribe frame */
								 /*here is used to judged wheather this pid should be sended or not */
								 //here is important
                  if(g_lin_frame_ctrl[loc_i].reg[L_CTRL_TYPE_OFFSET] & PUBLISH)
                  {
#if ((defined J2602_SLAVE) && (!defined (GM_SPEC_ADAPTION)))
                     J2602_CREATE_STATUSBYTE();
#endif /* end #if (defined J2602_SLAVE) && !(defined GM_SPEC_ADAPTION) */
                     /* config LIN for unconditional tx data */
                     lin_hal_set_tx_pid(g_lin_active_pid);
                     lin_hal_tx_response(LIN_PROTOCOL_TYPE, l_LinData.Frame[loc_i].frame_data, g_lin_frame_ctrl[loc_i].reg[L_CTRL_LENGTH_OFFSET]);
                  }
                  else
                  {
                     /* config LIN for rx data */
                     lin_hal_set_rx_pid(g_lin_active_pid);
                     lin_hal_rx_response(LIN_PROTOCOL_TYPE, g_lin_frame_ctrl[loc_i].reg[L_CTRL_LENGTH_OFFSET]);
                  }
                  lin_frame_found = 1u;
                  break;
               }
            }
            else
            {
               g_lin_frame_index = 0xFFu;
            }
#ifdef LIN_CYCLIC_TASK_PERFORMANCE
            pointer = pointer + L_CTRL_NEXT_OFFSET;
#endif /* #ifdef LIN_CYCLIC_TASK_PERFORMANCE */
         }
         if (lin_frame_found == 0u)
         {
            /* We don't know this PID, so we can stop receiving and wait for next BREAK. */
            lin_hal_wait_for_break();
#ifdef SUPPORT_TWO_BAUDRATES
            /* reset to higher baudrate if not yet set to a fixed value
            to be able to detect the lower one */
            l_hal_reset_to_high_baudrate();
#endif /*#ifdef SUPPORT_TWO_BAUDRATES*/
#ifdef J2602_SLAVE
            /* Check if the PID is correct */
            l_x0 = (rx_pid_uc & 0x01u)^((rx_pid_uc >> 1u) & 0x01u)^((rx_pid_uc >> 2u) & 0x01u)^((rx_pid_uc >> 4u) & 0x01u);
            l_x1 = (l_u8)(~(((rx_pid_uc >> 1u) & 0x01u)^((rx_pid_uc >> 3u) & 0x01u)^((rx_pid_uc >> 4u) & 0x01u)^((rx_pid_uc >> 5u) & 0x01u)));
            if ((l_x0 != ((rx_pid_uc >> 6u) & 0x01u)) || ((l_x1 & 0x01u) != ((rx_pid_uc >> 7u) & 0x01u)))
            {
               /* PID error is highest prioritized error, so no check is needed, if this error is
                * already set and because all three error bits have to be set, no deleting is needed */
               g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate | 0x10u;
            }
#endif /* end #ifdef J2602_SLAVE */
         }
         break;
   } /* switch */
   return;
}

/* ---------------------------------------------------------------------------
 *  void lin_slave_task_rx_data(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Cleans up all states after a successful frame receive
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_slave_task_rx_data(void)
{
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
   l_u8 loc_i;
#endif /* end #ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
#ifdef J2602_SLAVE
   l_u8 i, dnn, frame;
#endif /* end #ifdef J2602_SLAVE */
   l_u8 loc_n;

   switch(g_lin_frame_index)
   {
      /* invalid frame index */
      case(0xFFu):
         break;

#ifdef J2602_SLAVE
      /* J2602 configuration request */
      case(0xFA):
         /* copy rx data to config buffer */
         if(g_lin_status_word.flag.last_pid != 0x0u)
         {
            g_lin_status_word.flag.overrun = L_SET;
         }
         g_lin_status_word.flag.last_pid = g_lin_active_pid;
         g_lin_status_word.flag.successful_transfer = L_SET;

         g_lin_irqState = l_sys_irq_disable();
         for (loc_n = 0u; loc_n < g_lin_frame_data_size; loc_n++)
         {
            j2602_config_frame.byte[loc_n]= lin_temp_buffer.frame.DataBuffer[loc_n];
         }
         l_sys_irq_restore(g_lin_irqState);
         g_lin_rx_ctrl.flag.rx_config = 1u;
         break;
#endif /* end #ifdef J2602_SLAVE */

      /* diagnostic master request */
      case(0xFBu):
         /* copy rx data to diag buffer */
         if(g_lin_status_word.flag.last_pid != 0x0u)
         {
            g_lin_status_word.flag.overrun = L_SET;
         }
         g_lin_status_word.flag.last_pid = g_lin_active_pid;
         g_lin_status_word.flag.successful_transfer = L_SET;

         g_lin_irqState = l_sys_irq_disable();
         for (loc_n = 0u; loc_n < g_lin_frame_data_size; loc_n++)
         {
            LIN_DIAG_RECEIVE_FRAME.byte[loc_n]= lin_temp_buffer.frame.DataBuffer[loc_n];
         }
         l_sys_irq_restore(g_lin_irqState);

         /* Notification Flag for cyclic Diagnostic-Task to inform about received frame */
         g_lin_rx_ctrl.flag.rx_diag = 1u;
         break;

#ifdef J2602_SLAVE
      /* j2602 broadcast frames */
      case(0xFC):
         /* DNN is the last 4 bits of the NAD */
         dnn = g_lin_prod_id.NAD & 0x0Fu;
         /* NAD and PID are matched in lin_slave_task_rx_pid */
         if ((g_lin_active_pid == 0x78u) || (g_lin_active_pid == 0x39u))
         {
            frame = 0u;
            l_bool_wr_new_bc_data0();
         }
         else
         {
            frame = 1u;
            l_bool_wr_new_bc_data1();
         }

         /* If dnn < 8 then this is the number of the first byte in the broadcast frame which
          * is reserved for this slave
          * If dnn > 8 then the last 3 bits are the number of the first byte */
         if (dnn >= 0x08u)
         {
            dnn = dnn & 0x07u;
         }
         /* Clearing the array before storing data into it */
         for (i = 0u; i < J2602_DNN_NUMBER_OF_BYTES; i++)
         {
            j2602_broadcast_bytes[frame][i] = 0u;
         }
         /* disable LIN interrupts */
         g_lin_irqState = l_sys_irq_disable();
         for (i = 0u; i < J2602_DNN_NUMBER_OF_BYTES; i++)
         {
            /* (MISRA-C:2004 Non MISRA Warning, e661, e662) Possible access of out-of-bounds pointer (1 beyond end of data) by operator '['
             * Reason: This code part is created to only copy dedicated bytes from broadcast frames, will not
             * go out of bounds. */
            j2602_broadcast_bytes[frame][i] = lin_temp_buffer.frame.DataBuffer[dnn+i]; /*lint !e662, !e661*/
         }
         /* restore LIN interrupts */
         l_sys_irq_restore(g_lin_irqState);
         break;
#endif /* end #ifdef J2602_SLAVE */

      /* application frames */
      default:
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
         if(g_lin_rx_ctrl.flag.evttrgfrm)
         {
            g_lin_rx_ctrl.flag.evttrgfrm = L_RESET;
            /* find frame according to pid */
            for(loc_i = 0u; loc_i < LIN_NUMBER_OF_FRAMES; loc_i++)
            {
               if(g_lin_frame_ctrl[loc_i].reg[L_CTRL_PID_OFFSET] == lin_temp_buffer.frame.DataBuffer[0])
               {
                  if(g_lin_status_word.flag.last_pid != 0x0u)
                  {
                     g_lin_status_word.flag.overrun = L_SET;
                  }
                  g_lin_status_word.flag.last_pid = g_lin_active_pid;
                  g_lin_status_word.flag.successful_transfer = L_SET;

                  g_lin_irqState = l_sys_irq_disable();
                  for (loc_n = 0u; loc_n < g_lin_frame_data_size; loc_n++)
                  {
                     l_LinData.Frame[loc_i].frame_data[loc_n]=lin_temp_buffer.frame.DataBuffer[loc_n];
                  }
                  l_sys_irq_restore(g_lin_irqState);
                  l_Update_flags_frame(loc_i);
                  break;
               }
            }
         }
         else
#endif /* end #ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
         {
            if(g_lin_status_word.flag.last_pid != 0x0u)
            {
               g_lin_status_word.flag.overrun = L_SET;
            }
            g_lin_status_word.flag.last_pid = g_lin_active_pid;
            g_lin_status_word.flag.successful_transfer = L_SET;
            /* copy rx data to appl. frame buffer */
            g_lin_irqState = l_sys_irq_disable();
            for (loc_n = 0u; loc_n < g_lin_frame_data_size; loc_n++)
            {
               l_LinData.Frame[g_lin_frame_index].frame_data[loc_n]= lin_temp_buffer.frame.DataBuffer[loc_n];
            }
            l_sys_irq_restore(g_lin_irqState);

            /* Update Frame Flag */
            g_lin_frame_ctrl[g_lin_frame_index].frame.frame_type.update_flag = 1u;
            /* Update Signal flags */
            //l_Update_flags_frame(g_lin_frame_index);
            
#ifdef SIGNAL_IN_MULTIFRAMES
            /* Update Signal in multiframe */
            g_lin_irqState = l_sys_irq_disable();
            l_Update_Multi_Frame(g_lin_frame_index);
            l_sys_irq_restore(g_lin_irqState);
#endif   /* SIGNAL_IN_MULTIFRAMES  */
         }
         break;
   }
}


