/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: Ltloi.h
 *
 *  Description: LTLOI HAL object
 *
 *  Copyright:        Copyright (c) 2011-2012, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *  Revision History:
 *  $Id: LtLoi.h,v 1.6 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 *  $Log: LtLoi.h,v $
 *  Revision 1.6  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.5  2014/06/26 10:01:16  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.4  2012/03/21 06:49:08  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.3  2009/02/24 17:13:42  epm-na\rsaha
 *  Revised to display build number of the version information.
 *
 *  Revision 1.2  2007/08/09 07:13:26  EPM-NA\ssatpute
 *  Added below functions to return receive pipe file descriptor - streaming data.
 *  1) CANGetRxStrmFd() in CANComm.h
 *  2) GetRxStreamingFd() in LtLoi.h
 *  [-Kashyap]
 *
 *  Revision 1.1  2007/04/05 13:14:51  epm-na\rthomas
 *  LTLOI: Added HAL function to access the LTLOI interface on the base IO
 *  board.
 *
 *
 *************************************************************************/

#include "BaseDev.h"
#include "BaseIOProtocol.h"

#define MAX_LTLOI_BYTE_INDEX  11
#define MAX_LTLOI_BYTES     (MAX_LTLOI_BYTE_INDEX + 1)
#define MAX_NUM_CLK_BITS    (MAX_LTLOI_BYTES * 8)

typedef enum
{
  LTLOI_APP_STATUS_MIN,
  LTLOI_APP_STATUS_OFF = LTLOI_APP_STATUS_MIN,
  LTLOI_APP_STATUS_ON,
  LTLOI_APP_STATUS_MAX
}LTLOI_APP_STATUS_ENUM;

class CLtLoi : public CBaseDev {
private:
  int RxStreamingByteCommon(unsigned char *ByteVal, unsigned char *ByteNum, unsigned int *Timeout);
  
public:
  CLtLoi();
  ~CLtLoi();

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();
  
  int SetOutputByte(const unsigned char ByteVal, unsigned char ByteNum); 
  int SetOutputFlash(const unsigned char FlashByte, unsigned char ByteNum);
  int SetNumClkBits(unsigned char NumClkBits);
  int ScanCtrl(LTLOI_APP_STATUS_ENUM ScanState);
  int ReadInputByte(unsigned char *ByteVal, unsigned char ByteNum);
  int StreamingState(LTLOI_APP_STATUS_ENUM StreamingOnOff);
  int RxStreamingByteBlocking(unsigned char *ByteVal, unsigned char *ByteNum);
  int RxStreamingByte(unsigned char *ByteVal, unsigned char *ByteNum, unsigned int Timeout);
  int GetBoardInfo (unsigned char* byFirmwareVerMaj, unsigned char* byFirmwareVerMin, unsigned char* byFirmwareVerBuild, unsigned char* byBoardRevision);
  int GetOnBoardTemp(int* TempMilliDegC);
  int GetRxStreamingFd();
};

