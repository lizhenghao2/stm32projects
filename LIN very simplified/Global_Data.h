#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H
#include "Headers.h"

/* Data Structure Definition */

typedef struct {
uint32 WholeSize;  //MB
uint32 UsedSize;    //MB
uint8  SaveMode;   //0:save ontinue 1:save from beginning
uint8  CyclicTimes;
uint8  Device_Memory_Status;
}TMemory_State;

typedef struct 
{
  uint8 MD_Cycle[3];
  uint8 MR_Cycle[3];
}  MD_MR_Cycle_Format; 

typedef struct  
{
  uint8 Start1;  //ͨ�ſ�ʼ 0x55
  uint8 Start2;   //ͨѶ��ʼ 0xAA
  uint8 CMD;        //���ͣ�DATAFRAME
  uint8 Num;        //�ֽ���,33
  uint8 Valid;       //�ǲ�����Ч����//1
  /* ����Ϊ����� */
  //uint8 SunroofEnabled        :1;    //Modified By Eric 2015/06/06
  //uint8 SunshadeEnabled       :1;
  uint8 MulitiDataFiled       :2;      //Added By Eric:��Modified By Eric 2015/06/06         
  uint8 Vol_Cur_Err_Type      :3;               //��������Ĺ�ѹ������������
  uint8 Anti_Pintch_Err_Type  :3;               //MD MR���з������
  uint8 MD_Current_Step       :4;            //MD��ǰ����
  uint8 MR_Current_Step       :4;            //MR��ǰ����
  MD_MR_Cycle_Format MD_MR_CURRENT_CYCLE;       //6
  uint32 LIN_Second;                            //4
  uint16 LIN_mSecond;                           //2
  uint8  LINDATA[11];                           //11
  
  /* ��ѹ�ź� */
  uint8 Eng_Voltage;                  
  /* �����ź� */
  uint8 Eng_Current; 
  /*
    //MulitiDataFiled = 0:Eng_Temp��Eng_Humi�����¶ȣ�ʪ��
    //MulitiDataFiled = 1:������������
    //MultiDataFiled =2:������������
  */         
  /* �¶��ź� */
  uint8 Eng_Temp;     //MRTarget:765,MRReal:432:MDEnable:1MREnable:0
  /* ʪ���ź� */
  uint8 Eng_Humi;     //MDTarget:7654,MDReal:3210
  
  // MDMRĿ�꿪�ȣ�ǰ��λMD������λMR
  uint8 MD_Position_Target   :4     ;  //��λ1-9��MD_PULL����Ϊ10 MD_PUSH����Ϊ11  ��������Ϊ15
  uint8 MR_Position_Target   :4     ;  //1Ϊ�� 2 Ϊ�ر� 0Ϊ���ź�  ��������Ϊ15
  // MDMRʵ�ʿ��ȣ�ǰ��λMD������λMR 
  uint8 MD_Position_Real   :4     ;  //��λ1-9, ��������Ϊ15
  uint8 MR_Position_Real   :4     ;  //1Ϊ�� 2 Ϊ�ر�  ��������Ϊ15                    
    
  /* ��ǰϵͳ����ģʽ����ǰ���� */
  uint8 MD_Run_With_Sensor :1    ;  //�Ƿ����������
  //�ϴ��ĳ������ڸ�Ϊ����ķ�ʽ��77��Ϊ���뿪ͷ������3*3λ��Ϊ���ͱ��룺�ܹ���֧��512��ͺ�
  //ÿһ���������ݵ��ϴ�����Ҫ�ܹ�5������
  uint8 Vehicle_Style   :3     ;  
  uint8 MR_Run_With_Sensor :1    ;  //�Ƿ����������
  uint8 Operate_Mode   :1     ;    //��ǰģʽ �ֶ��Զ�
  uint8 MD_Position_Arrived :1   ;  //MDλ�õ���  0�����һ�� λ�ÿ�ʼ�ı� 1λ�õ���
  uint8 MR_Position_Arrived :1   ;  //MRλ�õ���  0�����һ�� λ�ÿ�ʼ�ı� 1λ�õ���
  
  
  uint8 DataIsReady;            //����ʶ����ָʾ�����Ƿ�������������д���
} Upload_Format, *pUpload_Format;

/* Fram �Ĵ洢״̬30�ֽ�*/
typedef struct 
{
  uint8 LDF_Status;
  uint8 MD_Config_Status;
  uint8 MR_Config_Status;
  uint8 LDFFrameNum;      
  uint8 MD_Step_Num;
  uint8 MR_Step_Num;
  uint32 LDF_CheckSum;
  uint32 MD_CheckSum;
  uint32 MR_CheckSum;
  uint8 Vehicle_Type;  //�Ƿ����AutoRunning
  uint32 MD_Current_Cycle;  //�ϴ�ִ�е���ѭ����
  uint32 MR_Current_Cycle;  //�ϴ�ִ�е���ѭ����
  uint8 ThresholdCurrent;    //���е�����ֵ
  uint8 ThresholdAntipinchtimes;  //���д�����ֵ
  uint8 Buzzer_Control;         //�Ƿ�ʹ��Buzzer
} FRAMMANAGER;

typedef struct 
{
  uint8 Enable_PWM1;
  uint32 PWM_HighLevel_Time1;           //ms
  uint32 PWM_Cycle_Time1;                        //ms
  uint8 Enable_PWM2;
  uint32 PWM_HighLevel_Time2;           //ms
  uint32 PWM_Cycle_Time2;                        //ms
}PWMSignalManager;

/* ϵͳ����״̬��־ */
typedef union {
      uint32 value;
      struct {
        byte MD_Pos_Number_ERR         :1;                         /* MDλ�ô������ź��������󣬴���1 */
        byte MR_Pos_Number_ERR         :1;                         /* MRλ�ô������ź��������󣬴���1 */
        byte MD_Control_InValid        :1;                         /* MDû����Ч�ĵ�λ�����ź� */
        byte MR_Control_InValid        :1;                         /* MRû����Ч�ĵ�λ�����ź� */
        byte SCI_OVERRUN         :1;                         /* SCI���ռĴ���OVERRUN */
        byte FRAM_STATUS_ERR         :1;                          /* FRAM��ȡ״̬�������� */
        byte FRAM_READ_LDF_ERR         :1;                                       /* ��ȡLDF�������� */
        byte FRAM_WRITE_LDF_ERR         :1;                                       /* д��LDF�������� */
        byte FRAM_WRITE_MDMR_ERR         :1;                                       /* д��MDMR���ô��� */
        byte FRAM_READ_MDMR_ERR         :1;                                       /* ��ȡMDMR���ô��� */
        byte SCI_Receive_Time_Over         :1;                       /* SCI���ճ�ʱ */
        byte LIN_DATA_OVERRUN         :1;                                       /* LIN�Ľ��ջ������ */
        byte NO_VALID_TIME         :1;                                       /* û����Ч��ȫ��ʱ�� */
        byte LIN_PREPAIRED   :1;                                     /* LIN ׼��״�� */
        byte Reserved        :2;
        byte Vehicle_Type :8;            /* ���ͣ����֧��8�� */ //Eric_X 2015/03 
        //���ͣ��޸�Ϊ֧��256�֣�0423 Eric
      } Bits;
    } Status_Format;

typedef struct {
uint8 Stable_Position_Value;  //if Engine Stopped, update this value
uint8 Last_Position_Value;    //Last Time Received Position
uint8 Current_Position_Value; //Current Position
uint8 Stable_Position;
uint8 Current_Position;
uint8 Aim;
uint8 Last_Aim;
uint8 Anti_Pintch;
uint8 Anti_Pintch_Recorded;
uint8 Position_Target;
uint8 Engine_Speed;
uint8 Engine_Speed_Status;
}T_NF_POSITION;    

typedef struct LDFTableStructTag
/* LDF ��Ŀ���ݽṹ */
{
  byte  FramePID;
  byte  FrameDelay;
  byte  FrameDirect;
  byte  FrameLen;
  byte  FrameData[8];
  byte  FrameCheckType; //FrameCheck --> FrameCheckType 20130516 WWS
} LDFTableStruct;

/*��������*/
typedef struct 
{
unsigned char Sychronous;  //�Ƿ������ͬ����//��仰����PC������
unsigned char RunMode;     //Data Recorder;Config;DataReader
unsigned char Running;  //Running or Pause//Recorder״̬����Running����Pause
}TRunningTimePara;
    
/* LDF������λ����Ϣ�����ݽṹ */
typedef struct 
{
  uint8 ReceivingData;   //��0������ʼ����LDF����
  uint8 VechicleType;    //����
  uint8 Enable;          //��0�����������LDF����
  uint8 LDFTagNum;      //�ܹ��� LDF TAG����
  uint8 GottedNum;       //�Ѿ���õ�LDF TAG����
  int8  CurrentIndex;    //ÿ�����ݰ��ڵ�ǰ��ȡ���ڼ���TAG
  uint8 NumInthisPackage;//��ǰ���ݰ������� TAG����
  uint8 LDF_Status;      //LDF�Ƿ���Ч
  uint8 LIN_Master_Enable;   //ʹ�����ڵ㹦�ܣ�����LIN����, 20130516 WWS
  LDFTableStruct*  PLDFData; //ָ��LDF TAG�����ָ��
 }TLDFManager;

/* MD �� MR ÿ��Step����Ϣ */
/* MD �� MR ÿ��Step����Ϣ */
 typedef struct
{
  uint8  StepIndex;
  uint8  StartPosChanged;
  uint8  StartPos;
  uint8  EndPosChanged;
  uint8  EndPos;
  uint32 RunningTresholdTime;
  uint8  RunningTresholdTimeUnit;           //???
  uint32 DelayTime;
  uint8  UnitValue;
  uint8  MDorMR;   //Eric20141115      //0==MD 1==MR
} TStepInfo;

#define MAX_STEPMODULE 10
#define MAX_PACKAGE_NUM 5
 
 /* MD �� MR ���պʹ洢SCI���ݵĹ���ṹ�� */
 /*
 typedef struct 
 {
  uint8 ReceivingData;
  uint8 Enable;
  uint8 RunwithPosSensor;
  uint8 CyclicType;
  uint32 StartTime;     //��ʼʱ��/������
  uint32 EndTime;       //����ʱ��/������
  uint32 CyclicTimes;   //����ʱ��/������
  uint8 StepNums;///һ�����ڵĲ��������� //�������ڵĲ�������
  uint8 GottedNum;///��ǰ�����ݽṹ���յ��ļ���������Ϣ ///STM32����SCI����ʱû��???
   int8 CurrentIndex; ///��ǰ���յ����ݰ�����;???���ǵ�ǰִ�еĲ��Բ���???
  uint8 NumInthisPackage; ///��ǰ���ݰ��д��ڵĲ��Բ�������  ///STM32����SCI����ʱû��???
  uint8 Config_Status;  //���������ļ��Ƿ���Ч
  TStepInfo*  PStepData;    ///STM32����SCI����ʱû��???
  uint16 G_StartYear;
  uint8 G_StartMonth;
  uint8 G_StartDay;
  uint8 G_StartHour;
  uint8 G_StartMinute;
  uint8 G_StartSecond;
  uint16 G_EndYear;
  uint8 G_EndMonth;
  uint8 G_EndDay;
  uint8 G_EndHour;
  uint8 G_EndMinute;
  uint8 G_EndSecond; 
  uint8 StopWhenError;
 }TStepModuleManager;
 */
 
 /* MD �� MR ���պʹ洢SCI���ݵĹ���ṹ�� */
 typedef struct
 {
  uint8 ReceivingData;
  uint8 Enable;
  uint8 RunwithPosSensor;
  uint8 CyclicType;
  uint32 StartTime;     //��ʼʱ��/������
  uint32 EndTime;       //����ʱ��/������
  //uint32 CyclicTimes;   //ѭ����������
  uint8    windloadstep;      //
  uint8    windunloadstep;  //
  uint16   CyclicTimes;
  uint8 StepNums;///һ�����ڵĲ��������� //�������ڵĲ�������
  uint8 GottedNum;///��ǰ�����ݽṹ���յ��ļ���������Ϣ ///STM32����SCI����ʱû��???
   int8 CurrentIndex; ///��ǰ���յ����ݰ�����;???���ǵ�ǰִ�еĲ��Բ���???
  uint8 NumInthisPackage; ///��ǰ���ݰ��д��ڵĲ��Բ�������  ///STM32����SCI����ʱû��???
  uint8 Config_Status;  //���������ļ��Ƿ���Ч
  TStepInfo*  PStepData;    ///STM32����SCI����ʱû��???
  uint16 G_StartYear;
  uint8 G_StartMonth;
  uint8 G_StartDay;
  uint8 G_StartHour;
  uint8 G_StartMinute;
  uint8 G_StartSecond;
  uint16 G_EndYear;
  uint8 G_EndMonth;
  uint8 G_EndDay;
  uint8 G_EndHour;
  uint8 G_EndMinute;
  uint8 G_EndSecond;
  /*Low 4 bit ,1: Stop with error; 0: Dont Stop when error
    High 4 bit,1: WindLoad Test,Stop until Get Sensor Signal*/
  uint8 StopWhenError;
 }TStepModuleManager;
                       
/* �ݴ浱ǰSCI�շ����ݵĹ���ṹ�� */ 
typedef struct 
{
  byte Step;
  byte DataLength;
  byte Cnt;           //���浱ǰ�ɼ�������������
  word Checksum;      //��ǰ��õ�У���
  word GotChecksum;   //��õ�У���
  byte GotValidData;
  byte PointerPos;
  byte* PRxBuf;       //������λ��ָ��ʱ����Ϊ���ջ��棬����ʱ����Ϊ���ͻ���
}TISRData,*PISRData;

/* �ݴ�LIN���ݵĽṹ��Ԫ */
typedef struct ALIN_Data
{
  uint32 Second;
  uint16 mSecond;
  uint8 LinData[11];  //0:ID;
  uint8 Status;
  struct ALIN_Data* next;
} LIN_DATA, *pLIN_DATA;  

/* �ݴ�CAN���ݵĽṹ��Ԫ */
typedef struct ACAN_Data
{
  uint32 Second;
  uint16 mSecond;
  uint8 CanData[8];
  uint8 Status;
} CAN_DATA, *pCAN_DATA;  
 
/* LIN�����Ľṹ��Ԫ */
typedef enum
{
  ReceiveIdle,      /* 0 Receive idle state */
  ReceiveBreak,     /* 1 Receive break field state */
  ReceiveSync,      /* 2 Receive sync byte state */
  ReceivePID,       /* 3 Receive PID byte state */
  ReceiveData,      /* 4 Receive data byte state */
  ReceiveCheck,     /* 5 Receive checksum byte state */
} LINStates;

/* LIN�����Ľṹ��Ԫ */ 
typedef enum
{
  LINDataOK,        /* 0 Data OK */
  LINSyncError,     /* 1 Sync error */
  LINFramingError,  /* 2 Framing error */
  LINPIDError,      /* 3 PID Parity error */
  LINNoResp,        /* 4 No data received */
  LINTimeout,       /* 5 Receive data timeout */
  LINCheckError,    /* 6 Checksum error */
  LINDataError,     /* 7 Data bit readback error */
} LINDataStates;             

typedef struct
/* MDMR����״̬ */
{
  byte AutoRun_Status;
  byte Position_Target;
  byte Position_Engine;
  uint32 Current_Cycle;
  byte Current_Step;
  byte Run_Request;     //������ת�� ʵ����ʱ
  byte Delay_Timer_Status;  //��ʱTIMER״̬
  byte ALLOW_PUSH;
  byte PUSH_Timer_Status;  //PUSH��ʱTIMER״̬
  byte Cycle_plused;       //cycle�Ѿ��ӹ�һ��
} Steps_AutoRun_Info;

/* �Զ����д����ֳ� */
typedef struct AErr_Info
{
  uint8 Err_type;
  uint16 G_Year;
  uint8 G_Month;
  uint8 G_Day;
  uint8 G_Hour;
  uint8 G_Minute;
  uint8 G_Second;
  struct AErr_Info* pnext;
} Err_Info,*pErr_Info;


typedef struct 
{
    uint8 MD;  //MD
    uint8 MR;      //MR
    uint8 ERROR;
    uint8 SLEEP;
    uint8 POWER;
    uint8 Light_Normal;
    uint8 Light_Warning;
    uint8 Light_Error;
    uint8 Buzzer;
}TLED_State;

typedef enum
{
  ON,      /* 0  */
  OFF,     /* 1 */
  FLIKER    /* 2 */
} LEDStates;





extern unsigned char CurrentErrIndex; 
extern Err_Info Err_Scene_Cache[11][ERR_INFO_NUM]; //���������д����ֳ�
extern pErr_Info pNext_Err_Info[11];              //������һ����ָ��
extern pErr_Info pLatest_Err_Info[11];            //�������µ����д���ָ�� 

typedef struct 
{   
  uint16 Err_NUM;
  uint16 Warn_NUM;
}ERRMANAGER;  

/* Global Data Declare */
/* ����SAD��ǰ����״̬ */
extern Status_Format SAD_Status; 
/* ����FRAM��ǰ״̬ */
extern FRAMMANAGER FRAM_Status; 
/* ����PWM�źŵĵ�ǰ״̬*/
extern PWMSignalManager PWMSignal_Ctl;
/* ����LDF�����Լ�����״̬ */
extern TLDFManager LDFManager;        
/* LDF LIN֡����λ�� */
 
extern LDFTableStruct  LDFData_YETI[28];
extern LDFTableStruct  LDFData_SP108[7];
extern LDFTableStruct  LDFData_MQB[33];
extern LDFTableStruct  LDFData_MQB373[7];
extern LDFTableStruct  LDFData_H20[3];
extern LDFTableStruct  LDFData_CN300[3];
/* ����MD�������� */
extern TStepInfo SunroofData[MD_MAX_STEP_NUM];        
/* ����MD�������� */
extern TStepInfo SunshadeData[MR_MAX_STEP_NUM];       
/* ����MD�������ݵĽṹ�� */
extern TStepModuleManager SunroofStepModules;        
/* ����MR�������ݵĽṹ�� */
extern TStepModuleManager SunshadeStepModules; 
/* SCI1���շ������ݻ��� */
extern uint8 vRX[64]; 
/* SCI�����жϴ������ṹ�� */
extern TISRData SCI1Manager; 
/* ָ��SCI�жϴ������ṹ���ָ�� */
extern PISRData PData; 
/* �豸ȫ�ִ洢״̬ */
extern TMemory_State MemoryState;
/* LIN���ݱ���Ļ��� */
extern LIN_DATA LIN_DATA_Cache[LIN_CACHE_NUM]; 
/* CAN���ݱ���Ļ��� */
extern CAN_DATA CAN_DATA_Cache; 
/* ���͵�STM32�����ݰ� */
extern Upload_Format Upload_Data[2];
/* ����MD�Զ����еĲ��� */
extern Steps_AutoRun_Info MD_Auto_RunStatus;
/* ����MR�Զ����еĲ��� */
extern Steps_AutoRun_Info MR_Auto_RunStatus;
/* ���������д����ֳ� */
extern Err_Info Err_Scene_Cache[11][ERR_INFO_NUM]; //���������д����ֳ� 
/* ������һ�����д���ָ�� */
extern pErr_Info pNext_Err_Info[11]; 
/* �������µ����д���ָ�� */
extern pErr_Info pLatest_Err_Info[11];
/* LCD������ʾ���� */ 
//extern const uint8 Numbers[];
/* ����;���״̬ */
extern ERRMANAGER Err_Manager;
extern Upload_Format Upload_Data[2];   //���͵�STM32�����ݰ�
extern uint8 MD_Last_Position;        //Ĭ��MD��һ��λ��ʼΪ0
extern TRunningTimePara RunningPara;
extern uint8 MD_Current_Antipinchtimes;  //��ǰ���д�����ֵ
extern uint8 MR_Current_Antipinchtimes;  //��ǰ���д�����ֵ
//extern uint8 EnableBuzzer; 

//�����豸����״̬��
 extern uint8 CyclicsampleTimeSetted;
 extern uint8 RunningMode;   
 extern Upload_Format* pData;
 extern unsigned char CurrentMDTargetPosition;
 extern unsigned char CurrentMRTargetPosition; 
 extern unsigned char UploadLabel;
 extern unsigned char CurrentVol;
 extern unsigned char DeCurrentVol;
 extern unsigned char CurrentCur;
 extern unsigned char DeCurrentCur;
 extern char CurrentTempera;
 extern unsigned char DeCurrentTempera;
 extern unsigned char CurrentHumi;
 extern unsigned char DeCurrentHumi;
 extern unsigned char ModeLock;  
 extern unsigned char MemoryErrOrWar;
 
 //
 
 extern byte new_step_started;
 
 
 extern unsigned char RunConditionIsOK;
 extern unsigned char RunErrorHappened;
 extern unsigned char ResumeRun;
 
 extern unsigned char MD_RunErrorHappened;
 extern unsigned char MR_RunErrorHappened;
 
 extern T_NF_POSITION Super_NF_MD_Position ;
 extern T_NF_POSITION Webasto_X_MD_Position ;
 extern T_NF_POSITION RY_X_MD_Position ;
 extern T_NF_POSITION H20_MD_Position;
 extern T_NF_POSITION Webasto_Y_MD_Position;
 extern TLED_State Led_State;
 
 extern uint8 PWM_Signal_Flag1;
 extern uint8 PWM_Signal_Flag2;
 
 extern bool Real_Time_Flag;
 extern uint32 real_time_cnt; 
 extern byte Action_SP108;
 extern byte Action_EP21;
 extern byte Action_EP21_MR;
 extern uint8 posi_old;
 extern uint16 posi_cnt;
 extern uint8 posi_former;
 extern bool stop_flag;
 extern byte Action_CN300;
 
 extern uint8 ASUV_ManualSwitchDetected;
 
#endif