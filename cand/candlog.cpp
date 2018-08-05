/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: candlog.cpp
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
 * *  $Id: candlog.cpp,v 1.5 2015/05/04 14:32:10 EMRSN\sboyidi Exp $
 * *  $Log: candlog.cpp,v $
 * *  Revision 1.5  2015/05/04 14:32:10  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.4  2014/06/26 09:59:42  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.3  2012/03/21 06:48:41  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.2  2006/12/08 21:24:40  epm-na\DPrabhakar
 * *  Added Daniel file headers.
 * *  Added README file containing application execution information.
 * *
 * *
 * *************************************************************************/



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "candlog.h"

//Default constructor
CCANDLog::CCANDLog()
{
  if (InitQueue(&stQInfo, stQData, CMD_LOG_SIZE) < 0)
  {
    printf("CANDLog: Error initializing Queue.\n");
  }
}

//Default destructor
CCANDLog::~CCANDLog()
{
}

//Add information to the CAND log queue
int CCANDLog::AddCmdRespToLog(CANDLogInfo stLogInfo)
{
  //If the buffer is full, discard the oldest data
  if (GetSize(&stQInfo) >= (CMD_LOG_SIZE - 1))
  {
    CANDLogInfo stTmpLogInfo;
    //Remove the oldest data
    if (RemoveHead(&stTmpLogInfo, sizeof(CANDLogInfo), &stQInfo) < 0)
    {
      printf("CANDLog: Error removing old data from Q to accommodate new data.\n");
      return -1;
    }
  }

  //Reset before getting the current time stamp
  stLogInfo.cmdTime.tv_sec = 0;
  stLogInfo.cmdTime.tv_usec = 0;

  //Get and store the time stamp
  if (gettimeofday(&stLogInfo.cmdTime, NULL) < 0)
  {
    printf("CANDLog: Error getting time for this command.\n");
  }

  //Add the CAND log information to the queue
  if (AddTail(&stLogInfo, sizeof(CANDLogInfo), &stQInfo) < 0)
  {
    printf("CANDLog: Error adding new data to Q.\n");
  }
  
  return 0;
}

//Dump information in the queue to a file
int CCANDLog::DumpLogToFile()
{
  char fileName[255];
  int fdDump;
  int nRetVal;
  struct timeval dumpTime;
  CANDLogInfo stLogInfo;
  char lineInfo[255];
  char strData[5];
  //Log file header - CSV format
  char *headerInfo = "Dir, TS (sec), TS (usec), Sl.ID, Fn, Fn.Cnt, Len, Data,\n"; 

  //Reset before getting the current time stamp
  dumpTime.tv_sec = 0;
  dumpTime.tv_usec = 0;
  
  //Get the current time stamp
  if (gettimeofday(&dumpTime, NULL) < 0)
  {
    printf("CANDLog: Error getting dump time.\n");
  }

  //Form the file name for the log file based on the current time stamp
  sprintf(fileName, "/tmp/candlog_%ld_%ld.csv", dumpTime.tv_sec, dumpTime.tv_usec);

  printf("Logging to: %s\n", fileName);

  //Open the log file
  fdDump = open(fileName, O_RDWR | O_CREAT);

  if (fdDump < 0)
  {
    printf("Error opening dump file: %s\n", fileName);
    return -1;
  }

  //
  nRetVal = write(fdDump, headerInfo, strlen(headerInfo));

  if (nRetVal != (int) strlen(headerInfo))
  {
    printf("CANDLog: Dump - Error writing header to log file.\n");
  }

  //Dump all data in the queue
  while (!IsQueueEmpty(&stQInfo))
  {
    //Get the oldest data
    if (RemoveHead(&stLogInfo, sizeof(CANDLogInfo), &stQInfo) < 0)
    {
      printf("CANDLog: Dump - Error reading from the queue.\n");
      break;
    }

    memset(lineInfo, 0, 255);

    //Format and store the data to a buffer in CSV format
    sprintf(lineInfo, "%s, %ld, %ld, %d, %d, %d, %d, ", 
            (stLogInfo.ucDir == 1 ? "Tx" : "Rx"),
            stLogInfo.cmdTime.tv_sec, stLogInfo.cmdTime.tv_usec, 
            stLogInfo.ucSlId, stLogInfo.ucFn, stLogInfo.ucFnCnt,
            stLogInfo.ucDataLen);

    //Append data information to the end of the line
    for (int nCnt = 0; nCnt < stLogInfo.ucDataLen; nCnt++)
    {
      sprintf(strData, "%d ", stLogInfo.ucData[nCnt]);
      strcat(lineInfo, strData);
    }

    //New line information 
    strcat(lineInfo, "\n");

    //Write the formatted data to the file
    nRetVal = write(fdDump, lineInfo, strlen(lineInfo));

    if (nRetVal != (int) strlen(lineInfo))
    {
      printf("CANDLog: Dump - Error writing to log file.\n");
      break;
    }
  }


  //Close the dump file
  close(fdDump);

  return 0;
}




