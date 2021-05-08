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
********************     Workfile:      lin_config.c     **********************
*                                                                             *
*  PROJECT-DESCRIPTION:  LIN Driver Protocol Layer                            *
*  FILE-DESCRIPTION:     All Routines for LIN Node Configuration Services     *
*                                                                             *
*******************************************************************************
*                                                                             *
*       Revision:        $Rev:: 115         $                                 *
*       Responsible:     B.Roegl                                              *
*       Co-Responsible:  P.Koch                                               *
*       Last Modtime:    $Date:: 2016-04-13#$                                 *
*                                                                             *
******************************************************************************/
/**
@file lin_config.c
@brief All Routines for LIN Node Configuration Services
 */

/* ===========================================================================
 *  Header files
 * ==========================================================================*/

#include "genLinConfig.h"
#include "lin_type.h"
#include "lin_driver_api.h"
#include "lin_main.h"


#ifdef LIN_COOKED_API
#include "lin_diag_tp.h"
#endif /* end #ifdef LIN_COOKED_API */

#include "lin_config.h"


/* ===========================================================================
 *  Global Variables
 * ==========================================================================*/

/* ===========================================================================
 *  Functions
 * ==========================================================================*/

#if ((defined(J2602_SLAVE)) || (defined(LIN_SLAVE)))
/* ---------------------------------------------------------------------------
 *  void l_handle_ConditionalChangeNAD (const l_u8 ConditionalData[], l_bool* ConditionResult);
 * --------------------------------------------------------------------------*/
/**
   @brief  Driver Callback to check condition for service.
   @pre    LIN driver initialized
   @param  l_u8* ConditionalData:   pointer to conditional data from request
   @param  l_bool* ConditionResult: pointer to bool variable to return check result
                                    to LIN driver
   @return void
*/
#ifdef  LIN_ENABLE_COND_CHG_NAD
static void l_handle_ConditionalChangeNAD (const l_u8 ConditionalData[], l_bool* ConditionResult)
{
   l_u8 selByte = 0u;
   l_u8 ret_cond = 0u;
#ifdef LIN_ENABLE_RBI_RD_ECU_SN
   l_u8 temp_SerNum[4];
#endif /* end ifdef LIN_ENABLE_RBI_RD_ECU_SN */

   /* check if condition is okay to change NAD or not. */
#if ((defined(LIN_PROTOCOL_VERSION_2_1)) && (!defined(LIN_ENABLE_RBI_RD_ECU_SN)))
   if (ConditionalData[0] == 0u)
   {
      switch(ConditionalData[1])
      {
         case(1u):
            selByte = g_lin_prod_id.Supplier_lo;
            break;
         case(2u):
            selByte = g_lin_prod_id.Supplier_hi;
            break;
         case(3u):
            selByte = g_lin_prod_id.Function_lo;
            break;
         case(4u):
            selByte = g_lin_prod_id.Function_hi;
            break;
         case(5u):
            selByte = g_lin_prod_id.Variant;
            break;
         default:
            *ConditionResult = 0u;
            ret_cond = 1u;
            break;
      }
   }
#else
   switch(ConditionalData[0])
   {
      case(0u):
         switch(ConditionalData[1])
         {
            case(1u):
               selByte = g_lin_prod_id.Supplier_lo;
               break;
            case(2u):
               selByte = g_lin_prod_id.Supplier_hi;
               break;
            case(3u):
               selByte = g_lin_prod_id.Function_lo;
               break;
            case(4u):
               selByte = g_lin_prod_id.Function_hi;
               break;
            case(5u):
               selByte = g_lin_prod_id.Variant;
               break;
            default:
               *ConditionResult = 0u;
               ret_cond = 1u;
               break;
         }
         break;
#ifdef LIN_ENABLE_RBI_RD_ECU_SN
      case(1u):
         if((ConditionalData[1] > 0u) && (ConditionalData[1] < 5u))
         {
            l_callback_RBI_ReadEcuSerialNumber(&temp_SerNum[0]);
            selByte = temp_SerNum[ConditionalData[1] - 1u];
         }
         else
         {
            *ConditionResult = 0u;
            ret_cond = 1u;
         }
         break;
#endif /* #ifdef LIN_ENABLE_RBI_RD_ECU_SN */
#ifdef LIN_PROTOCOL_VERSION_2_0
      case(16u):
      case(17u):
      case(18u):
      case(19u):
      case(20u):
      case(21u):
      case(22u):
      case(23u):
      case(24u):
      case(25u):
      case(26u):
      case(27u):
      case(28u):
      case(29u):
      case(30u):
      case(31u):
         if((ConditionalData[1] > 0u) && (ConditionalData[1] < 4u))
         {
            l_u8 loc_id= (l_u8)(ConditionalData[0] - 16u);
            if(loc_id < LIN_NUMBER_OF_FRAMES)
            {
               selByte = g_lin_frame_ctrl[loc_id].reg[ConditionalData[1] - 1u];
            }
            else
            {
               *ConditionResult = 0u;
               ret_cond = 1u;
            }
         }
         else
         {
            *ConditionResult = 0u;
            ret_cond = 1u;
         }
         break;
#endif /* ifdef LIN_PROTOCOL_VERSION_2_0 */
      default:
         *ConditionResult = 0u;
         ret_cond = 1u;
         break;
   }
#endif /* end #if defined(LIN_PROTOCOL_VERSION_2_1) && (!defined(LIN_ENABLE_RBI_RD_ECU_SN)) */

   if (ret_cond != 1u)
   {
      selByte ^= ConditionalData[3];
      selByte &= ConditionalData[2];

      if(selByte)
      {
         *ConditionResult = 0u;
      }
      else
      {
         *ConditionResult = 1u;
      }
   }
   return;
   /* return result of check to LIN driver */
}
#endif /* end #ifdef  LIN_ENABLE_COND_CHG_NAD */
#endif /* #if ((defined(J2602_SLAVE)) || (defined(LIN_SLAVE))) */


/* ---------------------------------------------------------------------------
 *  void lin_handle_config(void);
 * --------------------------------------------------------------------------*/
/**
   @brief  Function to handle the node configuration frames for the node
   @pre    LIN driver initialized
   @param  void
   @return void
*/
#if ((defined(J2602_SLAVE)) || (defined(LIN_SLAVE)))
#if (!defined(LIN_PROTOCOL_VERSION_1_3)) && ((!defined (LIN_USER_DEF_DIAG_ENABLE)) || (defined(LIN_USER_DEF_DIAG_MANDSERV_ENABLE)))
#if ((defined (LIN_COOKED_API)) || (defined (LIN_RAW_API)))
void lin_handle_config(void)
#else
static void lin_handle_config(void)
#endif /* #if ((defined LIN_COOKED_API) || (defined LIN_RAW_API)) */
{
#ifdef LIN_ENABLE_RBI_USER_DEFINED
   l_u8 ret_cond = 0u;
#endif /* end #ifdef LIN_ENABLE_RBI_USER_DEFINED */
#ifdef J2602_SLAVE
#if ((defined(LIN_ENABLE_ASSIGN_NAD)) || (defined(LIN_ENABLE_ASSIGN_FRAME_ID)) || (defined(LIN_ENABLE_ASSIGN_FRAME_ID_RANGE)))
   l_u8 loc_i;
#endif /* end #if defined(LIN_ENABLE_ASSIGN_NAD) || defined(LIN_ENABLE_ASSIGN_FRAME_ID) || defined(LIN_ENABLE_ASSIGN_FRAME_ID_RANGE) */
   l_u8 l_j;
#if (defined(LIN_ENABLE_ASSIGN_NAD))
   l_u8 loc_j;
#endif /* end #ifdef LIN_ENABLE_ASSIGN_NAD */
#ifdef LIN_ENABLE_ASSIGN_NAD
   l_u8 loc_p0, loc_p1;
#endif /* end #ifdef LIN_ENABLE_ASSIGN_NAD */
#else /* else #ifdef J2602_SLAVE */
#if ((defined(LIN_ENABLE_ASSIGN_FRAME_ID)) || (defined(LIN_ENABLE_ASSIGN_FRAME_ID_RANGE)))
   l_u8 loc_i;
#endif /* end #if defined(LIN_ENABLE_ASSIGN_NAD) || defined(LIN_ENABLE_ASSIGN_FRAME_ID) || defined(LIN_ENABLE_ASSIGN_FRAME_ID_RANGE) */
#endif /* end #ifdef J2602_SLAVE */
#ifdef  LIN_ENABLE_COND_CHG_NAD
   l_bool ConditionResult;
   l_u8 temp = 0u;
#endif /* #ifdef  LIN_ENABLE_COND_CHG_NAD */
#ifdef    LIN_ENABLE_ASSIGN_FRAME_ID_RANGE
   l_u8 loc_temp = 0u;
#endif /* #ifdef    LIN_ENABLE_ASSIGN_FRAME_ID_RANGE */


   switch(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID)
   {
#ifdef LIN_ENABLE_ASSIGN_NAD
      case(0xB0u):
         /* Assign NAD:
          * Response shall be sent if NAD (Initial or Broadcast), Supplier ID and Function ID match.
          * Wildcards for Supplier-ID, Function-ID and Message-ID will not be supported.
          * The NAD used for the response shall be the same as in the request */
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.Initial_NAD)||(LIN_DIAG_RECEIVE_FRAME.frame.NAD == LIN_BROADCAST_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u))
         {
            if(((( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == g_lin_prod_id.Supplier_lo) &&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == g_lin_prod_id.Supplier_hi)) ||
               (( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == LIN_SUPPLIER_ID_WILDCARD_LO ) &&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == LIN_SUPPLIER_ID_WILDCARD_HI ))) &&
               ((( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2] == g_lin_prod_id.Function_lo) &&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3] == g_lin_prod_id.Function_hi)) ||
               (( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2] == LIN_FUNCTION_ID_WILDCARD_LO )&&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3] == LIN_FUNCTION_ID_WILDCARD_HI ))))
            {
#ifdef LIN_ASSIGN_NAD_CALLBACK
               l_callback_assign_NAD (g_lin_prod_id.NAD, LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4]);
#endif /* #ifdef LIN_ASSIGN_NAD_CALLBACK */
#ifndef J2602_SLAVE
               /* set for positive response */
               g_lin_prod_id.NAD = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4];
#else
               if((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4] >= 0x60u) && (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4] <= 0x6Fu))
               {
                  g_lin_prod_id.NAD = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4];
                  loc_i = (l_u8)((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4] & 0x0Fu) << 2u);
                  loc_i = loc_i & 0xFCu;
                  for (loc_j = 0u; loc_j < LIN_NUMBER_OF_FRAMES; loc_j++)
                  {
                     /* PID */
                     loc_p0 = (loc_i & 0x01u)^((loc_i >> 1u) & 0x01u)^((loc_i >> 2u) & 0x01u)^((loc_i >> 4u) & 0x01u);
                     loc_p1 = (l_u8)(~(((loc_i >> 1u) & 0x01u)^((loc_i >> 3u) & 0x01u)^((loc_i >> 4u) & 0x01u)^((loc_i >> 5u) & 0x01u))) & 0x01u;
                     g_lin_frame_ctrl[loc_j].frame.pid = (l_u8)(loc_i | (l_u8)(loc_p0 << 6u)) | ((l_u8)(loc_p1 << 7u));
                     loc_i++;
                     /* Frame length and type are left unchanged */
                  }
               }
#endif /* end #else of #ifndef J2602_SLAVE */
               /* set for positive response */
               LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.Initial_NAD;
               LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x01u);
               LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
               j2602_allow_slaveresponse();
               ld_allow_response();
            }
            else
            {
               /* no response */
            }
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.Initial_NAD) */
         break;
#endif /* end #ifdef LIN_ENABLE_ASSIGN_NAD */

#ifdef LIN_ENABLE_ASSIGN_FRAME_ID
      case(0xB1u):
         /* Assign Frame ID:
          * Response shall be sent if NAD (Initial or Broadcast), Supplier ID match.
          */
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.NAD)||(LIN_DIAG_RECEIVE_FRAME.frame.NAD == LIN_BROADCAST_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == (l_u8)0x06))
         {
            if(((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == g_lin_prod_id.Supplier_lo) &&
                (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == g_lin_prod_id.Supplier_hi)) ||
               ((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == LIN_SUPPLIER_ID_WILDCARD_LO) &&
                (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == LIN_SUPPLIER_ID_WILDCARD_HI)))
            {
               for(loc_i = 0u; loc_i < LIN_NUMBER_OF_FRAMES; loc_i++)
               {
                  if((g_lin_frame_ctrl[loc_i].frame.msg_id.lo == LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2]) &&
                     (g_lin_frame_ctrl[loc_i].frame.msg_id.hi == LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3]))
                  {
#ifdef LIN_ASSIGN_FRAME_ID_CALLBACK
                     l_callback_assign_frame_id(loc_i, g_lin_frame_ctrl[loc_i].frame.pid, 
                                                LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4]);
#endif /* #ifdef LIN_ASSIGN_FRAME_ID_CALLBACK */
                     g_lin_frame_ctrl[loc_i].frame.pid = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4];
                     /* positive response */
                     LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                     LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x01u);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                     break;
                  }
                  else
                  {
                     /* set for negative response */
                     LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                     LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                  }
               } /* end for */
               j2602_allow_slaveresponse();
               ld_allow_response();
               ld_allow_userdef_response();
            } /* end if */
            else
            {
               /* no response */
            }
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) */
         break;
#endif /* end #ifdef LIN_ENABLE_ASSIGN_FRAME_ID */

      case(0xB2u):
         /* Read by ID */
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) || (g_lin_prod_id.NAD == g_lin_prod_id.Initial_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u))
         {
            if((((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == g_lin_prod_id.Supplier_lo) &&
                (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2] == g_lin_prod_id.Supplier_hi)) ||
               ((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == LIN_SUPPLIER_ID_WILDCARD_LO) &&
                (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2] == LIN_SUPPLIER_ID_WILDCARD_HI))) &&
               (((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3] == g_lin_prod_id.Function_lo) &&
                (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4] == g_lin_prod_id.Function_hi)) ||
               ((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3] == LIN_FUNCTION_ID_WILDCARD_LO)&&
                (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4] == LIN_FUNCTION_ID_WILDCARD_HI))))
            {
#if ((!defined(LIN_ENABLE_RBI_RD_ECU_SN)) && (!defined(LIN_ENABLE_RBI_RD_MSG_ID_PID)) && (!defined(LIN_ENABLE_RBI_USER_DEFINED)))
               if (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == 0u)
               {
                  /* set for positive response */
                  LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                  LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x06u);
                  LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = g_lin_prod_id.Supplier_lo;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = g_lin_prod_id.Supplier_hi;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = g_lin_prod_id.Function_lo;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = g_lin_prod_id.Function_hi;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = g_lin_prod_id.Variant;
               }
               else
               {
                  /* set for negative response */
                  LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                  LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
                  LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                  LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
               }
#else /* #if (!defined(LIN_ENABLE_RBI_RD_ECU_SN)) && (!defined(LIN_ENABLE_RBI_RD_MSG_ID_PID)) && (!defined(LIN_ENABLE_RBI_USER_DEFINED)) */
               switch(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0])
               {
                  /* id 0 */
                  case(0u):
                     /* set for positive response */
                     LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                     LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x06u);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = g_lin_prod_id.Supplier_lo;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = g_lin_prod_id.Supplier_hi;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = g_lin_prod_id.Function_lo;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = g_lin_prod_id.Function_hi;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = g_lin_prod_id.Variant;

                     break;
                  /* id 1 */
#ifdef    LIN_ENABLE_RBI_RD_ECU_SN
                  case(1u):
                     l_callback_RBI_ReadEcuSerialNumber(&LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0]);
                     LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                     LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x05u);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                     break;
#endif /* end #ifdef  LIN_ENABLE_RBI_RD_ECU_SN */

#if ((defined (LIN_ENABLE_RBI_RD_MSG_ID_PID)) && ((defined (LIN_PROTOCOL_VERSION_2_0)) || (defined(J2602_SLAVE))))
                  /* id 16-31 */
                  case(16u):
                  case(17u):
                  case(18u):
                  case(19u):
                  case(20u):
                  case(21u):
                  case(22u):
                  case(23u):
                  case(24u):
                  case(25u):
                  case(26u):
                  case(27u):
                  case(28u):
                  case(29u):
                  case(30u):
                  case(31u):
                     if((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] -16u) < LIN_NUMBER_OF_FRAMES)
                     {
                        l_u8 loc_id;
                        loc_id = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] - 16u);
                        /* set for positive response */
                        LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                        LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x04u);
                        LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = g_lin_frame_ctrl[loc_id].frame.msg_id.lo;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = g_lin_frame_ctrl[loc_id].frame.msg_id.hi;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = g_lin_frame_ctrl[loc_id].frame.pid;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                     }
                     else
                     {
                        /* set for negative response */
                        LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                        LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
                        LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                        LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                     }
                     break;
#endif /* end #ifdef  LIN_ENABLE_RBI_RD_MSG_ID_PID */

#if defined(LIN_ENABLE_RBI_USER_DEFINED)
                  case(32u):
                  case(33u):
                  case(34u):
                  case(35u):
                  case(36u):
                  case(37u):
                  case(38u):
                  case(39u):
                  case(40u):
                  case(41u):
                  case(42u):
                  case(43u):
                  case(44u):
                  case(45u):
                  case(46u):
                  case(47u):
                  case(48u):
                  case(49u):
                  case(50u):
                  case(51u):
                  case(52u):
                  case(53u):
                  case(54u):
                  case(55u):
                  case(56u):
                  case(57u):
                  case(58u):
                  case(59u):
                  case(60u):
                  case(61u):
                  case(62u):
                  case(63u):
                     loc_i = l_callback_RBI_UserDefined (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0], &LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0]);
                     if (loc_i > 5u)
                     {
                        /* Response too long */
                        loc_i = LD_NO_RESPONSE;
                     }
                     else
                     {
                        if (loc_i != LD_NEGATIVE_RESPONSE)
                        {
#ifndef LIN_RBI_UDEF_2_2
                           loc_i = LD_POSITIVE_RESPONSE;
#else
                           /* Add RSID to length information */
                           loc_i++;
#endif /* end #ifndef LIN_RBI_UDEF_2_2 */
                        }
                     }
                     switch(loc_i)
                     {
                        case LD_NEGATIVE_RESPONSE :
                           /* negative response */
                           LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                           LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
                           LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
                           LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
                           LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
                           LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
                           LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                           LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                           break;

                        case LD_NO_RESPONSE:
                           ret_cond = 1u;
                           break;
#ifndef LIN_RBI_UDEF_2_2
                        case LD_POSITIVE_RESPONSE:
                           /* positive response */
                           LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                           LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x06u);
                           LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                           /* Remaining data filled by application in callback */
                           break;
#endif /* end #ifndef LIN_RBI_UDEF_2_2 */
                        default:
#ifdef LIN_RBI_UDEF_2_2
                           /* positive response */
                           LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                           LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | loc_i);
                           LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
                           /* Used data bytes are filled by application in callback and now fill unused bytes with 0xFF */
                           while (loc_i < 6u)
                           {
                              LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[loc_i - 1u] = 0xFFu;
                              loc_i++;
                           }
#endif /* end #ifdef LIN_RBI_UDEF_2_2 */
                           break;
                     }
                     break;
#endif /* end #ifdef LIN_ENABLE_RBI_USER_DEFINED */

                  default:
                     /* set for negative response */
                     LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
                     LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
                     LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
                     break;
               }
#endif /* end #if (!defined(LIN_ENABLE_RBI_RD_ECU_SN)) && (!defined(LIN_ENABLE_RBI_RD_MSG_ID_PID)) && (!defined(LIN_ENABLE_RBI_USER_DEFINED)) */

#ifdef LIN_ENABLE_RBI_USER_DEFINED
               if (ret_cond != 1u)
#endif /* end #ifdef LIN_ENABLE_RBI_USER_DEFINED */
               {
                  j2602_allow_slaveresponse();
                  ld_allow_response();
                  ld_allow_userdef_response();
               }
            } /* end if supply id verification */
            else
            {
               /* no negative response */
            }
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) */
         break;

      case(0xB3u):
         /* Conditional Change NAD */
#ifdef  LIN_ENABLE_COND_CHG_NAD
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) || (g_lin_prod_id.NAD == g_lin_prod_id.Initial_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u))
         {
            l_handle_ConditionalChangeNAD(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data, &ConditionResult);
            if(ConditionResult)
            {
               temp = g_lin_prod_id.NAD;
               g_lin_prod_id.NAD = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4];
               /* set for positive response */
               LIN_DIAG_SEND_FRAME.frame.NAD = temp;
               LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x01u);
               LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
               j2602_allow_slaveresponse();
               ld_allow_response();

            }
            else
            {
               /* no negative response */
            }
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) */
#endif /* end #ifdef LIN_ENABLE_COND_CHG_NAD */
         break;

      case(0xB4u):
         /* Data Dump */
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) || (g_lin_prod_id.NAD == g_lin_prod_id.Initial_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u))
         {
#ifdef    LIN_ENABLE_DATA_DUMP
            /* positive response */
            l_callback_DataDump(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data,LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data);
            LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
            LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x06u);
            LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
#else
            /* negative response */
            LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
            LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
            LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
#endif /* end #ifdef LIN_ENABLE_DATA_DUMP */

            j2602_allow_slaveresponse();
            ld_allow_response();
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) */
         break;

      case(0xB5u):
         /* Node Position Detection, see the related specification */
         /* In LIN not supported, shall not answer due to J2602 compliance */
         /* J2602 reset - response behavior according to SAE J2602 */
#ifdef J2602_SLAVE
         if((LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.NAD) || (LIN_DIAG_RECEIVE_FRAME.frame.NAD == LIN_BROADCAST_NAD))
         {
            if ((LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u) && (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID == 0xB5u))
            {
               for (l_j = 0u; l_j < 5u; l_j++)
               {
                  if (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[l_j] != 0xFFu)
                  {
                     l_j = 0u;
                     break;
                  }
               }
               if (l_j > 0u)
               {
                  if (LIN_DIAG_RECEIVE_FRAME.frame.NAD != LIN_BROADCAST_NAD)
                  {
                     l_u8_wr_Reset(1u);   /* set Reset Flag: Target Reset is received */
                     g_j2602_status.flags.tx_resp = 1u;
                  }
                  else
                  {
                     l_u8_wr_Reset(2u);   /* set Reset Flag: Broadcast Target Reset is received */
                     /* If a target reset was sent before without Slave Response */
                     g_j2602_status.flags.tx_resp = 0u;
                  }
               }
            }
         }
#endif /* end #ifdef J2602_SLAVE */
         break;

#ifdef LIN_PROTOCOL_VERSION_2_1
      case(0xB6u):
         /* Save Configuration */
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) || (g_lin_prod_id.NAD == g_lin_prod_id.Initial_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u))
         {
#ifdef      LIN_ENABLE_SAVE_CONFIG
            g_lin_status_word.flag.save_config = L_SET;
            /* positive response */
            LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
            LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x01u);
            LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
#else
            LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
            LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
            LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
#endif /* end #ifdef LIN_ENABLE_SAVE_CONFIG */

            ld_allow_response();
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) */
         break;

      case(0xB7u):
         /* Assign Frame ID range */
         if(((LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) || (g_lin_prod_id.NAD == g_lin_prod_id.Initial_NAD)) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u))
         {
#ifdef    LIN_ENABLE_ASSIGN_FRAME_ID_RANGE
            for(loc_i = 4u ; loc_i > 0u; loc_i--)
            {
               if(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[loc_i] != 0xFFu)
               {
                  loc_temp = loc_i - 1u;
                  break;
               }
            }
            if((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] + loc_temp) >= LIN_NUMBER_OF_FRAMES)
            {
               /* set for negative response */
               LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
               LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
               LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
            }
            else
            {
               l_u8 loc_index;
#ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK
               l_u8 loc_pids[4];
#endif /* #ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK */
               loc_index = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0];
               for(loc_i = 0u; loc_i <= loc_temp; loc_i++)
               {
#ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK
                  loc_pids[loc_i] = g_lin_frame_ctrl[loc_index+loc_i].frame.pid;
#endif /* #ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK */
                  if (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[loc_i + 1u] != 0xFFu)
                  {
                     g_lin_frame_ctrl[loc_index+loc_i].frame.pid = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[loc_i + 1u];
                  }
               }
#ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK
               l_callback_assign_frame_id_range(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0], &loc_pids[0],
                                                &LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1]);
#endif /* #ifdef LIN_ASSIGN_FRAME_ID_RANGE_CALLBACK */
               /* positive response */
               LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
               LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x01u);
               LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (l_u8)(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID + RSID_OFFSET);
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
            }
#else
            LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
            LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
            LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_SubfunctionNotSupported;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
            LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
#endif /* end #ifdef LIN_ENABLE_ASSIGN_FRAME_ID_RANGE */

            ld_allow_response();
            ld_allow_userdef_response();
         } /* end if(LIN_DIAG_RECEIVE_FRAME.frame.NAD != g_lin_prod_id.Initial_NAD) */
         break;
#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 -> B5, B6, B7 */

      default:
#if ((!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_USER_DEF_DIAG_ENABLE)))
         g_lin_diag_config_active = 0x0u;
#endif /* end #if (!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_USER_DEF_DIAG_ENABLE)) */
         break;
   }
}
#endif /* #if (!defined(LIN_PROTOCOL_VERSION_1_3)) && ((defined (LIN_DIAG_ENABLE)) || (defined(LIN_USER_DEF_DIAG_MANDSERV_ENABLE))) */
#endif /* #if ((defined(J2602_SLAVE)) || (defined(LIN_SLAVE))) */

/* ---------------------------------------------------------------------------
 *  void lin_handle_FunctionalNAD(void);
 * --------------------------------------------------------------------------*/
/**
   @brief  Function to handle configuration messages with functional NAD.
   @pre    LIN driver initialized
   @param  void
   @return void
*/
#if (defined(LIN_PROTOCOL_VERSION_2_1)) && (((!defined(LIN_USER_DEF_DIAG_ENABLE)) && (!defined(LIN_DIAG_ENABLE))))
static void lin_handle_FunctionalNAD(void)
{
#if (defined(LIN_ENABLE_ASSIGN_FRAME_ID_RANGE))
   l_u8 loc_i;
#endif /* end #if defined(LIN_ENABLE_ASSIGN_NAD) || defined(LIN_ENABLE_ASSIGN_FRAME_ID) || defined(LIN_ENABLE_ASSIGN_FRAME_ID_RANGE) */
#ifdef  LIN_ENABLE_COND_CHG_NAD
   l_bool ConditionResult_2;
#endif /* end #ifdef LIN_ENABLE_COND_CHG_NAD */
#ifdef    LIN_ENABLE_ASSIGN_FRAME_ID_RANGE
   l_u8  loc_temp = 0u;
#endif /* #ifdef    LIN_ENABLE_ASSIGN_FRAME_ID_RANGE */


   switch(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID)
   {
      case(0xB0u):
         /* Assign NAD */
         if (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u)
         {
            if(((( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == g_lin_prod_id.Supplier_lo )&&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == g_lin_prod_id.Supplier_hi )) ||
               (( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == LIN_SUPPLIER_ID_WILDCARD_LO )&&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == LIN_SUPPLIER_ID_WILDCARD_HI ))) &&
               ((( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2] == g_lin_prod_id.Function_lo )&&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3] == g_lin_prod_id.Function_hi )) ||
               (( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2] == LIN_FUNCTION_ID_WILDCARD_LO )&&
                ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3] == LIN_FUNCTION_ID_WILDCARD_HI ))))
            {
               g_lin_prod_id.NAD = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4];
            }
         }
         break;
#ifdef LIN_ENABLE_ASSIGN_FRAME_ID
      case(0xB1u):
         /* Assign Frame ID   */
         if (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u)
         {
            if(( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == g_lin_prod_id.Supplier_lo &&
                 LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == g_lin_prod_id.Supplier_hi) ||
               ( LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == LIN_SUPPLIER_ID_WILDCARD_LO &&
                 LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1] == LIN_SUPPLIER_ID_WILDCARD_HI ))
            {
               for(loc_i = 0u; loc_i < LIN_NUMBER_OF_FRAMES; loc_i++)
               {
                  if((g_lin_frame_ctrl[loc_i].frame.msg_id.lo == LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[2]) &&
                     (g_lin_frame_ctrl[loc_i].frame.msg_id.hi == LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[3]))
                  {
                     g_lin_frame_ctrl[loc_i].frame.pid = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[4];
                     break;
                  }
               }
            }
         }
         break;
#endif /* end   #ifdef LIN_ENABLE_ASSIGN_FRAME_ID */
      case(0xB2u):
         /* Read by ID */
         /* Do nothing now */
         break;

      case(0xB3u):
         /* Conditional Change NAD */
#ifdef  LIN_ENABLE_COND_CHG_NAD
         if (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u)
         {
            l_handle_ConditionalChangeNAD(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data, &ConditionResult_2);
         }
#endif /* end #ifdef LIN_ENABLE_COND_CHG_NAD */
         break;

      case(0xB4u):
         /* Data Dump */
         /* Do Nothing now */
         break;

      case(0xB5u):
         /* Node Position Detection, see the related specification */
         /* Do Nothing now */
         break;

      case(0xB6u):
         /* Save Configuration */
#ifdef      LIN_ENABLE_SAVE_CONFIG
         if (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u)
         {
            g_lin_status_word.flag.save_config = L_SET;
         }
#endif /* end #ifdef LIN_ENABLE_SAVE_CONFIG */
         break;

      case(0xB7u):
         /* Assign Frame ID range */
#ifdef    LIN_ENABLE_ASSIGN_FRAME_ID_RANGE
         if (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u)
         {
            for(loc_i = 4u; loc_i > 0u; loc_i--)
            {
               if(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[loc_i] != 0xFFu)
               {
                  loc_temp = loc_i - 1u;
                  break;
               }
            }
            if((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] + loc_temp) >= LIN_NUMBER_OF_FRAMES)
            {
            }
            else
            {
               l_u8 loc_index;
               loc_index = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0];
               for(loc_i = 0u; loc_i <= loc_temp; loc_i++)
               {
                  g_lin_frame_ctrl[loc_index+loc_i].frame.pid = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[loc_i + 1u];
               }
            }
         }
#endif /* end #ifdef LIN_ENABLE_ASSIGN_FRAME_ID_RANGE */
         break;

      default:
         break;
   } /* end switch */
}
#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 */

/* ---------------------------------------------------------------------------
 *  void ld_config_task(void);
 * --------------------------------------------------------------------------*/
/**
   @brief  This function handles the timout monitoring
   @note   This function is called via the cyclic task in the application
           (ld_task).
   @pre    LIN driver initialized
   @param  void
   @return void
*/

#if ((!defined(LIN_USER_DEF_DIAG_ENABLE)) && (!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_PROTOCOL_VERSION_1_3)))
void ld_config_task(void)
{
#ifdef J2602_SLAVE
   l_u8 ret_val = 0u;
#endif /*end #ifdef J2602_SLAVE */

   /* timeout added */
   if(g_lin_diag_config_active)
   {
      g_lin_tp_timeoutcounter++;
      if(g_lin_tp_timeoutcounter > (l_u16)LIN_TP_COUNTER_MAX)
      {
         g_lin_diag_config_active = 0u;
#ifdef J2602_SLAVE
         /* If a previous master request had allowed a slave response, but that response was never fetched
          * a slave response must be forbidden - if it is eventually allowed, this flag will be set later. */
         g_j2602_status.flags.sresp = 0u;
         g_j2602_status.flags.tx_resp = 0u;
#endif /*end #ifdef J2602_SLAVE */
      }
   }
   else
   {
      g_lin_tp_timeoutcounter = 0u;
   }

   if(g_lin_rx_ctrl.flag.rx_diag == 1u)
   {
      g_lin_rx_ctrl.flag.rx_diag = 0u;

#ifdef LIN_PROTOCOL_VERSION_2_1
      /* reset pending slave responses if no functional NAD was sent */
      if (LIN_DIAG_RECEIVE_FRAME.frame.NAD != LIN_FUNCTIONAL_NAD)
#endif /* ifdef LIN_PROTOCOL_VERSION_2_1 */
      {
         g_lin_diag_config_active = 0u;
      }

      /* check if NAD is valid */
      if((LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.NAD) ||
         (LIN_DIAG_RECEIVE_FRAME.frame.NAD == LIN_BROADCAST_NAD) ||
         (LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.Initial_NAD))
      {
         if((LIN_DIAG_RECEIVE_FRAME.frame.PCI & 0xF0u)==SINGLE_FRAME)
         {  /* check if config */
            if((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID >= 0xB0u) && (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID <=0xB7u))
            {
               lin_handle_config();
            }
#ifdef J2602_SLAVE
            else
            {
               ret_val = j2602_callback_configuration_request(LIN_DIAG_RECEIVE_FRAME.byte, LIN_DIAG_SEND_FRAME.byte);
            }
#else
            /* If user defined diagnostics is active, handle config, but do not set negative response */
            else
            {
               LIN_DIAG_SEND_FRAME.frame.NAD = g_lin_prod_id.NAD;
               LIN_DIAG_SEND_FRAME.frame.PCI = (SINGLE_FRAME | 0x03u);
               LIN_DIAG_SEND_FRAME.frame.type.SF.SID = (NEG_RESPONSE_RSID);
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[0] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[1] = NRC_FunctionNotSupported;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[2] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[3] = 0xFFu;
               LIN_DIAG_SEND_FRAME.frame.type.SF.diag_data[4] = 0xFFu;
            }
#endif /*end #ifdef J2602_SLAVE */
         }
#ifdef J2602_SLAVE
         else
         {
            ret_val = j2602_callback_configuration_request(LIN_DIAG_RECEIVE_FRAME.byte, LIN_DIAG_SEND_FRAME.byte);
         }
#endif /*end #ifdef J2602_SLAVE */
      }
      else
      {
         /* for the sleep */
         if(LIN_DIAG_RECEIVE_FRAME.frame.NAD == 0x0u)
         {
            g_lin_status_word.flag.last_pid = g_lin_active_pid;
            g_lin_status_word.flag.successful_transfer = L_SET;
            g_lin_status_word.flag.goto_sleep = L_SET;
         }
         else
         {
#ifdef LIN_PROTOCOL_VERSION_2_1
            if(LIN_DIAG_RECEIVE_FRAME.frame.NAD == LIN_FUNCTIONAL_NAD)
            {
               if((LIN_DIAG_RECEIVE_FRAME.frame.PCI & 0xF0u)==SINGLE_FRAME)
               {  /* check if config */
                  if((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID >= 0xB0u) && (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID <=0xB7u))
                  {
                     lin_handle_FunctionalNAD();
                  }
               }
            }
            else
            {
               /* reset pending slave responses */
               g_lin_diag_config_active = 0u;
            }
#else
            /* reset pending slave responses */
            g_lin_diag_config_active = 0u;
#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 */
#ifdef J2602_SLAVE
            ret_val = j2602_callback_configuration_request(LIN_DIAG_RECEIVE_FRAME.byte, LIN_DIAG_SEND_FRAME.byte);
#endif /*end #ifdef J2602_SLAVE */
         }
      }
   }
#ifdef J2602_SLAVE
   if (g_lin_rx_ctrl.flag.rx_config == 1u)
   {
      g_lin_rx_ctrl.flag.rx_config = 0u;
      ret_val = j2602_callback_configuration_request(j2602_config_frame.byte, LIN_DIAG_SEND_FRAME.byte);
   }
   if (ret_val == LD_RESPONSE)
   {
      j2602_allow_slaveresponse();
   }
#endif /*end #ifdef J2602_SLAVE */
}
#endif /* end #if ((!defined(LIN_USER_DEF_DIAG_ENABLE)) && (!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_PROTOCOL_VERSION_1_3))) */

#ifdef LIN_USER_DEF_DIAG_MANDSERV_ENABLE
void ld_config_task(void)
{
#ifdef J2602_SLAVE
   l_u8 ret_val;
#endif /*end #ifdef J2602_SLAVE */
#ifdef LIN_USER_DEF_DIAG_ENABLE
   if(g_lin_user_diag_active)
   {
      g_lin_tp_timeoutcounter++;
      if(g_lin_tp_timeoutcounter > LIN_TP_COUNTER_MAX)
      {
         g_lin_user_diag_active = 0u;
      }
   }
   else
   {
      g_lin_tp_timeoutcounter = 0u;
   }
#endif /* end #ifdef LIN_USER_DEF_DIAG_ENABLE */

   if(g_lin_rx_ctrl.flag.rx_diag == 1u)
   {
      /* Reset pending slave responses after receiving a new master request, appropriate flags will be set afterwards again */
#ifdef LIN_PROTOCOL_VERSION_2_1
      /* reset pending slave responses if no functional NAD was sent */
      if (LIN_DIAG_RECEIVE_FRAME.frame.NAD != LIN_FUNCTIONAL_NAD)
#endif /* ifdef LIN_PROTOCOL_VERSION_2_1 */
      {
         g_lin_user_diag_active = 0u;
#ifdef J2602_SLAVE
        /* If a previous master request had allowed a slave response, but that response was never fetched
         * a slave response must be forbidden - if it is eventually allowed, this flag will be set later. */
        g_j2602_status.flags.sresp = 0u;
        g_j2602_status.flags.tx_resp = 0u;
#endif /*end #ifdef J2602_SLAVE */
      }

      /* check if NAD is valid */
      if((LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.NAD) ||
         (LIN_DIAG_RECEIVE_FRAME.frame.NAD == LIN_BROADCAST_NAD) ||
         (LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_prod_id.Initial_NAD))
      {
         if((LIN_DIAG_RECEIVE_FRAME.frame.PCI & 0xF0u)==SINGLE_FRAME)
         {  /* check if mandatory service */
#ifdef LIN_PROTOCOL_VERSION_2_1
            if(((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID == 0xB2u) && (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == 0x00u)) || (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID ==0xB7u))
#endif /* end #ifdef LIN_PROTOCOL_VERSION_2_1 */
#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined J2602_PROTOCOL))
            if(((LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID == 0xB2u) && (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0] == 0x00u)) || (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID ==0xB1u))
#endif /* end #if defined LIN_PROTOCOL_VERSION_2_0 || defined J2602_PROTOCOL */
            {
               lin_handle_config();
               g_lin_rx_ctrl.flag.rx_diag = 0u;
            }
         }
      }
   }
#ifdef J2602_SLAVE
   if (g_lin_rx_ctrl.flag.rx_config == 1u)
   {
      g_lin_rx_ctrl.flag.rx_config = 0u;
      ret_val = j2602_callback_configuration_request(j2602_config_frame.byte, LIN_DIAG_SEND_FRAME.byte);
      if (ret_val == LD_RESPONSE)
      {
         j2602_allow_slaveresponse();
      }
   }
#endif /*end #ifdef J2602_SLAVE */
}
#endif /* #ifdef LIN_USER_DEF_DIAG_MANDSERV_ENABLE */

#if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1))
/* ---------------------------------------------------------------------------
 *  void lin_diag_rx_chksum_error(void);
 * --------------------------------------------------------------------------*/
/**
   @brief  This function handles checksum errors in master requests.
   @note   This function is called from the HAL when an error was detected.
   @pre    LIN driver initialized
   @param  void
   @return void
*/
void lin_diag_rx_chksum_error(void)
{
#ifdef LIN_COOKED_API
   g_lin_tp_tx_rx_ctrl.tx_on = 0u;
   g_lin_tp_tx_rx_ctrl.tx_t_out_on = 0u;
#endif /* end #ifdef LIN_COOKED_API */

#ifdef LIN_RAW_API
   lin_diag_raw_tx_read_idx = lin_diag_raw_tx_write_idx;
#endif /* end #ifdef LIN_RAW_API */

#if ((!defined(LIN_DIAG_ENABLE)) && (!defined(LIN_USER_DEF_DIAG_ENABLE)))
   g_lin_diag_config_active = 0x0u;
#endif /* end #ifndef LIN_DIAG_ENABLE */

#ifdef LIN_USER_DEF_DIAG_ENABLE
   g_lin_user_diag_active = 0x0u;
#endif /* end #ifdef LIN_USER_DEF_DIAG_ENABLE */
}
#endif /* #if ((defined LIN_PROTOCOL_VERSION_2_0) || (defined LIN_PROTOCOL_VERSION_2_1)) */

#if ((defined LIN_MASTER)||(defined J2602_MASTER))
#ifndef LIN_PROTOCOL_VERSION_1_3

void lin_handle_config_response(void)
{
//   l_u8 loc_i = 0u;
//   l_u8 loc_slave = LIN_NUMBER_OF_SLAVES;

//   if (g_lin_config_status == LD_RESPONSE_FINISHED)
//   {
//      g_lin_config_status = LD_SERVICE_IDLE;
//      if ((LIN_DIAG_RECEIVE_FRAME.frame.NAD == g_lin_config_buffer[0])
//              || (LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID == 0x7Fu))
//      {
//         /* Search for slave for which configuration was done */
//         /* Search for initial NAD (assign NAD) */
//         if ( g_lin_config_buffer[2] == 0xB0u)
//         {
//            for (loc_i = 0u; loc_i < LIN_NUMBER_OF_SLAVES; loc_i++)
//            {
//               if (l_node_diag_info_list[loc_i]->data.init_NAD ==  g_lin_config_buffer[0])
//               {
//                  loc_slave = loc_i;
//                  loc_i = LIN_NUMBER_OF_SLAVES;
//               }
//            }
//         }

//         switch(LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID)
//         {
//            /* Assign NAD Response */
//            case(0xF0):
//               if ((g_lin_config_buffer[2] == 0xB0u) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u))
//               {
//                  if (loc_slave < LIN_NUMBER_OF_SLAVES)
//                  {
//                     l_node_diag_info_list[loc_slave]->data.conf_NAD =  g_lin_config_buffer[1];
//                     loc_slave = LIN_NUMBER_OF_SLAVES;
//                  }
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 0u;
//               }
//               else
//               {
//                  /* Set error */
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 1u;
//               }
//               break;

//            /* Assign Frame ID */
//            case(0xF1):
//               if ((g_lin_config_buffer[2] == 0xB1u) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x06u))
//               {
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 0u;
//               }
//               else
//               {
//                  /* Set error */
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 1u;
//               }
//               break;

//            /* Read by ID */
//            case(0xF2):
//               if ((g_lin_config_buffer[2] == 0xB2u)
//                       && ((g_lin_config_rbid_data) != 0u)
//                       && (LIN_DIAG_RECEIVE_FRAME.frame.PCI > 2u)
//                       && (LIN_DIAG_RECEIVE_FRAME.frame.PCI <= 6u))
//               {
//                  /* copy the received data into the buffer for the application */
//                  for (loc_i = 0u; loc_i < 5u; loc_i++)
//                  {
//                    (*g_lin_config_rbid_data)[loc_i] = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[loc_i];
//                  }
//                  /* (MISRA-C:2004 Non MISRA Warning, e661) Possible access of out-of-bounds pointer (1 beyond end of data) by operator '['
//                   * Reason: This code part will only be executed if the slave is part of the available list and therefore the value range
//                   * is valid. */
//                  l_node_diag_info_list[loc_slave]->data.conf_NAD =  g_lin_config_buffer[1]; /*lint !e661*/
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 0u;
//               }
//               else
//               {
//                  /* Set error */
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 1u;
//               }
//               break;

//            /* Conditional Change NAD */
//            case(0xF3):
//               if ((g_lin_config_buffer[2] == 0xB3u) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u))
//               {
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 0u;
//               }
//               else
//               {
//                  /* Set error */
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 1u;
//               }
//               break;

//            /* F4 - Data Dump */
//            /* Configuration Service not available in master node -> Realize via Diagnostic API */

//            /* F5 - SNPD not available */

//            /* Save Configuration */
//            case(0xF6):
//               if ((g_lin_config_buffer[2] == 0xB6u) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u))
//               {
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 0u;
//               }
//               else
//               {
//                  /* Set error */
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 1u;
//               }
//               break;

//            /* Assign Frame ID Range */
//            case(0xF7):
//               if ((g_lin_config_buffer[2] == 0xB7u) && (LIN_DIAG_RECEIVE_FRAME.frame.PCI == 0x01u))
//               {
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 0u;
//               }
//               else
//               {
//                  /* Set error */
//                  g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.SID;
//                  g_ld_NodeConfErr = 1u;
//               }
//               break;

//            /* Service not supported */
//            case(0x7F):
//               g_ld_NodeConfRSID = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[0];
//               g_ld_NodeConfErr = LIN_DIAG_RECEIVE_FRAME.frame.type.SF.diag_data[1];
//               /* Enhance timeout, reset state to request finished. Slave needs more time
//                * to complete the node configuration service */
//               if ((g_ld_NodeConfErr == 0x78u) && (g_ld_timer_p2 != 0xFFFFu))
//               {
//                  g_ld_timer_p2 = 0u;
//                  g_lin_config_status = LD_REQUEST_FINISHED;
//               }
//               loc_slave = LIN_NUMBER_OF_SLAVES;
//               break;

//            default:
//            break;
//         }
//      }
//   }
   return;
}

#endif /* ifdef LIN_PROTOCOL_VERSION_1_3 */
#endif /* #if ((defined LIN_MASTER)||(defined J2602_MASTER)) */
