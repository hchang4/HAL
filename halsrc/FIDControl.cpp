/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FIDControl.cpp
 *
 *  Description: FID control access layer.
 *
 *  Copyright:        Copyright (c) 2011-2012, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C++'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *  Revision History:
 *  $Id: FIDControl.cpp,v 1.9 2016/09/06 09:38:11 emrsn\jbeatty Exp $
 *  $Log: FIDControl.cpp,v $
 *  Revision 1.9  2016/09/06 09:38:11  emrsn\jbeatty
 *  Fix Igniter on/off, Gain low/high for backplane FID (was writing binary 0 or 1 instead of ascii characters  '0' and '1')
 *
 *  Revision 1.8  2016/09/02 15:20:36  emrsn\jbeatty
 *  Fixes for 1500XA R2 FIDs
 *
 *  Revision 1.7  2016/08/30 09:57:09  emrsn\jbeatty
 *  Mods for 1500XA R2 support
 *
 *  Revision 1.6  2016/08/09 17:09:06  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.5  2015/05/04 14:49:26  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.4  2014/06/26 10:00:19  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.3  2013/01/30 06:18:36  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.2  2012/03/21 06:48:50  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2009/04/15 13:34:25  epm-na\rsaha
 *  Added support for FID Ignition and Gain control.
 *
 *
 *************************************************************************/
  
#include "debug.h"

#include "FixEndian.h"
#include "BaseIOProtocol.h"
#include "FIDControl.h"

int CFIDCtrl::OpenHal(char *pszDevName)
{
  if (pszDevName == NULL)
    return ERR_INVALID_ARGS;

  return CBaseDev::OpenHal(pszDevName, FALSE);
}

int CFIDCtrl::CloseHal()
{
  return CBaseDev::CloseHal();
}

int CFIDCtrl::SendCommand(FID_CMD_ENUM eFIDCmmd)
{
  int nRetVal = ERR_SUCCESS;

  if (m_bIsDevOpen)
  {
    CmdAckUnion stCmd = {0};
    CAN_BASEIO_STATUS_STRUCT stResp = {0};

    switch (m_eCommType)
    {
      case CAN_COMM:
        if (m_pobCAN)
        {
          SetCmdAckCommand(&stCmd.byCmdAck, eFIDCmmd);
          // Send command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *)&stCmd,   // Command
                                                       sizeof(stCmd),             // Size of command
                                                       (unsigned char *)&stResp,  // Response from remote board
                                                       sizeof(stResp));           // Size of expected response


          // Check if we got the correct response packet
          if (nRetVal == sizeof(stResp))
          {
            // Check if the device ACK'd or NACK'd
            if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
            {
              nRetVal = TransDevError(stResp.stData.StatusDataUnion.ErrorType);
              DEBUG2("CFIDCtrl::SendCommand() : Device sent a NACK! Dev error code = %d!", nRetVal);
            }
            else
              nRetVal = ERR_SUCCESS;
          }
          else if (nRetVal >= 0)
          {
            DEBUG2("CFIDCtrl::SendCommand() : Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CFIDCtrl::SendCommand() : CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
        }
        else
        {
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CFIDCtrl::SendCommand() : Unexpected invalid pointer!");
        }
        break;

      case FID_COMM:    // FID2 on 1500XA R2 backplane
        {
          unsigned char bVal;
          switch (eFIDCmmd)
          {
            case CMD_FID_FN_SET_IGNITION_OFF:
              bVal = '0';
              if (write(m_fdFidBkpIgnite, &bVal, 1) != 1)
              {
                nRetVal = ERR_CMD_FAILED;
                DEBUG2("CFIDCtrl::SendCommand() : Ignition OFF failed");
              }
              break;
            case CMD_FID_FN_SET_IGNITION_ON:
              bVal = '1';
              if (write(m_fdFidBkpIgnite, &bVal, 1) != 1)
              {
                nRetVal = ERR_CMD_FAILED;
                DEBUG2("CFIDCtrl::SendCommand() : Ignition ON failed");
              }
              break;
            case CMD_FID_FN_SET_GAIN_LOW:
              bVal = '0';
              if (write(m_fdFidBkpGain, &bVal, 1) != 1)
              {
                nRetVal = ERR_CMD_FAILED;
                DEBUG2("CFIDCtrl::SendCommand() : Gain LOW failed");
              }
              break;
            case CMD_FID_FN_SET_GAIN_HIGH:
              bVal = '1';
              if (write(m_fdFidBkpGain, &bVal, 1) != 1)
              {
                nRetVal = ERR_CMD_FAILED;
                DEBUG2("CFIDCtrl::SendCommand() : Gain HIGH failed");
              }
              break;
            default:
              DEBUG2("CFIDCtrl::SendCommand() : Invalid FID command %d", (int)eFIDCmmd);
              break;
          }
        }
        break;

      case SPI_COMM:
        // TODO: To be implemented
      case SERIAL_COMM:
        // TODO:  To be implemented
      case COMM_NONE:
      default:
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CFIDCtrl::SendCommand() : Invalid switch case");
        break;
    }
  }
  else
  {
    // Function called before Open Call
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CFIDCtrl::SendCommand() - Function called before Open Call");
  }

  return nRetVal;
}

int CFIDCtrl::TurnOffIgnition()
{
  return SendCommand(CMD_FID_FN_SET_IGNITION_OFF);
}

int CFIDCtrl::TurnOnIgnition()
{
  return SendCommand(CMD_FID_FN_SET_IGNITION_ON);
}

int CFIDCtrl::SetGainLow()
{
  return SendCommand(CMD_FID_FN_SET_GAIN_LOW);
}

int CFIDCtrl::SetGainHigh()
{
  return SendCommand(CMD_FID_FN_SET_GAIN_HIGH);
}
