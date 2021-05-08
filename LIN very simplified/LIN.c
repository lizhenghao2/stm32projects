#include "Headers.h"
#include "motor.h"
static byte MasterEnabled;
static byte LINState;
static byte FrameTimeCount;
static byte RxByteCount;

/* 0:Len, 1-8:byte0-7, 9:Check, 10:Error, 11:PID */
static byte DataReceived[12];
/* 0:Len, 1-8:byte0-7, 9:Check, 10:CheckType, 11:PID, 12:Direct */
static byte DataToBeSent[13]; 
static byte FrameIndex;
static byte TableTimeCount;
byte DateCoped[2];  //0:MD ,1:MR
byte DateCoped_CN300[2]; //0:MD ,1:MR

/* Frame Timeout Value */
static byte const LINFrameTimeoutValue[9] = {2u, 2u, 2u, 3u, 3u, 4u, 4u, 4u, 5u};

byte MQB373_Action_State = Stop;
byte Action=Stop;
byte MRAction = Stop;
byte Motor_Position=0;//天窗电机位置
byte Motor_Position_R = 0;
byte Aim_Position[7]={0x00,0x8F,0x9E,0xB2,0xC1,0xD0,0xE4};//天窗目标位置
byte Aim_Position_SP108_Auto[7]={0x00,0x64,0x9E,0xB2,0xC1,0xD0,0xE4};//天窗目标位置

byte Aim_Position_CN300[7]={0x00,0x64,0x8B,0x9C,0xA9,0xB6,0xC8};//天窗目标位置
byte Motor_Position_CN300_R = 0;


extern byte MQB373_Self_Learn;
extern byte MQB371_Self_Learn;
extern byte C_Class_Self_Learn;
extern byte YETI_Self_Learn_MD;
extern byte YETI_Self_Learn_MR;
extern byte motor_needs_self_learn;
extern byte MQB373_switch_position;

//static void LIN_ISR(void);
void LIN_ISR(void);
void LIN_Timer_ISR(void);


static void LIN_Goto_Idle(void);
static byte LIN_CalC_Check(byte* buffer, byte pid, byte checktype);  
static void Send_Data_To_GUI(void);

void LIN_Init(void){
  /* Disable use of alternative registers */ 
  SCI0SR2 = 0x00u; 
  /* Set baud rate 19200 = 48000000/16/156 */
  SCI0BD = 0x9Cu;
  /* SCI enabled in wait mode */
  SCI0CR1 = 0x00u;
  /* Enable use of alternative registers and 13bit breaks for LIN */ 
  SCI0SR2 = (SCI0SR2_AMAP_MASK|SCI0SR2_BRK13_MASK);
  /* Enable break detect interrupt */ 
  SCI0ACR1 = SCI0ACR1_BKDIE_MASK;
  /* Enable break detect circuit */  
  SCI0ACR2 = SCI0ACR2_BKDFE_MASK;
  /* Enable SCI Channel and interrupt */
  SCI0CR2 = (SCI0CR2_RE_MASK|SCI0CR2_TE_MASK|SCI0CR2_RIE_MASK);
  
  /* Enable sending table in 200*500us = 100ms */ 
  TableTimeCount = 0xC8;

  /* Send frame index 0 */ 
  FrameIndex = LINRESET; 
  /* Disable master enable state */ 
  MasterEnabled = False;
  
  /* Enter idle state */ 
  LIN_Goto_Idle();
}    

/* Define a specific segment to store the code */ 
void LIN_ISR(void)
{
  volatile byte sci_reg_sr1;
  volatile byte sci_reg_drl;
  volatile byte sci_reg_asr1;
  byte tempbyte;
  sci_reg_sr1 = SCI0SR1;
  sci_reg_asr1 = SCI0ASR1;
  sci_reg_drl = SCI0DRL;
  /* BREAK DETECTED */
  if(LINRESET != (sci_reg_asr1&SCI0ASR1_BKDIF_MASK))
  {
    /* Clear break detection flag */ 
    SCI0ASR1 = SCI0ASR1_BKDIF_MASK; 
    if(True == MasterEnabled)
    {
      /* Send SYN */
      SCI0DRL = 0x55u;
      /* Wait for sync */
      LINState = ReceiveSync; 
    }
    else
    {
      /* Enter idle state */ 
      LIN_Goto_Idle();  
    }
    return;
  }
  /* FRAME ERROR DETECTED */
  if(LINRESET != (sci_reg_sr1&SCI0SR1_FE_MASK))
  {
    switch(LINState)
    {
      /* Receiving data */
      case ReceiveData:
        RxByteCount++;
        /* Copy data to receive buffer */
        DataReceived[RxByteCount] = sci_reg_drl;
        /* Set right data len */
        DataReceived[0] = RxByteCount; 
        /* Set LIN error state */ 
    	  DataReceived[10] = LINFramingError; 
        /* Send LIN data to GUI */ 
        Send_Data_To_GUI();
        break;
      /* Receiving check */
      case ReceiveCheck:
        /* Copy check to receive buffer */
        DataReceived[9] = sci_reg_drl;
        /* Set LIN error state */ 
    	  DataReceived[10] = LINFramingError; 
        /* Send LIN data to GUI */ 
        Send_Data_To_GUI();
        break;
      default: ;
    }
    /* Enter idle state */ 
    LIN_Goto_Idle();  
    return;
  }
  /* BIT ERROR DETECTED */  
  if(LINRESET != (sci_reg_asr1&SCI0ASR1_BERRIF_MASK))
  {
    /* Clear bit error flag */ 
	  SCI0ASR1 = SCI0ASR1_BERRIF_MASK; 
    switch(LINState)
    {
      /* Receiving data */
      case ReceiveData:
        RxByteCount++;
        /* Copy data to receive buffer */
        DataReceived[RxByteCount] = sci_reg_drl;
        /* Set right data len */
        DataReceived[0] = RxByteCount; 
        /* Set LIN error state */ 
    	  DataReceived[10] = LINDataError; 
        /* Send LIN data to GUI */ 
        Send_Data_To_GUI();
        break;
      /* Receiving check */
      case ReceiveCheck:
        /* Copy check to receive buffer */
        DataReceived[9] = sci_reg_drl;
        /* Set LIN error state */ 
    	  DataReceived[10] = LINDataError; 
        /* Send LIN data to GUI */ 
        Send_Data_To_GUI();
        break;
      default: ;
    }
    /* Enter idle state */ 
    LIN_Goto_Idle();  
	  return;
  }
  /* BYTE RECIEVED */
  if(LINRESET != (sci_reg_sr1&SCI0SR1_RDRF_MASK)){
    switch(LINState)
    {
      /* Receiving Sync byte */      
      case ReceiveSync:
        if(0x55u == sci_reg_drl)
        {
          /* Send PID */
          SCI0DRL = DataToBeSent[11];
          /* Wait for PID */
          LINState = ReceivePID;
          /* Copy len byte to receive buffer */
          DataReceived[0] = DataToBeSent[0];
        }
        else
        {
          /* Enter idle state */ 
          LIN_Goto_Idle();  
        }
        break;
      /* Receiving PID */      
      case ReceivePID: 
        /* Copy PID to receive buffer */
        DataReceived[11] = sci_reg_drl;
        /* Clear byte count value */
        RxByteCount = LINRESET;
        /* Wait for data */
        LINState = ReceiveData;
        if(FramePub == DataToBeSent[12])
        {
          /* Send Data 0 */
          SCI0DRL = DataToBeSent[1];
        }
        else if(FrameSub == DataToBeSent[12])
        {
          /* Set frame timeout value */
          FrameTimeCount = LINFrameTimeoutValue[DataToBeSent[0]];
        }
        else
        {
          /* Enter idle state */ 
          LIN_Goto_Idle();  
        }
        break;
      /* Receiving data */
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
          else if(FrameSub == DataToBeSent[12])
          {
            /* Set frame timeout value */
            FrameTimeCount = LINFrameTimeoutValue[DataToBeSent[0]];
          }
          else
          {
            /* Enter idle state */ 
            LIN_Goto_Idle();  
          }
        }
        else
        {
          /* Wait for checksum */
          LINState = ReceiveCheck;
          if(FramePub == DataToBeSent[12])
          {
            /* Send checksum */
            SCI0DRL = DataToBeSent[9];
          }
          else if(FrameSub == DataToBeSent[12])
          {
            /* Set frame timeout value */
            FrameTimeCount = LINFrameTimeoutValue[DataToBeSent[0]];
          }
          else
          {
            /* Enter idle state */ 
            LIN_Goto_Idle();  
          }
        }
        break;
      /* Receiving checksum */
      case ReceiveCheck:
        /* Copy check to receive buffer */
        DataReceived[9] = sci_reg_drl;
        /* Check checksum byte */ 
        tempbyte = LIN_CalC_Check(&DataReceived[0], DataReceived[11], DataToBeSent[10]);
        if(tempbyte != sci_reg_drl)
        //if(sci_reg_drl != DataToBeSent[9])
        {
          /* Set LIN error state */ 
          DataReceived[10] = LINCheckError;
        }
        /* Send LIN data to GUI */ 
        Send_Data_To_GUI();
        /* Enter idle state */ 
        LIN_Goto_Idle();  
        break;
      default:
        break;
    }      
  }
}
byte action = 0;
void LIN_Timer_ISR(void)
{
  byte tempbyte;

  if(LINRESET != TableTimeCount)
  {
    TableTimeCount--;
    if(LINRESET == TableTimeCount)
    {
      /* Read master enable state */ 
      MasterEnabled = LDFManager.LIN_Master_Enable;
      /* Check master enabled */
      if(True == MasterEnabled)
      {
        /* Check LDF file */
        if(FrameIndex < LDFManager.LDFTagNum)
        {
          /* Send break */
          SCI0CR2 |= LINSET;  
          SCI0CR2 &= (byte)(~LINSET); 
          /* Wait for break */
          LINState = ReceiveBreak;
          
          /* Prepare data to be sent */
          if (( SAD_Status.Bits.Vehicle_Type == MQB373)||( SAD_Status.Bits.Vehicle_Type == AUDI_S6)||( SAD_Status.Bits.Vehicle_Type == AUDI_C7)||( SAD_Status.Bits.Vehicle_Type == C_Class)) 
          {
            DataToBeSent[0] = LDFData_MQB373[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_MQB373[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_MQB373[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_MQB373[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_MQB373[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
            TableTimeCount = (LDFData_MQB373[FrameIndex].FrameDelay) <<1u;
            if(DataToBeSent[11]==0x50)
            {
              
              if ( Get_Mode_Switch() == MODE_MANUAL ){  // manual mode uses LIN control                
                 if(MQB373_Self_Learn == TRUE){
                   DataToBeSent[5] |= 0x01;
                 }else{               
                   if(Action==Close)
                   {
                     DataToBeSent[4] = ((DataToBeSent[4]&0xEC)|0x10);
                     //DataToBeSent[9] = 0x0C;
                   } 
                   else if(Action==Popup)
                   {
                     DataToBeSent[4] = ((DataToBeSent[4]&0xEC)|0x01);
                     //DataToBeSent[9] = 0x1B;
                   } 
                   else if(Action==Open)
                   {
                     DataToBeSent[4] = ((DataToBeSent[4]&0xEC)|0x02);
                     //DataToBeSent[9] = 0x1A;
                   }
                   else if(Action==Stop)
                   {
                     DataToBeSent[4] = (DataToBeSent[4]&0xEC);
                     //DataToBeSent[9] = 0x1C;
                   }
                 }
                DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);     
              }
            }
          } 
          else if ( SAD_Status.Bits.Vehicle_Type == H20) 
          {
            DataToBeSent[0] = LDFData_H20[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_H20[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_H20[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_H20[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_H20[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
            TableTimeCount = (LDFData_H20[FrameIndex].FrameDelay) <<1u;
          }  
          else  if (( SAD_Status.Bits.Vehicle_Type == MQB371 )||( SAD_Status.Bits.Vehicle_Type == MODEL_Z )) 
          {
            DataToBeSent[0] = LDFData_MQB[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_MQB[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_MQB[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_MQB[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_MQB[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
            TableTimeCount = (LDFData_MQB[FrameIndex].FrameDelay) <<1u;
            if(DataToBeSent[11]==0x50)
            {  
              if ( Get_Mode_Switch() == MODE_MANUAL ){  // manual mode uses LIN control                
                 if(MQB371_Self_Learn == TRUE){
                   DataToBeSent[5] |= 0x01;
                 }else{
                   if(Action==Close)
                   {
                     DataToBeSent[4] = ((DataToBeSent[4]&0xEC)|0x10);
                     //DataToBeSent[9] = 0x0C;
                   } 
                   else if(Action==Popup)
                   {
                     DataToBeSent[4] = ((DataToBeSent[4]&0xEC)|0x01);
                     //DataToBeSent[9] = 0x1B;
                   } 
                   else if(Action==Open)
                   {
                     DataToBeSent[4] = ((DataToBeSent[4]&0xEC)|0x02);
                     //DataToBeSent[9] = 0x1A;
                   }
                   else if(Action==Stop)
                   {
                     DataToBeSent[4] = (DataToBeSent[4]&0xEC);
                     //DataToBeSent[9] = 0x1C;
                   }
                 }
                 DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
              }
            }
            
          } 
          else if ( SAD_Status.Bits.Vehicle_Type == YETI ) 
          {
            DataToBeSent[0] = LDFData_YETI[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_YETI[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_YETI[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_YETI[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_YETI[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);         
            TableTimeCount = (LDFData_YETI[FrameIndex].FrameDelay) <<1u;
            
            if(DataToBeSent[11]==0x50)
            { 
              if( Get_Mode_Switch() == MODE_MANUAL ){  // manual mode uses LIN control                
                 DataToBeSent[4] &= 0xC8;
                 if(YETI_Self_Learn_MD == TRUE || YETI_Self_Learn_MR == TRUE){
                   DataToBeSent[5] |= 0x03;	// MD 0x01,  MR 0x02
                 }else{          
                   if(Action==Close)
                   {
                     DataToBeSent[4] |= 0x10;
                   } 
                   else if(Action==Popup)
                   {
                     DataToBeSent[4] |= 0x01;
                   } 
                   else if(Action==Open)
                   {
                     DataToBeSent[4] |= 0x02;
                   }
                   
                   if(MRAction == Open){
                      DataToBeSent[4] |= 0x04;
                   }else if(MRAction == Close){
                      DataToBeSent[4] |= 0x20;
                   }
                 }
                 DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]); 
              }
            }
          }
          else if (( SAD_Status.Bits.Vehicle_Type == SP108_Servo )||( SAD_Status.Bits.Vehicle_Type == EP21 )) 
          {
            DataToBeSent[0] = LDFData_SP108[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_SP108[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_SP108[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_SP108[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_SP108[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);         
            TableTimeCount = (LDFData_SP108[FrameIndex].FrameDelay) <<1u;
          
            if(DataToBeSent[11]==0x50)
            {
              
              if(SP108_Self_Learn_Denormalization) 
              {
                 DataToBeSent[1] |=0X80;//去初始化Denormalization_SAD ,此处不需要复位，报文数值0x28
               
                 SP108_Self_Learn_Denormalization=False;
                 SP108_Self_Learn_MD_MR=True;
              }
              DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
            }
          }
             /*即使Octavia_Combi基础报文是MQB373里面的，但不能和另外的电机一起调用，貌似会出问题*/
           else if ( SAD_Status.Bits.Vehicle_Type == Octavia_Combi) 
           {
           
            DataToBeSent[0] = LDFData_MQB373[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_MQB373[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_MQB373[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_MQB373[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_MQB373[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
            TableTimeCount = (LDFData_MQB373[FrameIndex].FrameDelay) <<1u;          
           }
           else if ( SAD_Status.Bits.Vehicle_Type == CN300) 
           {
           
            DataToBeSent[0] = LDFData_CN300[FrameIndex].FrameLen;
            for(tempbyte=LINRESET; tempbyte<8u; tempbyte++){
              DataToBeSent[tempbyte+1u] = LDFData_CN300[FrameIndex].FrameData[tempbyte];
            }
            DataToBeSent[10] = LDFData_CN300[FrameIndex].FrameCheckType;
            DataToBeSent[11] = LDFData_CN300[FrameIndex].FramePID;
            DataToBeSent[12] = LDFData_CN300[FrameIndex].FrameDirect;
            DataToBeSent[9] = LIN_CalC_Check(&DataToBeSent[0], DataToBeSent[11], DataToBeSent[10]);
            TableTimeCount = (LDFData_CN300[FrameIndex].FrameDelay) <<1u;          
           }
          
          FrameIndex++;
          if(FrameIndex == LDFManager.LDFTagNum)
          {
            FrameIndex = LINRESET;
          } 
        }
        else
        {
          FrameIndex = LINRESET;
          /* Enable sending table in 200*500us = 100ms */ 
          TableTimeCount = 0xC8; 
        }
      }
      else
      {
        /* Enable sending table in 200*500us = 100ms */ 
        TableTimeCount = 0xC8; 
      }
    }
  }
  if(LINRESET != FrameTimeCount)
  {
    FrameTimeCount--;
    if(LINRESET == FrameTimeCount)
    {
      switch(LINState)
      {
        case ReceiveData: 
        case ReceiveCheck: 
          /* Set right data len */
          DataReceived[0] = RxByteCount;
          /* Receive data timeout occur */
          if(LINRESET == RxByteCount)
          {
            /* Set LIN error state */ 
            DataReceived[10] = LINNoResp;
          }
          else
          {
            /* Set LIN error state */ 
  	        DataReceived[10] = LINTimeout; 
          }
          /* Send LIN data to GUI */ 
          Send_Data_To_GUI();
          /* Enter idle state */ 
          LIN_Goto_Idle();    
          break;
        default:
          break;
      }
    }
  }    
}  

//type: YETI, MQB_TYPE, MQB373, AEntry,AUDI_S6,AUDI_C7      ,
void LIN_Network_Change(byte type)
{
  switch (type) 
  {
    case YETI:
    {
      SAD_Status.Bits.Vehicle_Type = YETI;
      LDFManager.LDFTagNum = 28;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }
    case MQB371:
    {
      SAD_Status.Bits.Vehicle_Type = MQB371;
      LDFManager.LDFTagNum = 33;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }
    case MQB373:
    {
      SAD_Status.Bits.Vehicle_Type = MQB373;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }
    case SUPER_NF: {      
     SAD_Status.Bits.Vehicle_Type = SUPER_NF;
     LDFManager.LDFTagNum = 28;
     LDFManager.LIN_Master_Enable = FALSE;
    break;
    }
    case AUDI_S6:
    {
      SAD_Status.Bits.Vehicle_Type = AUDI_S6;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }

    case AUDI_C7:
    {
      SAD_Status.Bits.Vehicle_Type = AUDI_C7;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }
    
    case AEntry:
    {
      SAD_Status.Bits.Vehicle_Type = AEntry;
      LDFManager.LDFTagNum = 0;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;                                                  
     }    
    case NF_TOURAN: {
     SAD_Status.Bits.Vehicle_Type = NF_TOURAN;
     LDFManager.LDFTagNum = 28;
     LDFManager.LIN_Master_Enable = FALSE;
     break;         
    }
    case PSDB: {
     SAD_Status.Bits.Vehicle_Type = PSDB;
     LDFManager.LDFTagNum = 28;
     LDFManager.LIN_Master_Enable = FALSE;
     break; 
    }
    case ASUV: {
     SAD_Status.Bits.Vehicle_Type = ASUV;
     LDFManager.LDFTagNum = 0;
     LDFManager.LIN_Master_Enable = FALSE;
     break; 
    }
    case ASUV_FULL: {
     SAD_Status.Bits.Vehicle_Type = ASUV_FULL;
     LDFManager.LDFTagNum = 0;
     LDFManager.LIN_Master_Enable = FALSE;
     break; 
    }
     case C_Class:
    {
      SAD_Status.Bits.Vehicle_Type = C_Class;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }
    case MODEL_Z:
    {
      SAD_Status.Bits.Vehicle_Type = MODEL_Z;
      LDFManager.LDFTagNum = 33;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;
    }
    case RY_X:
    {
      SAD_Status.Bits.Vehicle_Type = RY_X;
      LDFManager.LDFTagNum = 0;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;                                                  
     }  
     case H20:
    {
      SAD_Status.Bits.Vehicle_Type = H20;
      LDFManager.LDFTagNum = 3;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;                                                  
     } 
    case Webasto_Y:
    {
      SAD_Status.Bits.Vehicle_Type = Webasto_Y;
      LDFManager.LDFTagNum = 0;
      LDFManager.LIN_Master_Enable = FALSE;
      FrameIndex = LINRESET;
      break;                                                  
     } 
    case SP108_Servo:
    {
      SAD_Status.Bits.Vehicle_Type = SP108_Servo;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;                                                  
     } 
    case Octavia_Combi:
    {
      SAD_Status.Bits.Vehicle_Type = Octavia_Combi;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;                                                  
     }
    case EP21:
    {
      SAD_Status.Bits.Vehicle_Type = EP21;
      LDFManager.LDFTagNum = 7;
      LDFManager.LIN_Master_Enable = True;
      FrameIndex = LINRESET;
      break;                                                  
     } 
     case Sanye:
    {
      SAD_Status.Bits.Vehicle_Type = Sanye;
      LDFManager.LDFTagNum = 0;
      LDFManager.LIN_Master_Enable = FALSE;
      FrameIndex = LINRESET;
      break;                                                  
     } 
    case NMS_THaru: 
    {
     SAD_Status.Bits.Vehicle_Type = NMS_THaru;
     LDFManager.LDFTagNum = 28;
     LDFManager.LIN_Master_Enable = FALSE;
     break; 
    }
    case CN300: 
    {
     SAD_Status.Bits.Vehicle_Type = CN300;
     LDFManager.LDFTagNum = 3;
     LDFManager.LIN_Master_Enable = True;
     break; 
    }
    default: ;
  }
  motor_type_callback((byte)SAD_Status.Bits.Vehicle_Type);
}

static void LIN_Goto_Idle(void)
{
  /* Clear frame time count value */
  FrameTimeCount = LINRESET;
  /* Clear byte count value */
  RxByteCount = LINRESET;
  /* Go to idle state */
  LINState = ReceiveIdle;
}

static byte LIN_CalC_Check(byte* buffer, byte pid, byte checktype)
{
  byte i;
  word tempcheck;
  if(ClassicCheck == checktype)
  {
    tempcheck = LINRESET;
  }
  else
  {
    tempcheck = pid;
  }
  
  for(i=*buffer; i>LINRESET; i--)
  {                     
    buffer++;                                    
    tempcheck += (word)(*buffer);
  }
   
  if(tempcheck > 0x00FFu)
  {
    tempcheck = (tempcheck&0x00FFu)+((tempcheck&0xFF00u)>>8);
  }
  return ((byte)(~tempcheck));                                           
}   

extern void MQB373_Stop_Run(void);
extern void CN300_Stop_Run(void);

extern byte new_step_started;

static void Send_Data_To_GUI(void)
{
  byte tempbyte;
  byte tempbuffer[11];
  byte temp_raise_value = 0;
  if ( SAD_Status.Bits.Vehicle_Type == MQB373)
        {
            temp_raise_value = 0x63;
        }
        else if ( SAD_Status.Bits.Vehicle_Type == AUDI_S6)
        {
            temp_raise_value = 0x40;
        }   
        else if ( SAD_Status.Bits.Vehicle_Type == AUDI_C7)
        {
            temp_raise_value = 0x34;
        }   
         else if ( SAD_Status.Bits.Vehicle_Type == C_Class)
        {
            temp_raise_value = 0x40;
        }
         else if ( SAD_Status.Bits.Vehicle_Type == SP108_Servo)
        {
            temp_raise_value = 0x64;
        }
         else if ( SAD_Status.Bits.Vehicle_Type == CN300)
        {
            temp_raise_value = 0x64;
        }     
  if((DataReceived[11]==0x06) && (DataReceived[10]==0))  //判断pid，且帧无错误   MD1s_01
  {
    DateCoped[0]= DataReceived[2]; //只用于该类电机的
    if(( SAD_Status.Bits.Vehicle_Type == MQB373 ) || ( SAD_Status.Bits.Vehicle_Type == AUDI_S6)|| ( SAD_Status.Bits.Vehicle_Type == AUDI_C7))//|| ( SAD_Status.Bits.Vehicle_Type == C_Class))
    { 
      Motor_Position = DataReceived[1];
      // following if checks the running state of MQB373, openning or closing or stopped.
      if(DataReceived[4] & 0x01)
      {
        MQB373_Action_State = Open;
      }else if(DataReceived[4] & 0x02)
      {
        MQB373_Action_State = Close;
      }else
      {
        MQB373_Action_State = Stop;
      }
      if(Get_Mode_Switch() == MODE_AUTO && new_step_started)
      {
        if((MD_Auto_RunStatus.Position_Target == 11)&&(Motor_Position >= temp_raise_value))
        {
          MQB373_Stop_Run();
        } // 起翘打开结束
        else if((MD_Auto_RunStatus.Position_Target == 0)&&(Motor_Position <= 1)){
          //Action=Stop;
          SetTimer(SOFT_TIMER_MD_DELAY_STOP,500,MQB373_Stop_Run);//软件中断,进行当前步骤运动完成后的延时(DelayTime)等待;         
        } // 起翘关闭结束
        else if((MD_Auto_RunStatus.Position_Target > 0 && MD_Auto_RunStatus.Position_Target <= 6) && 
                (Motor_Position > (Aim_Position[MD_Auto_RunStatus.Position_Target]-3)) &&
                (Motor_Position < (Aim_Position[MD_Auto_RunStatus.Position_Target]+3)))
        {
          MQB373_Stop_Run();
        } // 正常打开结束 // 正常关闭结束
        else {
        // Do nothing
        }
      }else if(Get_Mode_Switch() == MODE_MANUAL){
        if((DataReceived[2] & 0x03) == 0x03 && MQB373_Self_Learn == TRUE){
          MQB373_Self_Learn = FALSE;
          motor_needs_self_learn = FALSE; 
        }else if((DataReceived[2] & 0x03) != 0x03 && MQB373_Self_Learn == FALSE) {
          motor_needs_self_learn = TRUE; 
        }
#ifdef AUDI_SWITCH     
        if(MQB373_switch_position == 11 && Motor_Position < temp_raise_value){
          Action = Popup; 
        }else if( MQB373_switch_position == 0 && Motor_Position > 1){
          Action = Close;
        }else if(MQB373_switch_position > 0 && MQB373_switch_position <= 6){
          if(Motor_Position < Aim_Position[MQB373_switch_position] - 3){            
            Action = Open;
          } else if(Motor_Position > Aim_Position[MQB373_switch_position] + 3){
            Action = Close;
          } else{
            Action = Stop; 
          }
        }else{
          Action = Stop; 
        }
        if(((Action == Popup || Action == Open) && (MQB373_Action_State == Close)) ||
          ((Action == Close) && (MQB373_Action_State == Open)))
        {
          Action = Stop;
        }
#endif                         
      }
    }
    else if(SAD_Status.Bits.Vehicle_Type == YETI)
    {
      if((DataReceived[2] & 0x03) != 0x03 && YETI_Self_Learn_MD == FALSE)  // need self learn
      {
        motor_needs_self_learn = TRUE; 
      }
       Motor_Position = DataReceived[1];
       if(Get_Mode_Switch() == MODE_MANUAL){
        if((DataReceived[2] & 0x03) == 0x03 && YETI_Self_Learn_MD == TRUE){
          YETI_Self_Learn_MD = FALSE;
          motor_needs_self_learn = FALSE;  
        }else if((DataReceived[2] & 0x03) != 0x03 && YETI_Self_Learn_MD == FALSE) {
          motor_needs_self_learn = TRUE; 
        }
       }       
    }
     else if(SAD_Status.Bits.Vehicle_Type == C_Class)
   { Motor_Position = DataReceived[1];
      // following if checks the running state of MQB373, openning or closing or stopped.
      
      if(Get_Mode_Switch() == MODE_AUTO && new_step_started)
      {
        if((MD_Auto_RunStatus.Position_Target == 11)&&(Motor_Position >= temp_raise_value))
        {
          MQB373_Stop_Run();
        } // 起翘打开结束
        else if((MD_Auto_RunStatus.Position_Target == 0)&&(Motor_Position <= 1)){
          //Action=Stop;
          SetTimer(SOFT_TIMER_MD_DELAY_STOP,500,MQB373_Stop_Run);//软件中断,进行当前步骤运动完成后的延时(DelayTime)等待;         
        } // 起翘关闭结束
        else if((MD_Auto_RunStatus.Position_Target > 0 && MD_Auto_RunStatus.Position_Target <= 6) && 
                (Motor_Position > (Aim_Position[MD_Auto_RunStatus.Position_Target]-3)) &&
                (Motor_Position < (Aim_Position[MD_Auto_RunStatus.Position_Target]+3)))
        {
          MQB373_Stop_Run();
        } // 正常打开结束 // 正常关闭结束
        else {
        // Do nothing
        }
      }else if(Get_Mode_Switch() == MODE_MANUAL){
                            
      }
   }
         
    else if((SAD_Status.Bits.Vehicle_Type == MQB371)||(SAD_Status.Bits.Vehicle_Type == MODEL_Z))
    {
      if((DataReceived[2] & 0x03) != 0x03 && MQB371_Self_Learn == FALSE)  // need self learn
      {
        motor_needs_self_learn = TRUE; 
      }
       Motor_Position = DataReceived[1];
       if(Get_Mode_Switch() == MODE_MANUAL){
        if((DataReceived[2] & 0x03) == 0x03 && MQB371_Self_Learn == TRUE){
          MQB371_Self_Learn = FALSE;
          motor_needs_self_learn = FALSE; 
        }else if((DataReceived[2] & 0x03) != 0x03 && MQB371_Self_Learn == FALSE) {
          motor_needs_self_learn = TRUE; 
        }
       }
    }
    
    else if(SAD_Status.Bits.Vehicle_Type == SP108_Servo) 
    {
       Motor_Position = DataReceived[1];
       if(Get_Mode_Switch() == MODE_AUTO && new_step_started)
      {
        if((MD_Auto_RunStatus.Position_Target == 11)&&(Motor_Position >=temp_raise_value))
        {
          //SP108_Stop_Run_Res();
          SP108_Stop_Run();
        } // 起翘打开结束
        else if((MD_Auto_RunStatus.Position_Target == 0)&&(Motor_Position <= 1)){
          SP108_Stop_Run();
        } // 起翘关闭结束
        else if((MD_Auto_RunStatus.Position_Target > 0 && MD_Auto_RunStatus.Position_Target <= 6) && 
                (Motor_Position > (Aim_Position_SP108_Auto[MD_Auto_RunStatus.Position_Target]-2)) &&
                (Motor_Position < (Aim_Position_SP108_Auto[MD_Auto_RunStatus.Position_Target]+2)))
        {
          SP108_Stop_Run_Res();
          SP108_Stop_Run();
        } // 正常打开结束 // 正常关闭结束
        else {
        // Do nothing
        }
            
      }
      switch(Action_SP108) 
         {         
               case Open:
                   Res_Matrix_Output(Res_Matrix_Cmd_SP108_MD[0]);  //长开
                   break;
               case Close:
                   Res_Matrix_Output(Res_Matrix_Cmd_SP108_MD[2]);  //长关
                  break;
               case Stop:
                  Res_Matrix_Output(Res_Matrix_Cmd_SP108_MD[4]); //IDLE    
                  break;
               case RAISE:
                  Res_Matrix_Output(Res_Matrix_Cmd_SP108_MD[1]);  //点开,起翘时开关点触一下    
                  break;                 
         }
        
    }
   else if(SAD_Status.Bits.Vehicle_Type == EP21) 
    {
      Motor_Position = DataReceived[1];
      if(Get_Mode_Switch() == MODE_AUTO && new_step_started)
      {
        if((MD_Auto_RunStatus.Position_Target == 11)&&(Motor_Position >temp_raise_value))
        {
          new_step_started = FALSE; 
        } // 起翘打开结束
        else if((MD_Auto_RunStatus.Position_Target == 0)&&(Motor_Position < 1))
        {
          new_step_started = FALSE; 
        } // 起翘关闭结束
        else if((MD_Auto_RunStatus.Position_Target == 6)&&(Motor_Position >0))
        {
          new_step_started = FALSE; 
        } // 全开打开结束，不需要下面暂停函数
        else if((MD_Auto_RunStatus.Position_Target > 0 && MD_Auto_RunStatus.Position_Target < 6) && 
                (Motor_Position > (Aim_Position_SP108_Auto[MD_Auto_RunStatus.Position_Target]-2)) &&
                (Motor_Position < (Aim_Position_SP108_Auto[MD_Auto_RunStatus.Position_Target]+2)))
        {
          EP21_Stop_300ms();
          new_step_started = FALSE; 
        } // 正常打开结束 // 正常关闭结束
        else {
        // Do nothing
        }
            
      }
  if(Get_Mode_Switch() == MODE_AUTO) 
  {  
    switch(Action_EP21) 
      { 
         case Open: //2
         {
          
             Res_Matrix_Output(Res_Matrix_Cmd_EP21_GND[0]);  //长开
             Res_Matrix_Output(Res_Matrix_Cmd_EP21_MD[0]);
             Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
             AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);   //
             AdditionRes_Output(TTL_Matrix_Cmd_EP21_MD[1]);  //CLOSE
         }
             break;
         case Close: //0
         {
          
             Res_Matrix_Output(Res_Matrix_Cmd_EP21_GND[0]);  //长关
             Res_Matrix_Output(Res_Matrix_Cmd_EP21_MD[1]);
             Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
             AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);   //
             AdditionRes_Output(TTL_Matrix_Cmd_EP21_MD[1]);   //CLOSE
         }
            break;
         case Stop:  //3
         {
          
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_GND[0]); //
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MD[4]);  //IDLE
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);   // 
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MD[0]);   //disable
         }
            break;
         case RAISE: //1
         {
          
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_GND[0]);  //
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MD[2]);  
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);   //
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MD[1]);  //close
         }
            break;
         case DOWN:  //4
         {
          
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_GND[0]);  //
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MD[3]);  
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);   //
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MD[1]);  //close 
         }
            break;
         case Manual_Open: {
          
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_GND[0]); //
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MD[6]);  //IDLE
            Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);   // 
            AdditionRes_Output(TTL_Matrix_Cmd_EP21_MD[1]);   //disable
         }
            break;
         default:break;       
    }
    switch ( Action_EP21_MR ) 
    {
      case 0xff: {
        Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[2]);
        AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);
        break;
      }
      case 1: {
        Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[0]);
        AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[1]);
        break;
      }
      case 0: {
        Res_Matrix_Output(Res_Matrix_Cmd_EP21_MR[1]);
        AdditionRes_Output(TTL_Matrix_Cmd_EP21_MR[0]);        
        break;
      }
      default:break;
    }
  }
    }
  }
   
  else if(DataReceived[11] == 0x47 && DataReceived[10]==0) // MRollos_01 
  {
   DateCoped[1]= DataReceived[2];
    if((SAD_Status.Bits.Vehicle_Type == YETI)||(SAD_Status.Bits.Vehicle_Type == SP108_Servo))
    {
      Motor_Position_R = DataReceived[1] & 0x7f; //最高位无意义
      if(Get_Mode_Switch() == MODE_MANUAL){
        if((DataReceived[2] & 0x03) == 0x03 && YETI_Self_Learn_MR == TRUE){
          YETI_Self_Learn_MR = FALSE; 
        }
      }
    }
    
  else if(SAD_Status.Bits.Vehicle_Type == Octavia_Combi)
   {
     Motor_Position_R = DataReceived[1] & 0x7f;
   }
  }
  else if(DataReceived[11] == 0x14 && DataReceived[10]==0) //CN300 MD
  {
   
   if(SAD_Status.Bits.Vehicle_Type == CN300)
   {  
       Motor_Position = DataReceived[3];
       DateCoped_CN300[0]= DataReceived[1];  //只用于该类电机的
      // following if checks the running state of CN300, openning or closing or stopped.
      if(Get_Mode_Switch() == MODE_AUTO && new_step_started)
      {
        if((MD_Auto_RunStatus.Position_Target == 11)&&(Motor_Position >= temp_raise_value))
        {
          //CN300_Stop_Run();
        } // 起翘打开结束
        else if((MD_Auto_RunStatus.Position_Target == 0)&&(Motor_Position <= 1)){
          //Action=Stop;
          SetTimer(SOFT_TIMER_MD_DELAY_STOP,500,CN300_Stop_Run);//软件中断,进行当前步骤运动完成后的延时(DelayTime)等待;         
        } // 起翘关闭结束
        else if((MD_Auto_RunStatus.Position_Target > 0 && MD_Auto_RunStatus.Position_Target <= 6) && 
                (Motor_Position > (Aim_Position_CN300[MD_Auto_RunStatus.Position_Target]-2)) &&
                (Motor_Position < (Aim_Position_CN300[MD_Auto_RunStatus.Position_Target]+2)))
        {
          CN300_Stop_Run();
        } // 正常打开结束 // 正常关闭结束
        else {
        // Do nothing
        }
      }else if(Get_Mode_Switch() == MODE_MANUAL)
      {}
   }
  }
   else if((DataReceived[11] == 0x55)&& (DataReceived[10]==0)) //CN300 MR
  {
   Motor_Position_CN300_R = DataReceived[3]; 
   DateCoped_CN300[1]= DataReceived[1];  //只用于该类电机的 
  }
  tempbuffer[0] = DataReceived[11];
  tempbuffer[1] = DataReceived[10]|((byte)(DataReceived[0]<<4));
  for(tempbyte=LINSET; tempbyte<10u; tempbyte++)
  {
    tempbuffer[tempbyte+1] = DataReceived[tempbyte];
    /* Clear receive data buffer */
    DataReceived[tempbyte] = LINRESET;
  }
  DataReceived[0] = LINRESET;
  DataReceived[10] = LINRESET;
  DataReceived[11] = LINRESET;
  Update_Lin_Data(tempbuffer);
 }
