/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: BaseDev.h
 * *
 * *  Description: HAL base class for all device functions
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
 * *  $Id: BaseDev.h,v 1.15 2016/09/02 15:21:34 emrsn\jbeatty Exp $
 * *  $Log: BaseDev.h,v $
 * *  Revision 1.15  2016/09/02 15:21:34  emrsn\jbeatty
 * *  Fixes for 1500XA R2 FIDs
 * *
 * *  Revision 1.14  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.13  2015/04/30 09:32:30  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.12  2014/06/26 10:01:09  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.11  2012/03/21 06:49:03  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.10  2011/08/13 10:47:25  emrsn\rsaha
 * *  Revised as per Rony's comment.
 * *  No long having sequence number process specific; instead storing it in /tmp/UDPCommSequenceFile for access across system.
 * *
 * *  Revision 1.9  2011/07/15 12:05:45  emrsn\rsaha
 * *  Instead of having 2 different copies of UDP.h file in mk3upgrade, HAL/include; we are now haing it in global_h_files directory.
 * *
 * *  UDP.h file name is changed to UDPClient.h otherwise contents are all same.
 * *
 * *  Revision 1.8  2011/07/13 10:52:51  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.7  2008/11/16 00:39:00  epm-na\rthomas
 * *  Added support for ROC expansion cards.
 * *
 * *  Revision 1.6  2007/07/03 17:01:01  epm-na\lratner
 * *  Removed 'extra' qualifications from various member function declarations.
 * *  This is in line with GCC 4.1 requirements (and the ISO standard).
 * *  Not strictly needed, but a good idea moving forward.
 * *
 * *  Revision 1.5  2007/03/13 08:41:17  epm-na\rthomas
 * *  Header updates
 * *
 * *  Revision 1.4  2006/12/17 21:00:52  EPM-NA\ssatpute
 * *  GetErrorMsg() method added to resolve error message from error code.
 * *
 * *  Revision 1.3  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.

 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _BASE_DEV_H
#define _BASE_DEV_H

#include "../include/Definitions.h"
#include "ResolveDevName.h" // For CResolveDevName
#include "CANComm.h"        // For CCANComm object
#include "Reliability.h"    // For the CReliability object
#include "UDPClient.h"

// Base class 
class CBaseDev
{
protected:
  char m_szDevName[MAX_DEV_NAME];    // Name of the device

  // Pointers to objects of Comm classes. Depending on the Communication medium with the 
  // add-on board, one of these objects are created dynamically. Please note that only 
  // one of these objects will be used for communicating with the device. 
  //CSerialComm* m_pobSerial; // TODO:
  //CSPIComm* m_pobSPI;   // TODO:
  CCANComm* m_pobCAN;
  int m_fdFidBkpIgnite;
  int m_fdFidBkpGain;

  // File descriptor for accessing SPI driver (to talk to ROC expansion cards on 
  // the backplane SPI bus)
  int m_nExpSlotSPIfd;

  // Pointer to UDP communication class object
  CUDPClient * m_pobUDP;

  CReliability *m_pobReliabilityCAN;  //Reliability layer for CAN

  // Communication channel in use (CAN / SPI / Serial)
  COMM_TYPE m_eCommType;

  // For resolving device name into device address
  CResolveDevName m_obResDevName;

  BOOL m_bIsDevOpen;  // Is the device open

  // TODO: Do we really need this?
  // Device Address
  unsigned char m_bySlotID, m_byFnType, m_byFnEnum;

  // Does the device support streaming?
  BOOL m_bIsStreaming;

  // OpenHal the device. Returns 0 on success, negative error code on failure
  int OpenHal(char* pszDevNamed,       // Name of the device to open
              BOOL bStream = FALSE);   // Is the device a streaming device?
  
  // Closes the device. Returns 0 on success, negative error code on failure
  int CloseHal();

  // Translate device error into HAL error code
  int TransDevError(int nDevError);

public:
  CBaseDev(); // Default Constructor
  
  ~CBaseDev();  // Destructor

  // Function will resolve error message from error code
  CHAR *GetErrorMsg(ERR_CODE iErrorCode);
};
#endif // #ifndef _BASE_DEV_H
