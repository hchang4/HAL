/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: AnalogIn.cpp 
 * *
 * *  Description: Analog Input device function access layer.
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
 * *  $Id: AnalogIn.cpp,v 1.14 2016/08/09 17:09:06 emrsn\jbeatty Exp $
 * *  $Log: AnalogIn.cpp,v $
 * *  Revision 1.14  2016/08/09 17:09:06  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.13  2015/05/04 14:49:24  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.12  2014/06/26 09:59:56  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.11  2013/01/30 06:18:30  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.10  2012/03/21 06:48:44  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.9  2011/03/25 16:20:41  emrsn\rsaha
 * *  Updated to convert ROCIO AI-16 voltage as mentioned in Datasheet.
 * *
 * *  Revision 1.8  2011/02/15 15:53:45  emrsn\rsaha
 * *  Revised to read ROCIO 16bit Analog Inputs.
 * *  TODO  - For ROCIO GetVoltage() method presently returning current value (in nA) only. Will revise it once we know the conversion.
 * *
 * *  Revision 1.7  2009/02/24 17:09:46  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.6  2008/03/06 20:47:03  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.5  2007/08/21 21:25:00  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.4  2007/03/13 08:39:39  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.3  2006/12/13 23:27:07  EPM-NA\slove2
 * *  added stdio.h so code compiles
 * *
 * *  Revision 1.2  2006/12/08 21:32:38  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include "string.h"
#include "stdio.h"
#include "unistd.h"

#include "FixEndian.h"
  
#include "debug.h"
#include "AnalogIn.h"

#define NR_ENUM_ROC_ANALOG_IN 4
#define DATA_BUFF_SZ 256

bool CAnalogIn::m_bROCIO1_InitDone = false;
bool CAnalogIn::m_bROCIO2_InitDone = false;
bool CAnalogIn::m_bROCIO3_InitDone = false;
bool CAnalogIn::m_bROCIO4_InitDone = false;
int CAnalogIn::m_nROCIOAICount[4 * NR_ENUM_ROC_ANALOG_IN] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

CAnalogIn::CAnalogIn()  // Default Constructor
{
  m_bROCIO1_InitDone = false;
  m_bROCIO2_InitDone = false;
  m_bROCIO3_InitDone = false;
  m_bROCIO4_InitDone = false;
  memset(&m_nROCIOAICount, 0, sizeof(m_nROCIOAICount));
}

CAnalogIn::~CAnalogIn() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CAnalogIn::OpenHal(char* pszDevName)
{
  int nRetVal = CBaseDev::OpenHal(pszDevName, FALSE);
  if ((nRetVal == ERR_SUCCESS) && (m_eCommType == SPI_COMM))
  {
    if ((m_bySlotID == 0) && !m_bROCIO1_InitDone)
    {
      m_bROCIO1_InitDone = true;
      nRetVal = InitROCIO();
    }
    else if ((m_bySlotID == 1) && !m_bROCIO2_InitDone)
    {
      m_bROCIO2_InitDone = true;
      nRetVal = InitROCIO();
    }
    else if ((m_bySlotID == 2) && !m_bROCIO3_InitDone)
    {
      m_bROCIO3_InitDone = true;
      nRetVal = InitROCIO();
    }
    else if ((m_bySlotID == 3) && !m_bROCIO4_InitDone)
    {
      m_bROCIO3_InitDone = true;
      nRetVal = InitROCIO();
    }
  }
  return nRetVal;
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CAnalogIn::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

/*
// Command message to init AI16 module
static const SpiCommandPair S_AI16BitInitMsg[] =
{
{ SPI_CLOCK_RATE , CLOCK_833K             }, // Set clock rate
{ SPI_CLOCK_STATE, CLOCK_STATE_HIGH       }, // Set clock state to high
{ SPI_CLOCK_PHASE, CLOCK_PHASE_BEGINNING  }, // Set clock phase to beginning
{ SPI_BIT_ORDER  , MSB_FIRST              }, // Set bit order to most sig bit
 
{ SPI_CHIP_SELECT, ENABLE_CS              }, // Select module chip select

{ SPI_WRITE      , 0x38                   }, // Write out init command
{ SPI_WRITE      , 0x01                   }, // Garbage coming back
{ SPI_WRITE      , 0x01                   }, // Garbage coming back
{ SPI_WRITE      , 0x01                   }, // Garbage coming back

{ SPI_WRITE      , 0xE0                   }, // Checksum byte #1
{ SPI_WRITE      , 0xD4                   }, // Checksum byte #2


{ SPI_WRITE      , 0x01                   }, // Garbage coming back
{ SPI_WRITE      , 0x01                   }, // Garbage coming back
{ SPI_READ_WRITE , 0x01                   }, // Ack or Nack
};
*/

int CAnalogIn::InitROCIO()
{
  int nRetVal = ERR_SUCCESS;
  char szData[DATA_BUFF_SZ] = {0};
  if (m_nExpSlotSPIfd != -1)
  {
    sprintf(szData, "%s", "bn 0x38 0x01 0x01 0x01 0xE0 0xD4 0x01 0x01 0x01" );
    if (write(m_nExpSlotSPIfd, szData, strlen (szData)) != (int) strlen (szData))
    {
      nRetVal = ERR_CMD_FAILED;
      DEBUG2("CAnalogIn::InitROCIO(): Error initializing ROC SPI AI device!");
    }
    memset( szData, 0, DATA_BUFF_SZ );
    read( m_nExpSlotSPIfd, szData, DATA_BUFF_SZ );
    //Find whether the response was an +ve Ack (0xF0) or -ve Ack (0x0F).
    char * szAck = strrchr( szData, 'x');
    if( szAck )
    {
      szAck++;
      if( ('F' == szAck[0] || 'f' == szAck[0]) &&
          ('0' == szAck[1] ) )
      {
      }
      else
      {
        DEBUG2("CAnalogIn::InitROCIO(): Received -ve Ack from ROCIO AI while trying to initialize it!");
        nRetVal = ERR_CMD_FAILED;
      }
    }
    else
    {
      DEBUG2("CAnalogIn::InitROCIO(): Failed to receive Init command response!");
      nRetVal = ERR_CMD_FAILED;
    }
  }
  else
  {
    nRetVal = ERR_INTERNAL_ERR;
    DEBUG2("CAnalogIn::InitROCIO(): Unexpected invalid file descriptor!");
  }
  return nRetVal;
}

void CAnalogIn::CalcCRC(const int *nDataPtr, int nNumBytes, 
                        char & byCRC1, char & byCRC2)
{
  byCRC1 = 0xA5;
  byCRC2 = 0x5A;

  for(int nIndex = 0; nIndex < nNumBytes; nIndex++)
  {
    byCRC1 += nDataPtr[nIndex];
    byCRC2 += byCRC1;
  }
}

/*
// Command message needed for reading raw values from AI 16-Bit Module via SPI
static const SpiCommandPair S_AI16BitSpiMsg[] =
{
{ SPI_CLOCK_RATE , CLOCK_833K             }, // Set clock rate
{ SPI_CLOCK_STATE, CLOCK_STATE_HIGH       }, // Set clock state to high
{ SPI_CLOCK_PHASE, CLOCK_PHASE_BEGINNING  }, // Set clock phase to beginning
{ SPI_BIT_ORDER  , MSB_FIRST              }, // Set bit order to most sig bit

{ SPI_CHIP_SELECT, ENABLE_CS              }, // Select module chip select

{ SPI_WRITE      , 0x80                   }, // Write out read command
{ SPI_WRITE      , 0x01                   }, // Garbage coming back
{ SPI_WRITE      , 0x01                   }, // Garbage coming back
{ SPI_WRITE      , 0x01                   }, // Garbage coming back

{ SPI_READ_WRITE , 0x01                   }, // CH 0 LSB
{ SPI_READ_WRITE , 0x01                   }, // CH 0 MSB
{ SPI_READ_WRITE , 0x01                   }, // CH 1 LSB
{ SPI_READ_WRITE , 0x01                   }, // CH 1 MSB
{ SPI_READ_WRITE , 0x01                   }, // CH 2 LSB
{ SPI_READ_WRITE , 0x01                   }, // CH 2 MSB
{ SPI_READ_WRITE , 0x01                   }, // CH 3 LSB
{ SPI_READ_WRITE , 0x01                   }, // CH 3 MSB

{ SPI_READ_WRITE , 0x01                   }, // Checksum #1
{ SPI_READ_WRITE , 0x01                   }, // Checksum #2

};
*/

int CAnalogIn::GetROCIOReading()
{
  int nRetVal = ERR_SUCCESS;
  char szData[DATA_BUFF_SZ] = {0};
  int nReadData[14] = {0};

  if (m_nExpSlotSPIfd != -1)
  {
    sprintf (szData, "%s", "bn 0x80 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01 0x01" );
    if (write (m_nExpSlotSPIfd, szData, strlen (szData)) != (int) strlen (szData))
    {
      nRetVal = ERR_CMD_FAILED;
      DEBUG2("CAnalogIn::GetROCIOReading(): Error sending read command to ROC SPI AI device!");
    }
    memset( szData, 0, DATA_BUFF_SZ );
    read( m_nExpSlotSPIfd, szData, DATA_BUFF_SZ );

    sscanf( szData, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            &nReadData[0], &nReadData[1], &nReadData[2], &nReadData[3], 
            &nReadData[4], &nReadData[5], &nReadData[6], &nReadData[7], 
            &nReadData[8], &nReadData[9], &nReadData[10], &nReadData[11], 
            &nReadData[12], &nReadData[13] );

    char byCRC1 = 0, byCRC2 = 0;
    CalcCRC( &nReadData[4], 8, byCRC1, byCRC2 );
    if( (byCRC1 != nReadData[12]) ||
        (byCRC2 != nReadData[13]) )
    {
      nRetVal = ERR_CMD_FAILED;
      DEBUG2( "CAnalogIn::GetROCIOReading(): CRC failure in read response!" );
    }
    else
    {
      int nStartIndex = (m_bySlotID - 1) * NR_ENUM_ROC_ANALOG_IN ;
      m_nROCIOAICount[0 + nStartIndex] = (nReadData[4] & 0xFF) << 8;
      m_nROCIOAICount[0 + nStartIndex] += (nReadData[5] & 0xFF);
      m_nROCIOAICount[1 + nStartIndex] = (nReadData[6] & 0xFF)<< 8;
      m_nROCIOAICount[1 + nStartIndex] += (nReadData[7] & 0xFF);
      m_nROCIOAICount[2 + nStartIndex] = (nReadData[8] & 0xFF)<< 8;
      m_nROCIOAICount[2 + nStartIndex] += (nReadData[9] & 0xFF);
      m_nROCIOAICount[3 + nStartIndex] = (nReadData[10] & 0xFF)<< 8;
      m_nROCIOAICount[3 + nStartIndex] += (nReadData[11] & 0xFF);
    }
  }
  else
  {
    nRetVal = ERR_INTERNAL_ERR;
    DEBUG2("CAnalogOut::GetROCIOReading(): Unexpected invalid file descriptor!");
  }
  return nRetVal;
}

// Get analog in current - Returns 0 on success, negative error code on failure.
int CAnalogIn::GetCurrent(unsigned long *pCurrentNanoAmps)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_ANALOG_IN_DATA_STRUCT stResp = {0};

  if (NULL == pCurrentNanoAmps)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_ANALOG_IN_FN_GET_CURRENT);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.CurrentNanoAmps);
          *pCurrentNanoAmps = stResp.stData.BaseIOData.CurrentNanoAmps;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CAnalogIn::GetCurrent(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogIn::GetCurrent(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogIn::GetCurrent(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      if( ERR_SUCCESS == GetROCIOReading() )
      {
        int nStartIndex = (m_bySlotID - 1) * NR_ENUM_ROC_ANALOG_IN ;
        *pCurrentNanoAmps = m_nROCIOAICount[m_byFnEnum-1 + nStartIndex];
        *pCurrentNanoAmps = (*pCurrentNanoAmps) * (24000000 / 65536);
      }
      break;
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogIn::GetCurrent(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogIn::GetCurrent(): Function called before Open Call!");
  }

  return nRetVal;
}


// Get analog in voltage - Returns 0 on success, negative error code on failure.
int CAnalogIn::GetVoltage(unsigned long *pMilliVolts)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_ANALOG_IN_DATA_STRUCT stResp = {0};

  if (NULL == pMilliVolts)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_ANALOG_IN_FN_GET_VOLTAGE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.MilliVolts);
          *pMilliVolts = stResp.stData.BaseIOData.MilliVolts;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CAnalogIn::GetVoltage(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;

        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogIn::GetVoltage(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogIn::GetVoltage(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
      if( ERR_SUCCESS == GetROCIOReading() )
      {
        int nStartIndex = (m_bySlotID - 1) * NR_ENUM_ROC_ANALOG_IN ;
        *pMilliVolts = m_nROCIOAICount[m_byFnEnum-1 + nStartIndex];
        //As per datasheet of AI-16 Module (ROC800-Series), Voltage reading is 91.55 uV/Count
        *pMilliVolts = (unsigned long)((*pMilliVolts) * 0.09155); 
      }
      break;
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogIn::GetVoltage(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogIn::GetVoltage(): Function called before Open Call!");
  }

  return nRetVal;
}


// Get Device Status
int CAnalogIn::GetStatus (int* pnStatus)      // Pointer to write status to
{
  return ERR_NOT_IMPLEMENTED;
}

//Get board revision information
int CAnalogIn::GetBoardInfo (   unsigned char* byFirmwareVerMaj,    // Firmware Version - Major Number
                                unsigned char* byFirmwareVerMin,    // Firmware Version - Minor Number
                                unsigned char* byFirmwareVerBuild,    // Firmware Version - Minor Number
                                unsigned char* byBoardRevision)     // Board Revision
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
          DEBUG2("CAnalogIn::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogIn::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogIn::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogIn::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogIn::GetBoardInfo(): Function called before Open Call!");
  }

  return nRetVal;

  
  return ERR_NOT_IMPLEMENTED;
}



// Gets the on-board temperature in Milli Degree C.
int CAnalogIn::GetOnBoardTemp(int* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_ANALOG_IN_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_ANALOG_IN_FN_GET_ON_BD_TEMP);

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
          DEBUG2("CAnalogIn::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogIn::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogIn::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogIn::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogIn::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
}
