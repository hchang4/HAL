/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: candlog.h
 * *
 * *  Description: CAN dameon command/response logging utility. Maintains
 * *               a log of last 1000 commands and responses routed 
 * *               through the CAN daemon (i.e. those commands written
 * *               to the CAN driver and those responses read from the
 * *               CAN driver for registered devices)
 * *               NOTE: Streaming responses are not logged
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
 * *  $Id: candlog.h,v 1.5 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 * *  $Log: candlog.h,v $
 * *  Revision 1.5  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.4  2014/06/26 10:01:19  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.3  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.2  2006/12/08 21:32:41  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/




#ifndef CAND_CMDLOG_H
#define CAND_CMDLOG_H

#include <sys/time.h>

#include "dfifo.h"

//The max. number of Command/Responses in total in the queue
#define CMD_LOG_SIZE    1000

//The log structure
typedef struct 
{
  unsigned char ucDir:1;
  unsigned char ucSlId;
  unsigned char ucFn;
  unsigned char ucFnCnt;
  unsigned char ucData[8];
  unsigned char ucDataLen:4;
  struct timeval cmdTime;
} CANDLogInfo;


//CAND log class
class CCANDLog
{
private:
  //The queue info structure
  QueueStruct stQInfo;
  //Queue data buffer
  QueueData stQData[CMD_LOG_SIZE];

public:
  //Default constructor
  CCANDLog();
  //Default destructor
  ~CCANDLog();
  //Add log information to the queue
  int AddCmdRespToLog(CANDLogInfo stLogInfo);
  //Dump existing log information in the queue to a file
  int DumpLogToFile();
};


#endif //CAND_CMDLOG_H

