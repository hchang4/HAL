/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Serial.cpp
 * *
 * *  Description: Serial IO device function access layer
 * *
 * *  Copyright:        Copyright (c) 2011-2012, 
 * *                    Rosemount Analytical 
 * *                    All Rights Reserved.
 * *
 * *  Operating System:  None.
 * *  Language:          'C++'
 * *  Target:            Gas Chromatograph Model GC700XP
 * *
 * *  Revision History:
 * *  $Id: Serial.cpp,v 1.20 2015/05/04 14:49:28 EMRSN\sboyidi Exp $
 * *  $Log: Serial.cpp,v $
 * *  Revision 1.20  2015/05/04 14:49:28  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.19  2014/12/11 03:40:32  EMRSN\jchekkala
 * *  Corrected a comment and replaced couple of printfs with DEBUG.
 * *
 * *  Revision 1.18  2014/12/02 13:32:12  emrsn\jchekkala
 * *  IsNeedHWSwitches() method is applicable for Serial Ports on BaseIO Board. For other ports it will return TRUE by default.
 * *  IsRS422ModeSupported() method is applicable for Serial Ports on BaseIO Board. For other ports it will return FALSE by default.
 * *
 * *  Revision 1.17  2014/12/02 11:39:58  emrsn\jchekkala
 * *  set Serial driver mode to either .RS485. or .RS422 will be supported for serial ports on Base IO G2 board.
 * *  Will not indicate the need of Hardware Switches for serial ports on Base IO G2 board.
 * *
 * *  Revision 1.16  2014/06/26 10:01:02  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.15  2013/01/30 06:18:52  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.14  2012/03/21 06:49:00  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.13  2010/07/24 14:30:27  emrsn\rrthomas
 * *  Flush the stream pipe after an open call to prevent user application from getting data from a previous session.
 * *
 * *  Revision 1.12  2010/01/07 06:50:34  emrsn\rsaha
 * *  Revised to write - virtual serial response messages in one shot which the already existing fragmentation layer in HAL splits up in smaller CAN packets and sends to BaseIO with computed CRC code appended.
 * *  Previous to this response message was written in smaller packets without any help of fragmentation layer, preventing BaseIO to comprehend the size of the total response. Due to this BaseIO used to write each small packet as soon as it received to USART, instead of waiting for the full response, resulting in check-sum errors for Modbus client applications.
 * *
 * *  Note - With these GC firmware changes running, BaseIO needs the corresponding updates too. Latest BaseIO with old GC firmware or vice versa, would lead to ERRORs in Virtual serial communication.
 * *
 * *  Revision 1.11  2009/02/24 17:09:47  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.10  2008/03/06 20:47:04  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.9  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.8  2007/03/13 08:39:40  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.7  2006/12/17 21:01:24  EPM-NA\ssatpute
 * *  GetErrorMsg() method added to resolve error message from error code.
 * *
 * *  Revision 1.6  2006/12/15 16:32:42  epm-na\DPrabhakar
 * *  CSerial's Read() and Write() APIs were clashing the Read and Write macros
 * *  defined for GC700XP. Have changed the names to ReadCh(), ReadChBlocking(),
 * *  and WriteCh() to work around this issue.
 * *
 * *  Related changes to the TestHAL application.
 * *
 * *  Revision 1.5  2006/12/13 22:21:14  epm-na\DPrabhakar
 * *  Code clean up
 * *    - Removed un-used enum in BaseIOProtocol.h
 * *
 * *  Revision 1.4  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include "FixEndian.h"
  
#include "debug.h"
#include "Serial.h"
#include "BoardSlotInfo.h"


CSerial::CSerial()  // Default Constructor
{
  m_pobRxFifo = NULL;
  m_pobRxFifo = new CFifo(MAX_SER_CH_RX_FIFO_SIZE);
  m_iBoardRevision = -1;
}
  
CSerial::~CSerial() // Destructor
{
  if (m_pobRxFifo)
  {
    delete m_pobRxFifo;
    m_pobRxFifo = NULL;
  }
}

// Open the device. Returns 0 on success, negative error code on failure.
int CSerial::OpenHal (char* pszDevName)
{
  int nRetVal = CBaseDev::OpenHal (pszDevName, TRUE); 

  // Flush any residual data
  if (nRetVal == ERR_SUCCESS)
  {
    nRetVal = Flush();
  }

  return nRetVal;
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CSerial::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Get Device Status
int CSerial::GetStatus (int* pnStatus)      // Pointer to write status to
{
  return ERR_NOT_IMPLEMENTED;
}

int CSerial::GetBoardInfo(unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                          unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                          unsigned char* byFirmwareVerBuild,  // Firmware Version - Build Number
                          unsigned char* byBoardRevision) // Board Revision
{
  int nRetVal = ERR_SUCCESS;
  
  CmdAckUnion stCmd = {0};
  CAN_BASEIO_SYSINFO_STRUCT stResp = {0};
  
  if ( (NULL == byFirmwareVerMaj) || (NULL == byFirmwareVerMin) || 
       (NULL == byFirmwareVerBuild) || (NULL == byBoardRevision))
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, BD_GET_SYSTEM_INFO);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *)&stResp,  // Response from remote board
                                                     sizeof(stResp));     // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof(stResp))
        {
          *byFirmwareVerMaj = stResp.stData.majorVer;
          *byFirmwareVerMin = stResp.stData.minorVer;
          *byFirmwareVerBuild = stResp.stData.buildVer;
          *byBoardRevision = stResp.stData.Revision;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CSerial::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CSerial::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::GetBoardInfo(): Function called before Open Call!");
  }

  return nRetVal;

  
}


// Set Serial Port parameters
int CSerial::SetCommParams(BASEIO_SER_BAUD_ENUM eBaudRate, 
                           BASEIO_SER_DATA_LEN_ENUM eDataLen,
                           BASEIO_SER_PARITY_ENUM eParity,
                           BASEIO_SER_STOP_BITS_ENUM eStopBits,
                           BASEIO_SER_PORT_MODE_ENUM ePortMode) 
{
  int nRetVal = ERR_SUCCESS;

  CAN_SERIALIO_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};


  if( !IsRS422ModeSupported() && SERIALIO_PORT_RS422 == ePortMode )
  {
    DEBUG2("G1 Board SERIALIO_PORT_RS485/SERIALIO_PORT_RS422 (1)");
    ePortMode = SERIALIO_PORT_RS485; 
  }
  else
  {
    DEBUG2("G2 Board SERIALIO_PORT_RS485 (1) and SERIALIO_PORT_RS422 (3)");
  }

  if ( (eBaudRate < MIN_BAUD_RATE) || (eBaudRate >= MAX_BAUD_RATE) || 
       (eDataLen < MIN_SER_DATA_LEN) || (eDataLen >= MAX_SER_DATA_LEN) ||
       (eParity < MIN_PARITY_SETTING) || (eParity >= MAX_PARITY_SETTING) || 
       (eStopBits < MIN_STOP_BITS) || (eStopBits >= MAX_STOP_BITS) || 
       (ePortMode < MIN_SERIALIO_PORT_MODE) || (ePortMode >= MAX_SERIALIO_PORT_MODE))
  {
    return ERR_INVALID_ARGS;
  }

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_SERIALIO_FN_SET_PORT_CONFIG);

        stCmd.stData.BaseIOData.serialConfig.serialBaudRate = eBaudRate;
        stCmd.stData.BaseIOData.serialConfig.serialDataLen = eDataLen;
        stCmd.stData.BaseIOData.serialConfig.serialParity = eParity;
        stCmd.stData.BaseIOData.serialConfig.serialStopBits = eStopBits;
        stCmd.stData.BaseIOData.serialConfig.serialPortMode = ePortMode;

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CSerial::SetCommParams(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CSerial::SetCommParams(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CSerial::SetCommParams(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::SetCommParams(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::SetCommParams(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::SetCommParams(): Function called before Open Call!");
  }

  return nRetVal;
}

// Set Serial Port parameters
int CSerial::GetCommParams(BASEIO_SER_BAUD_ENUM *eBaudRate, 
                           BASEIO_SER_DATA_LEN_ENUM *eDataLen,
                           BASEIO_SER_PARITY_ENUM *eParity,
                           BASEIO_SER_STOP_BITS_ENUM *eStopBits,
                           BASEIO_SER_PORT_MODE_ENUM *ePortMode) 
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_SERIALIO_DATA_STRUCT stResp = {0};

  if ( (NULL == eBaudRate) || (NULL == eDataLen) || (NULL == eParity) ||
       (NULL == eStopBits) || (NULL == ePortMode) )
  {
    return ERR_INVALID_ARGS;
  }

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_SERIALIO_FN_GET_PORT_CONFIG);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *eBaudRate = (BASEIO_SER_BAUD_ENUM)      stResp.stData.BaseIOData.serialConfig.serialBaudRate;
          *eDataLen  = (BASEIO_SER_DATA_LEN_ENUM)  stResp.stData.BaseIOData.serialConfig.serialDataLen;
          *eParity   = (BASEIO_SER_PARITY_ENUM)    stResp.stData.BaseIOData.serialConfig.serialParity;
          *eStopBits = (BASEIO_SER_STOP_BITS_ENUM) stResp.stData.BaseIOData.serialConfig.serialStopBits;
          *ePortMode = (BASEIO_SER_PORT_MODE_ENUM) stResp.stData.BaseIOData.serialConfig.serialPortMode;

          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CSerial::GetCommParams(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CSerial::GetCommParams(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::GetCommParams(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::GetCommParams(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::GetCommParams(): Function called before Open Call!");
  }

  return nRetVal;
}


//Set the timeout (in ms) for transmission of less than a packet of data on the device    
int CSerial::SetTxTimeoutMs(unsigned long ulTimeout)
{
  int nRetVal = ERR_SUCCESS;

  CAN_SERIALIO_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};

  if (0 == ulTimeout)
  {
    return ERR_INVALID_ARGS;
  }

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_SERIALIO_FN_SET_TX_TIMEOUT);

        stCmd.stData.BaseIOData.SerialIOTxTimeoutMs = ulTimeout;
        FixEndian(stCmd.stData.BaseIOData.SerialIOTxTimeoutMs);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CSerial::SetTxTimeoutMs(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CSerial::SetTxTimeoutMs(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CSerial::SetTxTimeoutMs(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::SetTxTimeoutMs(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::SetTxTimeoutMs(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::SetTxTimeoutMs(): Function called before Open Call!");
  }

  return nRetVal;
}

// Reads data from Remote Serial Port. Note: This is a streaming data read method and reads the data queued up in the rx fifo. 
// Data will queue up to max FIFO size which is at least 4kB.
int CSerial::ReadCh(unsigned char* Data, unsigned int NumBytes, unsigned int Timeout)
{
  return ReadCommon(Data, NumBytes, &Timeout);
}

// Reads data from Remote Serial Port. Note: This is a streaming data read method and reads the data queued up in the rx fifo. 
// Data will queue up to max FIFO size which is at least 4kB.
int CSerial::ReadChBlocking(unsigned char* Data, unsigned int NumBytes)
{
  return ReadCommon(Data, NumBytes, NULL);
}

// Write data to remote serial port
int CSerial::WriteCh(unsigned char* Data, unsigned int NumBytes)
{
  int nRetVal = ERR_SUCCESS;

  CAN_BASEIO_STATUS_STRUCT stResp = {0};

  if ( (NULL == Data) || (0 == NumBytes) )
  {
    return ERR_INVALID_ARGS;
  }

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        // Send Serial IO streaming data across the CAN interface
        nRetVal = m_pobReliabilityCAN->GetRemoteResp(   Data,       // Serial data
                                                        NumBytes,     // Size of command
                                                        (unsigned char *) &stResp,  // Response from remote board
                                                        sizeof (stResp),    // Size of expected response
                                                        TRUE);        // Indicate streaming data


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CSerial::Write(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CSerial::Write(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CSerial::Write(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::SetWrite(): Unexpected invalid pointer!");

      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::Write(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::Write(): Function called before Open Call!");
  }

  return nRetVal;
}

// Gets the on-board temperature in Milli Degree C.
int CSerial::GetOnBoardTemp(int* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_SERIALIO_DATA_STRUCT stResp = {0};

  if (NULL == TempMilliDegC)
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_SERIALIO_FN_GET_ON_BD_TEMP);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.OnBdTempmDegC);
          *TempMilliDegC = stResp.stData.BaseIOData.OnBdTempmDegC;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CSerial::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CSerial::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
}




// Flushes the contents of the Named Pipe that is used for streaming data for this device.
int CSerial::Flush() 
{
  if (m_pobCAN)
  {
    return m_pobCAN->CANFlushStreamPipe ();
  }
  else
  {
    return ERR_INTERNAL_ERR;
  }
}


//Common logic for both blocking and non blocking reads
int CSerial::ReadCommon(unsigned char *Data, unsigned int NumBytes,  unsigned int *Timeout)
{
  int nRetVal = ERR_SUCCESS;
  SERIALIO_STREAM_STRUCT stResp;
  
  unsigned int unRemTimeout = 0;


  //Do not allow the user to specify NumBytes more than what we can
  //  store in our Rx buffer (minus 1 packet length of serial data - because
  //  we first write to the buffer and then check if there is sufficient
  //  data available)
  //NOTE: The current datatype for NumBytes will allow a max request of 255 bytes only
  if ((NULL == Data) || (0 == NumBytes) || 
      (NumBytes > (MAX_SER_CH_RX_FIFO_SIZE - MAX_SERIALIO_PKT_LEN) ))
  {
    return ERR_INVALID_ARGS;
  }

  if (Timeout)
  {
    unRemTimeout = *Timeout;
  }
  else
  {
    unRemTimeout = 1;
  }

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
        
      if (m_pobCAN)
      {
        if (m_pobRxFifo)
        {
          //First check and see if the receive store contains the 
          //  requested amount of data
          if (m_pobRxFifo->GetLen() >= NumBytes)
          {
            m_pobRxFifo->ReadFifo(Data, NumBytes);
            nRetVal = NumBytes;
          }
          else
          {
            while (ERR_SUCCESS == nRetVal)
            {
              if (Timeout)
              {
                //Receive streaming data (non-blocking mode)
                nRetVal = m_pobCAN->CANRxStrmTimeout((unsigned char *) &stResp,   // Response from remote board
                                                     sizeof (stResp), // Size of the expected response 
                                                     unRemTimeout, // Timeout value
                                                     &unRemTimeout);  // Store the time remaining out of the specified timeout
              }
              else
              {
                //Receive streaming data (blocking mode)
                nRetVal = m_pobCAN->CANRxStrmBlocking((unsigned char *) &stResp,// Response from remote board
                                                      sizeof (stResp));
              }

              // Check if we got the correct response packet
              if (nRetVal > 0)  
              {
                unsigned int unWriteLen = 0;
                //The user can only request a maximum of our buffer size
                //  (MAX_SER_CH_RX_FIFO_SIZE) minus 1 packet length (MAX_SERIALIO_PKT_LEN)
                //  So, we'll not have the condition of running out of receive buffer
                //  space when we write to the rx buffer
                unWriteLen = m_pobRxFifo->WriteFifo(stResp.ucData, nRetVal);
                if ( (int)unWriteLen != nRetVal)
                {
                  DEBUG2("CSerial::ReadCommon(): Couldn't write all RX data to FIFO.");
                }

                //Reset this to ERR_SUCCESS so that we don't quit reading till we
                //  get enough data or till timeout
                nRetVal = ERR_SUCCESS;

                if (m_pobRxFifo->GetLen() >= NumBytes)
                {
                  nRetVal = m_pobRxFifo->ReadFifo(Data, NumBytes);
                }
              }
              else if (ERR_TIMEOUT == nRetVal)
              {
                //If a timeout occurred, if there is some serial data,
                // return the same to the user.
                if (m_pobRxFifo->GetLen())
                {
                  //nRetVal = m_pobRxFifo->Read(Data, NumBytes);
                  nRetVal = m_pobRxFifo->ReadFifo(Data, m_pobRxFifo->GetLen());
                }
              }
              else if (nRetVal < 0)
              {
                DEBUG2("CSerial::ReadCommon(): CCANComm::CANRxStrmTimeout failed with error code %d!", 
                       nRetVal);
              }
            }
          }
        }
        else
        {
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CSerial::ReadCommon(): Error accessing receive store!");
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CSerial::ReadCommon(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CSerial::ReadCommon(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CSerial::ReadCommon(): Function called before Open Call!");
  }

  return nRetVal;


  return ERR_NOT_IMPLEMENTED;
};


// Returns FALSE if G2 BaseIO Board else TRUE
bool CSerial::IsNeedHWSwitches( void )
{
  bool bIsNeedHWSwitches = TRUE; // Def: G1 Board. Need Hard Ware Switches

  if( m_iBoardRevision < 0 && ( strstr(CBaseDev::m_szDevName, "BASE_IO") != NULL ) )
  {
    // Get From current_software_info.txt. Once Only
    CBoardSlotInfo oCBoardSlotInfo;
    unsigned char ucCANId = CBaseDev::m_bySlotID;
    oCBoardSlotInfo.GetBoardRevision( "BaseIO", &ucCANId, m_iBoardRevision );
  }
  
  if( m_iBoardRevision >= G2_BASEIO_BOARD )
  {
    // G2 Board. No Need of Hard Ware Switches
    bIsNeedHWSwitches = FALSE;
  }

  return bIsNeedHWSwitches;
};

// Returns TRUE if G2 BaseIO Board else FALSE
bool CSerial::IsRS422ModeSupported( void )
{
  bool bRS422Support = FALSE; // Def: G1 Board. RS422 Mode NOT Supported 

  if( m_iBoardRevision < 0 && ( strstr(CBaseDev::m_szDevName, "BASE_IO") != NULL ) )
  {
    // Get From current_software_info.txt. Once Only
    CBoardSlotInfo oCBoardSlotInfo;
    unsigned char ucCANId = CBaseDev::m_bySlotID;
    oCBoardSlotInfo.GetBoardRevision( "BaseIO", &ucCANId, m_iBoardRevision );
  }
  
  if( m_iBoardRevision >= G2_BASEIO_BOARD )
  {
    // G2 Board. RS422 Mode Supported
    bRS422Support = TRUE;
  }

  return bRS422Support;
}



