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
  uint8 Start1;  //通信开始 0x55
  uint8 Start2;   //通讯开始 0xAA
  uint8 CMD;        //类型，DATAFRAME
  uint8 Num;        //字节数,33
  uint8 Valid;       //是不是有效数据//1
  /* 以上为传输层 */
  //uint8 SunroofEnabled        :1;    //Modified By Eric 2015/06/06
  //uint8 SunshadeEnabled       :1;
  uint8 MulitiDataFiled       :2;      //Added By Eric:：Modified By Eric 2015/06/06         
  uint8 Vol_Cur_Err_Type      :3;               //最近发生的过压过流错误类型
  uint8 Anti_Pintch_Err_Type  :3;               //MD MR防夹发生溢出
  uint8 MD_Current_Step       :4;            //MD当前步骤
  uint8 MR_Current_Step       :4;            //MR当前步骤
  MD_MR_Cycle_Format MD_MR_CURRENT_CYCLE;       //6
  uint32 LIN_Second;                            //4
  uint16 LIN_mSecond;                           //2
  uint8  LINDATA[11];                           //11
  
  /* 电压信号 */
  uint8 Eng_Voltage;                  
  /* 电流信号 */
  uint8 Eng_Current; 
  /*
    //MulitiDataFiled = 0:Eng_Temp，Eng_Humi传输温度，湿度
    //MulitiDataFiled = 1:传输其他数据
    //MultiDataFiled =2:传输其他数据
  */         
  /* 温度信号 */
  uint8 Eng_Temp;     //MRTarget:765,MRReal:432:MDEnable:1MREnable:0
  /* 湿度信号 */
  uint8 Eng_Humi;     //MDTarget:7654,MDReal:3210
  
  // MDMR目标开度，前四位MD，后四位MR
  uint8 MD_Position_Target   :4     ;  //档位1-9，MD_PULL设置为10 MD_PUSH设置为11  错误设置为15
  uint8 MR_Position_Target   :4     ;  //1为打开 2 为关闭 0为无信号  错误设置为15
  // MDMR实际开度，前四位MD，后四位MR 
  uint8 MD_Position_Real   :4     ;  //档位1-9, 错误设置为15
  uint8 MR_Position_Real   :4     ;  //1为打开 2 为关闭  错误设置为15                    
    
  /* 当前系统操作模式，当前车型 */
  uint8 MD_Run_With_Sensor :1    ;  //是否带传感器跑
  //上传的车型现在改为编码的方式：77作为编码开头，后面3*3位作为车型编码：总共可支持512款车型号
  //每一个车型数据的上传，需要总共5包数据
  uint8 Vehicle_Style   :3     ;  
  uint8 MR_Run_With_Sensor :1    ;  //是否带传感器跑
  uint8 Operate_Mode   :1     ;    //当前模式 手动自动
  uint8 MD_Position_Arrived :1   ;  //MD位置到了  0输出下一档 位置开始改变 1位置到达
  uint8 MR_Position_Arrived :1   ;  //MR位置到了  0输出下一档 位置开始改变 1位置到达
  
  
  uint8 DataIsReady;            //本标识用于指示数据是否就绪，而不进行传输
} Upload_Format, *pUpload_Format;

/* Fram 的存储状态30字节*/
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
  uint8 Vehicle_Type;  //是否继续AutoRunning
  uint32 MD_Current_Cycle;  //上次执行到的循环数
  uint32 MR_Current_Cycle;  //上次执行到的循环数
  uint8 ThresholdCurrent;    //防夹电流阀值
  uint8 ThresholdAntipinchtimes;  //防夹次数阀值
  uint8 Buzzer_Control;         //是否使用Buzzer
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

/* 系统工作状态标志 */
typedef union {
      uint32 value;
      struct {
        byte MD_Pos_Number_ERR         :1;                         /* MD位置传感器信号数量错误，大于1 */
        byte MR_Pos_Number_ERR         :1;                         /* MR位置传感器信号数量错误，大于1 */
        byte MD_Control_InValid        :1;                         /* MD没有有效的档位控制信号 */
        byte MR_Control_InValid        :1;                         /* MR没有有效的档位控制信号 */
        byte SCI_OVERRUN         :1;                         /* SCI接收寄存器OVERRUN */
        byte FRAM_STATUS_ERR         :1;                          /* FRAM获取状态发生错误 */
        byte FRAM_READ_LDF_ERR         :1;                                       /* 读取LDF发生错误 */
        byte FRAM_WRITE_LDF_ERR         :1;                                       /* 写入LDF发生错误 */
        byte FRAM_WRITE_MDMR_ERR         :1;                                       /* 写入MDMR配置错误 */
        byte FRAM_READ_MDMR_ERR         :1;                                       /* 读取MDMR配置错误 */
        byte SCI_Receive_Time_Over         :1;                       /* SCI接收超时 */
        byte LIN_DATA_OVERRUN         :1;                                       /* LIN的接收缓存溢出 */
        byte NO_VALID_TIME         :1;                                       /* 没有有效的全局时间 */
        byte LIN_PREPAIRED   :1;                                     /* LIN 准备状况 */
        byte Reserved        :2;
        byte Vehicle_Type :8;            /* 车型，最多支持8种 */ //Eric_X 2015/03 
        //车型，修改为支持256种：0423 Eric
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
/* LDF 条目数据结构 */
{
  byte  FramePID;
  byte  FrameDelay;
  byte  FrameDirect;
  byte  FrameLen;
  byte  FrameData[8];
  byte  FrameCheckType; //FrameCheck --> FrameCheckType 20130516 WWS
} LDFTableStruct;

/*参数管理*/
typedef struct 
{
unsigned char Sychronous;  //是否与电脑同步了//这句话放在PC端上面
unsigned char RunMode;     //Data Recorder;Config;DataReader
unsigned char Running;  //Running or Pause//Recorder状态下是Running还是Pause
}TRunningTimePara;
    
/* LDF接收上位机信息的数据结构 */
typedef struct 
{
  uint8 ReceivingData;   //非0表明开始接收LDF数据
  uint8 VechicleType;    //车型
  uint8 Enable;          //非0表明允许接收LDF数据
  uint8 LDFTagNum;      //总共的 LDF TAG数量
  uint8 GottedNum;       //已经获得的LDF TAG数量
  int8  CurrentIndex;    //每个数据包内当前读取到第几个TAG
  uint8 NumInthisPackage;//当前数据包包含的 TAG数量
  uint8 LDF_Status;      //LDF是否有效
  uint8 LIN_Master_Enable;   //使能主节点功能，发送LIN数据, 20130516 WWS
  LDFTableStruct*  PLDFData; //指向LDF TAG数组的指针
 }TLDFManager;

/* MD 和 MR 每个Step的信息 */
/* MD 和 MR 每个Step的信息 */
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
 
 /* MD 和 MR 接收和存储SCI数据的管理结构体 */
 /*
 typedef struct 
 {
  uint8 ReceivingData;
  uint8 Enable;
  uint8 RunwithPosSensor;
  uint8 CyclicType;
  uint32 StartTime;     //开始时间/周期数
  uint32 EndTime;       //结束时间/周期数
  uint32 CyclicTimes;   //结束时间/周期数
  uint8 StepNums;///一个周期的步骤总数量 //单个周期的步骤数量
  uint8 GottedNum;///当前本数据结构中收到的几个步骤信息 ///STM32传输SCI数据时没有???
   int8 CurrentIndex; ///当前接收的数据包索引;???还是当前执行的测试步骤???
  uint8 NumInthisPackage; ///当前数据包中存在的测试步骤数量  ///STM32传输SCI数据时没有???
  uint8 Config_Status;  //运行配置文件是否有效
  TStepInfo*  PStepData;    ///STM32传输SCI数据时没有???
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
 
 /* MD 和 MR 接收和存储SCI数据的管理结构体 */
 typedef struct
 {
  uint8 ReceivingData;
  uint8 Enable;
  uint8 RunwithPosSensor;
  uint8 CyclicType;
  uint32 StartTime;     //开始时间/周期数
  uint32 EndTime;       //结束时间/周期数
  //uint32 CyclicTimes;   //循环的周期数
  uint8    windloadstep;      //
  uint8    windunloadstep;  //
  uint16   CyclicTimes;
  uint8 StepNums;///一个周期的步骤总数量 //单个周期的步骤数量
  uint8 GottedNum;///当前本数据结构中收到的几个步骤信息 ///STM32传输SCI数据时没有???
   int8 CurrentIndex; ///当前接收的数据包索引;???还是当前执行的测试步骤???
  uint8 NumInthisPackage; ///当前数据包中存在的测试步骤数量  ///STM32传输SCI数据时没有???
  uint8 Config_Status;  //运行配置文件是否有效
  TStepInfo*  PStepData;    ///STM32传输SCI数据时没有???
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
                       
/* 暂存当前SCI收发数据的管理结构体 */ 
typedef struct 
{
  byte Step;
  byte DataLength;
  byte Cnt;           //保存当前采集到的数据数量
  word Checksum;      //当前算得的校验和
  word GotChecksum;   //获得的校验和
  byte GotValidData;
  byte PointerPos;
  byte* PRxBuf;       //接收上位机指令时，作为接收缓存，反馈时，作为发送缓存
}TISRData,*PISRData;

/* 暂存LIN数据的结构单元 */
typedef struct ALIN_Data
{
  uint32 Second;
  uint16 mSecond;
  uint8 LinData[11];  //0:ID;
  uint8 Status;
  struct ALIN_Data* next;
} LIN_DATA, *pLIN_DATA;  

/* 暂存CAN数据的结构单元 */
typedef struct ACAN_Data
{
  uint32 Second;
  uint16 mSecond;
  uint8 CanData[8];
  uint8 Status;
} CAN_DATA, *pCAN_DATA;  
 
/* LIN驱动的结构单元 */
typedef enum
{
  ReceiveIdle,      /* 0 Receive idle state */
  ReceiveBreak,     /* 1 Receive break field state */
  ReceiveSync,      /* 2 Receive sync byte state */
  ReceivePID,       /* 3 Receive PID byte state */
  ReceiveData,      /* 4 Receive data byte state */
  ReceiveCheck,     /* 5 Receive checksum byte state */
} LINStates;

/* LIN驱动的结构单元 */ 
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
/* MDMR运行状态 */
{
  byte AutoRun_Status;
  byte Position_Target;
  byte Position_Engine;
  uint32 Current_Cycle;
  byte Current_Step;
  byte Run_Request;     //允许电机转动 实现延时
  byte Delay_Timer_Status;  //延时TIMER状态
  byte ALLOW_PUSH;
  byte PUSH_Timer_Status;  //PUSH延时TIMER状态
  byte Cycle_plused;       //cycle已经加过一次
} Steps_AutoRun_Info;

/* 自动运行错误现场 */
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
extern Err_Info Err_Scene_Cache[11][ERR_INFO_NUM]; //保存运行中错误现场
extern pErr_Info pNext_Err_Info[11];              //保存下一个误指针
extern pErr_Info pLatest_Err_Info[11];            //保存最新的运行错误指针 

typedef struct 
{   
  uint16 Err_NUM;
  uint16 Warn_NUM;
}ERRMANAGER;  

/* Global Data Declare */
/* 保存SAD当前错误状态 */
extern Status_Format SAD_Status; 
/* 保存FRAM当前状态 */
extern FRAMMANAGER FRAM_Status; 
/* 保存PWM信号的当前状态*/
extern PWMSignalManager PWMSignal_Ctl;
/* 保存LDF数据以及传输状态 */
extern TLDFManager LDFManager;        
/* LDF LIN帧保存位置 */
 
extern LDFTableStruct  LDFData_YETI[28];
extern LDFTableStruct  LDFData_SP108[7];
extern LDFTableStruct  LDFData_MQB[33];
extern LDFTableStruct  LDFData_MQB373[7];
extern LDFTableStruct  LDFData_H20[3];
extern LDFTableStruct  LDFData_CN300[3];
/* 保存MD运行数据 */
extern TStepInfo SunroofData[MD_MAX_STEP_NUM];        
/* 保存MD运行数据 */
extern TStepInfo SunshadeData[MR_MAX_STEP_NUM];       
/* 保存MD总体数据的结构体 */
extern TStepModuleManager SunroofStepModules;        
/* 保存MR总体数据的结构体 */
extern TStepModuleManager SunshadeStepModules; 
/* SCI1接收发送数据缓存 */
extern uint8 vRX[64]; 
/* SCI接收中断处理管理结构体 */
extern TISRData SCI1Manager; 
/* 指向SCI中断处理管理结构体的指针 */
extern PISRData PData; 
/* 设备全局存储状态 */
extern TMemory_State MemoryState;
/* LIN数据保存的缓存 */
extern LIN_DATA LIN_DATA_Cache[LIN_CACHE_NUM]; 
/* CAN数据保存的缓存 */
extern CAN_DATA CAN_DATA_Cache; 
/* 发送到STM32的数据包 */
extern Upload_Format Upload_Data[2];
/* 保存MD自动运行的参数 */
extern Steps_AutoRun_Info MD_Auto_RunStatus;
/* 保存MR自动运行的参数 */
extern Steps_AutoRun_Info MR_Auto_RunStatus;
/* 保存运行中错误现场 */
extern Err_Info Err_Scene_Cache[11][ERR_INFO_NUM]; //保存运行中错误现场 
/* 保存下一个运行错误指针 */
extern pErr_Info pNext_Err_Info[11]; 
/* 保存最新的运行错误指针 */
extern pErr_Info pLatest_Err_Info[11];
/* LCD数字显示数组 */ 
//extern const uint8 Numbers[];
/* 错误和警告状态 */
extern ERRMANAGER Err_Manager;
extern Upload_Format Upload_Data[2];   //发送到STM32的数据包
extern uint8 MD_Last_Position;        //默认MD上一档位初始为0
extern TRunningTimePara RunningPara;
extern uint8 MD_Current_Antipinchtimes;  //当前防夹次数阀值
extern uint8 MR_Current_Antipinchtimes;  //当前防夹次数阀值
//extern uint8 EnableBuzzer; 

//管理设备运行状态的
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