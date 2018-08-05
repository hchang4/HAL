/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FpdG2control.cpp
 *
 *  Description: FPD G2 control access layer
 *
 *  Copyright:        Copyright (c) 2016-
 *                    Rosemount Analytical
 *                    All Rights Reserved
 *
 *  Operating System:  Linux
 *  Language:          C++
 *  Target:            Gas Chromatograph Model GC700/1500/1500XA R2
 *
 *  Revision History:
 *  - Initial version
 *************************************************************************/
  
#include "debug.h"

#include "FixEndian.h"
#include "FpdG2control.h"

int CFpdG2control::OpenHal(char *pszDevName)
{
  if (pszDevName == NULL)
    return ERR_INVALID_ARGS;

  return CBaseDev::OpenHal(pszDevName, FALSE);
}

int CFpdG2control::CloseHal()
{
  return CBaseDev::CloseHal();
}

int CFpdG2control::SendCommand(FPD_G2_CMD_ENUM eCmd)
{
  if (!m_bIsDevOpen)
  {
    // Function called before Open Call
    DEBUG2("CFpdG2control::SendCommand(): Function called before Open Call");
    return ERR_INVALID_SEQ;
  }

  if (m_eCommType != CAN_COMM)
  {
    DEBUG2("CFpdG2control::SendCommand(): Invalid comm type");
    return ERR_INTERNAL_ERR;
  }

  if (!m_pobCAN)
  {
    DEBUG2("CFpdG2control::SendCommand(): Unexpected invalid pointer");
    return ERR_INTERNAL_ERR;
  }

  CmdAckUnion stCmd;
  CAN_CMD_FPD_G2_STATUS_STRUCT stResp;
  SetCmdAckCommand(&stCmd.byCmdAck, eCmd);

  // Send command and wait for ackowledgement from remote device
  int nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *)&stCmd,   // command
                                                   sizeof(stCmd),             // size of command
                                                   (unsigned char *)&stResp,  // response from remote board
                                                   sizeof(stResp));           // size of expected response
  // Check if we got the correct response packet
  if (nRetVal == sizeof(stResp))
  {
    // Check if the device ACK'd or NACK'd
    if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
    {
      nRetVal = TransDevError(stResp.stData.StatusDataUnion.ErrorType);
      DEBUG2("CFpdG2control::SendCommand(): Device sent a NACK. Dev error code = %d", nRetVal);
    }
    else
      nRetVal = ERR_SUCCESS;
  }
  else if (nRetVal >= 0)
  {
    DEBUG2("CFpdG2control::SendCommand(): Unexpected packet size: %d", nRetVal);
    nRetVal = ERR_PROTOCOL;
  }
  else // if (nRetVal < 0)
    DEBUG2("CFpdG2control::SendCommand(): CCANComm::CANGetRemoteResp failed with error code %d", nRetVal);

  return nRetVal;
}

int CFpdG2control::SetData(FPD_G2_CMD_ENUM eCmd, CAN_CMD_FPD_G2_DATA_STRUCT &data)
{
  if (!m_bIsDevOpen)
  {
    // Function called before Open Call
    DEBUG2("CFpdG2control::SetData(): Function called before Open Call");
    return ERR_INVALID_SEQ;
  }

  if (m_eCommType != CAN_COMM)
  {
    DEBUG2("CFpdG2control::SetData(): Invalid comm type");
    return ERR_INTERNAL_ERR;
  }

  if (!m_pobCAN)
  {
    DEBUG2("CFpdG2control::SetData(): Unexpected invalid pointer");
    return ERR_INTERNAL_ERR;
  }

  CAN_CMD_FPD_G2_STATUS_STRUCT stResp;
  SetCmdAckCommand(&data.stCmd.byCmdAck, eCmd);
  // Handle the byte swapping
  FixEndian(data.stData.fpdG2Data);

  // Send command and wait for ackowledgement from remote device
  int nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *)&data,    // command
                                                   sizeof(data),              // size of command
                                                   (unsigned char *)&stResp,  // response from remote board
                                                   sizeof(stResp));           // size of expected response
  // Check if we got the correct response packet
  if (nRetVal == sizeof(stResp))
  {
    // Check if the device ACK'd or NACK'd
    if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
    {
      nRetVal = TransDevError(stResp.stData.StatusDataUnion.ErrorType);
      DEBUG2("CFpdG2control::SetData(): Device sent a NACK. Dev error code = %d", nRetVal);
    }
    else
      nRetVal = ERR_SUCCESS;
  }
  else if (nRetVal >= 0)
  {
    DEBUG2("CFpdG2control::SetData(): Unexpected packet size: %d", nRetVal);
    nRetVal = ERR_PROTOCOL;
  }
  else // if (nRetVal < 0)
    DEBUG2("CFpdG2control::SetData(): CCANComm::CANGetRemoteResp failed with error code %d", nRetVal);

  return nRetVal;
}

int CFpdG2control::GetData(FPD_G2_CMD_ENUM eCmd, CAN_CMD_FPD_G2_DATA_STRUCT &data)
{
  if (!m_bIsDevOpen)
  {
    // Function called before Open Call
    DEBUG2("CFpdG2control::GetData(): Function called before Open Call");
    return ERR_INVALID_SEQ;
  }

  if (m_eCommType != CAN_COMM)
  {
    DEBUG2("CFpdG2control::GetData(): Invalid comm type");
    return ERR_INTERNAL_ERR;
  }

  if (!m_pobCAN)
  {
    DEBUG2("CFpdG2control::GetData(): Unexpected invalid pointer");
    return ERR_INTERNAL_ERR;
  }

  CmdAckUnion stCmd;
  SetCmdAckCommand(&stCmd.byCmdAck, eCmd);

  // Send command and wait for ackowledgement from remote device
  memset(&data, 0, sizeof(data));
  int nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *)&stCmd,   // command
                                                   sizeof(stCmd),             // size of command
                                                   (unsigned char *)&data,    // response from remote board
                                                   sizeof(data));             // size of expected response
  // Check if we got the correct response packet
  if (nRetVal == sizeof(data))
  {
    // Handle the byte swapping
    FixEndian(data.stData.fpdG2Data);
    // Check if the device ACK'd or NACK'd
    if (GetCmdAckError(&data.stCmd.byCmdAck) == 1)
    {
      nRetVal = TransDevError(data.stData.fpdG2Data.ErrorType);
      DEBUG2("CFpdG2control::GetData(): Device sent a NACK. Dev error code = %d", nRetVal);
    }
    else
      nRetVal = ERR_SUCCESS;
  }
  else if (nRetVal >= 0)
  {
    DEBUG2("CFpdG2control::GetData(): Unexpected packet size: %d", nRetVal);
    nRetVal = ERR_PROTOCOL;
  }
  else // if (nRetVal < 0)
    DEBUG2("CFpdG2control::GetData(): CCANComm::CANGetRemoteResp failed with error code %d", nRetVal);

  return nRetVal;
}

int CFpdG2control::SetTECsetpoint(long tecTemp)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.tecTemp = tecTemp;
  return SetData(CMD_FPD_G2_FN_SET_TEC_SETPOINT, data);
}

int CFpdG2control::SetIgnitionMode(FPD_G2_IGN_MODE ignMode)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.ignMode = ignMode;
  return SetData(CMD_FPD_G2_FN_SET_IGN_MODE, data);
}

int CFpdG2control::SetIgnitionAttempts(long ignTries)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.ignTries = ignTries;
  return SetData(CMD_FPD_G2_FN_SET_IGN_TRIES, data);
}

int CFpdG2control::SetWaitTime(long waitTime)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.waitTime = waitTime;
  return SetData(CMD_FPD_G2_FN_SET_WAIT_TIME, data);
}

int CFpdG2control::SetIgniterOnTime(long ignDur)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.ignDur = ignDur;
  return SetData(CMD_FPD_G2_FN_SET_IGN_DURATION, data);
}

int CFpdG2control::SetFlameOnTemp(long flameOnTemp)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.flameOnTemp = flameOnTemp;
  return SetData(CMD_FPD_G2_FN_SET_FLAME_ON_TEMP, data);
}

int CFpdG2control::SetFlameOffTemp(long flameOffTemp)
{
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  memset(&data, 0, sizeof(data));
  data.stData.fpdG2Data.flameOffTemp = flameOffTemp;
  return SetData(CMD_FPD_G2_FN_SET_FLAME_OFF_TEMP, data);
}

int CFpdG2control::GetTECsetpoint(long &tecTemp)
{
  tecTemp = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_TEC_SETPOINT, data);
  if (nRet == ERR_SUCCESS)
    tecTemp = data.stData.fpdG2Data.tecTemp;
  return nRet;
}

int CFpdG2control::GetIgnitionMode(FPD_G2_IGN_MODE &ignMode)
{
  ignMode = MIN_FPD_G2_IGN_MODE;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_IGN_MODE, data);
  if (nRet == ERR_SUCCESS)
    ignMode = data.stData.fpdG2Data.ignMode;
  return nRet;
}

int CFpdG2control::GetIgnitionAttempts(long &ignTries)
{
  ignTries = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_IGN_TRIES, data);
  if (nRet == ERR_SUCCESS)
    ignTries = data.stData.fpdG2Data.ignTries;
  return nRet;
}

int CFpdG2control::GetWaitTime(long &waitTime)
{
  waitTime = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_WAIT_TIME, data);
  if (nRet == ERR_SUCCESS)
    waitTime = data.stData.fpdG2Data.waitTime;
  return nRet;
}

int CFpdG2control::GetIgniterOnTime(long &ignDur)
{
  ignDur = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_IGN_DURATION, data);
  if (nRet == ERR_SUCCESS)
    ignDur = data.stData.fpdG2Data.ignDur;
  return nRet;
}

int CFpdG2control::GetFlameOnTemp(long &flameOnTemp)
{
  flameOnTemp = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_FLAME_ON_TEMP, data);
  if (nRet == ERR_SUCCESS)
    flameOnTemp = data.stData.fpdG2Data.flameOnTemp;
  return nRet;
}

int CFpdG2control::GetFlameOffTemp(long &flameOffTemp)
{
  flameOffTemp = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_FLAME_OFF_TEMP, data);
  if (nRet == ERR_SUCCESS)
    flameOffTemp = data.stData.fpdG2Data.flameOffTemp;
  return nRet;
}

int CFpdG2control::GetFlameTemp(long &flameTemp)
{
  flameTemp = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_FLAME_TEMP, data);
  if (nRet == ERR_SUCCESS)
    flameTemp = data.stData.fpdG2Data.flameTemp;
  return nRet;
}

int CFpdG2control::GetFlameStatus(FPD_G2_FLAME_STATUS &flameStatus)
{
  flameStatus = MIN_FPD_G2_FLAME_STATUS;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_FLAME_STATUS, data);
  if (nRet == ERR_SUCCESS)
    flameStatus = data.stData.fpdG2Data.flameStatus;
  return nRet;
}

int CFpdG2control::GetTECtemp(long &tecTemp)
{
  tecTemp = 0;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_TEC_TEMP, data);
  if (nRet == ERR_SUCCESS)
    tecTemp = data.stData.fpdG2Data.tecTemp;
  return nRet;
}

int CFpdG2control::GetIgnitionStatus(FPD_G2_IGN_STATUS &ignStatus)
{
  ignStatus = FPD_G2_IGN_OFF;
  CAN_CMD_FPD_G2_DATA_STRUCT data;
  int nRet = GetData(CMD_FPD_G2_FN_GET_IGN_STATUS, data);
  if (nRet == ERR_SUCCESS)
    ignStatus = data.stData.fpdG2Data.ignStatus;
  return nRet;
}

int CFpdG2control::TurnOffIgnition()
{
  return SendCommand(CMD_FPD_G2_FN_STOP_IGNITION);
}

int CFpdG2control::TurnOnIgnition()
{
  return SendCommand(CMD_FPD_G2_FN_START_IGNITION);
}
