/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: HwInhibitCtrl.cpp
 *
 *  Description: Preamp hardware inhibit control access layer
 *
 *  Copyright:        Copyright (c) 2017-
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C++'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *************************************************************************/

#ifndef MODEL_370XA

#include "FixEndian.h"
  
#include "debug.h"
#include "HwInhibitCtrl.h"

CHwInhibitCtrl::CHwInhibitCtrl()    // Default Constructor
{
}

CHwInhibitCtrl::~CHwInhibitCtrl()   // Destructor
{
}

// Open the device, returning 0 on success or negative error code on failure
int CHwInhibitCtrl::OpenHal(char* pszDevName)
{
  return CBaseDev::OpenHal(pszDevName, FALSE);
}

// Close the device
int CHwInhibitCtrl::CloseHal()
{
  return CBaseDev::CloseHal();
}

// Set the hardware inhibit ON or OFF
int CHwInhibitCtrl::SetInhibit(PREAMP_HWINHIBIT_STATE eState)
{
  if (!m_bIsDevOpen)
  {
    // Function called before Open Call
    DEBUG2("CHwInhibitCtrl::SetInhibit(): Function called before calling HalOpen()");
    return ERR_INVALID_SEQ;
  }

  if (m_eCommType != CAN_COMM)
  {
    DEBUG2("CHwInhibitCtrl::SetInhibit(): Invalid comm type");
    return ERR_INTERNAL_ERR;
  }

  if (!m_pobCAN)
  {
    DEBUG2("CHwInhibitCtrl::SetInhibit(): Invalid pointer to CAN object");
    return ERR_INTERNAL_ERR;
  }

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};
  SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CTRL_FN_SET_HW_INHIBIT);
  stCmd.stData.preampData.HwInhibit = eState;
  FixEndian(stCmd.stData.preampData.HwInhibit);

  // Send a command and wait for ackowledgement from remote device
  int nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *)&stCmd,   // command
                                                   sizeof(stCmd),             // size of command
                                                   (unsigned char *)&stResp,  // response from remote board
                                                   sizeof(stResp));           // size of expected response

  // Check if we got the correct response packet
  if (nRetVal == sizeof(stResp))
  {
    // Don't worry about whether or not the preamp liked it - only supported by the G2 Preamp
    nRetVal = ERR_SUCCESS;
  }
  // Not the expected packet size
  else if (nRetVal >= 0)
  {
    DEBUG2("CHwInhibitCtrl::SetInhibit(): Unexpected packet size: %d", nRetVal);
    nRetVal = ERR_PROTOCOL;
  }
  else // if (nRetVal < 0)
    DEBUG2("CHwInhibitCtrl::SetInhibit(): CCANComm::CANGetRemoteResp failed with error code %d", nRetVal);

  return nRetVal;
}

#endif  // #ifndef MODEL_370XA
