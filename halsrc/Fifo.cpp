/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Fifo.cpp
 * *
 * *  Description: A simple FIFO implementation. Ported form the Linux
 * *               kfifo.
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
 * *  $Id: Fifo.cpp,v 1.7 2015/05/04 14:49:27 EMRSN\sboyidi Exp $
 * *  $Log: Fifo.cpp,v $
 * *  Revision 1.7  2015/05/04 14:49:27  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.6  2014/06/26 10:00:23  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.5  2012/03/21 06:48:51  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.4  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.3  2006/12/17 21:01:24  EPM-NA\ssatpute
 * *  GetErrorMsg() method added to resolve error message from error code.
 * *
 * *  Revision 1.2  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/




/*
 * A simple kernel FIFO implementation.
 *
 * Copyright (C) 2004 Stelian Pop <stelian@popies.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
/*
 * Rosemount GC: Modified for our use
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Fifo.h"


/**
 * Constructor: Initializes the fifo
 * @unFifoSize: Size of the fifo, in bytes
 * IMPORTANT: The Size must be a power of 2
 */
CFifo::CFifo(unsigned int unFifoSize)
{
  pucData = NULL;

  if (unFifoSize != 0)
  {
    pucData = (unsigned char *) malloc(unFifoSize);
  }
  
  stFifoInfo.buffer = pucData;
  stFifoInfo.in = 0;
  stFifoInfo.out = 0;
  stFifoInfo.size = unFifoSize;
}

/**
 * Desctuctor
 */
CFifo::~CFifo()
{
  if (pucData)
  {
    free(pucData);
    pucData = NULL;
  }
}

/**
 * Write - puts some data into the FIFO, no locking version
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most 'len' bytes from the 'buffer' into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int CFifo::WriteFifo(unsigned char *buffer, unsigned int len)
{
  unsigned int l;

  len = min(len, stFifoInfo.size - stFifoInfo.in + stFifoInfo.out);

  /* first put the data starting from stFifoInfo.in to buffer end */
  l = min(len, stFifoInfo.size - (stFifoInfo.in & (stFifoInfo.size - 1)));
  memcpy(stFifoInfo.buffer + (stFifoInfo.in & (stFifoInfo.size - 1)), buffer, l);

  /* then put the rest (if any) at the beginning of the buffer */
  memcpy(stFifoInfo.buffer, buffer + l, len - l);

  stFifoInfo.in += len;

  return len;
}

/**
 * Read - gets some data from the FIFO, no locking version
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most 'len' bytes from the FIFO into the
 * 'buffer' and returns the number of copied bytes.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these functions.
 */
unsigned int CFifo::ReadFifo(unsigned char *buffer, unsigned int len)
{
  unsigned int l;

  len = min(len, stFifoInfo.in - stFifoInfo.out);

  /* first get the data from stFifoInfo.out until the end of the buffer */
  l = min(len, stFifoInfo.size - (stFifoInfo.out & (stFifoInfo.size - 1)));
  memcpy(buffer, stFifoInfo.buffer + (stFifoInfo.out & (stFifoInfo.size - 1)), l);

  /* then get the rest (if any) from the beginning of the buffer */
  memcpy(buffer + l, stFifoInfo.buffer, len - l);

  stFifoInfo.out += len;

  //TODO - check if it's OK to put it here!!!! (this was in the 'protected' 
  //  section of the kfifo code.
  /*
   * optimization: if the FIFO is empty, set the indices to 0
   * so we don't wrap the next time
   */
  if (stFifoInfo.in == stFifoInfo.out)
    stFifoInfo.in = stFifoInfo.out = 0;

  return len;
}

/**
 * GetLen - returns the number of bytes available in the FIFO
 */
unsigned int CFifo::GetLen()
{
  return stFifoInfo.in - stFifoInfo.out;
}

