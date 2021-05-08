#include "Headers.h"
#include "motor.h"
/* Creation of all Global Data */
Status_Format SAD_Status;          //保存SAD当前运行状态
uint8 MD_Last_Position = 0;        //默认MD上一档位初始为0
FRAMMANAGER FRAM_Status;
PWMSignalManager PWMSignal_Ctl;    //保存PWM控制信号
TLDFManager LDFManager;        //保存LDF数据以及传输状态
TRunningTimePara RunningPara;
TStepInfo SunroofData[MD_MAX_STEP_NUM];        //保存MD运行数据
TStepInfo SunshadeData[MR_MAX_STEP_NUM];       //保存MR运行数据
TStepModuleManager SunroofStepModules;         //保存MD总体数据的结构体,其中SunroofStepModules.PStepData指向SunroofData[MD_MAX_STEP_NUM];
TStepModuleManager SunshadeStepModules;        //保存MR总体数据的结构体
TISRData SCI1Manager;  //SCI接收中断处理管理结构体
PISRData PData;        //指向SCI中断处理管理结构体的指针,指向SCI1Manager,在接收中断中进行赋值;
uint8 vRX[64];         //SCI1接收数据缓存
TMemory_State MemoryState; //保存全局Memory
uint8 MD_Current_Antipinchtimes;  //MD当前防夹次数阀值
uint8 MR_Current_Antipinchtimes;  //MR当前防夹次数阀值
LIN_DATA LIN_DATA_Cache[LIN_CACHE_NUM];   //LIN数据保存的缓存
CAN_DATA CAN_DATA_Cache;   //CAN数据保存的缓存,Super NF

Upload_Format Upload_Data[2];   //发送到STM32的数据包
Steps_AutoRun_Info MD_Auto_RunStatus;  //保存MD自动运行的参数
Steps_AutoRun_Info MR_Auto_RunStatus;  //保存MR自动运行的参数
Err_Info Err_Scene_Cache[11][ERR_INFO_NUM]; //保存运行中错误现场
pErr_Info pNext_Err_Info[11];              //保存下一个误指针
pErr_Info pLatest_Err_Info[11];            //保存最新的运行错误指针
unsigned char CurrentErrIndex;             //从当前最新的错误开始计算的错误和警告的Index
ERRMANAGER Err_Manager;
unsigned char MemoryErrOrWar;

/*运行参数管理*/                                                  
uint8 CyclicsampleTimeSetted;
uint8 RunningMode;
unsigned char ModeLock;

struct T_NF_POSITION Super_NF_MD_Position ;
struct T_NF_POSITION Webasto_X_MD_Position ;
struct T_NF_POSITION RY_X_MD_Position ;
struct T_NF_POSITION H20_MD_Position;
struct T_NF_POSITION Webasto_Y_MD_Position;

TLED_State Led_State;

//flag for asuv manual jack 12/16
uint8 ASUV_ManualSwitchDetected = 0;
                                       
LDFTableStruct  LDFData_YETI[28] =  
{
  // 与初始值不同：SAD_Freigabe = 1; ESP_v_Signal_8Bit = 0;
  {0x92u,  8u, FrameSub, 6u, {0xCFu, 0x80u, 0x88u, 0x88u, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x78u, 30u, FramePub, 8u, {0xFCu, 0xC0u, 0xFFu, 0x00u, 0x00u, 0xF0u, 0xFFu, 0xFFu}, EnhanceCheck},
  {0xDDu,  6u, FramePub, 2u, {0x80u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, ClassicCheck},
  {0x5Eu,  6u, FramePub, 2u, {0x80u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, ClassicCheck},
  {0x03u,  8u, FramePub, 6u, {0xC0u, 0x00u, 0xFEu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},

  {0x92u,  8u, FrameSub, 6u, {0xCFu, 0x80u, 0x88u, 0x88u, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x39u,  8u, FramePub, 6u, {0xC7u, 0x00u, 0xFBu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0xC4u,  8u, FramePub, 6u, {0xE0u, 0xF8u, 0x80u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x61u,  6u, FramePub, 3u, {0xC0u, 0xF8u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, ClassicCheck},
  {0x50u, 10u, FramePub, 8u, {0x28u, 0x00u, 0xFEu, 0x80u, 0xC0u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x08u,  8u, FramePub, 6u, {0x00u, 0xF0u, 0xE0u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},

  {0x92u,  8u, FrameSub, 6u, {0xCFu, 0x80u, 0x88u, 0x88u, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x78u, 30u, FramePub, 8u, {0xFCu, 0xC0u, 0xFFu, 0x00u, 0x00u, 0xF0u, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x47u,  8u, FrameSub, 6u, {0x00u, 0xF0u, 0xE0u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x06u,  8u, FrameSub, 6u, {0x00u, 0xF0u, 0xE0u, 0xFFu, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
                                                                                       
  {0x92u,  8u, FrameSub, 6u, {0xCFu, 0x80u, 0x88u, 0x88u, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x78u, 30u, FramePub, 8u, {0xFCu, 0xC0u, 0xFFu, 0x00u, 0x00u, 0xF0u, 0xFFu, 0xFFu}, EnhanceCheck},
  {0xDDu,  6u, FramePub, 2u, {0x80u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, ClassicCheck},
  {0x5Eu,  6u, FramePub, 2u, {0x80u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, ClassicCheck},
  {0x03u,  8u, FramePub, 6u, {0xC0u, 0x00u, 0xFEu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},

  {0x92u,  8u, FrameSub, 6u, {0xCFu, 0x80u, 0x88u, 0x88u, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x39u,  8u, FramePub, 6u, {0xC7u, 0x00u, 0xFBu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0xC4u,  8u, FramePub, 6u, {0xE0u, 0xF8u, 0x80u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x61u,  6u, FramePub, 3u, {0xC0u, 0xF8u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, ClassicCheck},
  {0x50u, 10u, FramePub, 8u, {0x28u, 0x00u, 0xFEu, 0x80u, 0xC0u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},

  {0x92u,  8u, FrameSub, 6u, {0xCFu, 0x80u, 0x88u, 0x88u, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x78u, 30u, FramePub, 8u, {0xFCu, 0xC0u, 0xFFu, 0x00u, 0x00u, 0xF0u, 0xFFu, 0xFFu}, EnhanceCheck},
  {0xE9u, 10u, FramePub, 8u, {0x00u, 0x7Cu, 0x00u, 0xFCu, 0x00u, 0x00u, 0xFFu, 0xFFu}, EnhanceCheck},
};  

LDFTableStruct  LDFData_SP108[7] = 
{
 //{0xCFu,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 //{0xD3u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck}, 
 {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 {0x5Eu,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 
 {0x6Au,  8u, FramePub, 8u, {0xFFu, 0xFFu, 0xFFu, 0x9Fu, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck}, 
 {0x06u,  8u, FrameSub, 6u, {0x00u, 0xF0u, 0xE0u, 0xFFu, 0xFEu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 //{0x50u, 10u, FramePub, 8u, {0x28u, 0x00u, 0xFEu, 0x80u, 0xC0u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 {0x50u, 10u, FramePub, 8u, {0x1Du, 0xFEu, 0xFEu, 0xC0u, 0xF8u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 
 {0xCAu, 10u, FramePub, 8u, {0x28u, 0x00u, 0xFEu, 0x80u, 0xC0u, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
 {0x47u,  8u, FrameSub, 6u, {0x00u, 0xF0u, 0xE0u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck}, 
}; 

LDFTableStruct  LDFData_MQB[33] =  
{
  //与初始值不同：SAD_Freigabe = 1; ESP_v_Signal_8Bit = 0;
  {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x1Au, 10u, FramePub, 8u, {0xFFu, 0xBFu, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu}, EnhanceCheck},
  {0x50u, 10u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x61u,  6u, FramePub, 2u, {0xC0u, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0xC4u,  8u, FramePub, 6u, {0xE0u, 0xF8u, 0x80u, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u}, EnhanceCheck},
  {0x5Eu,  6u, FrameSub, 2u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
     
  {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x55u, 10u, FrameSub, 8u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x50u, 10u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x03u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x06u, 18u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
     
  {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x1Au, 10u, FramePub, 8u, {0xFFu, 0xBFu, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu}, EnhanceCheck},
  {0x50u, 16u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x8Bu, 10u, FramePub, 8u, {0x3Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0xEEu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x5Bu, 10u, FramePub, 6u, {0xFFu, 0x7Eu, 0x07u, 0xC0u, 0xFCu, 0xFEu, 0x00u, 0x00u}, EnhanceCheck},
                                                                                     
  {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x55u, 10u, FrameSub, 8u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x50u, 10u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x61u,  6u, FramePub, 2u, {0xC0u, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0xC4u,  8u, FramePub, 6u, {0xE0u, 0xF8u, 0x80u, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u}, EnhanceCheck},
  {0x5Eu,  6u, FrameSub, 2u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
     
  {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x1Au, 10u, FramePub, 8u, {0xFFu, 0xBFu, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu}, EnhanceCheck},
  {0x50u, 10u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x03u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x47u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x49u, 10u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
     
  {0x92u,  8u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x55u, 10u, FrameSub, 8u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x50u, 10u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x08u, 18u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x5Bu,  8u, FramePub, 6u, {0xFFu, 0x7Eu, 0x07u, 0xC0u, 0xFCu, 0xFEu, 0x00u, 0x00u}, EnhanceCheck},
};

LDFTableStruct  LDFData_MQB373[7] = 
{     
  {0x92u, 10u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x50u, 10u, FramePub, 8u, {0x58u, 0x00u, 0xFEu, 0x00u, 0x00u, 0xEEu, 0x4Du, 0xFFu}, EnhanceCheck},
  {0x06u, 10u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x8Bu, 10u, FramePub, 8u, {0x3Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0xEEu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x47u, 10u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x49u, 10u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x08u, 10u, FrameSub, 6u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
};

LDFTableStruct  LDFData_H20[3]= 
{
  {0x20u, 10u, FramePub, 8u, {0x0Cu, 0xFFu, 0x00u, 0x70u, 0x00u, 0x00u, 0x30u, 0x60u}, EnhanceCheck},
  {0x61u, 10u, FramePub, 5u, {0xC0u, 0x00u, 0xC0u, 0x80u, 0xE0u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x25u, 10u, FrameSub, 4u, {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
};

LDFTableStruct  LDFData_CN300[3]= 
{
  //{0xB1u, 10u, FramePub, 8u, {0x00u, 0x50u, 0x00u, 0x3Cu, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  //{0xB1u, 10u, FramePub, 8u, {0x00u, 0xB0u, 0xF0u, 0xFCu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0xB1u, 10u, FramePub, 8u, {0x00u, 0x50u, 0xF0u, 0xFCu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}, EnhanceCheck},
  {0x55u, 10u, FrameSub, 8u, {0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
  {0x14u, 10u, FrameSub, 8u, {0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, EnhanceCheck},
};



void InitLDFManager(void) 
  { 
    LDFManager.ReceivingData = 0;
    LDFManager.NumInthisPackage = 0;
    LDFManager.CurrentIndex = 0;
    LDFManager.GottedNum = 0;
    LDFManager.LIN_Master_Enable = 0;
    if (( SAD_Status.Bits.Vehicle_Type == MQB371 )||( SAD_Status.Bits.Vehicle_Type == MODEL_Z ))  LDFManager.PLDFData = &LDFData_MQB[0];
    if (( SAD_Status.Bits.Vehicle_Type == MQB373 )||( SAD_Status.Bits.Vehicle_Type == AUDI_S6 )||( SAD_Status.Bits.Vehicle_Type == AUDI_C7 )||( SAD_Status.Bits.Vehicle_Type == C_Class )) LDFManager.PLDFData = &LDFData_MQB373[0];
    if ( SAD_Status.Bits.Vehicle_Type == YETI )  LDFManager.PLDFData = &LDFData_YETI[0];
    if ( SAD_Status.Bits.Vehicle_Type == H20 )  LDFManager.PLDFData = &LDFData_H20[0];
    if ((SAD_Status.Bits.Vehicle_Type == SP108_Servo)||(SAD_Status.Bits.Vehicle_Type == EP21))  LDFManager.PLDFData = &LDFData_SP108[0];
    if (SAD_Status.Bits.Vehicle_Type == Octavia_Combi)  LDFManager.PLDFData = &LDFData_MQB373[0];
    if (SAD_Status.Bits.Vehicle_Type == CN300)  LDFManager.PLDFData = &LDFData_CN300[0];
  }
  
void InitSunroofManager(void) 
 {
  SunroofStepModules.ReceivingData = 0;
  SunroofStepModules.GottedNum = 0;
  SunroofStepModules.CurrentIndex = 0;
  SunroofStepModules.NumInthisPackage = 0;
  SunroofStepModules.PStepData = SunroofData;///将SunroofStepModules.PStepData指向SunroofData
 }
 
void InitSunshadeManager(void) 
 {
  SunshadeStepModules.ReceivingData = 0;
  SunshadeStepModules.GottedNum = 0;
  SunshadeStepModules.CurrentIndex = 0;
  SunshadeStepModules.NumInthisPackage = 0;
  SunshadeStepModules.PStepData = SunshadeData;
 }
 
void InitMD_AutoRun_Status ( void ) 
{
  MD_Auto_RunStatus.AutoRun_Status = 0;
  MD_Auto_RunStatus.Current_Cycle = 0;
  MD_Auto_RunStatus.Current_Step = 0;
  MD_Auto_RunStatus.Delay_Timer_Status = 0;
  MD_Auto_RunStatus.Run_Request = 0;
}

void InitMR_AutoRun_Status ( void ) 
{
  MR_Auto_RunStatus.AutoRun_Status = 0;
  MR_Auto_RunStatus.Current_Cycle = 0;
  MR_Auto_RunStatus.Current_Step = 0;
  MR_Auto_RunStatus.Delay_Timer_Status = 0;
  MR_Auto_RunStatus.Run_Request = 0;
}

void Init_Global_Data (void) 
{
  int i,j;   
  InitLDFManager();
  InitSunroofManager();
  InitSunshadeManager();  
  for ( i=0; i<TIMERNUMBER; i++) 
  {
    SWTimer[i].Enable = 0;
    SWTimer[i].InternalTime = 0;
    SWTimer[i].Cnt = 0;
    SWTimer[i].Valid = 0;
    SWTimer[i].UserFunction = (void*)0;
  }
    
  SAD_Status.value = 0;   
  FRAM_Status.LDF_Status = 0;
  FRAM_Status.MD_Config_Status = 0;
  FRAM_Status.MR_Config_Status = 0;
  FRAM_Status.LDFFrameNum = 0;
  FRAM_Status.MD_Step_Num = 0;
  FRAM_Status.LDF_CheckSum = 0;
  FRAM_Status.MD_CheckSum = 0;
  FRAM_Status.MR_CheckSum = 0;
  FRAM_Status.Vehicle_Type = 0;
  FRAM_Status.MD_Current_Cycle = 0;
  FRAM_Status.MR_Current_Cycle = 0;
  FRAM_Status.ThresholdCurrent = 0;
  FRAM_Status.ThresholdAntipinchtimes = 0;
//  FRAM_Status.PWM_Frequency = 0;
//  FRAM_Status.PWM_Period = 0;

  for ( i=0; i<MD_MAX_STEP_NUM; i++) 
  {
    SunroofData[i].StepIndex = 0;
    SunroofData[i].StartPos = 0;
    SunroofData[i].EndPos = 0;
    SunroofData[i].RunningTresholdTime = 0;
    SunroofData[i].RunningTresholdTimeUnit = 0;
    SunroofData[i].DelayTime = 0;
    SunroofData[i].UnitValue = 0;
  }

  for ( i=0; i<MR_MAX_STEP_NUM; i++) 
  {
    SunshadeData[i].StepIndex = 0;
    SunshadeData[i].StartPos = 0;
    SunshadeData[i].EndPos = 0;
    SunshadeData[i].RunningTresholdTime = 0;
    SunshadeData[i].RunningTresholdTimeUnit = 0;
    SunshadeData[i].DelayTime = 0;
    SunshadeData[i].UnitValue = 0;
  }
  
  LDFManager.ReceivingData = 0;
  LDFManager.VechicleType = 0;
  LDFManager.Enable = 0;
  LDFManager.GottedNum = 0;
  LDFManager.CurrentIndex = 0;
  LDFManager.NumInthisPackage = 0;
  LDFManager.LDF_Status = 0;
  LDFManager.LIN_Master_Enable = 1;
  if (( SAD_Status.Bits.Vehicle_Type == MQB371 )||( SAD_Status.Bits.Vehicle_Type == MODEL_Z ))  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_MQB ;
  if (( SAD_Status.Bits.Vehicle_Type == MQB373 )|| ( SAD_Status.Bits.Vehicle_Type == AUDI_S6 )||( SAD_Status.Bits.Vehicle_Type == AUDI_C7 )||( SAD_Status.Bits.Vehicle_Type == C_Class ))  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_MQB373 ;
  if ( SAD_Status.Bits.Vehicle_Type == YETI )  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_YETI ;
  if ( SAD_Status.Bits.Vehicle_Type == H20 )  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_H20 ;
  if (( SAD_Status.Bits.Vehicle_Type == SP108_Servo )||(SAD_Status.Bits.Vehicle_Type == EP21))  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_SP108 ;
  if ( SAD_Status.Bits.Vehicle_Type == Octavia_Combi )  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_MQB373 ;
  if ( SAD_Status.Bits.Vehicle_Type == CN300 )  LDFManager.PLDFData = (LDFTableStruct*) &LDFData_CN300 ;
  SunroofStepModules.ReceivingData = 0;
  SunroofStepModules.Enable = 0;
  SunroofStepModules.CyclicType = 0;
  SunroofStepModules.StartTime = 0;
  SunroofStepModules.EndTime = 0;
  SunroofStepModules.StepNums = 0;
  SunroofStepModules.GottedNum = 0;
  SunroofStepModules.CurrentIndex = 0;
  SunroofStepModules.NumInthisPackage = 0;
  SunroofStepModules.Config_Status = 0;
  SunroofStepModules.PStepData = (TStepInfo*) &SunroofData;
  
  SunshadeStepModules.ReceivingData = 0;
  SunshadeStepModules.Enable = 0;
  SunshadeStepModules.CyclicType = 0;
  SunshadeStepModules.StartTime = 0;
  SunshadeStepModules.EndTime = 0;
  SunshadeStepModules.StepNums = 0;
  SunshadeStepModules.GottedNum = 0;
  SunshadeStepModules.CurrentIndex = 0;
  SunshadeStepModules.NumInthisPackage = 0;
  SunshadeStepModules.Config_Status = 0;
  SunshadeStepModules.PStepData = (TStepInfo*) &SunshadeData;
  
  for ( i=0; i<64; i++) 
  {
    vRX[i] = 0;
  }
  
  SCI1Manager.Step = 0;
  SCI1Manager.DataLength = 0;
  SCI1Manager.Cnt = 0;
  SCI1Manager.Checksum = 0;
  SCI1Manager.GotChecksum = 0;
  SCI1Manager.GotValidData = 0;
  SCI1Manager.PointerPos = 0;
  SCI1Manager.PRxBuf = (uint8*) &vRX;    
  PData = &SCI1Manager;           //指向SCI中断处理管理结构体的指针  
  CurrentDeviceTime.Device_Time_Status = DEVICE_TIME_INVALID; //初始化当前全局时间
  CurrentDeviceTime.Second = 0;
  CurrentDeviceTime.mSecond = 0;
  CurrentDeviceTime.G_Year = 0;
  CurrentDeviceTime.G_Month = 0;
  CurrentDeviceTime.G_Day = 0;
  CurrentDeviceTime.G_Hour = 0;
  CurrentDeviceTime.G_Minute = 0;
  CurrentDeviceTime.CyclicTimes = 0;
  
  MemoryState.Device_Memory_Status = DEVICE_TIME_INVALID;
  MemoryState.WholeSize = 0;
  MemoryState.UsedSize = 0;
  MemoryState.CyclicTimes = 0;  
  
  for ( i=0; i<LIN_CACHE_NUM; i++) 
  {
    LIN_DATA_Cache[i].Second = 0;
    LIN_DATA_Cache[i].mSecond = 0;                         
    LIN_DATA_Cache[i].Status = 0;
    for ( j=0; j<11; j++) 
    {
      LIN_DATA_Cache[i].LinData[j] = 0;
    }
  }
  
  for ( i=0; i<LIN_CACHE_NUM; i++) 
  {
    if ( i == LIN_CACHE_NUM-1 ) 
         LIN_DATA_Cache[i].next = (LIN_DATA*) &LIN_DATA_Cache[0];   //链表尾指向链表首
    else 
         LIN_DATA_Cache[i].next = (LIN_DATA*) &LIN_DATA_Cache[i+1];  
  }
  
  Upload_Data[0].Start1 = 0x55;
  Upload_Data[0].Start2 = 0xAA;            
  Upload_Data[0].CMD = DATAFRAME;
  Upload_Data[0].Num = 33;
  Upload_Data[0].MD_Current_Step = 0;
  Upload_Data[0].MR_Current_Step = 0;
  Upload_Data[0].LIN_Second = 0;
  Upload_Data[0].LIN_mSecond = 0;
  Upload_Data[0].LINDATA[11] = 0;
  Upload_Data[0].Eng_Voltage = 0;
  Upload_Data[0].Eng_Current = 0;
  Upload_Data[0].Eng_Temp = 0;
  Upload_Data[0].Eng_Humi = 0;
  Upload_Data[0].MD_Position_Target = 0;
  Upload_Data[0].MR_Position_Target = 0;
  Upload_Data[0].MD_Position_Real = 0;
  Upload_Data[0].MR_Position_Real = 0;
  Upload_Data[0].Vol_Cur_Err_Type = 0;
  Upload_Data[0].Anti_Pintch_Err_Type = 0;
  Upload_Data[0].Operate_Mode = 0;
  Upload_Data[0].Vehicle_Style = 0;
  Upload_Data[0].DataIsReady = 0;
  
  Upload_Data[1].Start1 = 0x55;
  Upload_Data[1].Start2 = 0xAA;            
  Upload_Data[1].CMD = DATAFRAME;
  Upload_Data[1].Num = 33;
  Upload_Data[1].MD_Current_Step = 0;
  Upload_Data[1].MR_Current_Step = 0;
  Upload_Data[1].LIN_Second = 0;
  Upload_Data[1].LIN_mSecond = 0;
  Upload_Data[1].LINDATA[11] = 0;
  Upload_Data[1].Eng_Voltage = 0;
  Upload_Data[1].Eng_Current = 0;
  Upload_Data[1].Eng_Temp = 0;
  Upload_Data[1].Eng_Humi = 0;
  Upload_Data[1].MD_Position_Target = 0;
  Upload_Data[1].MR_Position_Target = 0;
  Upload_Data[1].MD_Position_Real = 0;
  Upload_Data[1].MR_Position_Real = 0;
  Upload_Data[1].Vol_Cur_Err_Type = 0;
  Upload_Data[1].Anti_Pintch_Err_Type = 0;
  Upload_Data[1].Operate_Mode = 0;
  Upload_Data[1].Vehicle_Style = 0;
  Upload_Data[1].DataIsReady = 0;
  
  MD_Auto_RunStatus.AutoRun_Status = 0;
  MD_Auto_RunStatus.Current_Cycle = 0;
  MD_Auto_RunStatus.Current_Step = 0;
  MD_Auto_RunStatus.Delay_Timer_Status = 0;
  MD_Auto_RunStatus.Run_Request = 0;
  
  MR_Auto_RunStatus.AutoRun_Status = 0;
  MR_Auto_RunStatus.Current_Cycle = 0;
  MR_Auto_RunStatus.Current_Step = 0;
  MR_Auto_RunStatus.Delay_Timer_Status = 0;
  MR_Auto_RunStatus.Run_Request = 0;
  
  for ( j=0; j<11; j++) 
  {  
    for ( i=0; i<ERR_INFO_NUM; i++) 
    {
      Err_Scene_Cache[j][i].Err_type = 0;
      Err_Scene_Cache[j][i].G_Year = 0;
      Err_Scene_Cache[j][i].G_Month = 0;
      Err_Scene_Cache[j][i].G_Day = 0;
      Err_Scene_Cache[j][i].G_Hour = 0;
      Err_Scene_Cache[j][i].G_Minute = 0;
      Err_Scene_Cache[j][i].G_Second = 0;
    }
  }    
  ModeLock = 0;  
  for ( j=0; j<11; j++) 
  { 
    for ( i=0; i<ERR_INFO_NUM; i++) 
    {
      if ( i == ERR_INFO_NUM-1 ) Err_Scene_Cache[j][i].pnext = (Err_Info *) &Err_Scene_Cache[j][0];   //链表尾指向链表首
      else Err_Scene_Cache[j][i].pnext =(Err_Info*) &Err_Scene_Cache[j][i+1];  
    }
    pNext_Err_Info[j] =  &Err_Scene_Cache[j][0];      //错误信息入口初始化
    pLatest_Err_Info[j] =  &Err_Scene_Cache[j][0];      //错误信息入口初始化
  }

    Super_NF_MD_Position.Aim = 0;
    Super_NF_MD_Position.Anti_Pintch = 0;
    Super_NF_MD_Position.Anti_Pintch_Recorded = 0;
    Super_NF_MD_Position.Current_Position = 0;
    Super_NF_MD_Position.Current_Position_Value = 0;
    Super_NF_MD_Position.Engine_Speed = 0;
    Super_NF_MD_Position.Engine_Speed_Status = 0;
    Super_NF_MD_Position.Last_Position_Value = 0;
    Super_NF_MD_Position.Position_Target = 0;
    Super_NF_MD_Position.Stable_Position = 0;
    Super_NF_MD_Position.Stable_Position_Value = 0;
  // motor_type related control function initiation 
    Led_State.MD = OFF;
    Led_State.MR = OFF;
    Led_State.ERROR = OFF;
    Led_State.POWER = ON;
    Led_State.SLEEP = OFF;
    Led_State.Light_Error = OFF;
    Led_State.Light_Normal = ON;
    Led_State.Light_Warning = OFF;
    Led_State.Buzzer = OFF;   
    //
    Initial_Moto_Names();  
} 


