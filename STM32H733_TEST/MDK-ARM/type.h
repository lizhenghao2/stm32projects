#ifndef __TYPE_H__
#define __TYPE_H__
/* ---------------------------------------------------------------------------
 *  The data types shall be in accordance to:
		Enum = 8-bit enumeration
		Unsigned Byte = 8-bit unsigned numeric value
		Unsigned Word = 16-bit unsigned numeric value
		Byte Array = sequence of 8-bit aligned data
		Bit String = 8-bit binary code
 * --------------------------------------------------------------------------*/

typedef unsigned char unsigned_byte;
typedef unsigned short int unsigned_word;
typedef unsigned char bit_string;
typedef unsigned char byte_array;
typedef unsigned char enumeration;
typedef unsigned char byte;


typedef unsigned char  l_bool;   /* Boolean Datatype */
typedef unsigned char  l_u8;     /* 8Bit Datatype */
typedef unsigned int   l_u16;    /* 16Bit Datatype */
typedef unsigned long  l_u32;    /* 32Bit Datatype */
extern 
/* ---------------------------------------------------------------------------
Definitions by DLL
* --------------------------------------------------------------------------*/
unsigned char Ftype;//Range: {ReqField, RespField, SleepField, DiagNodeCfg, DiagNodeCfgOrSleepField};
unsigned_byte FrameID;//range:[0x16 to 0x7F]
unsigned_byte Length;//range:[0x01 to 0xFF]
byte_array PDU;//range:[0x00 to 0xFF]
unsigned char NetMngt;//range: {ev_wakeup_pulse_detect, ev_dominant_pulse_detect, ev_CLK_detect};
/* ---------------------------------------------------------------------------
NetMngt value discriptions:
ev_wakeup_pulse_detect 
This service primitive parameter value indicates the reception of the wake-up pulse event from the DLL.
ev_dominant_pulse_detect 
This service primitive parameter value indicates the reception of the dominant pulse event from the DLL.
ev_CLK_detect 
This service primitive parameter value indicates the reception of the clock detection event from the DLL
* --------------------------------------------------------------------------*/
unsigned char Cmd_Wakeup_Req;//Range: {cmd_CLK_generator_on, cmd_CLK_generator_off, cmd_wakeup_pulse_on};
/* ---------------------------------------------------------------------------
Cmd_Wakeup_Req value discriptions:request from higher OSI layer
cmd_CLK_generator_on 
This service primitive parameter value commands the clock generator to turn on the clock transmission to the DLL.
cmd_CLK_generator_off 
This service primitive parameter value commands the clock generator to turn off the clock transmission to the DLL.
cmd_wakeup_pulse_on 
This service primitive parameter value commands the transmission of the wake-up pulse to the DLL.
* --------------------------------------------------------------------------*/
unsigned char Ev_Wakeup_ind;
/* ---------------------------------------------------------------------------
A request from PHY
Cmd_Wakeup_Req and NetMngt have the same values and value discriptions:
* --------------------------------------------------------------------------*/
unsigned char NetMngtInfo;//Range: {sleep_indication,sleep_notification, wakeup_indication,wakeup_notification};
/* ---------------------------------------------------------------------------
NetMngtInfo value discriptions:
sleep indication/notification 
This service primitive parameter value indicates the reception of the sleep indication/notification from the DLL.
wakeup indication/notification 
This service primitive parameter value indicates the reception of the wakeup indication/notification from the DLL.
* --------------------------------------------------------------------------*/
unsigned_byte  FrameSCT;//range:[0b00 to 0b11]
bit_string Result;
/* ---------------------------------------------------------------------------
DLL:The Result parameter shall be of data type Bit String and shall contain the status relating to the outcome of a 
ReqField or RespField execution.
range:[OK, Err_DLL_Arbit_Lost, Err_DLL_Byte, Err_DLL_CRC, Err_DLL_DLC, Err_DLL_Parity, Err_DLL_Framing]
NL and TL:The Result parameter shall be of data type Bit String and shall contain the status relating to the outcome of a 
service execution. If two or more errors are discovered at the same time, then the transport or network layer 
entity shall set the appropriate error bit in the Result parameter.
Range: [OK, Err_TL_PacketType, Err_TL_PCI_SF_DL_Value, Err_TL_PCI_SF_DLext_Value]
*	--------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
The OK shall be issued to the service user when the service execution completes successfully.
The OK shall be issued to a service user on both, the sender and receiver side.
The Err_... shall be issued to the service user when an error detects.
The Err_... shall be issued to the service user on both, the sender and receiver side.
*	--------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
Definitions by TL and NL
* --------------------------------------------------------------------------*/
unsigned char Ptype;
unsigned_byte PacketId;//Range: [0x01 to 0x7F]
/* ---------------------------------------------------------------------------
Definitions by AL
* --------------------------------------------------------------------------*/
unsigned char Mtype;//{PTYPEField, Req_Field, Resp_Field, Sleep_Field};
unsigned_byte MsgID;//Range: [0x01 to 0x7F]
unsigned_byte MessageSCT;//range: [0b00 to 0b11]
struct response_field
{	
	unsigned_byte fi;
	unsigned_byte Length;
	unsigned char crc;
};
struct request_field
{
	
};
typedef struct CxpiFrameStruct
/* CXPI ?????? */
{
  byte  FramePID;
  byte  FrameDelay;
  byte  FrameInfo	;
  byte  DataLen;
  byte  FrameData[12];
  byte  FrameCheckType; //FrameCheck --> FrameCheckType 20130516 WWS
} CDFTableStruct;


typedef enum
{
  ReceiveSync,      /* 0 Receive sync byte state */
  ReceivePID,       /* 1 Receive PID byte state */
	ReceiveFrameInfo, /* 2 Receive frame information state */
  ReceiveData,      /* 3 Receive data byte state */
  ReceiveCheck,     /* 4 Receive crc checksum byte state */
	ReceiveIdle
} CXPIStates;
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
enum IFS_state
{
	isIFS,
	notIFS,
};
extern enum IFS_state ifsstat;

#endif