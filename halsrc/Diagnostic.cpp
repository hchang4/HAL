/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: Diagnostic.cpp
 *
 *  Description: Retrieves Diagnostic informations of different boards.
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
 *  $Id: Diagnostic.cpp,v 1.10 2016/08/30 09:57:09 emrsn\jbeatty Exp $
 *  $Log: Diagnostic.cpp,v $
 *  Revision 1.10  2016/08/30 09:57:09  emrsn\jbeatty
 *  Mods for 1500XA R2 support
 *
 *  Revision 1.9  2015/05/04 14:49:25  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.8  2014/06/26 10:00:11  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.7  2013/01/29 14:32:49  emrsn\rsaha
 *  Revised 370XA Analyzer IO diagnostics details as per Rev.E schematics.
 *
 *  Revision 1.6  2012/03/21 06:48:47  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.5  2010/10/14 18:03:42  emrsn\rsaha
 *  Revised to read V+, V- and 3.3V readings of Analyzer board.
 *
 *  Revision 1.4  2010/06/10 20:13:29  emrsn\rsaha
 *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 *
 *  Revision 1.3  2009/11/20 12:27:45  epm-na\rsaha
 *  Added support to retrieve LOI IRKeypad's diagnostic details.
 *
 *  Revision 1.2  2009/02/24 17:09:46  epm-na\rsaha
 *  Revised to display build number of the version information.
 *
 *  Revision 1.1  2008/11/25 00:13:23  epm-na\rsaha
 *  Has APIs to retrive diagnostic details of Preamp, BaseIO and Heater solenoid board.
 *
 *************************************************************************/

#include <string>
#include "debug.h"
#include "Diagnostic.h"

int CDiagnostic::OpenHal( char* pszDevName )
{
  if( NULL == pszDevName )
  {
    return ERR_INVALID_ARGS;
  }
    
  return CBaseDev::OpenHal( pszDevName, FALSE );
}

int CDiagnostic::CloseHal()
{
  return CBaseDev::CloseHal();
}


DIAGNOSTIC_TYPE CDiagnostic::GetDiagnoType()
{
  //Get the type of the diagnostic information from the device name.
  //Diagnostic type tells us which boards diagnostic information we
  //are going to retrieve.
  DIAGNOSTIC_TYPE DiagnoType = UNKNOWN_DIAGNO;

  if( strstr( m_szDevName, PREAMP_1_SLOT_STR ) != NULL )
  {
    DiagnoType = PREAMP_DIAGNO;
  }
  else if( strstr( m_szDevName, PREAMP_2_SLOT_STR ) != NULL )
  {
    DiagnoType = PREAMP_DIAGNO;
  }
  else if( strstr( m_szDevName, PREAMP_3_SLOT_STR ) != NULL )
  {
    DiagnoType = PREAMP_DIAGNO;
  }
  else if( strstr( m_szDevName, HTRSOL_1_SLOT_STR ) != NULL )
  {
    DiagnoType = HTRSOL_DIAGNO;
  }
  else if( strstr( m_szDevName, HTRSOL_2_SLOT_STR ) != NULL )
  {
    DiagnoType = HTRSOL_DIAGNO;
  }
  else if( strstr( m_szDevName, BASEIO_SLOT_STR ) != NULL )
  {
    DiagnoType = BASEIO_DIAGNO;
  }
  else if( strstr( m_szDevName, IRKEY_SLOT_STR ) != NULL )
  {
    DiagnoType = IRKEY_DIAGNO;
  }
  else if( strstr( m_szDevName, ANALYZER_SLOT_STR ) != NULL )
  {
    DiagnoType = ANALYZER_DIAGNO;
  }

  return DiagnoType;
}


int CDiagnostic::GetDiagnosticsDetails( DIAGNOSTIC_TYPE & DiagnoType,
                                        BOARD_DIAGNOSTICS & DiagnoDetails )
{
  int nRetVal = ERR_SUCCESS;
    
  if( m_bIsDevOpen )
  {
    DiagnoType = GetDiagnoType();
    if( UNKNOWN_DIAGNO != DiagnoType )
    {
      int nExpectedRxSize = 0;
      if( PREAMP_DIAGNO == DiagnoType )
      {
        nExpectedRxSize = sizeof( PREAMP_DIAGNOSTIC );
      }
      else if( HTRSOL_DIAGNO == DiagnoType )
      {
        nExpectedRxSize = sizeof( HTRSOL_DIAGNOSTIC );
      }
      else if( BASEIO_DIAGNO == DiagnoType )
      {
        nExpectedRxSize = sizeof( BASEIO_DIAGNOSTIC );
      }
      else if( IRKEY_DIAGNO == DiagnoType )
      {
        nExpectedRxSize = sizeof( IRKEY_DIAGNOSTIC );
      }
      else if( ANALYZER_DIAGNO == DiagnoType )
      {
        nExpectedRxSize = sizeof( ANALYZER_DIAGNOSTIC );
      }

      //Find out which 
      switch( m_eCommType )
      {
      case CAN_COMM:
        if( m_pobCAN )
        {
          unsigned char ucDummy = 0;
          unsigned char ucRxData[ MAX_DIAGNO_RX_DATA ] = {0};
          //Send command and wait for ackowledgement from remote device.
          //Send an Empty Command, since funtion type is enough to trigger the board to send back the diagnostic informations.
          nRetVal = m_pobReliabilityCAN->GetRemoteResp( & ucDummy,            // Empty Command.
                                                        1,                    // Size of command is zero.
                                                        ucRxData,             // Response from remote board.
                                                        MAX_DIAGNO_RX_DATA ); // Size of expected response.          
                        
                        
          //Check if we got the correct response packet.
          if( nRetVal < 0 )
          {
            DEBUG1("CDiagnostic::GetDiagnosticsDetails(() - CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          else if( nExpectedRxSize != nRetVal )
          {                            
            DEBUG1("CDiagnostic::GetDiagnosticsDetails(() - Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else
          {
            memcpy( & DiagnoDetails, ucRxData, nRetVal );
            switch( DiagnoType )
            {
            case PREAMP_DIAGNO:
              FixEndian( DiagnoDetails.Preamp.Input5V );
              FixEndian( DiagnoDetails.Preamp.Input16V );
              FixEndian( DiagnoDetails.Preamp.OnBoardTemp );
              break;

            case HTRSOL_DIAGNO:
              FixEndian( DiagnoDetails.HtrSol.Input3_3V );
              FixEndian( DiagnoDetails.HtrSol.Input5V );
              FixEndian( DiagnoDetails.HtrSol.OnBoardTemp );
              break;

            case BASEIO_DIAGNO:
              FixEndian( DiagnoDetails.BaseIO.Input3_3V );
              FixEndian( DiagnoDetails.BaseIO.Input5V );
              FixEndian( DiagnoDetails.BaseIO.Input24V );
              FixEndian( DiagnoDetails.BaseIO.Input24VCurrent );
              FixEndian( DiagnoDetails.BaseIO.OnBoardTemp );
              FixEndian( DiagnoDetails.BaseIO.FIDSenseVoltage );
              break;

            case IRKEY_DIAGNO:
              FixEndian( DiagnoDetails.IRKey.OnBoardTemp );
              break;

            case ANALYZER_DIAGNO:
              FixEndian( DiagnoDetails.Analyzer.Input10V );
              FixEndian( DiagnoDetails.Analyzer.Input22V );
              FixEndian( DiagnoDetails.Analyzer.OnBoardTemp );
              break;

            case UNKNOWN_DIAGNO:
            default:
              nRetVal = ERR_INTERNAL_ERR;
            }
          }
        }
        else
        {
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG1("CDiagnostic::GetDiagnosticsDetails(() - Unexpected invalid pointer!");
        }
                    
        break;
      case SPI_COMM:
        //TODO : To be implemented.
      case SERIAL_COMM:
        //TODO : To be implemented.
      case COMM_NONE:
      default:
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG1( "CDiagnostic::GetDiagnosticsDetails(() - Invalid switch case!" );
        break;
      }
    }
    else
    {
      //TBD - Check error type.
      nRetVal = ERR_INVALID_DEV_NAME;
      DEBUG1("CDiagnostic::GetDiagnosticsDetails() - Board does not support diagnostic information!");
    }
  }
  else
  {
    //Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CDiagnostic::GetDiagnosticsDetails() - Function called before Open Call!");
  }
    
  return nRetVal;
}


