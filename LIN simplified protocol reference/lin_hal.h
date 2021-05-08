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
********************     Workfile:      lin_hal.h        **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  Microchip Hal layer for a LIN Slave Driver           *
*  FILE-DESCRIPTION:     All defines for lin_hal.c                            *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 79          $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2015-12-10#$                                 *
*                                                                             *
******************************************************************************/
/**
@file lin_hal.h
@brief Header file of lin_hal.c

   Defines and declaration for lin_hal.c
*/

#ifndef LIN_HW_DRIVER_H /* to interpret header file only once */
#define LIN_HW_DRIVER_H

/* ===========================================================================
 *  Header Files
 * ==========================================================================*/

#include "genLinConfig.h"
//#include "stm32f10x.h" 

/* ===========================================================================
 *  Constants
 * ==========================================================================*/

#ifndef LIN_FRAME_TIMEOUT
#define LIN_FRAME_TIMEOUT       LIN_AUTOBAUD_TIMEOUT
#endif /* ifndef LIN_FRAME_TIMEOUT */

#define BREAK_FIELD_k  0x00u
#define SYNC_FIELD_k    0x55u

#ifndef CHECKSUM_NEXT
   #define CHECKSUM_NEXT 2u
#endif

#define PID_MASTER_REQ  0x3Cu

#define SYNC_FIELD_INTERRUPTS   5u

/* UART select */
#if defined(LIN_UART1)
#define        LIN_BASE_REG  USART1
#elif defined(LIN_UART2)
#define        LIN_BASE_REG  USART2
#elif defined(LIN_UART3)
#define        LIN_BASE_REG  USART3
#endif

#define lin_hal_tx_char(x)        (LIN_BASE_REG->DR = (x))
#define lin_hal_is_byte_received() (lin_hal_status_g.byte_received == L_SET)
#define lin_hal_reset_byte_received() (lin_hal_status_g.byte_received = L_RESET)

#define lin_hal_set_rx_pid(pid)   (lin_temp_buffer.frame.PID = (pid))
#define lin_hal_set_tx_pid(pid)   (lin_temp_buffer.frame.PID = (pid))

#define lin_hal_wait_for_break()        (lin_slave_state_g = idle)

#ifdef J2602_SLAVE
/**
   @brief  This is the J2602 Statusbyte
   @note   This byte is divided into two parts, the Application Information
           Field (Bit 0-4) and the  Error Field (Bit 5-7)
   @par
         Error Field Err[2:0]
         These bits report the four defined Error States that have been seen
         by the Slave node since an error state has been last reported to the
         Master node. The four states are "sticky" which means that they are
         retained until they are successfully reported to the Master node in a
         message without any detected errors, after which they are automatically
         cleared. Only one state may be reported at a time. There is an inherent
         hierarchy to the states, with the highest latched state reported first
         Bit 7 also serves the purpose of the Response_Error bit from LIN Rev.2.0
         Section 6.3
         Err2   Err1   Err0   Fault State               Priority
         0      0      0      No Detected Fault         0
         0      0      1      Reset                     1
         0      1      0      Reserved                  2
         0      1      1      Reserved                  3
         1      0      0      Data Error                4
         1      0      1      Checksum Error            5
         1      1      0      Byte Field Framing Error  6
         1      1      1      ID Parity Error           7 (highest)
   @par
         No Detected Fault
         A slave node shall indicate this state whenever none of the detectable fault
         states are active. This is the default state of the LIN device.
   @par
         Reset
         A slave node shall set this state upon interruption and resumption of power,
         after a watchdog timeout, or after receipt of a Reset Command. Note that for
         those devices that require configuration and store the configuration information
         in volatile memory, this state indicates that the device is currently unconfigured
         and requires configuration. For those devices that use non-volatile memory for
         configuration information storage, then the state indicates configuration is
         required the first time the part is powered on and only indicates a reset from then
         on.
   @par
         Data Error
         A slave or master node that is transmitting a bit on the bus shall also monitor the
         bus. A Data Error shall be detected when the bit or bye that is received is different
         from the bit or byte value that is transmitted.
         A slave node that is receiving shall detect a Data Error when the Data in the fixed
         form Sync Byte is received incorrectly, i.e. is not $55. A slave node that performs
         autobauding shall detect this error but is not required to set the error bit.
   @par
         Checksum Error
         A Checksum Error shall be detected if the inverted modulo-256 sum over all received
         data bytes and the protected identifier (when using enhanced checksum) and the received
         checksum byte field does not result in $FF.
   @par
         Byte Field Framing Error
         The receiver shall detect a Byte Field Framing Error if the ninth bit after a valid start
         bit is dominant.
   @par
         ID Parity Error
         The receiver shall detect an ID Parity Error if the received ID parity (bits 6 & 7) does
         not match the ID parity calculated from the equations in the LIN 2.0 Protocol Specification
         based on the received identifier (bits 0-5)
   @par
         Application Information Field (Bits 4-0)
   @par
         The bit APINFO 4 shall be used to indicate when the application requires attention from the
         Master Device. This shall be indicated by setting te bit to "1".
         When a Reset state is indicated by the Error Field when this bit is "1" is shall
         indicate that the device needs to be configured.
         The action taken by the Master when this bit is "1" and no Reset state ist indicated
         shall be documented (e.g. NCF, datasheet,...) and may include loading a special
         schedule table which queries the Application as to its status.
         The lower four bits of the application information field (APINFO[3:0]) are not
         explicitly defined, since their structure will be dependent on the specific application
         and implementation of the slave node. This field may be implemented as discrete status
         bits, a state encoded bit field, or a combination of the two. The clearing mechanism used
         is also implementation dependent. The explicit definition of the Application Information
         Field shall be documented (e.g. in the NCF, datasheet, ...)
         A value of zero (00000b) indicates that no application information currently needs to be
         reported. All other encodings are application specific and beyond the scope of J2602.
*/
#ifndef J_2602_STATUSBYTE
#define J_2602_STATUSBYTE   1

#define J2602_PID_ERROR         0x10u /* 0b111 when translated in J2602 statusbyte */
#define J2602_FRAMING_ERROR     0x08u /* 0b110 when translated in J2602 statusbyte */
#define J2602_CHECKSUM_ERROR    0x04u /* 0b101 when translated in J2602 statusbyte */
#define J2602_DATA_ERROR        0x02u /* 0b100 when translated in J2602 statusbyte */
#define J2602_RESET_STATUS      0x01u /* 0b001 when translated in J2602 statusbyte */
#endif /* end #ifndef J_2602_STATUSBYTE */
#endif /* end #ifdef J2602_SLAVE */

/* ===========================================================================
 *  Function Prototypes
 * ==========================================================================*/

extern void lin_hal_init (void);
#ifdef LIN_MASTER
extern void lin_hal_tx_header (l_u8 l_pid); 
#endif // end #ifdef LIN_MASTER
extern void lin_hal_rx_response (l_u8 l_type, l_u8 l_len);
extern void lin_hal_tx_response (l_u8 l_type, const l_u8 l_data[], l_u8 l_len);
#ifdef SUPPORT_TWO_BAUDRATES
extern void l_hal_reset_to_high_baudrate(void);
#endif /* #ifdef SUPPORT_TWO_BAUDRATES */
void l_hal_txchecksum_calculation(void);
void l_hal_rx_state_machine(void);
void lin_hal_ISR(void);
void lin_hal_tx_wake_up(void);
void lin_hal_init_uart(void);
#ifdef SUPPORT_TWO_BAUDRATES
void lin_hal_auto_baud(void);
#endif /* end #ifdef SUPPORT_TWO_BAUDRATES */
#if (defined J2602_PROTOCOL)
#ifdef GM_SPEC_ADAPTION
void lin_hal_set_resp_error (l_u8 j2602_fault);
#else
static void lin_hal_set_resp_error (l_u8 j2602_fault);
#endif /* #ifdef GM_SPEC_ADAPTION */
#else
/* Prototype for LIN protocols */
void lin_hal_set_resp_error (void);
#endif /* end #if defined J2602_PROTOCOL */
/* ===========================================================================
 *  Function Macros
 * ==========================================================================*/

#define l_hal_cyclic_call()   do { if(lin_hal_is_byte_received())                                                  \
                              { lin_hal_reset_byte_received();                                                     \
                                l_hal_rx_state_machine();}                                                         \
                                if (l_txchk_byte_nr > 0u)                                                          \
                                { l_hal_txchecksum_calculation(); } }while(0)

#define l_autobaud_control()  do { if(lin_timeout_ctrl.flag.autobaud != 0u)                                        \
                              { lin_ab_timeout_timer++;                                                            \
                                if(lin_ab_timeout_timer >= LIN_AUTOBAUD_TIMEOUT)                                   \
                                { lin_slave_state_g = idle;                                                        \
                                  lin_hal_init_uart();                                                             \
                                  lin_timeout_ctrl.flag.autobaud = 0u;                                             \
                                  lin_ab_timeout_timer = 0u; } } } while(0)

#if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1
#define l_frame_timeout_control()  do { if(lin_timeout_ctrl.flag.frame != 0u)                                      \
                                   { lin_frm_timeout_timer++;                                                      \
                                     if(lin_frm_timeout_timer >= LIN_FRAME_TIMEOUT)                                \
                                     { lin_slave_state_g = idle;                                                   \
                                       lin_hal_init_uart();                                                        \
                                       lin_hal_set_resp_error();                                                   \
                                       lin_timeout_ctrl.flag.frame = 0u;                                           \
                                       lin_frm_timeout_timer = 0u; } } } while (0)
#else
#define l_frame_timeout_control()   do {  if(lin_timeout_ctrl.flag.frame != 0u)                                    \
                                                { lin_frm_timeout_timer++;                                         \
                                                if(lin_frm_timeout_timer >= LIN_FRAME_TIMEOUT)                     \
                                                   { lin_slave_state_g = idle;                                     \
                                                   lin_hal_init_uart();                                            \
                                                   lin_timeout_ctrl.flag.frame = 0u;                               \
                                                   lin_frm_timeout_timer = 0u; } } } while (0)
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined LIN_PROTOCOL_VERSION_2_1 */

#define l_hal_baudrate_control()   do { } while (0u)
/* ===========================================================================
 * Following Macros are not used in this package, empty macros created
 * Necessary due to improvement of protocol layer for different derivatives
 * ==========================================================================*/

#ifdef AUTOBAUD_ENABLED
#define lin_hal_autobaud_calc_inpcapt()         do{ }while(0)
#ifdef BAUDRATESYNCH_WITH_PCINT
#define lin_hal_autobaud_calc_pcint()           do{ }while(0)
#endif /* end #ifdef BAUDRATESYNCH_WITH_PCINT */
#ifdef BAUDRATESYNCH_WITH_EXTINT
#define lin_hal_autobaud_calc_extint()          do{ }while(0)
#endif /* end #ifdef BAUDRATESYNCH_WITH_EXTINT */
#if defined(BAUDRATESYNCH_TIMER0) || defined(BAUDRATESYNCH_TIMER2)
#define lin_hal_count_timeroverflows()          do{ }while(0)
#endif /* end #if defined(BAUDRATESYNCH_TIMER0) || defined(BAUDRATESYNCH_TIMER2) */
#endif /* end #ifdef AUTOBAUD_ENABLED */

/* ===========================================================================
 *  Variables
 * ==========================================================================*/

extern LIN_hal_stat_reg_t        lin_hal_status_g;
extern t_lin_temp_frame_buffer   lin_temp_buffer;
extern LIN_SLAVE_STATE_t         lin_slave_state_g;
extern l_u8                      lin_ab_timeout_timer;
extern l_u8                      lin_frm_timeout_timer;
extern t_lin_timeout_ctrl        lin_timeout_ctrl;
extern l_u8                      l_txchk_byte_nr;
extern l_u8 LinDataSendThroughFlag;
extern t_lin_temp_frame_buffer linsendthroughbuffer;
#ifdef SUPPORT_TWO_BAUDRATES
extern LIN_AUTOBAUD_FLAGS_t      lin_autobaud_flags_g;
#endif /* #ifdef SUPPORT_TWO_BAUDRATES */

#endif /* end #ifndef LIN_HW_DRIVER_H */

