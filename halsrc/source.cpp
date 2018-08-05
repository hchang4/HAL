/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: source.cpp
 * *
 * *  Description: 
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
 * *  $Id: source.cpp,v 1.4 2015/05/04 14:49:29 EMRSN\sboyidi Exp $
 * *  $Log: source.cpp,v $
 * *  Revision 1.4  2015/05/04 14:49:29  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.3  2014/06/26 10:01:08  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.2  2012/03/21 06:49:02  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * * *****************************************************************/

// source.cpp : Defines the entry point for the console application.
//

#include "CANComm.h"


int main(int argc, char* argv[])
{
  CCANComm can;
  can.CANCommOpen (0x1, 0x2, 0x3);
  return 0;
}

