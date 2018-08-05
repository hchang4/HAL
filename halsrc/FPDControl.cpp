/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FPDControl.cpp
 *
 *  Description: FPD control access layer.
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
 *  $Id: FPDControl.cpp,v 1.7 2017/01/17 15:33:04 emrsn\jbeatty Exp $
 *  $Log: FPDControl.cpp,v $
 *  Revision 1.7  2017/01/17 15:33:04  emrsn\jbeatty
 *  Changed FPD autozero pulse to 51us (nominally 50us is required, was 50ms)
 *
 *  Revision 1.6  2016/11/17 13:55:11  emrsn\jbeatty
 *  Finished incomplete support for FPDs on 1500XA R2 backplane
 *
 *  Revision 1.5  2015/05/04 14:49:26  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.4  2014/06/26 10:00:22  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.3  2013/01/30 06:18:37  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.2  2012/03/21 06:48:51  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2009/12/17 14:10:18  emrsn\rsaha
 *  HAL class to talk to BaseIO and do FPD AutoZero.
 *
 *************************************************************************/
  
#include "debug.h"
#include "FixEndian.h"
#include "BaseIOProtocol.h"
#include "FPDControl.h"

int CFPDCtrl::OpenHal(char *pszDevName)
{
  if (pszDevName == NULL)
    return ERR_INVALID_ARGS;

  return CBaseDev::OpenHal(pszDevName, FALSE);
}

int CFPDCtrl::CloseHal()
{
  return CBaseDev::CloseHal();
}

int CFPDCtrl::AutoZero()
{
  int nRetVal = ERR_SUCCESS;

  if (m_bIsDevOpen)
  {
    CmdAckUnion stCmd = {0};
    CAN_BASEIO_STATUS_STRUCT stResp = {0};

    // Check if the device is open
    if (m_bIsDevOpen)
    {
      switch (m_eCommType)
      {
        case CAN_COMM:
          if (m_pobCAN)
          {
            SetCmdAckCommand(&stCmd.byCmdAck, CMD_FPD_FN_DO_AUTO_ZERO);
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
                DEBUG2("CFPDCtrl::AutoZero : Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
                nRetVal = ERR_SUCCESS;
            }
            else if (nRetVal >= 0)
            {
              DEBUG2("CFPDCtrl::AutoZero : Unexpected packet size: %d", nRetVal);
              nRetVal = ERR_PROTOCOL;
            }
            else // if (nRetVal < 0)
              DEBUG2("CFPDCtrl::AutoZero : CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_INTERNAL_ERR;
            DEBUG2("CFPDCtrl::AutoZero : Unexpected invalid pointer!");
          }
          break;

        case FID_COMM:    // FPD on 1500XA R2 backplane
          unsigned char bVal;
          bVal = '1';
          if (write(m_fdFidBkpIgnite, &bVal, 1) != 1)
          {
            nRetVal = ERR_CMD_FAILED;
            DEBUG2("CFPDCtrl::AutoZero() : Autozero failed");
          }
          else
          {
            // Hold it to make a 50us pulse. Per Curtis, it may not even need to be that long with the
            // newer hardware, and longer than the nominal 50us shouldn't hurt anything.
            usleep(51);

            bVal = '0';
            if (write(m_fdFidBkpIgnite, &bVal, 1) != 1)
            {
              nRetVal = ERR_CMD_FAILED;
              DEBUG2("CFPDCtrl::AutoZero() : Autozero failed");
            }
          }
          break;

        case SPI_COMM:
          //TODO: To be implemented
        case SERIAL_COMM:
          //TODO:  To be implemented
        case COMM_NONE:
        default:
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CFPDCtrl::AutoZero : Invalid switch case!");
          break;
      }
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CFPDCtrl::AutoZero - Function called before Open Call!");
  }

  return nRetVal;
}
