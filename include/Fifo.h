/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Fifo.h
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
 * *  $Id: Fifo.h,v 1.6 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 * *  $Log: Fifo.h,v $
 * *  Revision 1.6  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.5  2014/06/26 10:01:13  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.4  2012/03/21 06:49:06  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2006/12/17 21:00:52  EPM-NA\ssatpute
 * *  GetErrorMsg() method added to resolve error message from error code.
 * *
 * *  Revision 1.2  2006/12/08 21:32:40  epm-na\DPrabhakar
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

#ifndef _FIFO_H
#define _FIFO_H

#define min(x,y) (x < y ? x : y)

#define max(x,y) (x > y ? x : y)

typedef struct {
  unsigned char *buffer;  /* the buffer holding the data */
  unsigned int size;      /* the size of the allocated buffer */
  unsigned int in;        /* data is added at offset (in % size) */
  unsigned int out;       /* data is extracted from off. (out % size) */
} FifoInfo;


class CFifo
{
private:
  unsigned char *pucData;
  FifoInfo stFifoInfo;
public:
  //IMPORTANT: The FIFO size MUST be a power of 2
  //Constructor
  CFifo(unsigned int unFifoSize);
  //Destructor
  virtual ~CFifo();
  //Add data to the fifo
  unsigned int WriteFifo(unsigned char *buffer, unsigned int len);
  //Remove data from the fifo
  unsigned int ReadFifo(unsigned char *buffer, unsigned int len);
  //Get the length of the fifo
  unsigned int GetLen();
};

#endif //#ifndef _FIFO_H


