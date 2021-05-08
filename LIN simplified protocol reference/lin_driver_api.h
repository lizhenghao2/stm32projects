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
********************     Workfile:      lin_driver_api.h **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All defines for lin_driver_api.c                     *
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
@file   lin_driver_api.h
@brief   Header file for lin_driver_api.c
*/

#ifndef LIN_DRIVER_API_H /* to interpret header file only once */
#define LIN_DRIVER_API_H

#include "genLinConfig.h"
#include "lin_type.h"
#include "lin_main.h"
#include "lin_hal.h"
#if ((defined LIN_MASTER)||(defined J2602_MASTER))
#include "lin_master_task.h"
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

#ifdef LIN_COOKED_API
   #include "lin_diag_tp.h"
#endif /* end ifdef LIN_COOKED_API */

#ifdef LIN_RAW_API
   #include "lin_diag_raw.h"
#endif /* end ifdef LIN_RAW_API */

#ifndef LIN_DIAG_ENABLE
   #include "lin_config.h"
#endif /* end #ifndef LIN_DIAG_ENABLE */

/* ===========================================================================
 *  Constants
 * ==========================================================================*/

#define LD_READ_OK                0u
#define LD_LENGTH_TOO_SHORT       1u

#define LD_SET_OK                 0u
#define LD_LENGTH_NOT_CORRECT     1u
#define LD_DATA_ERROR             2u

#ifdef LIN_ENABLE_RBI_USER_DEFINED
/* for the ld_read_by_id_callout */
#define LD_NEGATIVE_RESPONSE      0u
#define LD_POSITIVE_RESPONSE      1u
#define LD_NO_RESPONSE            0xFFu
#endif /* end #ifdef LIN_ENABLE_RBI_USER_DEFINED */

#ifdef J2602_SLAVE
#define LD_RESPONSE               1u
#endif /* end #ifdef J2602_SLAVE */

#ifdef LIN_COOKED_API
/* The reception or transmission is not yet completed. */
#define LD_IN_PROGRESS            0u
/*The reception or transmission has completed successfully. */
#define LD_COMPLETED              1u
/* The reception or transmission ended in an error. */
#define LD_FAILED                 2u
/* The reception failed because of a N_Cr timeout. */
#define LD_N_CR_TIMEOUT           3u
/* The reception failed because of an unexpected  sequence number. */
#define LD_WRONG_SN               4u
/* The transmission failed because of a N_As timeout. */
#define LD_N_AS_TIMEOUT           5u
#endif /* end ifdef LIN_COOKED_API */

#ifdef LIN_RAW_API
/* The transmit queue is empty. */
#define LD_QUEUE_EMPTY            0u
/* The transmit queue has entries but is not full */
#define LD_QUEUE_AVAILABLE        1u
/* The transmit queue is full */
#define LD_QUEUE_FULL             2u
/* A transmission error occurred */
#define LD_TRANSMIT_ERROR         3u
/* The receive queue is empty */
#define LD_NO_DATA                0u
/* The receive queue contains data */
#define LD_DATA_AVAILABLE         1u
/* A receive error occurred */
#define LD_RECEIVE_ERROR          2u
#endif /* end #ifdef LIN_RAW_API */

#ifndef LIN_PROTOCOL_VERSION_1_3
/**
  @brief    request/response combination has been completed
  @note       Initial value.
  @par
              Node configuration: The request/response combination has been
              completed, i.e. the response is valid and may be analyzed.
  @par
              Diagnostics: This master node handles the schedule tables
              switch upon a diagnostic call itself. Refer to datasheet
              for more information.
  @see        LIN-Specification 2.0 and 2.1,
              LD-Pack_<controller>_<driverdate>_DS.pdf
*/
#define LD_SERVICE_IDLE      0u

/**
  @brief    request/response combination is ongoing
  @note       Node configuration: The service is ongoing.
  @par
              Diagnostics: This master node handles the schedule tables
              switch upon a diagnostic call itself. Refer to datasheet
              for more information.
  @see        LIN-Specification 2.0 and 2.1,
              LD-Pack_<controller>_<driverdate>_DS.pdf
*/
#define LD_SERVICE_BUSY      1u

/**
  @brief    request/response combination is ongoing
  @note       Node configuration: The service is ongoing.
  @par
              Diagnostics: This master node handles the schedule tables
              switch upon a diagnostic call itself. Refer to datasheet
              for more information.
  @see        LIN-Specification 2.0 and 2.1,
              LD-Pack_<controller>_<driverdate>_DS.pdf
*/
#define LD_REQUEST_FINISHED    2u

/**
  @brief    request/response combination experienced an error
  @note       Node configuration: The service experienced an error on
              the bus. A negative response will not result in an error.
  @par
              Diagnostics: This master node handles the schedule tables
              switch upon a diagnostic call itself. Refer to datasheet
              for more information.
  @see        LIN-Specification 2.0 and 2.1,
              LD-Pack_<controller>_<driverdate>_DS.pdf
*/
#define LD_SERVICE_ERROR    3u

/**
  @brief    internally used value
*/
#define LD_RESPONSE_FINISHED  4u

#endif /* ifndef LIN_PROTOCOL_VERSION_1_3 */

/* ===========================================================================
 *  Variables
 * ==========================================================================*/

#ifdef J2602_SLAVE
#ifndef GM_SPEC_ADAPTION
extern l_u8 j2602_statusbyte;
#endif /* #ifndef GM_SPEC_ADAPTION */
#endif /* #ifdef J2602_SLAVE */

/* ===========================================================================
 *  Prototypes
 * ==========================================================================*/
/* ===========================================================================
 *  Core API specified by LIN 1.3 / 2.x
 * ==========================================================================*/

/**
   @brief      Initialization of the LIN core.
   @par        l_sys_init performs the initialization of the LIN core. The scope of the initialization is
               the physical node (i.e. the complete node).
               The call to the l_sys_init is the first call a user must use in the LIN core before using
               any other API functions.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        MCU started, oscillator stable
   @param      void     No parameters
   @retval     Zero     Initialization succeeded.
   @retval     Non-Zero Initialization failed (not supported).
*/
l_bool l_sys_init (void);

/**
   @brief      Transmission of one Wake Up pulse.
   @par        The function will transmit one wake up signal. The wake up signal will be transmitted
               directly when this function is called. It is the responsibility of the application to
               retransmit the wake up signal according to the wake up sequence.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      void     No parameters
   @retval     void     No return values
*/
#define l_ifc_wake_up()                     (lin_hal_tx_wake_up())

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
/**
   @brief      Transmission of a Goto Sleep command.
   @par        This call requests slave nodes on the cluster connected to the interface to enter bus
               sleep mode by issuing one go to sleep command.
               The go to sleep command will be scheduled latest when the next schedule entry is
               due.
               The l_ifc_goto_sleep will not affect the power mode. It is up to the application to do
               this.
               If the go to sleep command was successfully transmitted on the cluster the go to sleep
               bit will be set in the status register.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      void     No parameters
   @retval     void     No return values
*/
void l_ifc_goto_sleep(void);
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

/**
   @brief      ISR for LIN driver (receive).
   @par        The application program is responsible for binding the interrupt and for
               setting the correct interface handle (if interrupt is used).
               For UART based implementations it may be called from a user-defined
               interrupt handler triggered by a UART when it receives one character
               of data. In this case the function will perform necessary operations on
               the UART control registers.
               For more complex LIN hardware it may be used to indicate the reception
               of a complete frame.
   @see        LIN specification (LIN 1.3, 2.x)
   @note       For allowed interrupt latencies check the capabilities of the used
               communication interface.
   @pre        LIN driver initialized.
   @param      void     No parameters
   @retval     void     No return values
*/
#define l_ifc_rx()                          (lin_hal_ISR())

/**
   @brief      Read out statusword to get state of communication.
   @par        This function will return the status of the previous communication. The call returns the
               status word (16 bit value).
               Bit 15 is MSB, bit 0 is LSB:
               8 - 15: Last frame PID
               7:
               6: Save configuration
               5: Event triggered frame collision
               4: Bus activity
               3: Go to sleep
               2: Overrun
               1: Successful transfer
               0: Error in response
               The status word is only set based on a frame transmitted or received by the node
               (except bus activity).
               The call is a read-reset call; meaning that after the call has returned, the status word is
               set to 0.
               In the master node the status word will be updated in the l_sch_tick function. In the
               slave node the status word is updated latest when the next frame is started.
   @par        Error in response is set if a frame error is detected in the frame response, e.g.
               checksum error, framing error, etc. An error in the header results in the header not
               being recognized and thus, the frame is ignored. It will not be set if there was no
               response on a received frame. Also, it will not be set if there is an error in the
               response (collision) of an event triggered frame.
   @par        Successful transfer is set if a frame has been transmitted/received without an error.
   @par        Overrun is set if two or more frames are  processed since the previous call to
               l_ifc_read_status. If this is the case, error in response and successful transfer
               represent logical ORed values for all processed frames.
   @par        Go to sleep is set in a slave node if a go to sleep command has been received, and
               set in a master node when the go to sleep command is successfully transmitted on the
               bus. After receiving the go to sleep command the power mode will not be affected.
               This must be done in the application.
   @par        Bus activity will be set if the node has detected bus activity on the bus. See
               LIN specification for definition of bus activity. A slave node is required to enter bus sleep
               mode after a period of bus inactivity on the bus. This can be implemented by the
               application monitoring the bus activity. Note the difference between bus
               activity and bus inactivity.
   @par        Event triggered frame collision is set as long the collision resolving schedule is
               executed. The intention is to use it in parallel with the return value from l_sch_tick. In the
               slave, this bit will always be 0 (zero). If the master node application switches schedule
               table during the collision is resolved the event triggered frame collision flag will be set
               to 0 (zero). See example below how this flag is set.
   @par        Save configuration is set when the save configuration request has been successfully received.
               It is set only in the slave node, in the master node it is always 0 (zero).
   @par        Last frame PIDis the PID last detected on the bus and processed in the node. If overrun is
               set one or more values of last frame PID are lost; only the latest value is maintained. It
               is set simultaneously with successful transfer or error in response.
               It is the responsibility of the node application to process the individual status reports.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      void           No parameters
   @retval     Statusbits     See description above.
*/
l_u16 l_ifc_read_status(void);

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
l_u8 l_sch_tick (void);

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
void l_sch_set (l_u16 schedule, l_u8 entry);
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

/* The LIN specifications require several routines that are not used in this single instance LIN driver. */
#ifdef LIN_DRIVER_NOT_USED_API_ROUTINES
/**
   @brief      Interface operations on LIN interface.
   @par        l_ifc_init initializes the controller specified by the name iii, i.e. sets up internal functions
               such as the baud rate. The default schedule set by the l_ifc_init call will be the
               L_NULL_SCHEDULE where no frames will be sent and received.
               This is the first call a user must perform, before using any other interface related LIN
               API functions.
               The function returns zero if the initialization was successful and non-zero if failed.
   @par        iii enhancement of function prototype not used because this is a single instance LIN driver.
   @par        Function is not used in this driver package.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        l_sys_init called.
   @param      void     No parameters
   @retval     Zero     Initialization succeeded.
   @retval     Non-Zero Initialization failed.
*/
l_bool l_ifc_init (void);

/**
   @brief      Interface operations on LIN interface.
   @par        This function controls functionality that is not covered by the other API calls.
               It is used for protocol specific parameters or hardware specific functionality.
               Example of such functionality can be to switch on/off the wake up signal detection.
               The iii is the name of the interface to which the operation defined in op should be
               applied. The pointer pv points to an optional parameter that may be provided to the
               function. Exactly which operations that are supported are implementation dependent.
   @par        iii enhancement of function prototype not used because this is a single instance LIN driver.
   @par        Function is not used in this driver package.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized
   @param[in]  op       Operation - implementation dependent.
   @param[in]  pv       Additional parameters
   @retval     0-65535  State of operation
*/
l_u16 l_ifc_ioctl (l_ioctl_op op, void* pv);

/**
   @brief      ISR for LIN driver (transmit).
   @par        The application program is responsible for binding the interrupt and for setting
               the correct interface handle (if interrupt is used). For UART based implementations
               it may be called from a user-defined interrupt handler triggered by a UART when
               it has transmitted one character of data. In this case the function will perform
               necessary operations on the UART control registers.
               For more complex LIN hardware it may be used to indicate the transmission of a complete frame.
   @par        Function is not used in this driver package.
   @see        LIN specification (LIN 1.3, 2.x)
   @note       For allowed interrupt latencies check the capabilities of the used
               communication interface.
   @pre        LIN driver initialized.
   @param      void     No parameters
   @retval     void     No return values
*/
void l_ifc_tx(void);

/**
   @brief      ISR for LIN driver (break / sync).
   @par        This function may be used in the slave nodes to synchronize to the break/sync field
               sequence transmitted by the master node on the interface specified by iii.
               It may, for example, be called from a user-defined interrupt handler raised upon a
               flank detection on a hardware pin connected to the interface iii.
               l_ifc_aux may only be used in a slave node.
               This function is strongly hardware connected and the exact implementation and usage
               is implementation dependent.
               This function might even be empty in cases where the break/sync field sequence
               detection is implemented in the l_ifc_rx function.
   @par        iii enhancement of function prototype not used because this is a single instance LIN driver.
   @par        Function is not used in this driver package.
   @see        LIN specification (LIN 1.3, 2.x)
   @note       For allowed interrupt latencies check the capabilities of the used
               communication interface.
   @pre        LIN driver initialized.
   @param      void     No parameters
   @retval     void     No return values
*/
void l_ifc_aux(void);
#endif /* #ifdef LIN_DRIVER_NOT_USED_API_ROUTINES */

/* ===========================================================================
 *  User provided call-outs, specified by LIN 1.3 / 2.x
 * ==========================================================================*/

/**
   @brief      Disable interrupts to ensure atomic operations on data for LIN driver.
   @par        The user implementation of this function must achieve a state in which no interrupts
               from the LIN communication can occur.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      void       No parameters
   @retval     l_irqmask  Save value for disabled interrupts.
*/
l_irqmask l_sys_irq_disable (void);

/**
   @brief      Enable previously disabled interrupts for LIN driver.
   @par        The user implementation of this function must restore the interrupt level identified by
               the provided l_irqmask previous.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        l_sys_irq_disable used previously.
   @param      mask       Stored value to identify interrupts which have to be enabled.
   @retval     void       No return values
*/
void l_sys_irq_restore (l_irqmask mask);

/* ===========================================================================
 *  API for Node configuration and identification
 * ==========================================================================*/

#ifndef LIN_PROTOCOL_VERSION_1_3
#if ((defined LIN_MASTER)||(defined J2602_MASTER))
/**
   @brief      Check state of node configuration service
   @par        This call returns the status of the last requested configuration service.
               The return values are interpreted as follows:
               LD_SERVICE_BUSY Service is ongoing.
               LD_REQUEST_FINISHED The configuration request has been completed. This is a
                  intermediate status between the configuration request and configuration response.
               LD_SERVICE_IDLE The configuration request/response combination has
                  been completed, i.e. the response is valid and may be analyzed. Also, this
                  value isreturned if no request has yet been called.
               LD_SERVICE_ERROR The configuration request or response experienced an
                  error. Error here means error on the bus, and not a negative configuration
                  response from the slave node.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      RSID                 Response Code
   @param      error_code           Returns a code containing error identification if available
   @retval     LD_SERVICE_BUSY      Service is ongoing.
   @retval     LD_REQUEST_FINISHED  The configuration request has been completed.
   @retval     LD_SERVICE_IDLE      The configuration request/response combination has been completed.
   @retval     LD_SERVICE_ERROR     The configuration request or response experienced an error.
*/
l_u8 ld_is_ready (void);

/**
   @brief      Check response of a node configuration service
   @par        This call returns the result of the last node configuration service, in the parameters
               RSID and error_code. A value in RSID is always returned but not always in the
               error_code. Default values for RSID and error_code is 0 (zero).
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      RSID                 Response Code
   @param      error_code           Returns a code containing error identification if available
   @retval     void                 No return value.
*/
void ld_check_response (l_u8* RSID,l_u8* error_code);

/**
   @brief      Read by ID (B2)
   @par        The call requests the slave node selected with the NAD to return the property
               associated with the id parameter, see Table 4.19in the Node configuration and
               Identification Specification, for interpretation of the id. When the next call
               to ld_is_ready returns LD_SERVICE_IDLE (after the ld_read_by_id is called),
               the RAM area specified by data contains between one and five bytes data
               according to the request. The result is returned in a big-endian style.
               It is up to little-endian CPUs to swap the bytes, not the LIN diagnostic
               driver. The reason for using big-endian data is to simplify message routing
               to a (e.g. CAN) back-bone network.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      NAD                  Node Address of Slave
   @param      supplier_id          16Bit Supplier ID of Slave
   @param      function_id          16Bit Function ID of Slave
   @param      id                   ID from read by identifier table (row)
   @param      data                 pointer to a buffer to get response data.
   @retval     void                 No return value.
*/
void ld_read_by_id (l_u8 NAD,l_u16 supplier_id,l_u16 function_id,l_u8 id,l_u8* const data);

#ifdef LIN_ENABLE_ASSIGN_NAD
/**
   @brief      Assign NAD (B0)
   @par        This call assigns the NAD (node diagnostic address) of all slave nodes that matches
               the initial_NAD, the supplier ID and the function ID. The new NAD of the slave node
               will be new_NAD.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      initial_nad          (Initial) Node Address of Slave
   @param      supplier_id          16Bit Supplier ID of Slave
   @param      function_id          16Bit Function ID of Slave
   @param      new_NAD              New configured Node Address of Slave
   @retval     void                 No return value.
*/
void ld_assign_NAD (l_u8 initial_nad,l_u16 supplier_id,l_u16 function_id,l_u8 new_NAD);
#endif /* #ifdef LIN_ENABLE_ASSIGN_NAD */


#ifdef LIN_ENABLE_ASSIGN_FRAME_ID_RANGE
/**
   @brief      Assign Frame ID Range (B7)
   @par        This call assigns the protected identifier of up to four frames in the slave node with the
               addressed NAD. The PIDs parameter shall be four bytes long, each byte shall contain
               a PID, do not care or unassign value.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      NAD                  Node Address of Slave
   @param      start_index          Start index in the frame table
   @param      PIDs                 Array containing new PIDs
   @retval     void                 No return value.
*/
void ld_assign_frame_id_range(l_u8 NAD, l_u8 start_index, const l_u8* const PIDs);
#endif /* #ifdef LIN_ENABLE_ASSIGN_FRAME_ID_RANGE */

#ifdef LIN_ENABLE_ASSIGN_FRAME_ID
/**
   @brief      Assign Frame ID (B1)
   @par        This call assigns the protected identifier ofa frame in the slave node with the address
               NAD and the specified supplier ID. The frame changed shall have the specified message
               ID and will after the call have PID as the the protected identifier.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      NAD                  Node Address of Slave
   @param      SUPP_ID              16Bit Supplier ID of Slave
   @param      MSG_ID               16Bit Message ID of Frame
   @param      start_index          Start index in the frame table
   @param      PID                  New PID
   @retval     void                 No return value.
*/
void ld_assign_frame_id(l_u8 NAD,l_u16 SUPP_ID,l_u16 MSG_ID,l_u8 PID);
#endif /* #ifdef LIN_ENABLE_ASSIGN_FRAME_ID */

#ifdef  LIN_ENABLE_COND_CHG_NAD
/**
   @brief      Conditional Change NAD (B3)
   @par        This call changes the NAD if the node properties fulfil the test specified by id, byte,
               mask and invert.
               Id shall be in the range 0 to 31, see Table 4.20, and byte in the range 1 to 5 (specifying
               the byte to use in the id). Mask and Invert shall have values between 0 and 255.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      NAD                  Node Address of Slave
   @param      id                   ID for service (same table valid as for Read by ID)
   @param      byte                 Selected data byte
   @param      mask                 Mask for bitwise AND
   @param      invert               Mask for bitwise XOR
   @param      new_NAD              New NAD for slave
   @retval     void                 No return value.
*/
void ld_conditional_change_NAD (l_u8 NAD, l_u8 id, l_u8 byte, l_u8 mask, l_u8 invert, l_u8 new_NAD);
#endif /* #ifdef  LIN_ENABLE_COND_CHG_NAD */

#ifdef LIN_ENABLE_SAVE_CONFIG
/**
   @brief      Save Configuration (B6)
   @par        This call will make a save configuration request to a specific slave node with the given
               NAD, or to all slave nodes if NAD is set to broadcast.
   @see        LIN specification (2.1 and newer)
   @pre        LIN driver initialized.
   @param      NAD                  Node Address of Slave
   @retval     void                 No return value.
*/
void ld_save_configuration (l_u8 NAD);
#endif /* #ifdef LIN_ENABLE_SAVE_CONFIG */
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
#endif /* #ifndef LIN_PROTOCOL_VERSION_1_3 */

/* The LIN specifications allows this ld_read_by_id_callout as optional, to ensure
 * better adaptability to the application the callout was implemented in a different
 * way for possible configurations (see below). */
#ifdef LIN_DRIVER_NOT_USED_API_ROUTINES
/**
   @brief      Read by ID (B2) - User defined identifiers (IDs 32-63).
   @par        This callout is used when the master node transmits a read by identifier request with
               an identifier in the user defined area. The slave node application will be called from
               the driver when such request is received.
               The id parameter is the identifier in the user defined area (32 to 63), from the read by
               identifier configuration request.
               The data pointer points to a data area with 5 bytes. This area will be used by the
               application to set up the positive response,
   @par        iii enhancement of function prototype not used because this is a single instance LIN driver.
   @par        Function is not used in this driver package.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param[in]  id                    Identifier received by B2 service.
   @param[out] data                  Pointer to data area with 5 data bytes.
   @retval     LD_NEGATIVE_RESPONSE  Send negative response
   @retval     LD_POSITIVE_RESPONSE  Send positive response
   @retval     LD_NO_RESPONSE        Don't send a response
*/
l_u8 ld_read_by_id_callout (l_ifc_handle iii, l_u8 id, l_u8* data);
#endif /* #ifdef LIN_DRIVER_NOT_USED_API_ROUTINES */

#if ((defined(LIN_SLAVE)) || (defined(J2602_SLAVE)))
#ifdef LIN_ENABLE_RBI_RD_ECU_SN
/**
   @brief      Read by ID (B2) - Serial Number (ID 1).
   @par        This callout is used when the master node transmits a read by identifier request with
               an identifier 1. The slave node application will be called from the driver when such a
               request is received.
               The data pointer points to a data area with 4 bytes because the length is fixed to
               PCI of 5 (RSID + data).
   @note       This is part of the implementation specific adaption for the read by identifier service.
   @pre        LIN driver initialized.
   @param[out] SerialnumberResult    Pointer to data area with 4 data bytes.
   @retval     void                  No return value.
*/
void l_callback_RBI_ReadEcuSerialNumber(l_u8 SerialnumberResult[]);
#endif /* end #ifdef LIN_ENABLE_RBI_RD_ECU_SN */

#ifdef LIN_ENABLE_RBI_USER_DEFINED
/**
   @brief      Read by ID (B2) - User defined identifiers (IDs 32-63).
   @par        This callout is used when the master node transmits a read by identifier request with
               an identifier in the user defined area. The slave node application will be
               called from the driver when such a request is received.
               The data pointer points to a data area with 5 bytes which is the max. allowed amount of
               data bytes. The PCI is fixed to 6 except if an additional define is added to the
               genLinConfig.h file (LIN_RBI_UDEF_2_2) then the return value has to be the number of
               data bytes (the remaining bytes will be filled with 0xFF).
   @note       This is part of the implementation specific adaption for the read by identifier service.
   @pre        LIN driver initialized.
   @param[in]  id                    Identifier received by B2 service.
   @param[out] UserDefinedResult     Pointer to data area with 5 data bytes.
   @note       Return values without LIN_RBI_UDEF_2_2
   @retval     LD_NEGATIVE_RESPONSE  Send negative response.
   @retval     LD_POSITIVE_RESPONSE  Send positive response with PCI 6.
   @retval     LD_NO_RESPONSE        Send no response (all return values > 5).
   @note       Return values with LIN_RBI_UDEF_2_2
   @retval     LD_NEGATIVE_RESPONSE  Send negative response.
   @retval     1                     Send positive response with PCI 2 (1 data byte).
   @retval     2                     Send positive response with PCI 3 (2 data byte).
   @retval     3                     Send positive response with PCI 4 (3 data byte).
   @retval     4                     Send positive response with PCI 5 (4 data byte).
   @retval     5                     Send positive response with PCI 6 (5 data byte).
   @retval     LD_NO_RESPONSE        Send no response (all return values > 5).
*/
l_u8 l_callback_RBI_UserDefined(l_u8 id,l_u8 UserDefinedResult[]);
#endif /* end #ifdef LIN_ENABLE_RBI_USER_DEFINED */

#ifdef    LIN_ENABLE_DATA_DUMP
/**
   @brief      Data Dump (B4) received.
   @par        This callout is used when the master node transmits a data dump request. The slave node
               application will be called from the driver when such a request is received.
               The data pointer points to a data area with 5 bytes which is the max. allowed amount of
               data bytes. The PCI is fixed to 6. All bytes have to be filled by the application.
   @note       This is part of the implementation specific adaption for the data dump service.
   @pre        LIN driver initialized.
   @param[in]  ReceivedData          Pointer to data area with 5 data bytes (request data).
   @param[out] TransmitData          Pointer to data area with 5 data bytes (response data).
   @retval     void                  No return value.
*/
void l_callback_DataDump(const l_u8 ReceivedData[],l_u8 TransmitData[]);
#endif /* end #ifdef    LIN_ENABLE_DATA_DUMP */

#ifdef LIN_ASSIGN_NAD_CALLBACK
/**
   @brief      Callback when NAD is changed by Assign NAD service
   @par        This function is called when the node receives an Assign NAD request (0xB0). The function will
               provide the old configured NAD and the new configured NAD for use by the application.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      old_NAD              Old Node Address of Slave
   @param      new_NAD              New configured Node Address sent by request frame
   @retval     void                 No return value.
*/
void l_callback_assign_NAD (const l_u8 old_NAD, const l_u8 new_NAD);
#endif /* #ifdef LIN_ASSIGN_NAD_CALLBACK */

#ifdef LIN_ASSIGN_FRAME_ID_CALLBACK
/**
   @brief      Callback when a frame identifier is changed by Assign FID service
   @par        his function is called when the node receives an Assign FID request (0xB1).
               The function will provide the old configured FID and the new configured FID
               as well as the index of the frame in the frame array (for use by the application).
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      index                Index of frame in the frame table
   @param      old_PID              Old PID
   @param      new_PID              New PID
   @retval     void                 No return value.
*/
void l_callback_assign_frame_id(const l_u8 index,const l_u8 old_PID,l_u8 new_PID);
#endif /* #ifdef LIN_ASSIGN_FRAME_ID_CALLBACK */

#ifdef LIN_PROTOCOL_VERSION_2_1
/**
   @brief      Read out current configuration of LIN node (NAD + PIDs).
   @par        This function will not transport anything on the bus.
   @par        This call will serialize the current configuration and copy it to the area
               (data pointer) provided by the application. The intention is to call this
               function when the save configuration request flag is set in the status
               register. After the call is finished the application is responsible to
               store the data in appropriate memory. The caller shall reserve bytes in
               the data area equal to length, before calling this function.
               The function will set the length parameter to the actual size of the
               configuration.
               In case the data area is too short the function will return with no action.
               In case the NAD has not been set by a previous call to ld_set_configuration or the
               master node has used the configuration services, the returned NAD will be the initial
               NAD.
               The data contains the NAD and the PIDs and occupies one byte each. The structure
               of the data is: NAD and then all PIDs for the frames. The order of the PIDs are the
               same as the frame list in the LDF.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param[in]  data                  Length of buffer.
   @param[out] data                  Actual length of configuration.
   @param[out] length                Actual configuration data (NAD first, then PIDs).
   @retval     LD_READ_OK            Configuration read out successful, data contains valid information
   @retval     LD_LENGTH_TOO_SHORT   Configuration read out not successful, data contains no valid information
*/
l_u8 ld_read_configuration (l_u8* const data, l_u8* const length);

/**
   @brief      Set current configuration of LIN node (NAD + PIDs).
   @par        This function will not transport anything on the bus.
   @par        The function will configure the NAD and the PIDs according to the configuration given
               by data. The intended usage is to restore a saved configuration or set an initial
               configuration (e.g. coded by I/O pins). The function shall be called after calling l_ifc_init.
               The caller shall set the size of the data area before calling the function.
               The data contains the NAD and the PIDs and occupies one byte each. The structure
               of the data is: NAD and then all PIDs for the frames. The order of the PIDs are the
               same as the frame list in the LDF.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param[in]  data                   Buffer containing the new configuration data (NAD + PIDs)
   @param[in]  length                 Length of configuration data (length has to match the actual configuration data)
   @retval     LD_SET_OK              Configuration set successfully
   @retval     LD_LENGTH_NOT_CORRECT  Length of configuration data does not match the current configuration
   @retval     LD_DATA_ERROR          Configuration could not be set (not used).
*/
l_u8 ld_set_configuration (const l_u8* const data, l_u8* const length);

#ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK
/**
   @brief      Callback when a frame identifier is changed by Assign FIDR service
   @par        his function is called when the node receives an Assign FIDR request (0xB7).
               The function will provide the old configured FID and the new configured FIDR
               as well as the index of the frame in the frame array (for use by the application).
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      start_index          Index of first frame to assign a PID
   @param      old_PIDs             Array containing old configured PIDs
   @param      new_PIDs             Array containing new configured PIDs
   @retval     void                 No return value.
*/
void l_callback_assign_frame_id_range(const l_u8 start_index, const l_u8 old_PIDs[], const l_u8 new_PIDs[]);
#endif /* #ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK */

#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 */
#endif /* end #if ((defined(LIN_SLAVE)) || (defined(J2602_SLAVE))) */

/* ===========================================================================
 *  API for transport layer
 * ==========================================================================*/

#ifdef LIN_DIAG_ENABLE
#ifdef LIN_RAW_API
/**
   @brief      Put one diagnostic frame into queue for transmission.
   @par        The call queues the transmission of 8 bytes of data in one frame.
               The data is sent in the next suitable frame (master request frame for master nodes
               and slave response frame for slave nodes).
               The data area will be copied in the call, the pointer will not be memorized.
               If no more queue resources are available,the data may be jettisoned and the appropriate
               error status will be set.
   @note       Check availability of queue space with ld_raw_tx_status
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param[in]  data                   Buffer containing a complete diagnostic frame correctly formatted.
   @retval     void                   No return value.
*/
void ld_put_raw(const l_u8* const data);

/**
   @brief      Get one diagnostic frame from queue after reception.
   @par        The call copies the oldest received diagnostic frame data to the memory specified by
               data.
               The data returned is received from master request frame for slave nodes and slave
               response frame for master nodes.
               If the receive queue is empty no data will be copied.
   @note       Check availability of queue data with ld_raw_rx_status
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param[out] data                   Buffer containing a complete diagnostic frame.
   @retval     void                   No return value.
*/
void ld_get_raw(l_u8* const data);

/**
   @brief      Status of queue for transmitting diagnostic data.
   @par        The call returns the status of the raw frame transmission function
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     LD_QUEUE_EMPTY         Queue empty, all frames already transmitted.
   @retval     LD_QUEUE_AVAILABLE     Queue not empty, some frames are due for transmission.
   @retval     LD_QUEUE_FULL          Queue full, frames are due for transmission.
   @retval     LD_TRANSMIT_ERROR      Errors occurred during transmission, re-initialize and restart transmission.
*/
l_u8 ld_raw_tx_status(void);

/**
   @brief      Status of queue for receiving diagnostic data.
   @par        The call returns the status of the raw frame receive function
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     LD_NO_DATA             Queue empty, no frames received yet.
   @retval     LD_DATA_AVAILABLE      Queue not empty, some frames have been received.
   @retval     LD_RECEIVE_ERROR       Errors occurred during reception, re-initialize and wait for new requests.
*/
l_u8 ld_raw_rx_status(void);


/**
   @brief      Initialize diagnostic transport layer.
   @par        This call will (re)initialize the raw and the cooked layers on the interface iii.
               All transport layer buffers will be initialized.
               If there is an ongoing diagnostic frame transporting a cooked or raw message on the
               bus, it will not be aborted.
   @par        iii enhancement of function prototype not used because this is a single instance LIN driver.
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     void                   No return value.
*/
#define ld_init()                  (lin_diag_raw_init())

#define ld_allow_response() do{ ld_put_raw(&LIN_DIAG_SEND_FRAME.frame.NAD); } while (0u)
#endif /* end #ifdef LIN_RAW_API */

#ifdef LIN_COOKED_API
/**
   @brief      Send one diagnostic message.
   @par        The call packs the information specified by data and length into one or multiple
               diagnostic frames. If the call is made in a master node application the frames are
               transmitted to the slave node with the address NAD. If the call is made in a slave node
               application the frames are transmitted to the master node with the address NAD. The
               parameter NAD is not used in slave nodes.
               The value of the SID (or RSID) shall be the first byte in the data area.
               Length must be in the range of 1 to 4095 bytes. The length shall also include the SID
               (or RSID) value, i.e. message length plus one.
               The call is asynchronous, i.e. not suspended until the message has been sent, and
               the buffer may not be changed by the application as long as calls to ld_tx_status
               returns LD_IN_PROGRESS.
               The data is transmitted in suitable frames (master request frame for master nodes and
               slave response frame for slave nodes).
               If there is a message in progress, the call will return with no action.
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param[in]  length                 Length of diagnostic message incl. SID/RSID
   @param[in]  data                   Buffer containing the complete diagnostic message.
   @retval     void                   No return value.
*/
#if ((defined LIN_MASTER)||(defined J2602_MASTER))
void ld_send_message(l_u16 length, l_u8 NAD, const l_u8* const data);
#else
void ld_send_message(l_u16 length, const l_u8* const data);
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER))*/

/**
   @brief      Send one diagnostic message.
   @par        The call prepares the LIN diagnostic module to receive one message and store it in
               the buffer pointed to by data. At the call, length shall specify the maximum length
               allowed. When the reception has completed, length is changed to the actual length
               and NAD to the NAD in the message.
               SID (or RSID) will be the first byte in the data area.
               Length will be in the range of 1 to 4095 bytes, but never more than the value originally
               set in the call. SID (or RSID) is included in the length.
               The parameter NAD is not used in slave nodes.
               The call is asynchronous, i.e. not suspended until the message has been received,
               and the buffer may not be changed by the application as long as calls to ld_rx_status
               returns LD_IN_PROGRESS. If the call is made after the message transmission has
               commenced on the bus (i.e. the SF or FF is already transmitted), this message will not
               be received. Instead the function will wait until next message commence.
               The data is received from the succeeding suitable frames (master request frame for
               slave nodes and slave response frame for master nodes).
               The application shall monitor the ld_rx_status and shall not call this function until the
               status is LD_COMPLETED. Otherwise this function may return inconsistent data in
               the parameters.
   @note       Clarification for the text above: The slave node requires calling this function before
               a master request is received to prepare the internal buffers. Then wait until the status
               return value is set to LD_COMPLETED, then the length and data information can be directly
               accessed by the application.
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param[in]  length                 Length of available buffer
   @param[out] length                 Length of diagnostic message incl. SID/RSID
   @param[out] data                   Buffer containing the complete diagnostic message.
   @retval     void                   No return value.
*/
#if ((defined LIN_MASTER)||(defined J2602_MASTER))
void ld_receive_message(l_u16* const length, l_u8* const NAD, l_u8* const data);
#else
void ld_receive_message(l_u16* const length, l_u8* const data);
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

/**
   @brief      Status of the diagnostic data (transmission).
   @par        The call returns the status of the last made call to ld_send_message.
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     LD_IN_PROGRESS         Transmission ongoing.
   @retval     LD_COMPLETED           Idle state, last transmission successfully finished.
   @retval     LD_FAILED              Last transmission ended in an error, re-initialize and restart transmission.
   @retval     LD_N_AS_TIMEOUT        Last transmission failed because of a timeout, re-initialize and remain idle.
*/
#define ld_tx_status()                (g_lin_tp_stat.tx_stat)

/**
   @brief      Status of the diagnostic data (reception).
   @par        The call returns the status of the last made call to ld_receive_message.
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     LD_IN_PROGRESS         Transmission ongoing.
   @retval     LD_COMPLETED           Idle state, last transmission successfully finished.
   @retval     LD_FAILED              Last transmission ended in an error, re-initialize and prepare for new reception.
   @retval     LD_N_CR_TIMEOUT        Last transmission failed because of a timeout, re-initialize and prepare for new reception.
   @retval     LD_WRONG_SN            Last transmission failed because of a timeout, re-initialize and prepare for new reception.
*/
#define ld_rx_status()                (g_lin_tp_stat.rx_stat)

/**
   @brief      Initialize diagnostic transport layer.
   @par        This call will (re)initialize the raw and the cooked layers on the interface iii.
               All transport layer buffers will be initialized.
               If there is an ongoing diagnostic frame transporting a cooked or raw message on the
               bus, it will not be aborted.
   @par        iii enhancement of function prototype not used because this is a single instance LIN driver.
   @see        LIN specification (LIN 2.x)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     void                   No return value.
*/
#define ld_init()                  (lin_tp_init())

#define ld_allow_response()    do                                                              \
                                        {                                                               \
                                           g_lin_tp_tx_rx_ctrl.tx_on = 1u;                   \
                                           g_lin_tp_tx_rx_ctrl.tx_t_out_on = 1u;             \
                                        } while (0u)
#define ld_response_finished() do                                                              \
                                        {                                                               \
                                           if(g_lin_tp_stat.rx_stat < LD_COMPLETED)          \
                                           {                                                            \
                                              g_lin_tp_stat.rx_stat = LD_COMPLETED;          \
                                           }                                                            \
                                           g_lin_tp_tx_rx_ctrl.rx_cf_on = 0u;                \
                                           g_lin_tp_tx_rx_ctrl.rx_t_out_on = 0u;             \
                                        } while (0u)
#define ld_response_failed()   do                                                              \
                                        {                                                               \
                                        if (g_lin_config_status != LD_SERVICE_IDLE)          \
                                        {                                                               \
                                           g_lin_config_status = LD_SERVICE_ERROR;           \
                                        }                                                               \
                                        g_lin_tp_tx_rx_ctrl.rx_cf_on = 0u;                   \
                                        g_lin_tp_tx_rx_ctrl.rx_t_out_on = 0u;                \
                                        } while (0u)
#endif /* end #ifdef LIN_COOKED_API */
#else
#if (!defined(LIN_USER_DEF_DIAG_ENABLE))
#define ld_allow_response() do{ g_lin_diag_config_active = 0x1u; \
                                         g_lin_tp_timeoutcounter = 0u; } while (0u)
#else
#define ld_allow_response() do{ } while (0u)
#endif /* end #ifndef LIN_USER_DEF_DIAG_ENABLE */
#endif /* end #ifdef LIN_DIAG_ENABLE */
#if ((defined(LIN_USER_DEF_DIAG_ENABLE)) && (defined(LIN_USER_DEF_DIAG_MANDSERV_ENABLE)))
#define ld_allow_userdef_response() do{ g_lin_user_diag_active = 1u; } while (0u)
#else
#define ld_allow_userdef_response() do{ } while (0u)
#endif /* ifdef LIN_USER_DEF_DIAG_ENABLE && LIN_USER_DEF_DIAG_MANDSERV_ENABLE */


/* ===========================================================================
 *  API for transport layer, additional
 * ==========================================================================*/

#ifdef LIN_USER_DEF_DIAG_ENABLE
/**
   @brief      Callback for application specific implementation of transport layer.
   @par        This call allows implementation of a specific implementation of the transport layer.
               All flow control, message allowances lie within responsibility of the application.
               No node configuration services are provided by the LIN driver, if those are necessary
               they have to be implemented by the application (except mandatory services if this is
               activated in the LDC-Tool)
   @pre        LIN driver initialized.
   @param      void                   No parameter.
   @retval     void                   No return value.
*/
void ld_user_define_diagnostic(void);
#endif /* end #ifdef LIN_USER_DEF_DIAG_ENABLE */

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
void lin_handle_config_response(void);
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */

/* ===========================================================================
 *  Additional API for LIN Master
 * ==========================================================================*/

/**
   @brief      Allows changing the PID in the current schedule tables
   @par        If a PID is changed for a slave node the preconfigured PIDs of the schedule
               tables are also adaptible.
   @param      old_pid                Value of old PID (incl. parity bits).
   @param      new_pid                Value of new PID (incl. parity bits).
   @retval     void                   No return value.
*/
void l_sch_change_pid (l_u8 old_pid, l_u8 new_pid);

/* ===========================================================================
 *  User provided call-outs, additional
 * ==========================================================================*/

/**
   @brief      Allows the initialization routine to activate the LIN transceiver.
   @par        The activation is done during the l_sys_init call. If this is not intended,
               the function should be left empty.
   @param      void                   No parameter.
   @retval     void                   No return value.
*/
void LIN_Enable_Transceiver(void);

/**
   @brief      Allows deactivation of the LIN transceiver.
   @par        This call is never used in the LIN driver.
   @param      void                   No parameter.
   @retval     void                   No return value.
*/
void LIN_Disable_Transceiver(void);

/* ===========================================================================
 *  Additional API for J2602 slave node handling
 * ==========================================================================*/

#ifdef J2602_SLAVE
/**
   @brief  Write the Reset Flag
   @note   This is for the driver to set the reset request flag to inform the application about
           a received target or broadcast reset request.
*/
#define l_u8_wr_Reset(x)       do {g_j2602_status.flags.reset=(l_u8)(x);} while(0)

/**
   @brief   Write the Broadcast-frame-received Flag
   @note    This is for the driver to set a flag to inform the application about
            a received broadcast frame where data bytes were defined for the application.
   @par
            This is for Frame ID 38 or 39 (depending on the DNN)
*/
#define l_bool_wr_new_bc_data0()  do {g_j2602_status.flags.new_frame0 = 1u;} while (0)

/**
   @brief   Write the Broadcast-frame-received Flag
   @note    This is for the driver to set a flag to inform the application about
            a received broadcast frame where data bytes were defined for the application.
   @par
            This is for Frame ID 3A or 3B (depending on the DNN)
*/
#define l_bool_wr_new_bc_data1()  do {g_j2602_status.flags.new_frame1 = 1u;} while (0)

/**
   @brief   Returns number of data bytes which is received from broadcast frames
   @note    This is for the application to check, how many bytes the driver extracted from
            the received broadcast frame (depending on the number of Frames which are assigned
            to the slave)
*/
#define l_get_nr_databytes()      (J2602_DNN_NUMBER_OF_BYTES)

/**
   @brief   Set the Flag for a Slave Response
   @note    This is for the application to allow the slave to send a slave response of a
            diagnostic message which is implemented by the application. If this flag is not set
            the slave would refuse to answer a previous master request except Target Reset,
            NAD Assign, Message ID Assign or Read by Identifier.
   @par
            The flags are reset after receiving the next master request (to the slave's NAD
            initial NAD, or broadcast NAD).
*/
#define j2602_allow_slaveresponse() do{g_j2602_status.flags.sresp = 1u;g_j2602_status.flags.tx_resp = 0u;}while(0)

/**
   @brief   Set the Flag for a Slave Response to Target Reset
   @note    This is for the application to allow the slave to send a slave response to a reset request
            (for example after POR or l_sys_init the information about the received request would be lost to the LIN driver).
            The application has to determine after the reset if a response to a 0x3D header is allowed or not. 
   @par
            The flags are reset after receiving the next master request (to the slave's NAD
            initial NAD, or broadcast NAD).
*/
#define j2602_allow_resetresponse() do{g_j2602_status.flags.sresp = 1u;g_j2602_status.flags.tx_resp = 1u;}while(0)

/**
   @brief   Check the Reset Flag
   @note    This is for the application to check if the driver has received a reset request.
*/
l_u8 l_flg_tst_Reset(void);

/**
   @brief   Delete the Reset Flag
   @note    This is for the application to delete the reset flag after beginning to work out or
            ignore the reset
*/
void l_flg_clr_Reset(void);

/**
   @brief   Test the Broadcast-frame-received Flag
   @note    This is for the application to check if data from broadcast frames was received.
   @par
            This is for Frame ID 38 or 39 (depending on the DNN)
*/
l_bool l_flg_tst_new_bc_data0(void);

/**
   @brief   Delete the Broadcast-frame-received Flag
   @note    This is for the application to reset the flag after fetching the received data.
   @par
            This is for Frame ID 38 or 39 (depending on the DNN)
*/
void l_flg_clr_new_bc_data0(void);

/**
   @brief   Test the Broadcast-frame-received Flag
   @note    This is for the application to check if data from broadcast frames was received.
   @par
            This is for Frame ID 3A or 3B (depending on the DNN)
*/
l_bool l_flg_tst_new_bc_data1(void);

/**
   @brief   Delete the Broadcast-frame-received Flag
   @note    This is for the application to reset the flag after fetching the received data.
   @par
            This is for Frame ID 3A or 3B (depending on the DNN)
*/
void l_flg_clr_new_bc_data1(void);

/**
   @brief   Fetch the Data from a broadcast frame
   @note    After receiving a broadcast frame the application has to fetch the data bytes which
            are assigned to it basing on the DNN and number of Frames with this function.
*/
l_u8* j2602_get_bc_data(l_u8 frame);

/**
   @brief   Reset the errorbits of the statusbyte
   @note    The reset has to be handled by the application. This function allows the application
            to make the necessary reset of the three errorbits of the statusbyte.
            Parameters are:
            Zero: Reset all error states, set Reset state
            Non-Zero: Keep all error states, set Reset state
*/
void j2602_hal_target_reset (l_u8 reset);

/**
   @brief   Configuration via 3E / 3C messages without node configuration services
   @note    The node configuration services defined in LIN 2.x specification are handled by
            driver without need of application interaction. J2602 specifies additional
            configuration services via 3C / 3E messages - this configuration is handled
            by application, therefore a callback is provided. The request_data as well as the
            response_data is stored in an array with length of 8 bytes so the buffers must be
            filled completely - including the NAD / J2602-Statusbyte in the first byte.
            Cooked API will handle 3C messages as all other diagnostic messages -> handle via
            ld_receive_message, 3E messages will be handled by this callback.
            Raw API
            User Defined Diagnostics has to handle 3C and 3E messages via ld_user_define_diagnostic()
            callback function.
            No TL API will handle 3C messages and 3E messages via this callback except SID
            B0 - B7 which are node configuration messages
            Return values are:
            LD_RESPONSE       Response allowed (data filled by application - pos. or neg. response)
            LD_NO_RESPONSE    No response allowed
            Parameters are:
            const l_u8* const request_data      Data from 3C or 3E request
            const l_u8* response_data           Data for the slave response
*/
l_u8 j2602_callback_configuration_request (const l_u8* const request_data, l_u8* const response_data);

/**
   @brief   Callback function for the driver to check if the reset was executed by the application.
   @note    The application has the opportunity to not execute a reset when received (target or
            broadcast reset). The driver has to check if the reset was executed to send the correct
            response (if a target reset was sent).
   @retval  1 Reset was executed
   @retval  2 Reset was rejected
*/
l_u8 l_callback_Reset_Response (void);

#ifndef GM_SPEC_ADAPTION
/**
   @brief   Assembly of the error bits for the status byte
   @note    This will delete old errors and check the available errors from highest to lowest
            priority.
 */
#define J2602_GET_ERRORBITS() do {                                       \
              j2602_statusbyte = 0u;                                     \
              if (g_j2602_status.flags.errorstate != 0u) {               \
                 if ((g_j2602_status.flags.errorstate & 0x10u)) {        \
                    j2602_statusbyte = 0x07u;                            \
                 } else if ((g_j2602_status.flags.errorstate & 0x08u)) { \
                    j2602_statusbyte = 0x06u;                            \
                 } else if ((g_j2602_status.flags.errorstate & 0x04u)) { \
                    j2602_statusbyte = 0x05u;                            \
                 } else if ((g_j2602_status.flags.errorstate & 0x02u)) { \
                    j2602_statusbyte = 0x04u;                            \
                 } else if ((g_j2602_status.flags.errorstate & 0x01u)) { \
                    j2602_statusbyte = 0x01u;                            \
                 } else {                                                \
                    j2602_statusbyte = 0u;                               \
                 }                                                       \
              }                                                          \
           }while(0)

/**
   @brief   Assembly of status byte for transmission
   @note    This will place the error bits and the application specific bits together into
            the status byte for transmission
 */
#define J2602_CREATE_STATUSBYTE()  do {                                  \
              J2602_GET_ERRORBITS();                                     \
              LIN_STATUS_BYTE(j2602_statusbyte);                         \
              g_j2602_status.flags.status_byte_transm = 1u;              \
           }while(0)
#endif /* #ifndef GM_SPEC_ADAPTION */
#else
/* Empty Macros */
#define j2602_allow_slaveresponse() do{ }while(0)
#endif /* end #ifdef J2602_SLAVE */

/* ===========================================================================
 *  Additional API for different baud rate synchronization mechanisms
 * ==========================================================================*/

#ifdef AUTOBAUD_ENABLED
/**
   @brief   ISR for input capture (baud rate sync by measuring the sync field)
   @note    No additional handling required.
   @par     Choice will be given by LDC-Tool if required.
 */
#define l_ifc_baudsynch_inpcapt()           do{lin_hal_autobaud_calc_inpcapt();}while(0)
#ifdef BAUDRATESYNCH_WITH_PCINT
/**
   @brief   ISR for pin change interrupt (baud rate sync by measuring the sync field)
   @note    No additional handling required.
   @par     Choice will be given by LDC-Tool if required.
 */
#define l_ifc_baudsynch_pcint()             lin_hal_autobaud_calc_pcint()
#endif /* end #ifdef BAUDRATESYNCH_WITH_PCINT */
#ifdef BAUDRATESYNCH_WITH_EXTINT
/**
   @brief   ISR for external interrupt (baud rate sync by measuring the sync field)
   @note    No additional handling required.
   @note    RX pin is either capable of external interrupts or another pin is used for this configuration.
   @par     Choice will be given by LDC-Tool if required.
 */
#define l_ifc_baudsynch_extint()            lin_hal_autobaud_calc_extint()
#endif /* end #ifdef BAUDRATESYNCH_WITH_EXTINT */
#if ((defined(BAUDRATESYNCH_TIMER0)) || (defined(BAUDRATESYNCH_TIMER2)))
/**
   @brief   ISR for timer overflow during baud rate sync by measuring the sync field
   @note    No additional handling required.
   @par     Choice will be given by LDC-Tool if required.
 */
#define l_ifc_count_timerov()               lin_hal_count_timeroverflows()
#endif /* end #if defined(BAUDRATESYNCH_TIMER0) || defined(BAUDRATESYNCH_TIMER2) */
#ifdef LIN_ENABLE_SYNC_VIA_NMI
/**
   @brief   Check if the baudrate synchronization is happening
   @note    This is for the application to check if the driver is making baudrate synchronization.
*/
#define l_flg_tst_BaudrateSynch()           (lin_timeout_ctrl.flag.autobaud == 1u)
#endif /* #ifdef LIN_ENABLE_SYNC_VIA_NMI */
#endif /* end #ifdef AUTOBAUD_ENABLED */

#ifdef AUTOBAUD_ENABLED
/**
   @brief   ISR for timer interrupt during baud rate sync by measuring the sync field
   @note    No additional handling required.
 */
#define l_ifc_t2()         (lin_hal_T2_ISR())
#endif /* #ifdef AUTOBAUD_ENABLED */

#ifdef EARLY_TRANSMIT_FIX_TIMER
/**
   @brief   Initialize timer for transmit delay
   @note    Required if UART is able to start transmission of a byte before reception
            of previous byte is completed.
   @par     Choice will be given by LDC-Tool if required.
 */
void ETFix_InitTimer (void);

/**
   @brief   Get timer value for transmit delay
   @note    Required if UART is able to start transmission of a byte before reception
            of previous byte is completed.
   @par     Choice will be given by LDC-Tool if required.
 */
void ETFix_GetStartTime (void);

/**
   @brief   Configure timer for transmit delay
   @note    Required if UART is able to start transmission of a byte before reception
            of previous byte is completed.
   @par     Choice will be given by LDC-Tool if required.
 */
void ETFix_GetTimeGetEtsDelayTime (l_u8 ByteToTransmit);

/**
   @brief   Timer ISR for transmit delay
   @note    Required if UART is able to start transmission of a byte before reception
            of previous byte is completed.
   @par     Choice will be given by LDC-Tool if required.
 */
#define ETFix_Timer_ISR(pc_Char)             lin_hal_Timer_ISR(pc_Char)

/**
   @brief   Get necessary delay for transmit delay
   @note    Required if UART is able to start transmission of a byte before reception
            of previous byte is completed.
   @par     Choice will be given by LDC-Tool if required.
 */
#define ETFix_GetTimerDelay(x)               lin_hal_Timer_Delay(x)
#endif /* #ifdef EARLY_TRANSMIT_FIX_TIMER */

/* ===========================================================================
 *  Additional API for LIN communication
 * ==========================================================================*/

/**
   @brief   Fast cyclic task for handling LIN communication
   @note    This task can be called either directly after the RX ISR routine or separately in
            a fast cyclic routine (see datasheet of driver package for timing constraints for this task.
 */
#define l_cyclic_com_task()                 do{l_hal_cyclic_call();}while(0)

/**
   @brief   Normal cyclic task for handling LIN communication
   @note    The time slice for this task is configured by the LDC-Tool in 1ms steps.
            Recommended time slices are 1ms, 2ms or 5ms.
   @par     This tasks provides the time base for the LIN driver and handles all
            task around diagnostic communication.
   @par     This task is assembled differently for different LIN configurations.
 */
#ifndef LIN_PROTOCOL_VERSION_1_3
   #ifdef LIN_COOKED_API
      #define ld_task()                do { ld_tp_task();                        \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
   #endif /* end #ifdef LIN_COOKED_API */
   #ifdef LIN_RAW_API
      #define ld_task()               do { ld_raw_task();                        \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
   #endif /* end #ifdef LIN_RAW_API */
   #ifdef LIN_USER_DEF_DIAG_ENABLE
      #ifdef LIN_USER_DEF_DIAG_MANDSERV_ENABLE
        #ifdef J2602_SLAVE
            #define ld_task()       do { if((g_lin_rx_ctrl.flag.rx_diag == 1u) || (g_lin_rx_ctrl.flag.rx_config == 1u)) \
                                         { ld_config_task();                     \
                                           if(g_lin_rx_ctrl.flag.rx_diag)        \
                                             { g_lin_rx_ctrl.flag.rx_diag = 0u;  \
                                               ld_user_define_diagnostic(); } }  \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
        #else
         #define ld_task()             do { if(g_lin_rx_ctrl.flag.rx_diag)       \
                                         { ld_config_task();                     \
                                           if(g_lin_rx_ctrl.flag.rx_diag)        \
                                             { g_lin_rx_ctrl.flag.rx_diag = 0u;  \
                                               ld_user_define_diagnostic(); } }  \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
        #endif /* end #ifdef J2602_SLAVE */
      #else
        #ifdef J2602_SLAVE
            #define ld_task()       do { if((g_lin_rx_ctrl.flag.rx_diag == 1u) || (g_lin_rx_ctrl.flag.rx_config == 1u)) \
                                         { g_lin_rx_ctrl.flag.rx_diag = 0u;      \
                                           g_lin_rx_ctrl.flag.rx_config = 0u;    \
                                           ld_user_define_diagnostic(); }        \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
      #else
         #define ld_task()             do { if(g_lin_rx_ctrl.flag.rx_diag)       \
                                         { g_lin_rx_ctrl.flag.rx_diag = 0u;      \
                                           ld_user_define_diagnostic(); }        \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
        #endif /* end #ifdef J2602_SLAVE */
      #endif /* end #ifdef LIN_USER_DEF_DIAG_MANDSERV_ENABLE */
   #endif /* end #ifdef LIN_USER_DEF_DIAG_ENABLE */
   #if ((!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_USER_DEF_DIAG_ENABLE)))
      #define ld_task()                do { ld_config_task();                    \
                                         l_frame_timeout_control();              \
                                         l_autobaud_control();                   \
                                         l_hal_baudrate_control(); } while (0)
   #endif /* end #ifndef LIN_DIAG_ENABLE */
#else
   #define ld_task()                   do { l_autobaud_control();                \
                                         l_hal_baudrate_control(); } while (0)
#endif /* end #ifndef LIN_PROTOCOL_VERSION_1_3 */

#if ((defined LIN_SLAVE)||(defined J2602_SLAVE))
#ifndef LIN_INITIAL_NAD
/**
   @brief   Allows setting of the initial (LIN 2.1 +) and/or configured NAD during startup
   @note    This allows setting of the initial NAD during startup. Usually the NAD is
            fetched from the LDF by the LDC-Tool. Sometimes different behavior for the
            node is intended, the definition can then be removed manually from the
            genLinConfig.h file and this function can be used.
            Other configuration options do not allow setting a different initial NAD
            for LIN 2.1 (and newer protocols) by API.
 */
l_u8 l_callback_Get_Initial_NAD(void);
#endif /* end ifndef LIN_INITIAL_NAD */

#ifdef LIN_PID_RECOGNITION_CALLBACK
/**
   @brief      Callback when PID is received
   @par        This function is called when the node has finished the PID recognition.
               The function will provide the received PID only if the PID is supported
               by the node for use by the application.
   @see        LIN specification (LIN 1.3, 2.x)
   @pre        LIN driver initialized.
   @param      received_PID         Received PID from Master
   @retval     void                 No return value.
*/
void l_callback_PIDreceived (l_u8 receivedpid);
#endif /* #ifdef LIN_PID_RECOGNITION_CALLBACK */

#endif /* #if ((defined LIN_SLAVE)||(defined J2602_SLAVE)) */
#endif /* end #ifndef LIN_DRIVER_API_H */
