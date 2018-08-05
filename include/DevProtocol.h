#ifndef _DEV_PROTOCOL_H
#define _DEV_PROTOCOL_H

//Length of the device function name string field in 
//  the device capability structure
#define DEV_FN_CAP_NAME_LEN   24

// Plug in module Function Types in the GC700XP
enum DEV_FN_TYPE {
  FN_UNKNOWN = 0,   // Unknown function
  FN_ANA_IN = 1,    // Analog In
  FN_ANA_OUT = 2,   // Analog Out 
  FN_DIGI_IN = 3,   // Digital Input
  FN_DIGI_OUT = 4,  // Digital Output
  FN_PREAMP_STR = 5,  // Bridge Preamp streaming function (on dual channel bridge preamp board)
  FN_PREAMP_CFG = 6,  // Bridge Preamp configuration function (on dual channel bridge preamp board)
  FN_RTD = 7,         // RTD (on Heater/Solenoid ctrl board)
  FN_HTR_CTRL = 8,    // Heater Channel (On Heater/Solenoid board)
  FN_SOL = 9,         // Solenoid (on Heater/Solenoid board)
  FN_SERIAL = 10,     // Serial COM
  FN_EPC = 11,        // EPC Controller (On Base IO board)
  FN_LTLOI = 12,      // LED/Toggle LOI
  FN_FFB_STATUS = 13, // Funtion to send GC status information to FFB board
  FN_FFB_COMMAND = 14,    // Funtion to get remote commands from FFB board to configure GC
  FN_GRAPHICAL_LOI = 15,  // Graphical Local Operator Interface
  FN_DIAGNOSTIC = 16,     // Function to retrieve diagnostic information from all boards
  FN_FID = 17,            // Function to control FID Ignition / Gain
  FN_FPD = 18,            // Function to control FPD AutoZero
  FN_PRESSURE = 19,       // Function to retrieve Sample pressure
  FN_CTRL = 20,           // Function for Control pins - e.g. - RS232/RS485 toggle for 370XA
  FN_IMB_COMM = 21,       // Function for IMB board communication
  FN_FPD_G2 = 22,         // Function to control FPD G2
  FN_CYCLE_CLOCK_SYNC = 29,    // Start/Stop Cycle Clock
  FN_REBOOT = 30,         // Function to reboot the board
  FN_CAP = 31,    // Response to board capability query
};

// Commands acknowledged by all boards
// Max command size 1 byte
typedef enum {

  BD_GET_DEV_CAP = (unsigned char) 127,     // Get the Board Capability Information
  BD_GET_SYSTEM_INFO = (unsigned char) 126  // Get System Information

} BD_CMD_ENUM;

// Device Address (FnType : FnCount)- Host to Device Comm
typedef struct {

  unsigned short Reserved1 : 5; // Reserved 
  unsigned short FnType : 5;    // Function Type
  unsigned short FnCount : 4;   // Function Eumeration
  unsigned short Fragment : 1;  // Reserved - Fragmentation not supported Host -> Device
  unsigned short CmdType : 1;   // 0 - Command to Remote Device, 1 - Acknowledgement

} __attribute (( packed )) DEV_ADDR_HOST_2_DEV_COMM;

// Device Address (SlotID : FnType : FnCount)- Device to Host Comm
typedef struct {

  unsigned short SlotID : 5;    // Slot ID
  unsigned short FnType : 5;    // Function Type
  unsigned short FnCount : 4;   // Function Enumeration
  unsigned short Fragment : 1;  // 1 - Fragemented Packet, 0 - Otherwise
  unsigned short RemDataType : 1; // 0 - Normal data, 1 - Streaming data

} __attribute (( packed )) DEV_ADDR_DEV_2_HOST_COMM;

typedef union {

  unsigned short  usDevAd;
  DEV_ADDR_HOST_2_DEV_COMM stDevAdH2D;
  DEV_ADDR_DEV_2_HOST_COMM stDevAdD2H;

} DevAddrUnion;

typedef struct {

  unsigned char Command : 7;  // Function specific command
  unsigned char Err : 1;      // 0 -> Ack, 1 -> Nack - Used by the device when responding to the Host. 
  // Note - this bit is ignored by the device for Host -> Device Comm

} __attribute (( packed )) CMD_ACK_STRUCT;

typedef union {
  unsigned char byCmdAck;
  CMD_ACK_STRUCT stCmdAck;
} __attribute (( packed )) CmdAckUnion;


/*// Command / Response struct to be used for comm between Host and Device...
  typedef struct {

  DevAddrUnion    DevAddr;
  CmdAckUnion     CmdAck;

  } 
  #ifndef WIN32
  __attribute (( packed )) 
  #endif  
  COMMAND_RESP_STRUCT;*/

// Status Struct - To be used for returning Status / NACK from device to host
typedef struct {

  union {
    unsigned char ErrorType;    // Type of error that occurred
    unsigned char Status;       // Status
  }__attribute (( packed )) StatusDataUnion;

} __attribute (( packed )) STATUS_STRUCT;


typedef struct {

  unsigned short FnType : 5;    // Fn Type (eg. Analog In, Analog Out etc...)
  unsigned short FnCnt : 4;   // Number of enumerations of the Fn Type 
  // (eg. 4 Analog Inputs, 3 Digital Outputs etc)
  unsigned short Reserved : 7;  // Reserved
  char szDevName[DEV_FN_CAP_NAME_LEN];  //Device function name.
} __attribute (( packed )) DEV_FN_CAP_STRUCT;

typedef union
{
  DEV_FN_CAP_STRUCT stDevCapInfo;
  unsigned short usDevCap;
} DevFnCapUnion;

typedef struct { 
  
  CmdAckUnion stCmd;
  STATUS_STRUCT stStatus;
} __attribute (( packed )) NACK_STRUCT;

/* Board version informatino */
typedef struct {
  unsigned char majorVer;
  unsigned char minorVer;
  unsigned char buildVer;
  unsigned char Revision;
}__attribute(( packed ))DEVICE_SYSTEM_INFO_STRUCT;

// ACK / NACK sent from Device to Host in response to commands. 
// If the device understands the command and the parameters (if any) passed 
// along with command are valid and the device is able to successfully perform 
// the action, it ACKs. Otherwise it NACKs with an error code. 
// TODO when modifying HAL code - Make sure you make a translation for all new
// enums that you add here in CBaseDev::TransDevError function (BaseDev.cpp)
typedef enum  {
  ACK_NO_ERR = 0,           // ACK, no error, successful
  NACK_INVALID_FN_TYPE = 1, // Invalid Function Type
  NACK_INVALID_FN_ENUM = 2, // Invalid Function Enum 
  NACK_INVALID_CMD = 3,     // Unrecognized command
  NACK_INT_ERR = 4,         // Some internal error occurred
  NACK_CMD_FAILED = 5,      // Command failed
} ACK_CODE;

/*
  CAN Message Identifier Map
  --------------------------
  Discussion - 

  Function Type / Function Enumeration -
  The GC700XP can have upto 8 different types of boards plugged into it. Each 
  board can have one or more functions and multiple enumerations of each function. 
  For example, the Bridge Preamp Board supports only 1 function - Bride Preamp, but it 
  has 2 channels of preamps. So this board supports one function, but two enumerations 
  of this function. 

  In a given GC700XP, one can plug in four boards of each type. For example you can plug in 
  four preamp boards. Each board computes its CAN Msg Identifier using the following
  scheme - Each board has two dip switches. Each board on boot up reads its 
  dip switches and ORs its dip switch settings with the boards Base CAN Msg Identifier. 
  It then listens to all CAN Messages at this Message Identifier. 

  Each board sends responses back to the GC700XP CPU board at a CAN Message Idenfier address
  computed with the following scheme - The CPU boards base Message Identifier is 
  ORed with the devices CAN Message Identifier. 
  You might at this point be wondering why this is so! 
  Why not a common address for the host CPU and all devices send CAN packets 
  to that address. This is because it is entirely 
  possible in the life of the GC that more than one device will try to talk to the 
  Host CPU at the exact same moment. If data is being sent to the same address, 
  both devices wont't back off and both will try to transmit at the same time. 
  The bit arbitration will come into effect only when the data is being 
  transmitted. This can lead to a network lockup. In order to avoid this, 
  the devices will try and transmit to different host CPU Ids. The host CPU will 
  listen to all incoming packets on the network. 

  ****** Important Note ******
  These #defined addresses will be used by the Remote devices to compute 
  device addresses. The CPU board should not use these addresses for device discovery. 
  It should use the ping command to determine the device function supported on each board. 
  The CPU board should ping starting from CAN Msg ID 0 to 0x1F (31 d). 
*/

// CAN Message ID of the GC700XP Main CPU board 
#define GC700XP_HOST_BASE_CAN_MSG_ID             0x100

// Broadcast CAN Message ID for synchronizing Cycle Clocks 
#define CYCLE_CLOCK_SYNC_CAN_MSG_ID              0x1

// 370XA LOI slot
#define GC370XA_LOI_CAN_MSG_ID                   0x03

// FPD G2 board
#define GC700XP_FPD_G2_BASE_CAN_MSG_ID           0x0E

// Dual channel Preamp Board
#define GC700XP_PREAMP_BD_CAN_BASE_MSG_ID        0x10

// Solenoid / Heater Control Board
#define GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID  0x14

// Foundation Fieldbus Board
#define GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID      0x18

// Graphical Local Operator Interface
#define GC700XP_GRAPHICAL_LOI_BD_BASE_CAN_MSG_ID 0x19

// 370XA Analyzer Board
#define GC370XA_ANALYZER_BD_BASE_CAN_MSG_ID      0x1A

// 370XA IMB Board
#define GC370XA_IMB_BD_BASE_CAN_MSG_ID           0x1B

// Base IO Board
#define GC700XP_BASE_IO_BD_BASE_CAN_MSG_ID       0x1C

#endif // #ifndef _DEV_PROTOCOL_H
