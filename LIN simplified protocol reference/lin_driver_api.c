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
********************     Workfile:      lin_driver_api.c **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All routines for access from application to          *
*                        LIN driver and callback function from LIN driver     *
*                        to application                                       *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 111         $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2016-04-05#$                                 *
*                                                                             *
******************************************************************************/
/**
@file    lin_driver_api.c
@brief   Application Program Interface for the LIN Driver access
*/

/* ===========================================================================
 *  Header files
 * ==========================================================================*/

#include "genLinConfig.h"
#include "lin_slave_task.h"
#include "lin_driver_api.h"
#ifdef LIN_COOKED_API
#include "lin_diag_tp.h"
#endif /* end ifdef LIN_COOKED_API */
#ifdef LIN_RAW_API
#include "lin_diag_raw.h"
#endif /* end ifdef LIN_RAW_API */



/* ===========================================================================
 *  Global Variables
 * ==========================================================================*/

/* ===========================================================================
 *  Functions
 * ==========================================================================*/

/* ---------------------------------------------------------------------------
 *  l_bool l_sys_init (void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Initialise LIN driver system.
   @pre    MCU started
   @param  void
   @retval void
*/
l_bool l_sys_init (void)
{
   lin_main_init();
   return 0u;
}

#ifdef J2602_SLAVE

/* ---------------------------------------------------------------------------
 *  l_u8* j2602_get_bc_data(l_u8 frame)
 * --------------------------------------------------------------------------*/
/**
   @brief  Read the data of a received broadcastframe
   @pre    LIN driver initialized, broadcast frame received.
   @param  0 Read data from frame zero
   @param  1 Read data from frame one
   @retval l_u8* pointer to data buffer of frame.
*/
/* (MISRA-C:2004 8.10/R, e765) external ... could be made static
 * Reason: Part of API, configuration dependent usage. */
l_u8* j2602_get_bc_data(l_u8 frame)
{
   if (frame == 0u) {
      l_flg_clr_new_bc_data0();
   } else {
      l_flg_clr_new_bc_data1();
   }
    return &j2602_broadcast_bytes[frame][0];
}

#endif /* end #ifdef J2602_SLAVE */

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
#ifndef LIN_PROTOCOL_VERSION_1_3
/* ---------------------------------------------------------------------------
 *  l_u8 ld_is_ready (void)
 * --------------------------------------------------------------------------*/
/**
  @brief  Gives Information if diagnostic module is ready for a new command
  @pre    LIN driver initialised
  @param  void
  @return 1 if diagnostic module is ready
  @return 0 else
*/
l_u8 ld_is_ready (void)
{
   l_u8 ret_val = LD_SERVICE_IDLE;
   /* LD_RESPONSE_FINISHED is an internal state between receiving the response and processing
    * it. This should be finished within one call of ld_task, but to prevent interrupts from
    * getting a wrong information, this is caught here. */
   if (g_lin_config_status == LD_RESPONSE_FINISHED)
   {
      ret_val = LD_REQUEST_FINISHED;
   }
   else
   {
      ret_val = g_lin_config_status;
   }
   return ret_val;
}

/* ---------------------------------------------------------------------------
 *  void ld_check_response (l_u8* RSID,l_u8* error_code)
 * --------------------------------------------------------------------------*/
/**
  @brief  Get result of last node configuration call completed
  @pre    LIN driver initialised
  @param  RSID        pointer to 8Bit datatype to get last Service ID from driver
  @param  error_code  pointer to 8Bit datatype to get last error code from driver
  @return void
*/
void ld_check_response (l_u8* RSID,l_u8* error_code)
{
   *RSID = g_ld_NodeConfRSID;
   *error_code = g_ld_NodeConfErr;
   g_ld_NodeConfRSID = 0u;
   g_ld_NodeConfErr = 0u;
   return;
}
#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */

#ifndef LIN_PROTOCOL_VERSION_1_3
/* ---------------------------------------------------------------------------
 *  void ld_assign_frame_id(l_u8 NAD,l_u16 SUPP_ID,l_u16 MSG_ID,l_u8 PID)
 * --------------------------------------------------------------------------*/
/**
  @brief  start assign pid to frame in slave specified via NAD & SUPP_ID
  @pre    LIN driver initialised
  @param  NAD      Node Address of Slave
  @param  SUPP_ID  16Bit Supplier ID of Slave
  @param  MSG_ID   16Bit Message ID of frame to assign
  @param  PID      Protected Identifier which should be assigned to the frame above
  @return void
*/
void ld_assign_frame_id(l_u8 NAD,l_u16 SUPP_ID,l_u16 MSG_ID,l_u8 PID)
{
   l_u8 lin_tx_state = 1u;
   /* Save request data for response process */
   g_lin_config_buffer[0] = NAD;
   g_lin_config_buffer[1] = 0x00u;
   g_lin_config_buffer[2] = 0xB1u;

   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = NAD;
      LIN_DIAG_SEND_FRAME.byte[1] = 0x06u;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xB1u;
      LIN_DIAG_SEND_FRAME.byte[3] = (l_u8)(SUPP_ID & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[4] = (l_u8)((SUPP_ID >> 8) & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[5] = (l_u8)(MSG_ID & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[6] = (l_u8)((MSG_ID >> 8) & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[7] = PID;
      g_lin_config_status = LD_SERVICE_BUSY;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
      g_lin_config_status = LD_SERVICE_ERROR;
   }
}
#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */

#ifndef LIN_PROTOCOL_VERSION_1_3
#ifdef LIN_ENABLE_ASSIGN_NAD
/* ---------------------------------------------------------------------------
 *  void ld_assign_NAD (l_u8 initial_nad,l_u16 supplier_id,l_u16 function_id,l_u8 new_NAD)
 * --------------------------------------------------------------------------*/
/**
  @brief  assign a new NAD to the slave specified via INIT_NAD, SUPP_ID & FUNC_ID.
  @pre    LIN driver initialised
  @param  initial_nad  initial Node Address of Slave
  @param  supplier_id    16Bit Supplier ID of Slave
  @param  function_id    16Bit Function ID of Slave
  @param  new_NAD    new Node Address which should be assigned to the slave
  @return    void
*/
void ld_assign_NAD (l_u8 initial_nad,l_u16 supplier_id,l_u16 function_id,l_u8 new_NAD)
{
   l_u8 lin_tx_state = 1u;
   /* Save request data for response process */
   g_lin_config_buffer[0] = initial_nad;
   g_lin_config_buffer[1] = new_NAD;
   g_lin_config_buffer[2] = 0xB0u;

   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = initial_nad;
      LIN_DIAG_SEND_FRAME.byte[1] = 0x06u;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xB0u;
      LIN_DIAG_SEND_FRAME.byte[3] = (l_u8)(supplier_id & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[4] = (l_u8)((supplier_id>>8) & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[5] = (l_u8)(function_id & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[6] = (l_u8)((function_id>>8) & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[7] = new_NAD;
      g_lin_config_status = LD_SERVICE_BUSY;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
      g_lin_config_status = LD_SERVICE_ERROR;
   }
}
#endif /* #ifdef LIN_ENABLE_ASSIGN_NAD */
#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */


#ifdef LIN_PROTOCOL_VERSION_2_1
#ifdef LIN_ENABLE_SAVE_CONFIG
void ld_save_configuration (l_u8 NAD)
{
   l_u8 lin_tx_state = 1u;
   /* Save request data for response process */
   g_lin_config_buffer[0] = NAD;
   g_lin_config_buffer[1] = 0x00u;
   g_lin_config_buffer[2] = 0xB6u;

   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */ 
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = NAD;
      LIN_DIAG_SEND_FRAME.byte[1] = 0x01u;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xB6u;
      LIN_DIAG_SEND_FRAME.byte[3] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[4] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[5] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[6] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[7] = 0xFFu;
      g_lin_config_status = LD_SERVICE_BUSY;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
      g_lin_config_status = LD_SERVICE_ERROR;
   }
}
#endif /* #ifdef LIN_ENABLE_SAVE_CONFIG */

void ld_assign_frame_id_range(l_u8 NAD, l_u8 start_index, const l_u8* const PIDs)
{
   l_u8 lin_tx_state = 1u;
   l_u8  loc_i;
   const l_u8 (*ld_tmp_arr)[] = (const l_u8 (*)[])PIDs;
   /* Save request data for response process */
   g_lin_config_buffer[0] = NAD;
   g_lin_config_buffer[1] = 0x00u;
   g_lin_config_buffer[2] = 0xB7u;

   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = NAD;
      LIN_DIAG_SEND_FRAME.byte[1] = 0x06u;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xB7u;
      LIN_DIAG_SEND_FRAME.byte[3] = start_index;
      for(loc_i = 0u; loc_i < 4u; loc_i++)
      {
         LIN_DIAG_SEND_FRAME.byte[loc_i + 4u] = (*ld_tmp_arr)[loc_i];
      }
      g_lin_config_status = LD_SERVICE_BUSY;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
      g_lin_config_status = LD_SERVICE_ERROR;
   }
}
#endif /* end #ifdef LIN_PROTOCOL_VERSIOM_2_1 */
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

#ifdef LIN_PROTOCOL_VERSION_2_1
/* ---------------------------------------------------------------------------
 *  l_u8 ld_read_configuration (l_u8* const data,l_u8* const length)
 * --------------------------------------------------------------------------*/
/**
   @brief  Fill configuration data to data pointer from application.
   @pre    LIN driver initialized
   @param  l_u8* const data     pointer to application buffer for configuration data
   @param  l_u8* const length   pointer to length information for buffer
   @retval LD_READ_OK           Service was successful
   @retval LD_LENGTH_TOO_SHORT  Configuration size is greater than buffer size, no
                                valid configuration data contained in buffer.
*/
/* (MISRA-C:2004 8.10/R, e765) external ... could be made static
 * Reason: Part of API, configuration dependent usage. */
l_u8 ld_read_configuration (l_u8* const data,l_u8* const length)
{
   l_u8  loc_i = 0u;
   l_u8 ret_val;
   /* (MISRA-C:2004 11.4/A) A cast should not be performed between a pointer to object type and a different pointer to object type
    * Reason: make the pointer (req. by LIN specification) accessible as array. */
   l_u8 (*l_api_tmp_arr)[] = (l_u8 (*)[])data; /*lint !e927*/
   if(*length < (LIN_NUMBER_OF_FRAMES + 1u))
   {
      ret_val = LD_LENGTH_TOO_SHORT;
   }
   else
   {
      *length = LIN_NUMBER_OF_FRAMES + 1u;
      (*l_api_tmp_arr)[0] = g_lin_prod_id.NAD;
      for(loc_i = 0u; loc_i < LIN_NUMBER_OF_FRAMES; loc_i++)
      {
         (*l_api_tmp_arr)[loc_i + 1u] = g_lin_frame_ctrl[loc_i].frame.pid;
      }
      ret_val = LD_READ_OK;
   }
   return ret_val;
}

/* ---------------------------------------------------------------------------
 *  l_u8 ld_set_configuration (l_u8* const data,l_u8* const length)
 * --------------------------------------------------------------------------*/
/**
   @brief  Set configuration with to data from application.
   @pre    LIN driver initialized
   @param  l_u8* const data       pointer to application buffer for configuration data
   @param  l_u8* const length     pointer to length information for buffer
   @retval LD_SET_OK              Service was successful
   @retval LD_LENGTH_NOT_CORRECT  Configuration size is not equal to buffer size, no
                                  valid configuration data contained in buffer.
   @retval LD_DATA_ERROR          Service was not successful
*/
/* (MISRA-C:2004 8.10/R, e765) external ... could be made static
 * Reason: Part of API, configuration dependent usage. */
l_u8 ld_set_configuration (const l_u8* const data,l_u8* const length)
{
   l_u8 loc_i = 0u;
   l_u8 ret_val;
   /* (MISRA-C:2004 11.5/R) A cast shall not be performed that removes any const or volatile qualification from the type addressed by a pointer
    * Reason: make the pointer (req. by LIN specification) accessible as array. */
   const l_u8 (*l_api_tmp_arr)[] = (const l_u8 (*)[])data; /*lint !e927*/
   if(*length != (l_u8)(LIN_NUMBER_OF_FRAMES + 1u))
   {
      ret_val = LD_LENGTH_NOT_CORRECT;
   }
   else
   {
      g_lin_prod_id.NAD = (*l_api_tmp_arr)[0];
      for(loc_i = 0u; loc_i < LIN_NUMBER_OF_FRAMES; loc_i++)
      {
         g_lin_frame_ctrl[loc_i].frame.pid = (*l_api_tmp_arr)[loc_i + 1u];
      }
      ret_val = LD_SET_OK;
   }
   return ret_val;
}
#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 */

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
#ifndef LIN_PROTOCOL_VERSION_1_3
#ifdef  LIN_ENABLE_COND_CHG_NAD
/* ---------------------------------------------------------------------------
 *  void ld_conditional_change_NAD (l_u8 NAD,l_u8 id,l_u8 byte,l_u8 mask,l_u8 invert,l_u8 new_NAD)
 * --------------------------------------------------------------------------*/
/**
  @brief  assign a new NAD to the slave specified via NAD under conditions specified via
          id, byte, mask & invert.
  @pre    LIN driver initialised
  @param  NAD        Node Address of Slave
  @param  id        ID from read by identifier table (row)
  @param  byte      specify which byte from the read by identifier table (colum)
  @param  mask      mask to get condition result
  @param  invert    for a bitwise XOR with byte
  @param  new_NAD    new Node Address which should be assigned to the slave
  @return    void
*/
void ld_conditional_change_NAD (l_u8 NAD,l_u8 id,l_u8 byte,l_u8 mask,l_u8 invert,l_u8 new_NAD)
{
   l_u8 lin_tx_state = 1u;
   /* Save request data for response process */
   g_lin_config_buffer[0] = NAD;
   g_lin_config_buffer[1] = 0x00u;
   g_lin_config_buffer[2] = 0xB3u;

   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = NAD;
      LIN_DIAG_SEND_FRAME.byte[1] = 0x06u;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xB3u;
      LIN_DIAG_SEND_FRAME.byte[3] = id;
      LIN_DIAG_SEND_FRAME.byte[4] = byte;
      LIN_DIAG_SEND_FRAME.byte[5] = mask;
      LIN_DIAG_SEND_FRAME.byte[6] = invert;
      LIN_DIAG_SEND_FRAME.byte[7] = new_NAD;
      g_lin_config_status = LD_SERVICE_BUSY;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
      g_lin_config_status = LD_SERVICE_ERROR;
   }
}
#endif /* #ifdef  LIN_ENABLE_COND_CHG_NAD */
#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */

#ifndef LIN_PROTOCOL_VERSION_1_3
/* ---------------------------------------------------------------------------
 *  void ld_read_by_id (l_u8 NAD,l_u16 supplier_id,l_u16 function_id,l_u8 id,l_u8* const data)
 * --------------------------------------------------------------------------*/
/**
  @brief  read node configuration information from slave.
  @pre    LIN driver initialised
  @param  NAD            Node Address of Slave
  @param  supplier_id    16Bit Supplier ID of Slave
  @param  function_id    16Bit Function ID of Slave
  @param  id             ID from read by identifier table (row)
  @param  data           pointer to a buffer to get response data.
  @return void
*/
void ld_read_by_id (l_u8 NAD,l_u16 supplier_id,l_u16 function_id,l_u8 id,l_u8* const data)
{
   l_u8 lin_tx_state = 1u;
   /* (MISRA-C:2004 11.4/A) A cast should not be performed between a pointer to object type and a different pointer to object type
    * Reason: make the pointer (req. by LIN specification) accessible as array. */
   /* Save request data for response process */
   g_lin_config_buffer[0] = NAD;
   g_lin_config_buffer[1] = 0x00u;
   g_lin_config_buffer[2] = 0xB2u;

   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = NAD;
      LIN_DIAG_SEND_FRAME.byte[1] = 0x06u;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xB2u;
      LIN_DIAG_SEND_FRAME.byte[3] = id;
      LIN_DIAG_SEND_FRAME.byte[4] = (l_u8)(supplier_id & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[5] = (l_u8)((supplier_id >> 8) & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[6] = (l_u8)(function_id & 0xFFu);
      LIN_DIAG_SEND_FRAME.byte[7] = (l_u8)((function_id >> 8) & 0xFFu);
      g_lin_config_status = LD_SERVICE_BUSY;
      g_lin_config_rbid_data = (l_u8 (*)[])data;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
      g_lin_config_status = LD_SERVICE_ERROR;
   }
}
#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
/* ---------------------------------------------------------------------------
 *  l_u8 l_sch_tick (void)
 * --------------------------------------------------------------------------*/
/**
  @brief    Periodic check of LIN scheduletable and to required action

    The function follows a schedule. When a frame becomes due,
    its transmission is initiated. When the end of the current schedule
    is reached, l_sch_tick starts again at the beginning of the schedule.
    The l_sch_tick must be called periodically and individually for each
    interface within the node. The period is the time base set in the LDF.
    The period of the l_sch_tick call effectively sets the time base tick.
    Therefore it is essential that the time base period is uphold with
    minimum jitter. The call to l_sch_tick will not only start the
    transition of the next frame due, it will also update the signal
    values for those signals received since the previous call to l_sch_tick.
  @pre     LIN driver initialised
  @param   void
  @return  void
*/
l_u8 l_sch_tick (void)
{
   l_u8  retval = 0u;

   if (g_ld_timer_p2 < 0xFFFFu)
   {
      g_ld_timer_p2++;
   }

   if(!g_lin_zero_schedule)
   {
      /* First check if schedule table has to be switched to another schedule table
       * After sending an Goto Sleep command, do not switch to a new schedule table */
      if((g_ScheduleIdx != g_nextScheduleIdx) || (g_nextScheduleFrmIdx != 0xFFu))
      {
         g_ScheduleIdx =  g_nextScheduleIdx;
         g_activeScheduleIdx = g_ScheduleIdx;
         g_ScheduleFrmIdx = g_nextScheduleFrmIdx;
         g_nextScheduleFrmIdx = 0xFFu;
         if ((*scheduleList[g_activeScheduleIdx]) == 0)
         {
            g_lin_zero_schedule = L_SET;
         }
      }
      /* check if its time to send next frame header */
      if (!g_lin_zero_schedule)
      {
         if(g_lin_sched_data.lin_current_delay == 0u)
         {
            /* Finish previous frame via timeout procedure to ensure every state is correctly reset upon starting new frame header */
            lin_hal_wait_for_break();

            /*  Start next Frame in Schedule */
            lin_master_task_tx_header((l_u8)(*scheduleList[g_activeScheduleIdx])[g_ScheduleFrmIdx].pid);

            /*  Get the delay until the next Frame */
            g_lin_sched_data.lin_current_delay = (*scheduleList[g_activeScheduleIdx])[g_ScheduleFrmIdx].frame_delay;

#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
            if (g_ScheduleIdx == l_resolver_tbl[g_lin_coll_tbl_entry].resolver_sched_id)
            {
               g_lin_status_word.flag.event_triggerd_frame_coll = 1u;
               /* run through resolver table only once */
               if (g_lin_coll_res_active && (g_ScheduleFrmIdx == (schedTblSizeList[g_ScheduleIdx] - 1u)))
               {  /* Check if next frame would be sent for the second time, if so, switch */
                  l_sch_set(g_lin_coll_schedidx_save, g_lin_coll_frameidx_save);
                  g_lin_coll_res_active = 0u;
               }
               else
               {
                  if (g_ScheduleFrmIdx == 0u)
                  {  /* First Frame is sent now */
                     g_lin_coll_res_active = 1u;
                  }
               }
            }
#endif /* ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */

            /*  Prepare for next Frame transmission */
            /* get next frame in schedule */
            g_ScheduleFrmIdx ++;
            /* cycle in schedule */
            g_ScheduleFrmIdx %= (schedTblSizeList[g_ScheduleIdx]);

            /* reduce lin_current_delay with main_cyclic_looptime(ms) in 100ms */
            g_lin_sched_data.lin_current_delay -=(1u * (LIN_TASK_CYCLE_MS));
         }
         else /* no frame valid this time */
         {
            /* reduce lin_current_delay with main_cyclic_looptime(ms) in 100ms
             * and prevent underflow */
            retval = 0u;
            if (g_lin_sched_data.lin_current_delay <= (1u * (LIN_TASK_CYCLE_MS)))
            {
               g_lin_sched_data.lin_current_delay = 0u;
               retval = g_ScheduleFrmIdx + 1u;
            }
            else
            {
               g_lin_sched_data.lin_current_delay -= (1u * (LIN_TASK_CYCLE_MS));
            }
         }
      } /* end if active schedule != 0 */
   }
   return retval;
}
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */


#if ((defined LIN_MASTER)||(defined J2602_MASTER))
/* ---------------------------------------------------------------------------
 *  void l_sch_set (l_u16 schedule, l_u8 entry)
 * --------------------------------------------------------------------------*/
/**
  @brief   The function sets a new schedule in the schedule management
  @pre     LIN driver initialised
  @param   schedule  The new schedule table
  @param   entry  The next frame in new schedule table which had to be executed
  @return  void
*/
void l_sch_set (l_u16 schedule, l_u8 entry)
{
   if(schedule < NumberOfSchedTables)
   {
      g_nextScheduleIdx = schedule;
      g_nextScheduleFrmIdx = entry;
      g_lin_zero_schedule = L_RESET;
   }
   else
   {
      g_lin_zero_schedule = L_SET;
   }
}

void l_sch_change_pid (l_u8 old_pid, l_u8 new_pid)
{
   l_u8 loc_j, loc_k;
   for (loc_j = 0u; loc_j < NumberOfSchedTables; loc_j++)
   {
      for (loc_k = 0u; loc_k < schedTblSizeList[loc_j]; loc_k ++)
      {
         if ((*scheduleList[loc_j])[loc_k].pid == old_pid)
         {
            (*scheduleList[loc_j])[loc_k].pid = new_pid;
         }
      }
   }
}

#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

/* ---------------------------------------------------------------------------
 *  l_u16 l_ifc_read_status(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Get status information from LIN driver.
   @pre    LIN driver initialized
   @param  void
   @retval LIN_status word        MSB contains last PID information
                                  LSB contains status flags
   @see    LIN 2.0 and LIN 2.1 specification
*/
l_u16 l_ifc_read_status(void)
{
   l_u16 loc_temp;

   loc_temp = g_lin_status_word.reg;
   g_lin_status_word.reg = 0u;
   return loc_temp;
}

#ifdef J2602_SLAVE
/* ---------------------------------------------------------------------------
 *  l_u8 l_flg_tst_Reset(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Get information if a reset request has been received.
   @pre    LIN driver initialized
   @param  void
   @retval 0u        No request has been received.
           1u        Targeted Reset request has been received.
           2u        Broadcast Reset request has been received.
           3u-0xFFu  Invalid
*/
l_u8 l_flg_tst_Reset(void)
{
   return g_j2602_status.flags.reset;
}

/* ---------------------------------------------------------------------------
 *  l_bool l_flg_clr_Reset(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Clear information if a reset request has been received.
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void l_flg_clr_Reset(void)
{
   g_j2602_status.flags.reset = 0u;
}

/* ---------------------------------------------------------------------------
 *  l_bool l_flg_tst_new_bc_data0(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Get information if a new broadcast frame with ID 0x38 or 0x3A has 
           been received.
   @pre    LIN driver initialized
   @param  void
   @retval false (zero)        No broadcast data in such a frame has been received.
           true (non-zero)     Broadcast data has been received.
*/
l_bool l_flg_tst_new_bc_data0(void)
{
   return (l_bool)g_j2602_status.flags.new_frame0;
}

/* ---------------------------------------------------------------------------
 *  l_bool l_flg_clr_new_bc_data0(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Clear information if a new broadcast frame with ID 0x38 or 0x3A has 
           been received.
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void l_flg_clr_new_bc_data0(void)
{
   g_j2602_status.flags.new_frame0 = 0u;
}

/* ---------------------------------------------------------------------------
 *  l_bool l_flg_tst_new_bc_data1(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Get information if a new broadcast frame with ID 0x39 or 0x3B has 
           been received.
   @pre    LIN driver initialized
   @param  void
   @retval false (zero)        No broadcast data in such a frame has been received.
           true (non-zero)     Broadcast data has been received.
*/
l_bool l_flg_tst_new_bc_data1(void)
{
   return (l_bool)g_j2602_status.flags.new_frame1;
}

/* ---------------------------------------------------------------------------
 *  l_bool l_flg_clr_new_bc_data1(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Clear information if a new broadcast frame with ID 0x39 or 0x3B has 
           been received.
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void l_flg_clr_new_bc_data1(void)
{
   g_j2602_status.flags.new_frame1 = 0u;
}

#endif /* ifdef J2602_SLAVE */

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
#ifndef LIN_PROTOCOL_VERSION_1_3
/* ---------------------------------------------------------------------------
 *  l_ifc_goto_sleep(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Create a goto sleep master request.
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void l_ifc_goto_sleep(void)
{
   l_u8 lin_tx_state = 1u;
   /* Check if a new diagnostic request is currently possible */
#ifdef LIN_COOKED_API
   if (g_lin_tp_stat.tx_stat == LD_COMPLETED)
   {
      lin_tx_state = 0u;
   }
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
   lin_tx_state = ld_raw_tx_status();
#endif /* ends ifdef LIN_RAW_API */

   /* Fill data and start request */
   if (lin_tx_state == 0u)
   {
      LIN_DIAG_SEND_FRAME.byte[0] = 0x0u;
      LIN_DIAG_SEND_FRAME.byte[1] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[2] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[3] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[4] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[5] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[6] = 0xFFu;
      LIN_DIAG_SEND_FRAME.byte[7] = 0xFFu;

#ifdef LIN_COOKED_API
      /* length 0 and nad 0 will signal the diagnostic API that this command comes
       * from the LIN API and data is already correctly filled */
      ld_send_message(0u, 0u, LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_COOKED API */
#ifdef LIN_RAW_API
      ld_put_raw (LIN_DIAG_SEND_FRAME.byte);
#endif /* ends ifdef LIN_RAW_API */
   }
   /* Reject request */
   else
   {
   }
}
#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
