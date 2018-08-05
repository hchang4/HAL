/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: AnalogOut.cpp 
 * *
 * *  Description: Analog OUT device function access layer.
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
 * *  $Id: AnalogOut.cpp,v 1.21 2016/08/09 17:09:06 emrsn\jbeatty Exp $
 * *  $Log: AnalogOut.cpp,v $
 * *  Revision 1.21  2016/08/09 17:09:06  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.20  2015/05/04 14:49:24  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.19  2015/03/13 09:50:59  EMRSN\sboyidi
 * *  Bug 8188 - Need to restrict maximum ROC AO Output Current to 20.94 mA.
 * *  Fix : Restricted maximum ROC AO Output Current to 20.94 mA.
 * *
 * *  Revision 1.18  2014/12/02 13:29:36  emrsn\jchekkala
 * *  IsLoopPowerSelectable() method applicable only for AOs on BaseIO Board
 * *
 * *  Revision 1.17  2014/12/02 11:38:02  emrsn\jchekkala
 * *  Selectable Loop Power Feature will be supported for G2 BaseIO Board.
 * *  In case of G1 BaseIO board, selectable loop power feature will be disabled.
 * *
 * *  Revision 1.16  2014/06/26 09:59:59  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.15  2013/01/30 06:18:31  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.14  2012/04/25 09:32:38  emrsn\rsaha
 * *  Revised structure names as changed in LOI370XAUDPCommProtocol.h file.
 * *
 * *  Revision 1.13  2012/03/21 06:48:44  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.12  2011/08/13 10:52:38  emrsn\rsaha
 * *  Revised as per Rony's comment.
 * *  Revised sequence number to be read from /tmp/UDPCommSequenceFile so to make sure its serial order across system.
 * *  Retrying AO update if response is error. Previously AO update retry was done only for time-out cases.
 * *
 * *  Revision 1.11  2011/07/13 11:01:36  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.10  2010/09/01 20:07:13  emrsn\rsaha
 * *  Corrected an indexing error.
 * *
 * *  Revision 1.9  2010/05/06 23:25:28  EMRSN\RRTHOMAS
 * *  Added support for ROC Analog Output card on ROC Expansion Slot 2.
 * *
 * *  Revision 1.8  2009/02/24 17:09:46  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.7  2008/11/16 00:37:06  epm-na\rthomas
 * *  Added code to set current output (in mA) in the ROC Analog Outputs (which plug into the SPI expansion slots on the backplane). This has the following limitations -
 * *  1. The only functionality supported on these cards are setting Analog Output. Board revision, board temperature not supported.
 * *  2. The SetCurrent function has to update the "output current" to all four analog output channels at the same time. We can't individually address each channel. The code has been "hacked" into place to take care of this! Please read descriptive comment in AnalogOut.h.
 * *  3. For now, we only support one Analog Output ROC SPI card. For more, we need to add code here!
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "FixEndian.h"
  
#include "debug.h"
#include "AnalogOut.h"
#include "LOI370XAUDPCommProtocol.h"
#include "BoardSlotInfo.h"
#include "ResolveDevName.h"

unsigned short CAnalogOut::m_usCurrent[4 * NR_ENUM_ROC_ANALOG_OUT] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

CAnalogOut::CAnalogOut()  // Default Constructor
{
  m_iBoardRevision = -1;
}


CAnalogOut::~CAnalogOut() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CAnalogOut::OpenHal (char* pszDevName)
{
  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CAnalogOut::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Set the DigitalIn status - Returns 0 on success, negative error code on failure.
int CAnalogOut::SetCurrent(unsigned long CurrentNanoAmps)
{
  int nRetVal = ERR_SUCCESS;
  char szCmdStr [36];
  // Convert current in nA to mA (for SPI)
  float currentmA = CurrentNanoAmps / 1000000.0;

  CAN_ANALOG_OUT_DATA_STRUCT stCmd = {0};
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_ANALOG_OUT_FN_SET_CURRENT);
        
        stCmd.stData.BaseIOData.CurrentNanoAmps = CurrentNanoAmps;
        
        FixEndian(stCmd.stData.BaseIOData.CurrentNanoAmps);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),            // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));          // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CAnalogOut::SetCurrent(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CAnalogOut::SetCurrent(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogOut::SetCurrent(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogOut::SetCurrent(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      if (m_nExpSlotSPIfd != -1)
      {
        // Get start index into static array where we store the converted current (I) levels
        int nStartIndex = (m_bySlotID - 1) * NR_ENUM_ROC_ANALOG_OUT;

        if (currentmA > MAX_CURRENT_mA)
        {
          currentmA = MAX_CURRENT_mA;
        }
        m_usCurrent[m_byFnEnum -1 + nStartIndex] = (unsigned short) ROC_CONVERT_I_TO_LEVEL(currentmA);
        DEBUG2 ("Converted values = %d %d %d %d", 
                m_usCurrent[0 + nStartIndex], 
                m_usCurrent[1 + nStartIndex], 
                m_usCurrent[2 + nStartIndex], 
                m_usCurrent[3 + nStartIndex]);

        sprintf (szCmdStr, "s %d %d %d %d", 
                 m_usCurrent[0 + nStartIndex], m_usCurrent[1 + nStartIndex], m_usCurrent[2 + nStartIndex], m_usCurrent[3 + nStartIndex]);
        if (write (m_nExpSlotSPIfd, szCmdStr, strlen (szCmdStr)) != (int) strlen (szCmdStr)) 
        {
          nRetVal = ERR_CMD_FAILED;
          DEBUG2("CAnalogOut::SetCurrent(): Error writing to ROC SPI device!");
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogOut::SetCurrent(): Unexpected invalid file descriptor!");
      }
      break;

    case UDP_COMM:
    {
      LOI370XA_CMMD_AO_UPDATE_STRUCT stAOUpdateCmmd;
      LOI370XA_RESP_STRUCT stResp;
        
      switch( m_byFnEnum )
      {
      case 1:
        stAOUpdateCmmd.stHeader.byCmmd = LOI370XA_UPDATE_AO_1;
        break;
      case 2:
      default:
        stAOUpdateCmmd.stHeader.byCmmd = LOI370XA_UPDATE_AO_2;
        break;
      }

      stAOUpdateCmmd.stHeader.bySeqNum = m_pobUDP->GetNewSeqNumber();
      stAOUpdateCmmd.ulValue = CurrentNanoAmps;

      nRetVal = ERR_INTERNAL_ERR;
      int nRetryCount = MAX_NO_RETRIES;
      do
      {
        int nUDPRet = m_pobUDP->GetRemoteResponse( (char*) &stAOUpdateCmmd, 
                                                   sizeof(stAOUpdateCmmd), 
                                                   (char*) &stResp,
                                                   sizeof(stResp) );
        if( nUDPRet > 0 )
        {
          if( (stAOUpdateCmmd.stHeader.byCmmd == stResp.stHeader.byCmmd) &&
              (stAOUpdateCmmd.stHeader.bySeqNum == stResp.stHeader.bySeqNum) &&
              (LOI370XA_ACK == stResp.byAck) )
          {
            nRetVal = ERR_SUCCESS;
            break;
          }
          else
          {
            DEBUG2("CAnalogOut::SetCurrent(): Error in Response frame or NACK response (Rxd : Cmmd=%d, Seq=%d, Ack=%d | Txd : Cmmd=%d, Seq=%d)!", stResp.stHeader.byCmmd, stResp.stHeader.bySeqNum, stResp.byAck, stAOUpdateCmmd.stHeader.byCmmd, stAOUpdateCmmd.stHeader.bySeqNum);
          }
        }
        else if( CUDPClient::RET_RX_TIMEOUT == nUDPRet )
        {
          DEBUG2("CAnalogOut::SetCurrent(): CUDPClient::GetRemoteResponse() timed-out!");
        }
        else
        {
          DEBUG2("CAnalogOut::SetCurrent(): CUDPClient::GetRemoteResponse() error (%d)!", nUDPRet);
        }

        nRetryCount--;
      }while( nRetryCount > 0 );
    } 
    break;

    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogOut::SetCurrent(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogOut::SetCurrent(): Function called before Open Call!");
  }

  return nRetVal;
}

// Get Device Status
int CAnalogOut::GetStatus (int* pnStatus)     // Pointer to write status to
{
  return ERR_NOT_IMPLEMENTED;
}

//Get board revision information
int CAnalogOut::GetBoardInfo (  unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                                unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                                unsigned char* byFirmwareVerBuild,  // Firmware Version - Minor Number
                                unsigned char* byBoardRevision)   // Board Revision
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
          DEBUG2("CAnalogOut::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogOut::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogOut::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogOut::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogOut::GetBoardInfo(): Function called before Open Call!");
  }

  return nRetVal;
}



// Gets the on-board temperature in Milli Degree C.
int CAnalogOut::GetOnBoardTemp(int* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_ANALOG_OUT_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_ANALOG_OUT_FN_GET_ON_BD_TEMP);

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
          DEBUG2("CAnalogOut::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CAnalogOut::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogOut::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CAnalogOut::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogOut::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
};

// Return True if BaseIO G2 Board else False
bool CAnalogOut::IsLoopPowerSelectable( void ) 
{
  bool bRetVal = FALSE;

  if( m_bIsDevOpen )
  {
    if( m_iBoardRevision < 0 && ( strstr(CBaseDev::m_szDevName, "BASE_IO") != NULL ) )
    {
      // Get Slot Id using CAN Id in BaseDev
      if( CBaseDev::m_bySlotID == (unsigned char) -1 )
      {
        DEBUG1("Invalid Device Slot Id \n");
      }
      else
      {
        // Get Board Revision from current_software_info.txt. Once Only
        CBoardSlotInfo oCBoardSlotInfo;
        unsigned char ucCANId = CBaseDev::m_bySlotID;
        oCBoardSlotInfo.GetBoardRevision( "BaseIO", &ucCANId, m_iBoardRevision );
      }
    }

    if( m_iBoardRevision >= G2_BASEIO_BOARD )
    {
      // G2 Board. Supports Selectable Loop Power
      bRetVal = TRUE;
    }
    else
    {
      // G1 Board. Doesnot Support Selectable Loop Power
    }
  }
  else
  {
    // Device Not Opened. Let us return FALSE
  }

  return bRetVal;
};


// Set the Loop Power Internal/External - Returns 0 on success, negative error code on failure.
int CAnalogOut::SetLoopPower( BASEIO_ANLOUT_PWR_ENUM enumLoopPower )
{
  DEBUG2("SetLoopPower() Called by pid (%d)\n", (int)getpid());
  int nRetVal = ERR_SUCCESS; 

  // Check if the device is open!
  if( m_bIsDevOpen )
  {
    if( !IsLoopPowerSelectable() )
    {
      // G1 Board. Does not Support Selectable Loop Power Feature
      nRetVal = ERR_CMD_FAILED;
    }
    else if( enumLoopPower < ANALOG_OUT_PWR_INTERNAL || enumLoopPower > ANALOG_OUT_PWR_EXTERNAL )
    {
      // Invalid Loop Power Selection. Neither Internal Or External 
      nRetVal = ERR_INVALID_ARGS;
    }
    else
    { 
      // G2 Board. Supports Selectable Loop Power Feature
      CAN_ANALOG_OUT_DATA_STRUCT stCmd = {0};
      CAN_BASEIO_STATUS_STRUCT stResp = {0};
      
      switch( m_eCommType )
      {
      case CAN_COMM:
        // Transmit
        if (m_pobCAN)
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_ANALOG_OUT_FN_SEL_LOOP_POWER);
          stCmd.stData.BaseIOData.AnlOutPower = enumLoopPower; 
          DEBUG2("Setting CMD_ANALOG_OUT_FN_SEL_LOOP_POWER to %d\n", stCmd.stData.BaseIOData.AnlOutPower);
          FixEndian(stCmd.stData.BaseIOData.AnlOutPower);
          
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
              DEBUG2("CAnalogOut::SetCurrent(): Device sent a NACK! Dev error code = %d!", nRetVal);
              
            }
            else
            {
              nRetVal = ERR_SUCCESS;
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CAnalogOut::SetCurrent(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CAnalogOut::SetCurrent(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
        }
        else
        {
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CAnalogOut::SetCurrent(): Unexpected invalid pointer!");
        }
        break;
        
      case SPI_COMM:
        break;
        
      case UDP_COMM:
        nRetVal = ERR_NOT_IMPLEMENTED;
        break;
      
      case SERIAL_COMM: 
        nRetVal = ERR_NOT_IMPLEMENTED;
        break;
      
      case COMM_NONE:  
        nRetVal = ERR_NOT_IMPLEMENTED;
        break;
      default:
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CAnalogOut::SetCurrent(): Invalid switch case!");
        break;
        
      } // switch( m_eCommType )...{
      
    }
  
  } // if (m_bIsDevOpen)
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CAnalogOut::SetCurrent(): Function called before Open Call!");
  }
  
  return nRetVal;
}
