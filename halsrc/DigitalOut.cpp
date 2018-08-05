/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: DigitalOut.cpp
 * *
 * *  Description: Digital Output device function access layer.
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
 * *  $Id: DigitalOut.cpp,v 1.17 2016/08/09 17:09:06 emrsn\jbeatty Exp $
 * *  $Log: DigitalOut.cpp,v $
 * *  Revision 1.17  2016/08/09 17:09:06  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.16  2015/05/04 14:49:26  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.15  2014/06/26 10:00:12  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2013/01/30 06:18:33  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.13  2012/03/21 06:48:47  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.12  2011/08/13 11:08:42  emrsn\rsaha
 * *  Reverted few unwanted changes that got checked-in by mistake on 13th of July.
 * *
 * *  Revision 1.11  2011/07/13 11:01:37  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.10  2011/02/19 12:57:40  emrsn\rsaha
 * *  Added support for ROCIO DIs and DOs for both expansion slot 1 and 2.
 * *
 * *  Revision 1.9  2009/02/24 17:09:47  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.8  2008/03/06 20:47:03  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.7  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.6  2007/03/13 08:39:39  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.5  2006/12/13 23:27:07  EPM-NA\slove2
 * *  added stdio.h so code compiles
 * *
 * *  Revision 1.4  2006/12/08 21:32:38  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "FixEndian.h"
  
#include "debug.h"
#include "DigitalOut.h"

#define DATA_BUFF_SZ 256

bool CDigitalOut::m_bROCIO_DOState[4 * NR_ENUM_ROC_DIGITAL_OUT] = {false, false, false, false, false, 
                                                                   false, false, false, false, false,
                                                                   false, false, false, false, false,
                                                                   false, false, false, false, false};

CDigitalOut::CDigitalOut()  // Default Constructor
{
}

CDigitalOut::~CDigitalOut() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CDigitalOut::OpenHal (char* pszDevName)
{
  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CDigitalOut::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Set the DigitalIn status - Returns 0 on success, negative error code on failure.
int CDigitalOut::SetDigitalOutStatus(BOOL bStatus)
{
  int nRetVal = ERR_SUCCESS;

  CAN_DIGITAL_OUT_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_DIGITAL_OUT_FN_SET_CH_STATE);
        
        if (bStatus)
        {
          stCmd.stData.BaseIOData.DigIOState = DIO_STATE_ON;
        }
        else
        {
          stCmd.stData.BaseIOData.DigIOState = DIO_STATE_OFF;
        }
        
        FixEndian(stCmd.stData.BaseIOData.DigIOState);

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
            DEBUG2("CDigitalOut::SetDigitalOutStatus(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CDigitalOut::SetDigitalOutStatus(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CDigitalOut::SetDigitalOutStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CDigitalOut::SetDigitalOutStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      if (m_nExpSlotSPIfd != -1)
      {
        int nStartIndex = (m_bySlotID - 1) * NR_ENUM_ROC_DIGITAL_OUT;
        if( TRUE == bStatus )
        {
          m_bROCIO_DOState[m_byFnEnum-1 + nStartIndex] = true;
        }
        else
        {
          m_bROCIO_DOState[m_byFnEnum-1 + nStartIndex] = false;
        }
        int nCmdData = 0x0;
        for( int nInd = NR_ENUM_ROC_DIGITAL_OUT-1; nInd >= 0; nInd-- )
        {
          nCmdData = nCmdData << 1;
          nCmdData |= (m_bROCIO_DOState[nStartIndex + nInd])? 1:0;
        }
        nCmdData |= 0x80;

        char  szData[DATA_BUFF_SZ] = {0};
        sprintf(szData, "bn 0x%x", nCmdData);
        if(write (m_nExpSlotSPIfd, szData, strlen (szData)) != (int) strlen (szData))
        {
          nRetVal = ERR_CMD_FAILED;
          DEBUG2("CDigitalOut::GetDigitalOutStatus(): Error sending write command to ROC SPI DO device!");
        }
        //Flush out data received during write operation.
        read( m_nExpSlotSPIfd, szData, DATA_BUFF_SZ );
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CDigitalOut::SetDigitalOutStatus(): Unexpected invalid file descriptor!");
      }
      break;
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CDigitalOut::SetDigitalOutStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!

    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CDigitalOut::SetDigitalOutStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Get Device Status
int CDigitalOut::GetStatus (int* pnStatus)      // Pointer to write status to
{
  return ERR_NOT_IMPLEMENTED;
}

//Get board revision information
int CDigitalOut::GetBoardInfo ( unsigned char* byFirmwareVerMaj,    // Firmware Version - Major Number
                                unsigned char* byFirmwareVerMin,    // Firmware Version - Minor Number
                                unsigned char* byFirmwareVerBuild,    // Firmware Version - Minor Number
                                unsigned char* byBoardRevision)     // Board Revision
{
  int nRetVal = ERR_SUCCESS;
  
  CmdAckUnion stCmd = {0};
  CAN_BASEIO_SYSINFO_STRUCT stResp = {0};
  
  if ( (NULL == byFirmwareVerMaj) || (NULL == byFirmwareVerMin) || 
       (NULL == byFirmwareVerBuild) ||(NULL == byBoardRevision))
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
          DEBUG2("CDigitalOut::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CDigitalOut::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CDigitalOut::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CDigitalOut::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CDigitalOut::GetBoardInfo(): Function called before Open Call!");
  }

  return nRetVal;
}

// Gets the on-board temperature in Milli Degree C.
int CDigitalOut::GetOnBoardTemp(int* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_DIGITAL_OUT_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_DIGITAL_OUT_FN_GET_ON_BD_TEMP);

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
          DEBUG2("CDigitalOut::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CDigitalOut::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CDigitalOut::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CDigitalOut::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CDigitalOut::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
}
