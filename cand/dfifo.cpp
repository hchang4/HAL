/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: dfifo.cpp
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
 * *  $Id: dfifo.cpp,v 1.5 2015/05/04 14:32:10 EMRSN\sboyidi Exp $
 * *  $Log: dfifo.cpp,v $
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
#include <string.h>
#include "dfifo.h"

//Initialize a queue
int InitQueue(QueueStruct *pstQueueInfo, QueueData *pstQueueData, 
              unsigned int unQueueLength)
{
  //Parameter invalidity check...
  if ( (NULL == pstQueueInfo) || (NULL == pstQueueData) || 
       (0 == unQueueLength) )
  {
    return -1;
  }

  pstQueueInfo->m_pstQData = pstQueueData;
  pstQueueInfo->m_unQLen = unQueueLength;
  pstQueueInfo->m_unNrPkt = 0;
  pstQueueInfo->m_unWrIx = 0;
  pstQueueInfo->m_unRdIx = 0;

  return 0;
}

//Checks whether Queue is empty or not
int IsQueueEmpty(QueueStruct* pstQueue)
{
  if ((NULL == pstQueue) || (NULL == pstQueue->m_pstQData))
  {
    return -1;
  }
  
  //Parameter invalidity check...
  if (0 == pstQueue->m_unNrPkt)
  {
    return 1;
  }
  
  return 0;
}

//Checks whether Queue to be added or not
int AddTail(void* pData, unsigned int unLength, QueueStruct* pstQueue)
{
  //Parameter invalidity check...
  if ( (NULL == pData) || (0 == unLength) || (NULL == pstQueue) ||
       (NULL == pstQueue->m_pstQData) || (unLength > MAX_QUEUE_DATA_SIZE) )
  {
    return -1;
  }

  //Return with error if the queue is full...
  if (pstQueue->m_unNrPkt >= pstQueue->m_unQLen)
  {
    return -1;
  }
  
  //Copy data
  memcpy(pstQueue->m_pstQData[pstQueue->m_unWrIx].m_ucData, 
         pData, unLength);
  //Save length
  pstQueue->m_pstQData[pstQueue->m_unWrIx].m_ucLen = unLength;
  //Set processed to false
  pstQueue->m_pstQData[pstQueue->m_unWrIx].m_ucProc = 0;

  //Increment write index...
  //pstQueue->m_unWrIx = (pstQueue->m_unWrIx++) % pstQueue->m_unQLen;
  //pstQueue->m_unWrIx = (++pstQueue->m_unWrIx) % pstQueue->m_unQLen;

  pstQueue->m_unWrIx++;
  if (pstQueue->m_unWrIx >= pstQueue->m_unQLen)
  {
    pstQueue->m_unWrIx = 0;
  }
    
  //Number of elements in the queue...
  pstQueue->m_unNrPkt++;

  //Return the number of bytes added to the queue
  return unLength;
}

//Removes (and gets) the head of the queue
int RemoveHead(void *pData, unsigned int unLength, QueueStruct* pstQueue)
{
  int nRetVal = 0;

  //Parameter invalidity check...
  if ( (NULL == pData) || (0 == unLength) || (NULL == pstQueue) ||
       (NULL == pstQueue->m_pstQData) || (unLength > MAX_QUEUE_DATA_SIZE) )
  {
    return -1;
  }
  
  //Return with error if the queue is empty
  if (0 == pstQueue->m_unNrPkt)
  {
    return -1;

  }
  
  //Copy the data to the user buffer
  memcpy(pData, pstQueue->m_pstQData[pstQueue->m_unRdIx].m_ucData, 
         pstQueue->m_pstQData[pstQueue->m_unRdIx].m_ucLen);
  //Save the queue length as the return value
  nRetVal = pstQueue->m_pstQData[pstQueue->m_unRdIx].m_ucLen;
  //Set procecced flag
  pstQueue->m_pstQData[pstQueue->m_unRdIx].m_ucProc = 1;

  //Free up this slot and all subsequent 'processed' packets...
  do 
  {
    if (pstQueue->m_pstQData[pstQueue->m_unRdIx].m_ucProc)
    {
      //unTmpRdIx = (unTmpRdIx++) % pstQueue->m_unQLen; // Causing a problem different behaviors
      //unTmpRdIx = (++unTmpRdIx) % pstQueue->m_unQLen;   // in different compilers!!!

      pstQueue->m_unRdIx++;
      if (pstQueue->m_unRdIx >= pstQueue->m_unQLen)
      {
        pstQueue->m_unRdIx = 0;
      }

      //Free up the slot...Protect against int changing this variable!!
      pstQueue->m_unNrPkt--;
    }
    else
    {
      break;
    }
  } while (pstQueue->m_unRdIx != pstQueue->m_unWrIx);

  //Return the number of bytes in the packet
  return nRetVal;
}

//Removes (and gets) the data at nPos
int RemoveAt(void *pData, unsigned int unLength, unsigned int unPos, QueueStruct* pstQueue)
{
  int nRetVal = 0;

  if ( (nRetVal = GetAt(pData, unLength, unPos, pstQueue)) < 0)
  {
    return -1;
  }
  else
  {
    //Use it as an index...
    unPos--;

    //This is the actual position of the requested data in the queue...
    unPos = (pstQueue->m_unRdIx + unPos) % pstQueue->m_unQLen;
    
    //Mark as processed
    pstQueue->m_pstQData[unPos].m_ucProc = 1;
  }

  //Return the number of bytes in the packet
  return nRetVal;
}

// Get the element at nPos. This does not remove the element, just lets the user
// peek at the data.
int GetAt(void *pData, unsigned int unLength, unsigned int unPos, QueueStruct* pstQueue)
{
  int nRetVal = 0;

  //Parameter invalidity check...
  if ( (NULL == pData) || (0 == unLength) || (NULL == pstQueue) ||
       (NULL == pstQueue->m_pstQData) || (unLength > MAX_QUEUE_DATA_SIZE) )
  {
    return -1;
  }

  //Position invalidity
  if ( (0 == unPos) || (unPos > pstQueue->m_unQLen) 
       || (unPos > pstQueue->m_unNrPkt))
  {
    return -1;
  }
  
  //Use it as an index...
  unPos--;

  //This is the actual position of the requested data in the queue...
  unPos = (pstQueue->m_unRdIx + unPos) % pstQueue->m_unQLen;


  //If not 'processed', mark as processed... 
  if (!pstQueue->m_pstQData[unPos].m_ucProc)
  {
    //Copy data to user buffer
    memcpy(pData, pstQueue->m_pstQData[unPos].m_ucData, unLength);
    //Save number of bytes in the packet
    nRetVal = pstQueue->m_pstQData[unPos].m_ucLen;
  }
  // Return with error if already 'processed'
  else
  {
    return -1;
  }

  //Return the number of bytes in the packet
  return nRetVal;
}

//Returns the total number of items in the Queue
int GetSize(QueueStruct* pstQueue)
{
  if ((NULL == pstQueue) || (NULL == pstQueue->m_pstQData))
  {
    return -1;
  }
  
  //Return the number of packets in the queue
  //return pstQueue->m_unNrPkt;
  
  if (pstQueue->m_unWrIx >= pstQueue->m_unRdIx)
  {
    return (pstQueue->m_unWrIx - pstQueue->m_unRdIx);
  }
  else
  {
    return (pstQueue->m_unQLen - (pstQueue->m_unRdIx - pstQueue->m_unWrIx));
  }
}

