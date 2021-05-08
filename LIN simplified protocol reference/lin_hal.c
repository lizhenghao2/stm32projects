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
********************     Workfile:      lin_hal.c        **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  Freescale Hal layer for a LIN Slave Driver           *
*  FILE-DESCRIPTION:     All Hardware Access Routines for LIN Driver          *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 124         $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2016-05-09#$                                 *
*                                                                             *
******************************************************************************/

/**
@file lin_hal.c
@brief Microcontroller specific implementation of the LIN Driver communication
 */

/* ===========================================================================
 *  Header files
 * ==========================================================================*/
#include "genLinConfig.h"
#include "lin_main.h"
#include "lin_slave_task.h"
#include "lin_driver_api.h"
#include "lin_config.h"
#include "lin_type.h"
#include "lin_hal.h"
#include "lin.h"
#include "usart.h"

/* ===========================================================================
 *  Global Variables
 * ==========================================================================*/
LIN_hal_stat_reg_t      lin_hal_status_g;
static l_u8             lin_hal_rx_data_guc;
LIN_SLAVE_STATE_t       lin_slave_state_g;
t_lin_temp_frame_buffer lin_temp_buffer;
static l_u8             uc_frame_lin_version;
static l_u8             temp_data_size_uc;
static LIN_TX_DATA_t    lin_tx_data_g;
static LIN_FLAGS_t      lin_flag_g;
l_u8                    lin_ab_timeout_timer;
l_u8                    lin_frm_timeout_timer;
t_lin_timeout_ctrl      lin_timeout_ctrl;
static l_u16            l_chk_sum_ui;
l_u8                    l_txchk_byte_nr;
//static l_u16            baudrate_reg_value;
l_u8 LinDataSendThroughFlag;
t_lin_temp_frame_buffer linsendthroughbuffer;

#ifdef AUTOBAUD_ENABLED
static l_u8 auto_baud_step;
static l_u16 timer_value_init;
#endif /* end #ifdef AUTOBAUD_ENABLED */

/* ===========================================================================
 *  Functions
 * ==========================================================================*/
#ifdef AUTOBAUD_ENABLED
static void lin_hal_auto_baud(void);
#endif /* end #ifdef AUTOBAUD_ENABLED */
static void l_hal_tx_state_machine(void);
static void lin_hal_tx_checksum(l_u8 checksum);
static void lin_hal_tx_byte(l_u8 lin_hal_bytetoTX);

/* ---------------------------------------------------------------------------
 *  void lin_hal_init (void)
 * --------------------------------------------------------------------------*/

/**
   @brief  Initializes the HAL layer and UART / SCI
   @param  void
   @retval void
 */
void lin_hal_init (void)
{
   l_u8 loc_i = 0u;

   lin_hal_status_g.byte_received   = 0u;
   lin_hal_status_g.ferror          = 0u;
   lin_hal_status_g.stopbit         = 0u;
   lin_hal_status_g.isr_rec_break   = 0u;
   lin_hal_status_g.isr_rec_sync    = 0u;
   lin_hal_status_g.res0            = 0u;
   lin_hal_rx_data_guc              = 0u;
   lin_slave_state_g                = idle;
   uc_frame_lin_version             = 0u;
   temp_data_size_uc                = L_RESET;
   lin_tx_data_g.checksum           = 0u;
   lin_tx_data_g.data_puc           = 0u;
   lin_tx_data_g.index_uc           = 0u;
   lin_tx_data_g.length_c           = 0u;
   lin_tx_data_g.old_data_uc        = 0u;
   lin_flag_g.res0                  = 0u;
   lin_flag_g.tx_finished           = 0u;
   lin_flag_g.tx_next               = 0u;
   lin_ab_timeout_timer             = 0u;
   lin_frm_timeout_timer            = 0u;
   l_chk_sum_ui                     = 0u;
   l_txchk_byte_nr                  = 0u;
   lin_timeout_ctrl.byte            = 0u;

   for(loc_i = 0u; loc_i < 11u; loc_i++)
   {
      lin_temp_buffer.byte[loc_i] = 0u;
   }
   
//   baudrate_reg_value = LIN_BAUDRATE;
   
   lin_timeout_ctrl.flag.sys_init = 1u;
   lin_hal_init_uart();
   lin_timeout_ctrl.flag.sys_init = 0u;
   //LIN_Enable_Transceiver(); Jack had enable PF1 in CubeMX	 
   lin_slave_state_g = idle;
#if (defined J2602_SLAVE) && !(defined GM_SPEC_ADAPTION)
   g_j2602_status.flags.errorstate = g_j2602_status.flags.errorstate | J2602_RESET_STATUS;
#endif /* end #if (defined J2602_SLAVE) && !(defined GM_SPEC_ADAPTION) */
   return;
}

#ifdef LIN_MASTER
/* ---------------------------------------------------------------------------
 *  void lin_hal_tx_header (l_u8 l_pid) ( Added by jeffery 2018.01.30 )
 * --------------------------------------------------------------------------*/

/**
   @brief  This function commands the sending of the LIN header, MASTER task of MASTER node.
   @pre    LIN driver initialized
   @param  l_u8 l_pid  LIN identifier value.
   @retval void
 */
void lin_hal_tx_header (l_u8 l_pid) 
{                                                                                        
   l_u8 uc_data_temp = 0;

   /* UART1 send Break Field */
	 HAL_LIN_SendBreak(&huart3);

   /* Send Sync Field data to UART to transmit Sync Field */
   uc_data_temp = SYNC_FIELD_k;
   lin_hal_tx_char(uc_data_temp);

   /* Send PID data to UART to transmit PID Field */
	 //lin_hal_tx_char(l_pid);
	 
  return;
}
#endif // end #ifdef LIN_MASTER

#ifdef J2602_SLAVE
/* ---------------------------------------------------------------------------
 *  void j2602_hal_target_reset (l_u8 reset)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function resets the controller after receiving a reset request.
   @pre    LIN driver initialized
   @param  reset  Sets reset flag every time, but reset all errors if zero, keeps all errors if non-zero
   @retval void
 */
void j2602_hal_target_reset (l_u8 reset)
{
#ifndef GM_SPEC_ADAPTION
   /* Deleting all errors and set the status to RESET */
   if (0u != reset)
   {
      g_j2602_status.flags.errorstate |= J2602_RESET_STATUS;
   }
   else
   {
      g_j2602_status.flags.errorstate = J2602_RESET_STATUS;
   }
#endif /* #ifdef GM_SPEC_ADAPTION */
   /* Setting of the response data is made when the response header was received
    * The response header will be sent only if the targeted reset was called, so
    * there is no need to make a difference between broadcast and target reset
    * here.
    */
   return;
}
#endif /* end #ifdef J2602_SLAVE */

/* ---------------------------------------------------------------------------
 *  void lin_hal_tx_byte(l_u8 lin_hal_bytetoTX)
 * --------------------------------------------------------------------------*/

/**
   @brief  Transmits one byte on bus
   @pre    LIN driver initialized
   @param  lin_hal_bytetoTX           pointer to frame buffer
   @retval void
 */
static void lin_hal_tx_byte (l_u8 lin_hal_bytetoTX)
{
   /* Store value for receive check later */
   lin_tx_data_g.old_data_uc = lin_hal_bytetoTX;

   /* send data to UART, for transmit */
   lin_hal_tx_char(lin_hal_bytetoTX); 

   /* make configuration for tx next data  */
   lin_tx_data_g.length_c--;
   /* check if next field is CRC */
   if (!lin_tx_data_g.length_c)
   {
      lin_flag_g.tx_next = CHECKSUM_NEXT;
   }
   else
   {
      lin_flag_g.tx_next = L_SET;
   }
}

/* ---------------------------------------------------------------------------
 *  void lin_hal_tx_response (l_u8 l_type, l_u8 l_data[], l_u8 l_len)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function prepares the driver for transmitting a response frame.
   @pre    LIN driver initialized
   @param  l_u8 l_type   LIN type (1.3, 2.0, or 2.1)
   @param  l_u8 l_data[] Databuffer for frame data
   @param  l_u8 l_len    Length information of frame
   @retval void
 */
void lin_hal_tx_response (l_u8 l_type, const l_u8 l_data[], l_u8 l_len)
{
   l_u16 ui_I = 0u;
   l_u8 uc_temp_sum = 0u;

   lin_tx_data_g.index_uc = 0u;
   lin_slave_state_g = tx_data;
   lin_tx_data_g.length_c = l_len;
   g_lin_frame_data_size = l_len;
   if(l_type == LIN_2_X)
   {
      /* For LIN 2.x use PID & Data for CRC */
      l_chk_sum_ui = lin_temp_buffer.frame.PID;
   }
   else
   {
      l_chk_sum_ui = 0u;
   }
   g_lin_irqState = l_sys_irq_disable();
   /* Copy data from main buffer to the tx buffer */
   for(ui_I = 0u; ui_I < l_len; ui_I++)
   {
      lin_temp_buffer.frame.DataBuffer[ui_I] = l_data[ui_I];
   }
   l_sys_irq_restore(g_lin_irqState);

   l_txchk_byte_nr = (l_u8)(lin_tx_data_g.length_c - 1u);
   l_chk_sum_ui += lin_temp_buffer.frame.DataBuffer[l_txchk_byte_nr];
   /* if sum is bigger than 16 bit */
   uc_temp_sum = (l_u8)(l_chk_sum_ui >> 8u);
   l_chk_sum_ui = l_chk_sum_ui + uc_temp_sum;
   l_chk_sum_ui &= 0xFFu;
   /* frame with only 1 databyte */
   if (l_txchk_byte_nr == 0u)
   {
      lin_tx_data_g.checksum = (l_u8)(~(l_u8)l_chk_sum_ui);
   }
#ifndef AUTOBAUD_ENABLED
#ifdef J2602_SLAVE
   /* SYNCH Field and valid PID is received. Reset this flag for next frame. */
   if(lin_hal_status_g.isr_rec_sync == 1u)
   {
      lin_hal_status_g.isr_rec_sync = 0u;
   }
#endif /* end #ifdef J2602_SLAVE */
#endif /* end #ifndef AUTOBAUD_ENABLED */
   
   /* start transmit of data fields */
   lin_hal_tx_byte(lin_temp_buffer.frame.DataBuffer[0]);
   lin_temp_buffer.frame.BufferIndex = 1u; /* Index of next data to transmit */
   lin_timeout_ctrl.flag.transm_ongoing = 1u;

   return;
}

/* ---------------------------------------------------------------------------
 *  void l_hal_tx_state_machine (void)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function controls the transmission of frame data and checksum
   @pre    LIN driver initialized
   @param  void
   @retval void
 */
static void l_hal_tx_state_machine (void)
{
   /* tx crc of frame */
   if (lin_flag_g.tx_next == CHECKSUM_NEXT)
   {
      lin_hal_tx_checksum(lin_tx_data_g.checksum);
   }
   /* tx next byte of frame */
   if (lin_flag_g.tx_next == L_SET)
   {
      /* Set state machine to transmit data */
      lin_slave_state_g = tx_data;
      lin_hal_tx_byte(lin_temp_buffer.frame.DataBuffer[lin_temp_buffer.frame.BufferIndex]);
      lin_temp_buffer.frame.BufferIndex ++; /* Index of next data to transmit */
   }
}

/* ---------------------------------------------------------------------------
 *  void lin_hal_tx_wake_up(void)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function transmits a wake-up pulse on the bus
   @pre    LIN driver initialized
   @param  void
   @retval void
 */
/* (MISRA-C:2004 8.10/R, e765) external ... could be made static
 * Reason: Part of API, only used if slave may wake up LIN network */
void lin_hal_tx_wake_up (void)
{
   l_u8 loc_data = 0u;

   /* Send dummy data to UART to send Break */
   loc_data = 0xE0u;
   lin_hal_tx_char(loc_data); /*lint !e923*/ /* HW register access */
}

/* ---------------------------------------------------------------------------
 *  void l_hal_txchecksum_calculation(void)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function calculates the checksum of a publish frame step-wise
           for each byte that is actually transmitted.
   @pre    LIN driver initialized
   @param  void
   @retval void
 */
void l_hal_txchecksum_calculation (void)
{
   l_u8 uc_temp_sum;

   l_txchk_byte_nr--;
   /*  l_chk_sum_ui += l_txchk_data_p[l_txchk_byte_nr]; */
   l_chk_sum_ui += lin_temp_buffer.frame.DataBuffer[l_txchk_byte_nr];
   /* if sum is bigger than 16 bit */
   uc_temp_sum = (l_u8)(l_chk_sum_ui >> 8u);
   l_chk_sum_ui = l_chk_sum_ui + uc_temp_sum;
   l_chk_sum_ui &= 0xFFu;
   if (l_txchk_byte_nr == 0u)
   {
      lin_tx_data_g.checksum = (l_u8)(~(l_u8)l_chk_sum_ui);
      l_chk_sum_ui = 0u;
   }
}

/* ---------------------------------------------------------------------------
 *  lin_hal_tx_checksum(l_u8 checksum)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function transmits the checksum on the bus.
   @pre    LIN driver initialized
   @param  l_u8 checksum  The checksum of the frame.
   @retval void
 */
static void lin_hal_tx_checksum (l_u8 checksum)
{
   /* set next state in state machine */
   lin_slave_state_g = tx_checksum;

   /* send data to uart, for transmit */
   lin_hal_tx_char(checksum); /*lint !e923*/ /* HW register access */

   /* store data for receive check later */
   lin_tx_data_g.old_data_uc = checksum;
   /* reset tx state finish transmit of frame */
   lin_flag_g.tx_next = L_RESET;
}

/* ---------------------------------------------------------------------------
 *  void lin_hal_rx_response (l_u8 l_type, l_u8 l_len)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function prepares the driver for receiving a subscribe frame.
   @pre    LIN driver initialized
   @param  l_u8 l_type  The LIN protocol version of the frame.
   @param  l_u8 l_len   The length information of the frame.
   @retval void
 */
void lin_hal_rx_response (l_u8 l_type, l_u8 l_len)
{
   g_lin_frame_data_size = l_len;
   temp_data_size_uc = g_lin_frame_data_size;
   uc_frame_lin_version = l_type;
   lin_slave_state_g = rx_data;
   lin_timeout_ctrl.flag.frame = 1u;
   lin_frm_timeout_timer = 0u;
   
#ifndef AUTOBAUD_ENABLED
#ifdef J2602_SLAVE
   /* SYNCH Field and valid PID is received. Reset this flag for next frame. */
   if(lin_hal_status_g.isr_rec_sync == 1u)
   {
      lin_hal_status_g.isr_rec_sync = 0u;
   }
#endif /* end #ifdef J2602_SLAVE */
#endif /* end #ifndef AUTOBAUD_ENABLED */
   
   return;
}

/* ---------------------------------------------------------------------------
 *  void l_hal_rx_state_machine (void)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function stepts through the states for receiving frame headers
           and frame data.
   @pre    LIN driver initialized
   @param  void
   @retval void
 */
void l_hal_rx_state_machine (void)
{
   l_u8 uc_temp_sum = 0u;
   /* next computations depend on receive task state (state machine) */
   switch(lin_slave_state_g)
   {
      case idle:
         break;

      /* if break received next data is sync field */
      case break_received:
#ifndef AUTOBAUD_ENABLED
         /* check if sync field else invalid_field error */
         if (lin_hal_rx_data_guc == SYNC_FIELD_k)
         {
            lin_slave_state_g = sync_received;
					 
					 #ifdef LIN_MASTER
					 /* Send PID data to UART to transmit PID Field, added by jeffery 2018.01.30*/
           lin_hal_tx_char((l_u8)(*scheduleList[g_activeScheduleIdx])[(g_ScheduleFrmIdx + (schedTblSizeList[g_ScheduleIdx]) - 1u) % (schedTblSizeList[g_ScheduleIdx])].pid);
					 #endif /* end #ifdef LIN_MASTER */
					 
#ifdef J2602_SLAVE
            lin_hal_status_g.isr_rec_sync = 1u;
#endif /* end #ifdef J2602_SLAVE */
         }
         else
         {
            lin_hal_init_uart();
            lin_slave_state_g = idle;
#ifdef J2602_SLAVE
            /* J2602 slave without baudrate synchronization shall cause an error bit set if the sync field is inconsistent. */
            if(lin_hal_status_g.isr_rec_sync == 0u)
            {
               lin_timeout_ctrl.flag.recept_started = 1u;
               lin_hal_set_resp_error(J2602_DATA_ERROR);
            }
#endif /* end #ifdef J2602_SLAVE */
         }
#else /* else #ifndef AUTOBAUD_ENABLED */
         lin_slave_state_g = sync_received;
#endif /* end #ifndef AUTOBAUD_ENABLED */
         break;

      /* if sync field received next data is pid field */
      case sync_received:
         /* after sync field a pid field was received */
         /* reset lin_state_after receive of pid (Protocoll layer will set for further rx/tx action) */
         lin_slave_state_g = idle;
         /* check if pid is valid and if its tx or rx frame */
			lin_slave_task_rx_pid(lin_hal_rx_data_guc);   //这是必须得，这里有根据配置的lin的调度信息，以PID作为判断，判断是执行发送的主任务还是接受的从任务
				
         break;

      /* if it is rx frame, then data field comes next */
      case rx_data:
         /* store received data and check for type of next field */
					//lin_temp_buffer在透传lin信息中是有用的
         lin_temp_buffer.frame.DataBuffer[(g_lin_frame_data_size - temp_data_size_uc)] = lin_hal_rx_data_guc;
         lin_timeout_ctrl.flag.frame = 1u;
         lin_timeout_ctrl.flag.transm_ongoing = 1u;
         lin_timeout_ctrl.flag.recept_started = 1u;
         lin_frm_timeout_timer = 0u;

         /* calculate temporary checksum (step wise after each byte) */
         if(g_lin_frame_data_size == temp_data_size_uc)
         {
            if (uc_frame_lin_version == LIN_2_X)
            {
               /* For LIN 2.x use PID & Data for CRC */
               l_chk_sum_ui = lin_temp_buffer.frame.PID;
            }
            else
            {
               l_chk_sum_ui = 0u;
            }
         }
         l_chk_sum_ui += lin_temp_buffer.frame.DataBuffer[(g_lin_frame_data_size - temp_data_size_uc)];
         /* if sum is bigger than 8 bit */
         uc_temp_sum = (l_u8)(l_chk_sum_ui>>8);
         l_chk_sum_ui = l_chk_sum_ui + uc_temp_sum;
         l_chk_sum_ui &= 0xFFu;

         /* get number of data still to receive */
         temp_data_size_uc--;

         /* if no more data to receive got to rx_checksum state */
         if (temp_data_size_uc == 0u)
         {
            lin_slave_state_g = rx_checksum;
						//here may put flag symboling from motor to pc
         }
         break;

      /* if last data was received next is crc field */
      case rx_checksum:
         /* Store crc data and check if crc is okay */
         lin_temp_buffer.frame.Checksum = lin_hal_rx_data_guc;
         l_chk_sum_ui = ((l_u16)(~l_chk_sum_ui)) & 0x00FFu;
         /* check if checksum was faulty */
         if (lin_hal_rx_data_guc != (l_chk_sum_ui))
         {
#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
            if (g_lin_active_pid == 0x3Cu)
            {
               lin_diag_rx_chksum_error();
            }
            lin_hal_set_resp_error();
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
#ifdef J2602_PROTOCOL
            lin_hal_set_resp_error(J2602_CHECKSUM_ERROR);
#endif /* end #ifdef J2602_PROTOCOL */
         }
         else
         {
					 //这里我们提取lin透传的报文内容，并置位lin透传标志位
            /* notify upper layer about received frame. */
            //lin_slave_task_rx_data();
					 linsendthroughbuffer.frame.PID=lin_temp_buffer.frame.PID;
					 linsendthroughbuffer.frame.DataBuffer[0]=lin_temp_buffer.frame.DataBuffer[0];
					 linsendthroughbuffer.frame.DataBuffer[1]=lin_temp_buffer.frame.DataBuffer[1];
					 linsendthroughbuffer.frame.DataBuffer[2]=lin_temp_buffer.frame.DataBuffer[2];
					 linsendthroughbuffer.frame.DataBuffer[3]=lin_temp_buffer.frame.DataBuffer[3];
					 linsendthroughbuffer.frame.DataBuffer[4]=lin_temp_buffer.frame.DataBuffer[4];
					 linsendthroughbuffer.frame.DataBuffer[5]=lin_temp_buffer.frame.DataBuffer[5];
					 linsendthroughbuffer.frame.DataBuffer[6]=lin_temp_buffer.frame.DataBuffer[6];
					 linsendthroughbuffer.frame.DataBuffer[7]=lin_temp_buffer.frame.DataBuffer[7];
					 LinDataSendThroughFlag=1;
         }
         lin_timeout_ctrl.flag.frame = 0u;
         lin_timeout_ctrl.flag.transm_ongoing = 0u;
         lin_timeout_ctrl.flag.recept_started = 0u;
         lin_frm_timeout_timer = 0u;
         /* Set LIN slave state to idle to wait for next frame */
         lin_slave_state_g = idle;
         break;

      case tx_checksum:
         /* check if transmission has crashed */
         if(lin_hal_rx_data_guc != lin_tx_data_g.old_data_uc)
         {
            /* TX Response ERROR */
            lin_slave_state_g = idle;
            lin_flag_g.tx_next = L_RESET;
#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
            lin_hal_set_resp_error();
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
#ifdef J2602_PROTOCOL
            lin_hal_set_resp_error(J2602_DATA_ERROR);
#endif /* end #ifdef J2602_PROTOCOL */
         }
         else
         {
            /* if tx was okay, finish tx.  */
            lin_slave_state_g = idle;
            lin_slave_task_tx_data();
         }
         /* Reset transmission ongoing flag */
         lin_timeout_ctrl.flag.transm_ongoing = 0u;
         break;
      default:
         /* Impossible Slave State... */
         lin_slave_state_g = idle;
         break;
   }
}

#ifdef AUTOBAUD_ENABLED
/* ---------------------------------------------------------------------------
 *  void lin_hal_auto_baud(void)
 * --------------------------------------------------------------------------*/

/**
   @brief  This function handles auto baud function, including baud detection and synchronization.
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_hal_auto_baud(void)
{
   //l_u16 bits_value_sum;
   l_u16 timer_value_temp;
   volatile l_u16 bits_value_temp;
   l_u8 BRG_VAL_temp;
   //l_u8 dummy_data; /* do not remove this variable needed to empty SCI data register and to clear SCI status register*/

   /* clear received active edge flag */
   
   /* receive interrupt on each falling edge of the sync field */
   if (L_SET == auto_baud_step)
   {
      /* notice the time of the first falling edge of the sync field */
      //timer_value_init = *TIMER_TCNT_REG; /* HW register access */
   }
   /* increment counter on each falling edge during sync field
   * do nothing until last falling edge of sync field is received */
   auto_baud_step++;

   /* five sync bits (interrupts) are received ? */
   if ((SYNC_FIELD_INTERRUPTS + 1u) == auto_baud_step)
   {
      /* disable Receive Input Active Edge Interrupt */

      /* notice the time of the last received interrupt of the sync field */
      //timer_value_temp = *TIMER_TCNT_REG; /*lint !e923*/ /* HW register access */
      if (timer_value_temp < timer_value_init)
      {
         bits_value_temp = (0xFFFFu - timer_value_init) + timer_value_temp;
      }
      else
      {
         bits_value_temp = timer_value_temp - timer_value_init;
      }

      /* Calculation 2 bits time value */
      bits_value_temp += (1 << 1u);
      bits_value_temp = bits_value_temp >> 2u;
      /* middle sync bits value is on the valid range ? */
      /* allow baudrate deviation of 10 % is recommended to compensate failure produced by
      bits value calculation and almost deviation caused by timer interrupt latency */
      if((bits_value_temp > 2)&&(bits_value_temp < 1))
      {
         /* evaluate the middle sync bits value */
         /* baud rate = bus clock / (SBR[12:0] / 16 ) */
         /* bits_value_temp = (bus clock / baud rate ) x 32 */
         bits_value_temp += (1 << 4u);
         BRG_VAL_temp = bits_value_temp >> 5u;
         //if(BRG_VAL_temp != (LIN_BASE_REG->BAUD & 0x00001FFFu))
         //{
            /* Note! The baud rate generator is disabled after reset and not started
            * until the TE bit or the RE bit is set for the first time */
            //LIN_BASE_REG->CTRL = 0x00000000u;
            /* Set Baudrate Generator and enable */
            //LIN_BASE_REG->BAUD = BRG_VAL_temp;
            /* Enable UART reception and transmission */
            //LIN_BASE_REG->CTRL = LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK | LPUART_CTRL_RIE_MASK;
         //}
      }
      else
      {
         lin_hal_init_uart();
         lin_slave_state_g = idle;
      }

      auto_baud_step = 0u;
      lin_timeout_ctrl.flag.autobaud = 0x00u;
      lin_ab_timeout_timer = 0x00u;
   }
}
#endif /* end #ifdef AUTOBAUD_ENABLED */


/* ---------------------------------------------------------------------------
 *  void lin_hal_ISR(void)
 * --------------------------------------------------------------------------*/
/**
   @brief  Interrupt service routine for receive interrupt. Can be also used for
           polling (then it should be called once each bit).
   @pre    LIN driver initialized
   @param  void
   @retval void
*/
void lin_hal_ISR(void)
{
	l_u32 uart_reg_stat;
	uart_reg_stat = LIN_BASE_REG->SR;
	lin_hal_rx_data_guc = LIN_BASE_REG->DR;

#if defined(LIN_PROTOCOL_VERSION_2_0)|| defined(LIN_PROTOCOL_VERSION_2_1) || defined(J2602_PROTOCOL)
   g_lin_status_word.flag.bus_activity = L_SET;
#endif /* end #if defined(LIN_PROTOCOL_VERSION_2_0)|| defined(LIN_PROTOCOL_VERSION_2_1) || defined(J2602_PROTOCOL) */

   /* Break and End of Synch Field detected */
   /* no error in interface */
   if(0x00u == (uart_reg_stat & (USART_SR_FE | USART_SR_NE | USART_SR_ORE)))
   {
      /* data register full flag? */
      if(0x00u != (uart_reg_stat & USART_SR_RXNE))
      {
         /* tx data */
         if (lin_slave_state_g == tx_data)
         {
            /* check if transmission has crashed */
            if(lin_hal_rx_data_guc != lin_tx_data_g.old_data_uc)
            {
               /* TX Response ERROR */
               lin_slave_state_g = idle;
               lin_flag_g.tx_next = L_RESET;
#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
               lin_hal_set_resp_error();
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
#ifdef J2602_PROTOCOL
               lin_hal_set_resp_error(J2602_DATA_ERROR);
#endif /* end #ifdef J2602_PROTOCOL */
            }
            else
            {
               /* if tx was okay, send next field */
               l_hal_tx_state_machine();
            }
         }
         else
         {
            if (lin_slave_state_g != idle)
            {
               lin_hal_status_g.byte_received = L_SET;
            }
#ifdef AUTOBAUD_ENABLED
            else
            {
            	 lin_hal_init_uart();
            }
#endif /* end #ifdef AUTOBAUD_ENABLED */
         }
      }
      /* no data register full flag */
      else
      {
         /* active edge received? */
         if(0x00u != (0/*uart_reg_stat & LPUART_STAT_RXEDGIF_MASK)*/))
         {
#ifdef AUTOBAUD_ENABLED
            if(lin_slave_state_g == break_received)
            {
               lin_hal_auto_baud();
            }
            else
#endif /* end #ifdef AUTOBAUD_ENABLED */
            {
               lin_slave_state_g = idle;
            }
         }
      }
   }
   /* error on interface */
   else
   {
      /* framing error */
      if(0x00u != (uart_reg_stat & USART_SR_FE))
      {
         lin_timeout_ctrl.flag.frame = 0u;
         lin_frm_timeout_timer = 0u;
#if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1
         lin_hal_set_resp_error();
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
#ifdef J2602_PROTOCOL
         lin_hal_set_resp_error(J2602_FRAMING_ERROR);
#endif /* end #ifdef J2602_PROTOCOL */
         if(BREAK_FIELD_k == lin_hal_rx_data_guc)
         {
#ifdef AUTOBAUD_ENABLED
            //LIN_BASE_REG->STAT |= LPUART_STAT_RXEDGIF_MASK;
            //LIN_BASE_REG->BAUD |= LPUART_BAUD_RXEDGIE_MASK;
            auto_baud_step = 0x01u;
            lin_timeout_ctrl.flag.autobaud = 0x01u;
            lin_ab_timeout_timer = 0x00u;
#endif /* end #ifdef AUTOBAUD_ENABLED */
            lin_slave_state_g = break_received;
         }
         else
         {
#ifdef AUTOBAUD_ENABLED
            if(lin_slave_state_g == idle)
            {
            	 lin_hal_init_uart();
            }
#endif /* end #ifdef AUTOBAUD_ENABLED */
         }
      }
      /* other error */
      else
      {
#if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1
         lin_hal_set_resp_error();
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
#ifdef J2602_PROTOCOL
         lin_hal_set_resp_error(J2602_FRAMING_ERROR);
#endif /* end #ifdef J2602_PROTOCOL */
      }
   }
	 /* Clear USART_SR */
	LIN_BASE_REG->SR = 0x00u;

  return;
}

/* ---------------------------------------------------------------------------
 *  void lin_hal_init_uart(void)
 * --------------------------------------------------------------------------*/

/**
   @brief  Initializes the UART / SCI
   @param  void
   @retval void
 */
//This function is to implement LinInit(&huart3);
void lin_hal_init_uart(void)
{
//  GPIO_InitTypeDef GPIO_InitStructure;
//	UART_InitTypeDef USART_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure;
	
	void MX_USART3_UART_Init(void);
	LinInit(&huart3);
	HAL_UART_MspInit(&huart3);
	/* Enable the UART1 LIN mode */
	USART3->CR2 |= 0x4000;
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	
  
//	/* USART1_TX --> GPIOA.9 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
//  GPIO_Init(GPIOA, &GPIO_InitStructure); 
//   
//  /* USART1_RX --> GPIOA.10 */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
//  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  /* Usart1 NVIC configuration */
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ; 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//	NVIC_Init(&NVIC_InitStructure); 

//	USART_InitStructure.USART_BaudRate = baudrate_reg_value; 
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
//	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
//	USART_InitStructure.USART_Parity = USART_Parity_No; 
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
//	/* Enable UART reception and transmission */
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
//	/* UART1 initialization */
//  USART_Init(USART1, &USART_InitStructure); 
//	/* Enable the UART1 LIN mode */
//	USART_LINCmd(USART1, ENABLE);
//	/* Enable UART1 */
//  USART_Cmd(USART1, ENABLE); 

  #ifndef POLLING_MODE
    /* Receive interrupt enable */
		__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
	  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  #endif /* ifndef POLLING_MODE*/


#ifdef AUTOBAUD_ENABLED
	//LIN_BASE_REG->STAT |= LPUART_STAT_RXEDGIF_MASK;
	//LIN_BASE_REG->BAUD &= ~LPUART_BAUD_RXEDGIE_MASK;
  auto_baud_step = 0x00u;
#endif /* end #ifdef AUTOBAUD_ENABLED */
}

#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1) || (defined J2602_PROTOCOL))
/* ---------------------------------------------------------------------------
 *  void lin_hal_set_resp_error ()
 * --------------------------------------------------------------------------*/

/**
   @brief  Set Response Error flag and Statusword Update correctly dependent
           of current driver state.
   @note   Response error is set latest at beginning of a new synch break - if
           not timed out before via ld_task(). Flags are set again earliest at
           PID recognition.
   @param  void
   @retval void
 */
#if (defined J2602_PROTOCOL)
#ifdef GM_SPEC_ADAPTION
void lin_hal_set_resp_error (l_u8 j2602_fault)
#else
static void lin_hal_set_resp_error (l_u8 j2602_fault)
#endif /* #ifdef GM_SPEC_ADAPTION */
#else
/* Prototype for LIN protocols */
void lin_hal_set_resp_error (void)
#endif /* end #if defined J2602_PROTOCOL */
{
   /* Protect setting of variables in different contexts */
   g_lin_irqState = l_sys_irq_disable();
#ifndef GM_SPEC_ADAPTION
   if ((lin_timeout_ctrl.flag.recept_started != 0u) || (lin_timeout_ctrl.flag.transm_ongoing != 0u))
#else
   /* GM specific J2602 adaption needs also the PID errors shown in the response error flag */
   if ((lin_timeout_ctrl.flag.recept_started != 0u)
         || (lin_timeout_ctrl.flag.transm_ongoing != 0u)
         || ((g_j2602_status.flags.errorstate & 0x10) != 0u))
#endif /* #ifndef GM_SPEC_ADAPTION */
   {
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
      if(g_lin_rx_ctrl.flag.evttrgfrm == 0u)
#endif /* #ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
      {
#if ((defined(J2602_PROTOCOL)) && (!(defined(GM_SPEC_ADAPTION))))
         /* J2602 errors are assembled in a variable for assembling the statusword when required for
          * transmission of a designated frame containing the status byte.
          * If receiving a broadcast frame ignore errors. */
         if (lin_timeout_ctrl.flag.recept_started == 1u)
         {
            if ((g_lin_active_pid != 0x78u) && (g_lin_active_pid != 0xBAu)
            && (g_lin_active_pid != 0x39u) && (g_lin_active_pid != 0xFBu))
            {
               g_j2602_status.flags.errorstate |= j2602_fault;
            }
         }
         else
         {
            /* If - Else is necessary because after reception of the first byte during a subscribe frame
             * the transm_ongoing flag is also set to indicate that a first data byte of a frame has been
             * received.
             * If a frame was being transmitted, indicate that the status byte was not successfully transmitted. */
            if (lin_timeout_ctrl.flag.transm_ongoing == 1u)
            {
               g_j2602_status.flags.errorstate |= j2602_fault;
               g_j2602_status.flags.status_byte_transm = 0u;
            }
         }
#endif /* end #ifndef J2602_PROTOCOL */
#ifdef GM_SPEC_ADAPTION
         /* Set errors except for broadcast frames. */
         if ((g_lin_active_pid != 0x78u) && (g_lin_active_pid != 0xBAu)
            && (g_lin_active_pid != 0x39u) && (g_lin_active_pid != 0xFBu))
         {
            LIN_STATUS_BYTE(1u);
         }
#endif /* end #ifdef GM_SPEC_ADAPTION */
#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
         l_Set_Response_Error_Flag();
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */
      }
      if (g_lin_status_word.flag.last_pid != 0x0u)
      {
         g_lin_status_word.flag.overrun = L_SET;
      }
      g_lin_status_word.flag.last_pid = g_lin_active_pid;
      g_lin_status_word.flag.error_in_resp = L_SET;
   }
   lin_timeout_ctrl.flag.recept_started = 0u;
   lin_timeout_ctrl.flag.transm_ongoing = 0u;
#ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE
   /* Reset flag to indicate an event triggered frame (receive or transmit) */
   g_lin_rx_ctrl.flag.evttrgfrm = L_RESET;
#endif /* #ifdef LIN_EVENT_TRIGGERED_FRAME_ENABLE */
   l_sys_irq_restore(g_lin_irqState);
}
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 || defined J2602_PROTOCOL */
