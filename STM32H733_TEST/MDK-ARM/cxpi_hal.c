#include "type.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "cxpi_hal.h"

static byte MasterEnabled;
static byte CXPIState;
static byte FrameTimeCount;
static byte RxByteCount;
/* 0:Len, 1-12:byte0-11, 13:Check, 14:PID */
static byte DataReceived[15];
/* 0:Len, 1-12:byte0-11, 13:Check, 14:PID */
static byte DataToBeSent[16];
static byte FrameIndex;
static byte TableTimeCount;
static byte const CXPIFrameTimeoutValue[9] = {2u, 2u, 2u, 3u, 3u, 4u, 4u, 4u, 5u};
static void CXPI_ISR(void);
static byte SCI0SR1;
static byte SCI0ASR1;
static byte SCI0DRL=0x55u;
static byte CXPIRESET=0;
static byte FramePub;
static byte FrameSub;

static void CXPI_Goto_Idle(void);
void CXPI_ISR(void)
{
	volatile byte sci_reg_sr1;
  volatile byte sci_reg_drl;
  volatile byte sci_reg_asr1;
  byte tempbyte;
  sci_reg_sr1 = SCI0SR1;
  sci_reg_asr1 = SCI0ASR1;
  sci_reg_drl = SCI0DRL;
	if(1){
		ifsstat=notIFS;
    switch(CXPIState)
    {
      /* Receiving Sync byte */      
      case ReceiveSync:
        if(0x55u == sci_reg_drl)
        {
          /* Send PID */
          SCI0DRL = DataToBeSent[11];
          /* Wait for PID */
          CXPIState = ReceivePID;
          /* Copy len byte to receive buffer */
          DataReceived[0] = DataToBeSent[0];
        }
        else
        {
          /* Enter idle state */ 
          CXPI_Goto_Idle();  
        }
        break;
      /* Receiving PID */      
      case ReceivePID: 
        /* Copy PID to receive buffer */
        DataReceived[14] = sci_reg_drl;
        /* Clear byte count value */
        RxByteCount = CXPIRESET;
        /* Wait for data */
        CXPIState = ReceiveData;
        if(FramePub == DataToBeSent[15])
        {
          /* Send Data 0 */
          SCI0DRL = DataToBeSent[1];
        }
        else if(FrameSub == DataToBeSent[15])
        {
          /* Set frame timeout value */
          FrameTimeCount = CXPIFrameTimeoutValue[DataToBeSent[0]];
        }
        else
        {
          /* Enter idle state */ 
          CXPI_Goto_Idle();  
        }
        break;
      /* Receiving data */
			case ReceiveFrameInfo:
				break;
      case ReceiveData:
        /* Increase byte count value */
        RxByteCount++;
        /* Copy data to receive buffer */
        DataReceived[RxByteCount] = sci_reg_drl;
        if(RxByteCount < DataReceived[0])
        {
          if(FramePub == DataToBeSent[12])
          {
            /* Send Data x */
            SCI0DRL = DataToBeSent[RxByteCount+1];
          }
          else if(FrameSub == DataToBeSent[12])//Direct
          {
            /* Set frame timeout value */
            FrameTimeCount = CXPIFrameTimeoutValue[DataToBeSent[0]];
          }
          else
          {
            /* Enter idle state */ 
            CXPI_Goto_Idle();  
          }
        }
        else
        {
          /* Wait for checksum */
          CXPIState = ReceiveCheck;
          if(FramePub == DataToBeSent[15])
          {
            /* Send checksum */
            SCI0DRL = DataToBeSent[13];
          }
          else if(FrameSub == DataToBeSent[12])
          {
            /* Set frame timeout value */
            FrameTimeCount = CXPIFrameTimeoutValue[DataToBeSent[0]];
          }
          else
          {
            /* Enter idle state */ 
            CXPI_Goto_Idle();  
          }
        }
        break;
      /* Receiving checksum */
      case ReceiveCheck:
        /* Copy check to receive buffer */
        DataReceived[13] = sci_reg_drl;
        /* Check checksum byte */ 
        //tempbyte = CXPI_CalC_Check(&DataReceived[0], DataReceived[11], DataToBeSent[10]);
        if(tempbyte != sci_reg_drl)
        //if(sci_reg_drl != DataToBeSent[9])
        {
          /* Set CXPI error state */ 
          //DataReceived[10] = CXPICheckError;
        }
        /* Send CXPI data to GUI */ 
        //Send_Data_To_GUI();
        /* Enter idle state */ 
        CXPI_Goto_Idle();  
        break;
      default:
        break;
    }      
  }
};

static void CXPI_Goto_Idle(void)
{
  /* Clear frame time count value */
  FrameTimeCount = CXPIRESET;
  /* Clear byte count value */
  RxByteCount = CXPIRESET;
  /* Go to idle state */
  CXPIState = ReceiveIdle;
	ifsstat=isIFS;
	//TIM13->Instance->CNT = 0;
}