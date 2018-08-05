/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: dfifo.h
 * *
 * *  Description: Fixed packet size FIFO for storing CAND commands and
 * *               responses.
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
 * *  $Id: dfifo.h,v 1.6 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 * *  $Log: dfifo.h,v $
 * *  Revision 1.6  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.5  2014/06/26 10:01:22  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.4  2012/03/21 06:49:10  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2006/12/08 21:32:41  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/




#ifndef _DFIFO_H
#define _DFIFO_H

#define MAX_QUEUE_DATA_SIZE   30

typedef struct 
{
  unsigned char m_ucProc:1;
  unsigned char m_ucLen:7;
  unsigned char m_ucData[MAX_QUEUE_DATA_SIZE];
} QueueData;


typedef struct
{
  QueueData *m_pstQData;
  unsigned int m_unNrPkt;
  unsigned int m_unWrIx;
  unsigned int m_unRdIx;
  unsigned int m_unQLen;
} QueueStruct;


//Initializes the Queue 
int InitQueue(QueueStruct *pstQueueInfo, QueueData *pstQueueData, 
              unsigned int unQueueLength);

//Checks whether Queue is empty or not
int IsQueueEmpty(QueueStruct* pstQueue);

//Checks whether Queue to be added or not
int AddTail(void* pData, unsigned int unLength, 
            QueueStruct* pstQueue);

//Removes (and gets) the head of the queue
int RemoveHead(void *pData, unsigned int unLength, QueueStruct* pstQueue);

//Removes (and gets) the data from nPos
int RemoveAt(void *pData, unsigned int unLength, unsigned int unPos, QueueStruct* pstQueue);

// Get the element at nPos. This does not remove the element, just lets the user
// peek at the data.
int GetAt(void *pData, unsigned int unLength, unsigned int unPos, QueueStruct* pstQueue);

//Returns the total number of items in the Queue
int GetSize(QueueStruct* pstQueue);

#endif //_DFIFO_H

