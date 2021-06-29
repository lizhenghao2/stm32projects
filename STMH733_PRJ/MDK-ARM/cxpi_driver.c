#include "cxpi_driver.h"
#include "type.h"
#include "usart.h"

static byte MasterEnabled;

static byte FrameTimeCount;

static byte FrameIndex;

static byte TableTimeCount;
static byte const CXPIFrameTimeoutValue[9] = {2u, 2u, 2u, 3u, 3u, 4u, 4u, 4u, 5u};
static void CXPI_ISR(void);
static byte SCI0SR1;
static byte SCI0ASR1;
static byte SCI0DRL=0x55u;
static byte CXPIRESET;
static byte FramePub;
static byte FrameSub;


void CXPI_Goto_Idle(void);
void CXPI_State(void)
{
	if(CXPIState!=ReceiveIdle)
		{
			switch(CXPIState)
			{
				case ReceivePTYPE:
					HAL_UART_Transmit(&huart10, PTYPEfield, 1, 99);
					CXPIState=ReceivePID;
					break;
				case ReceivePID:
					if(event==0)
					{
						HAL_UART_Transmit(&huart3, PID+CDFcount, 1, 99);
						CDFcount++;
						if(CDFcount==CDFmax)
						{
						CDFcount=0;
						}
						DataToBeSent[0]=CDFData[CDFcount].FrameInfo;//复制帧信息
						for(short int i=0;i<12;i++)//复制帧长度
						{
							DataToBeSent[i+1]=CDFData[CDFcount].FrameData[i];
						}
						DataToBeSent[13]=CDFData[CDFcount].FrameCheck;//复制crc
						DataPointer=DataToBeSent;
						CXPIState=ReceiveFrameInfo;
					}
					else
					{
						HAL_UART_Transmit(&huart10,TxBuffer, 1, 99);
						for(int i=0;i<14;i++)
						{
							DataToBeSent[i]=EventData[i];
						}						
						event=0;
						DataPointer=DataToBeSent;
						CXPIState=ReceiveFrameInfo;
					}
					break;
				
				case ReceiveFrameInfo: 
					HAL_UART_Transmit(&huart10,DataPointer, 1, 99);
					Framelength=*DataPointer>>4;
					DataPointer++;
					CXPIState=ReceiveData;
					break;
				case ReceiveData:
					if(RxByteCount==Framelength)
					{
						CXPIState=ReceiveCheck;
						DataPointer=DataToBeSent+13;
						break;
					}
					else
					{
						HAL_UART_Transmit(&huart10, DataPointer, 1, 99);
						DataPointer++;
						RxByteCount++;
					}
					break;
				case ReceiveCheck:
					HAL_UART_Transmit(&huart10,DataToBeSent+13 , 1, 99);
					CXPI_Goto_Idle();
					break;
				default: break;
			}			
			//CXPIState=ReceiveIdle;
		}
}
void CXPI_ISR(void)
{
	volatile byte sci_reg_sr1;
  volatile byte sci_reg_drl;
  volatile byte sci_reg_asr1;
  byte tempbyte;
  sci_reg_sr1 = SCI0SR1;
  sci_reg_asr1 = SCI0ASR1;
  sci_reg_drl = SCI0DRL;
	//CXPIState=ReceiveFrameInfo;//已周期性发送PID
	if(1){
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
        DataReceived[13] = sci_reg_drl;
        /* Clear byte count value */
        RxByteCount = CXPIRESET;
        /* Wait for data */
        CXPIState = ReceiveData;
        if(FramePub == DataToBeSent[13])
        {
          /* Send Data 0 */
          SCI0DRL = DataToBeSent[1];
        }
        else if(FrameSub == DataToBeSent[13])
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
				Framelength=DataToBeSent[0]>>4;
				//DataReceived[0]= ;
				CXPIState=ReceiveData;
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
          if(FramePub == DataToBeSent[13])
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
        /* Enter idle state */ 
        CXPI_Goto_Idle();  
        break;
      default:
        break;
    }      
  }
};

void CXPI_Goto_Idle(void)
{
  /* Clear frame time count value */
  //FrameTimeCount = CXPIRESET;
  /* Clear byte count value */
  RxByteCount = 0;
  /* Go to idle state */
  CXPIState = ReceiveIdle;
	DataPointer=DataToBeSent;
}