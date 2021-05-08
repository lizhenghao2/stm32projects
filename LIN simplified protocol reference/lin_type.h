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
********************     Workfile:      lin_type.h       **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All defines special data types for the LIN driver    *
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
@file lin_type.h
@brief Header file for all LIN files
*/

#ifndef LIN_TYPE_H /* to interpret header file only once */
#define LIN_TYPE_H

/* ===========================================================================
 *  Type Definitions
 * ==========================================================================*/

#ifndef LIN_TYPES
#define LIN_TYPES
typedef unsigned char  l_bool;   /* Boolean Datatype */
typedef unsigned char  l_u8;     /* 8Bit Datatype */
typedef unsigned int   l_u16;    /* 16Bit Datatype */
typedef unsigned long  l_u32;    /* 32Bit Datatype */
typedef unsigned short l_irqmask, l_ioctl_op;                                            /* Datatypes for special LIN functions */
typedef unsigned short l_signal_handle, l_flag_handle, l_ifc_handle, l_schedule_handle;  /* Datatypes for special LIN functions */
#endif /* end #ifndef LIN_TYPES */

/* ===========================================================================
 *  Structures
 * ==========================================================================*/

#if ((defined LIN_ENABLE_ASSIGN_FRAME_ID) || (defined LIN_ENABLE_RBI_RD_MSG_ID_PID) || (defined J2602_PROTOCOL))
#ifdef USE_PACKED
typedef @packed union
#else
typedef union
#endif /* end #ifdef USE_PACKED */
{
   l_u8 reg[5];
#ifdef USE_PACKED
   @packed struct
#else
   struct
#endif /* end #ifdef USE_PACKED */
   {
      struct
      {
         l_u8  lo;
         l_u8  hi;
      } msg_id;
      l_u8 pid;
      struct
      {
         l_u8  publisher               :1;
         l_u8  update_flag             :1;
         l_u8  res                     :2;
         l_u8  transfer_type           :4;
      } frame_type;
      l_u8     length;
   } frame;
} t_Lin_Frame_Ctrl;
#else
typedef union
{
   l_u8 reg[3];
   struct
   {
      l_u8   pid;
      struct
      {
         l_u8  publisher               :1;
         l_u8  update_flag             :1;
         l_u8  res                     :2;
         l_u8  transfer_type           :4;
      } frame_type;
      l_u8     length;
   } frame;
} t_Lin_Frame_Ctrl;
#endif /* end #ifdef LIN_ENABLE_ASSIGN_FRAME_ID */

#ifdef USE_PACKED
typedef @packed union
#else
typedef union
#endif /* end #ifdef USE_PACKED */
{
   l_u8 byte[8];
#ifdef USE_PACKED
   @packed struct
#else
   struct
#endif /* end #ifdef USE_PACKED */
   {
      l_u8         NAD;
      l_u8         PCI;
      union
      {
         struct
         {
            l_u8   SID;
            l_u8   diag_data[5];
         }SF;
         struct
         {
            l_u8   LEN;
            l_u8   SID;
            l_u8   diag_data[4];
         }FF;
         struct
         {
            l_u8   diag_data[6];
         } CF;
      } type;
   } frame;
} t_lin_diag_frame;

#ifdef USE_PACKED
typedef @packed struct LIN_PROD_ID_s
#else
typedef struct LIN_PROD_ID_s
#endif /* end #ifdef USE_PACKED */
{
   l_u8  NAD;
   l_u8  Supplier_lo;
   l_u8  Supplier_hi;
   l_u8  Function_lo;
   l_u8  Function_hi;
   l_u8  Variant;
   l_u8  Initial_NAD;
} t_lin_prod_id;

typedef union u_lin_status_word
{
   struct
   {
      l_u16   error_in_resp                 :1;
      l_u16   successful_transfer           :1;
      l_u16   overrun                       :1;
      l_u16   goto_sleep                    :1;
      l_u16   bus_activity                  :1;
      l_u16   event_triggerd_frame_coll     :1;
      l_u16   save_config                   :1;
      l_u16   reserved                      :1;
      l_u16   last_pid                      :8;
   } flag;
   l_u16                 reg;
} t_lin_status_word;

typedef struct LIN_TP_RX_CF_CTRL
{
   l_u8    rx_cf_on    :1;
   l_u8    tx_on       :1;
   l_u8    rx_t_out_on :1;
   l_u8    tx_t_out_on :1;
   l_u8    rx_cf_num   :4;
} t_lin_tp_tx_rx_ctrl;

typedef union
{
   struct
   {
      l_u8   rx_diag     :1;
#ifdef J2602_SLAVE
      l_u8   rx_config   :1; /* J2602 0x3E configuration frames */
#else
      l_u8   res1        :1;
#endif /* end #ifdef J2602_SLAVE */
      l_u8   rx_multiframe_msg :1;
      l_u8   res2        :1;
      l_u8   evttrgfrm   :1;
      l_u8   res3        :3;
   } flag;
   l_u8      reg;
} t_lin_rx_ctrl;

typedef struct LIN_TP_STATUS
{
   l_u8    rx_stat        :4;
   l_u8    tx_stat        :4;
} t_lin_tp_status;

typedef struct LIN_SCHEDULE_TABLE_TYPE
{
   l_u16 pid;
   l_u16 frame_type;
   l_u16 frame_delay;
}t_lin_sched_table;

typedef t_lin_sched_table l_ScheduleTableList[];


typedef struct _lin_sched_data
{
   l_u8 lin_current_schedule;       /* store actual used schedule */
   l_u8 lin_next_frame;             /* store next frame to transfer */
   l_u16  lin_current_delay;        /* store delay until next transfer */
   l_u8 lin_next_PID;               /* store PID of next Frame */
} t_lin_sched_data;

typedef union l_node_diag_info
{
   l_u16    reg[6];
   struct
   {
      l_u16 supplier_id;
      l_u16 function_id;
      l_u8  variant;
      l_u8  number_of_frames;
      l_u8  init_NAD;
      l_u8  conf_NAD;
      l_u16 p2_min;
      l_u16 st_min;
   }data;
} t_l_node_diag_info;

typedef union l_conf_frame_info
{
   l_u8  reg[3];
   struct
   {
      l_u8  msg_id_lo;
      l_u8  msg_id_hi;
      l_u8  pid;
   }data;
} t_l_conf_frame_info;

typedef t_l_node_diag_info* t_l_node_diag_info_list;

typedef t_l_conf_frame_info* t_l_conf_frm_info_list;

typedef union
{
   struct
   {
      l_u8   lo;
      l_u8   hi;
   } b;
   l_u16     word;
} l_word;

typedef union
{
   struct LIN_TIMEOUT_CTRL
   {
      l_u8   autobaud        :1;
      l_u8   frame           :1;
      l_u8   transm_ongoing  :1;
      l_u8   recept_started  :1;
      l_u8   startup_config  :1;
      l_u8   lock_init_uart  :1;
      l_u8   sys_init        :1;
      l_u8   res             :1;
   } flag;
   l_u8 byte;
} t_lin_timeout_ctrl;

typedef union
{
   struct LIN_MASTER_CTRL
   {
      l_u8 syncf_transm      :1;
      l_u8 break_transm      :1;
      l_u8 res               :6;
   } flag;
   l_u8 byte;
} t_lin_master_ctrl;

typedef struct
{
   l_u8 ETF_id;
   l_u8 resolver_sched_id;
}t_lin_resolver_table;

typedef struct s_lin_hal_stat_reg
{
   l_u8 byte_received :1;
   l_u8 ferror        :1;
   l_u8 stopbit       :1;
   l_u8 isr_rec_break :1;
   l_u8 isr_rec_sync  :1;
   l_u8 res0          :3;
} LIN_hal_stat_reg_t;

typedef enum e_lin_slave_state
{
   idle = 0,
   break_received,
   sync_received,
   rx_data,
   rx_checksum,
   tx_data,
   tx_checksum
} LIN_SLAVE_STATE_t;

#ifdef USE_PACKED
typedef @packed union s_lin_temp_frame_buffer
#else
typedef union s_lin_temp_frame_buffer
#endif /* end #ifdef USE_PACKED */
{
   l_u8 byte[11];
#ifdef USE_PACKED
   @packed struct
#else
   struct
#endif /* end #ifdef USE_PACKED */
   {
      l_u8 PID;
      l_u8 DataBuffer[8];
      l_u8 Checksum;           /* used for subscribe frames CRC */
      l_u8 BufferIndex;
   }frame;
}t_lin_temp_frame_buffer;

/**
@struct LIN_TX_DATA_s
@brief   Structure to build a Transmit Frame
*/
#ifdef USE_PACKED
typedef @packed struct LIN_TX_DATA_s
#else
typedef struct LIN_TX_DATA_s
#endif /* end #ifdef USE_PACKED */
{
   l_u8 length_c;     /* length of frame data */
   l_u8* data_puc;    /* pointer to frame data */
   l_u8 old_data_uc;  /* TX data for check */
   l_u8 index_uc;     /* index to fill receive buffer with transmitted data */
   l_u8 checksum;     /* checksum for data transmit */
}LIN_TX_DATA_t;

/**
@struct LIN_FLAGS_s
@brief   Structure for transmit each byte of a Frame
*/
typedef struct LIN_FLAGS_s
{
 l_u8 tx_next      :2;  /* Indicates if next byte to transmit or not */
 l_u8 tx_finished  :1;  /* Indicated if transmission is finished */
 l_u8 res0         :5;
}LIN_FLAGS_t;

/**
@struct LIN_ERROR_STATE_s
@brief   Structure for Error State of the LIN Driver
*/
typedef union
{
   struct LIN_ERROR_STATE_s
   {
      l_u8 framing         :1;
      l_u8 overflow        :1;
      l_u8 invalid_break   :1;
      l_u8 invalid_field   :1;
      l_u8 bus_error       :1;
      l_u8 checksum        :1;
      l_u8 timeout         :1;
      l_u8 autobaud_error  :1;
   }e_bit;
   l_u8 byte;
}LIN_ERROR_STATE_t;

#ifdef SUPPORT_TWO_BAUDRATES
/**
@struct LIN_AUTOBAUD_FLAGS_s
@brief   Structure for autobaud identification
*/
typedef struct LIN_AUTOBAUD_FLAGS_s
{
 l_u8 baudrate_is_identified  :1;  /* Indicates if a valid baudrate was identified */
 l_u8 high_baudrate_frames    :2;  /* Indicates if two valid successive frames are received for the higher BR */
 l_u8 low_baudrate_frames     :2;  /* Indicates if two valid successive frames are received for the lower BR */
 l_u8 res0                    :3;
}LIN_AUTOBAUD_FLAGS_t;
#endif /* #ifdef SUPPORT_TWO_BAUDRATES */

#if (defined J2602_PROTOCOL)
typedef union J2602_STATUS
{
   struct
   {
      l_u16    res_0                :1;
      l_u16    tx_resp              :1;
      l_u16    new_frame0           :1;
      l_u16    new_frame1           :1;
      l_u16    sresp                :1;
      l_u16    reset                :2;   /* 0u No request has been received.
                                           * 1u Targeted Reset request has been received.
                                           * 2u Broadcast Reset request has been received. */
      l_u16    res_1                :1;
      l_u16    errorstate           :7;   /* Reset Response,
                                           * TX error,
                                           * ID Parity Error,
                                           * Byte Field Framing Error,
                                           * Checksum Error,
                                           * Data Error,
                                           * Reset */
      l_u16    status_byte_transm  :1;
   } flags;
   l_u16 reg[1];
}t_j2602_status;
#endif /* #if (defined J2602_PROTOCOL) */

#endif /* end ifndef LIN_TYPE_H */

