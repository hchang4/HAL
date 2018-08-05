/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: IRKeyPad.cpp
 *
 *  Description: IR Key Pad access layer.
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
 *  $Id: IRKeyPad.cpp,v 1.4 2015/05/04 14:49:28 EMRSN\sboyidi Exp $
 *  $Log: IRKeyPad.cpp,v $
 *  Revision 1.4  2015/05/04 14:49:28  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.3  2014/06/26 10:00:38  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.2  2012/03/21 06:48:56  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2009/06/25 19:00:35  epm-na\rsaha
 *  Added interfaces in HAL layer to set/get/save/restore IR key sensitivity settings.
 *
 ***********************************************************************/

#include "FixEndian.h"
#include "debug.h"
#include "IRKeyPadProtocol.h"
#include "IRKeyPad.h"

CIRKeyPad::CIRKeyPad()
{
}

CIRKeyPad::~CIRKeyPad()
{
}

int CIRKeyPad::OpenHal( char* pszDevName )
{
  if( NULL == pszDevName )
  {
    return ERR_INVALID_ARGS;
  }

  return CBaseDev::OpenHal( pszDevName, FALSE );
}

int CIRKeyPad::CloseHal()
{
  return CBaseDev::CloseHal();
}

int CIRKeyPad::SendCommand( unsigned char * ucTXData,
                            unsigned int unTXNumData,
                            unsigned char * ucRXData,
                            unsigned int unRXNumData,
                            unsigned int unTimeOut )
{
  int nRetVal = ERR_SUCCESS;

  if( m_bIsDevOpen )
  {
    switch( m_eCommType )
    {
    case CAN_COMM:
      if( m_pobCAN )
      {
        //Send the command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp(   ucTXData,           // Command
                                                        unTXNumData,        // Size of command
                                                        ucRXData,           // Response from remote board
                                                        unRXNumData,        // Size of expected response
                                                        FALSE,              // Not stream data
                                                        unTimeOut );        // Time out interval

        //Check if we got the correct response packet
        if( nRetVal < 0 )
        {
          DEBUG1("CIRKeyPad::SendCommand() - CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
        else if( nRetVal !=  (int)unRXNumData )
        {
          DEBUG1("CIRKeyPad::SendCommand() - Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG1("CIRKeyPad::SendCommand() - Unexpected invalid pointer!");
      }

      break;
    case SPI_COMM:
      //TODO : To be implemented.
    case SERIAL_COMM:
      //TODO : To be implemented.
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG1( "CIRKeyPad::SendCommand() - Invalid switch case!" );
      break;
    }
  }
  else
  {
    //Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CIRKeyPad::SendCommand() - Function called before Open Call!");
  }

  return nRetVal;
}

//Sets the specified IR Key Sensitivity setting to IRKeyPAD.
int CIRKeyPad::SetKeySensitivity( unsigned char ucKeyNum, unsigned char ucSensitivity )
{
  int nRetVal = ERR_SUCCESS;

  if( ucKeyNum >=1  && ucKeyNum <= 8 )
  {
    NACK_STRUCT stResp;
    CAN_SINGLE_IRKEY_STRUCT stIRKeyCmmd;
    stIRKeyCmmd.stData.KeyNum = ucKeyNum;
    stIRKeyCmmd.stData.KeySensitivity = ucSensitivity;

    SetCmdAckCommand( &stIRKeyCmmd.stCmd.byCmdAck, CMD_IRKEYPAD_FN_SET_INDIVIDUAL_KEY_SENSITIVITY );
    nRetVal = SendCommand( (unsigned char *) &stIRKeyCmmd, sizeof(stIRKeyCmmd), (unsigned char *) &stResp, sizeof(stResp) );
    if( sizeof(stResp) == nRetVal )
    {
      if( GetCmdAckError(&stResp.stCmd.byCmdAck) == 1 )
      {
        nRetVal = TransDevError( stResp.stStatus.StatusDataUnion.ErrorType );
        DEBUG1( "CIRKeyPad::SetKeySensitivity() - Device sent a NACK! Dev error code = %d !", nRetVal );
      }
      else
      {
        nRetVal = ERR_SUCCESS;
      }
    }
  }
  else
  {
    DEBUG1( "CIRKeyPad::SetKeySensitivity() - Invalid IR Key Number %d !", ucKeyNum );
    nRetVal = ERR_INVALID_ARGS;
  }

  return nRetVal;
}

//Sets all IR Key Sensitivity settings.
int CIRKeyPad::SetAllKeySensitivity( unsigned char ucSensitivity )
{
  int nRetVal = ERR_SUCCESS;

  NACK_STRUCT stResp;
  CAN_SINGLE_IRKEY_STRUCT stIRKeyCmmd;
  stIRKeyCmmd.stData.KeySensitivity = ucSensitivity;

  SetCmdAckCommand( &stIRKeyCmmd.stCmd.byCmdAck, CMD_IRKEYPAD_FN_SET_ALL_KEY_SENSITIVITY );
  nRetVal = SendCommand( (unsigned char *) &stIRKeyCmmd, sizeof(stIRKeyCmmd), (unsigned char *) &stResp, sizeof(stResp) );
  if( sizeof(stResp) == nRetVal )
  {
    if( GetCmdAckError(&stResp.stCmd.byCmdAck) == 1 )
    {
      nRetVal = TransDevError( stResp.stStatus.StatusDataUnion.ErrorType );
      DEBUG1( "CIRKeyPad::SetAllKeySensitivity() - Device sent a NACK! Dev error code = %d !", nRetVal );
    }
    else
    {
      nRetVal = ERR_SUCCESS;
    }
  }

  return nRetVal;
}

//Commands IRKeyPad to save the present IR Key Sensitivity settings to memory.
int CIRKeyPad::SaveKeySensitivitySettings()
{
  int nRetVal = ERR_SUCCESS;

  NACK_STRUCT stResp;
  CmdAckUnion IRKeyCmmd;

  SetCmdAckCommand( & IRKeyCmmd.byCmdAck, CMD_IRKEYPAD_FN_SAVE_ALL_KEY_SENSITIVITY );
  nRetVal = SendCommand( (unsigned char *) &IRKeyCmmd, sizeof(IRKeyCmmd), (unsigned char *) &stResp, sizeof(stResp) );
  if( sizeof(stResp) == nRetVal )
  {
    if( GetCmdAckError(&stResp.stCmd.byCmdAck) == 1 )
    {
      nRetVal = TransDevError( stResp.stStatus.StatusDataUnion.ErrorType );
      DEBUG1( "CIRKeyPad::SaveKeySensitivitySettings() - Device sent a NACK! Dev error code = %d !", nRetVal );
    }
    else
    {
      nRetVal = ERR_SUCCESS;
    }
  }

  return nRetVal;
}

//Retrieves the specific IR Key Sensitivity setting and retruns back to application.
int CIRKeyPad::GetKeySensitivity( unsigned char ucKeyNum, unsigned char & ucSensitivity )
{
  int nRetVal = ERR_SUCCESS;

  if( ucKeyNum >=1  && ucKeyNum <= 8 )
  {
    CAN_SINGLE_IRKEY_STRUCT stIRKeyCmmd;
    CAN_SINGLE_IRKEY_STRUCT stIRKeyResp;
    stIRKeyCmmd.stData.KeyNum = ucKeyNum;

    SetCmdAckCommand( &stIRKeyCmmd.stCmd.byCmdAck, CMD_IRKEYPAD_FN_GET_INDIVIDUAL_KEY_SENSITIVITY );
    nRetVal = SendCommand( (unsigned char *) &stIRKeyCmmd, sizeof(stIRKeyCmmd), (unsigned char *) &stIRKeyResp, sizeof(stIRKeyResp) );
    if( sizeof(stIRKeyResp) == nRetVal )
    {
      ucSensitivity = stIRKeyResp.stData.KeySensitivity;
      nRetVal = ERR_SUCCESS;
    }
  }
  else
  {
    DEBUG1( "CIRKeyPad::GetKeySensitivity() - Invalid IR Key Number %d !", ucKeyNum );
    nRetVal = ERR_INVALID_ARGS;
  }

  return nRetVal;
}

//Retrieves all IR Key Sensitivity settings and returns them back to application.
int CIRKeyPad::GetAllKeySensitivity( IRKeySettings & KeySettings )
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion IRKeyCmmd;
  CAN_ALL_IRKEY_STRUCT stIRKeyResp;

  SetCmdAckCommand( &IRKeyCmmd.byCmdAck, CMD_IRKEYPAD_FN_GET_ALL_KEY_SENSITIVITY );
  nRetVal = SendCommand( (unsigned char *) &IRKeyCmmd, sizeof(IRKeyCmmd), (unsigned char *) &stIRKeyResp, sizeof(stIRKeyResp) );
  if( sizeof(stIRKeyResp) == nRetVal )
  {
    for( unsigned char ucInd = 0; ucInd < 8; ucInd++ )
    {
      KeySettings.ucSensitivity[ucInd] = stIRKeyResp.arrKeySensitivity[ucInd];
    }
    nRetVal = ERR_SUCCESS;
  }

  return nRetVal;
}

//Commands IRKeyPad to load previously saved IR Key Sensitivity settings 
//and returns the setting values back to application.
int CIRKeyPad::LoadAllReviousKeySensitivity( IRKeySettings & KeySettings )
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion IRKeyCmmd;
  CAN_ALL_IRKEY_STRUCT stIRKeyResp;

  SetCmdAckCommand( &IRKeyCmmd.byCmdAck, CMD_IRKEYPAD_FN_LOAD_PREVIOUS_KEY_SENSITIVITY );
  nRetVal = SendCommand( (unsigned char *) &IRKeyCmmd, sizeof(IRKeyCmmd), (unsigned char *) &stIRKeyResp, sizeof(stIRKeyResp) );
  if( sizeof(stIRKeyResp) == nRetVal )
  {
    for( unsigned char ucInd = 0; ucInd < 8; ucInd++ )
    {
      KeySettings.ucSensitivity[ucInd] = stIRKeyResp.arrKeySensitivity[ucInd];
    }
    nRetVal = ERR_SUCCESS;
  }

  return nRetVal;
}

//Commands IRKeyPad to restore back all the default IR Key Sensitivity settings and 
//returns the default values back to application.
int CIRKeyPad::RestoreAllDefaultKeySensitivity( IRKeySettings & KeySettings )
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion IRKeyCmmd;
  CAN_ALL_IRKEY_STRUCT stIRKeyResp;

  SetCmdAckCommand( &IRKeyCmmd.byCmdAck, CMD_IRKEYPAD_FN_RESTORE_DEFAULT_KEY_SENSITIVITY );
  nRetVal = SendCommand( (unsigned char *) &IRKeyCmmd, sizeof(IRKeyCmmd), (unsigned char *) &stIRKeyResp, sizeof(stIRKeyResp) );
  if( sizeof(stIRKeyResp) == nRetVal )
  {
    for( unsigned char ucInd = 0; ucInd < 8; ucInd++ )
    {
      KeySettings.ucSensitivity[ucInd] = stIRKeyResp.arrKeySensitivity[ucInd];
    }
    nRetVal = ERR_SUCCESS;
  }

  return nRetVal;
}




