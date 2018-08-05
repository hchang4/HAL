/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: configHWSetup.h
 * *
 * *  Description: Device discovery helper class. Provides APIs to initialize
 * *               and update the database with device information.
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
 * *  $Id: configHWSetup.h,v 1.9 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: configHWSetup.h,v $
 * *  Revision 1.9  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.8  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.7  2014/06/26 10:01:20  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.6  2013/01/30 06:18:54  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.5  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.4  2010/06/02 00:26:43  emrsn\rrthomas
 * *  Add device enumeration entries to HW_CONFIG table (Installed Hardware database) based on contents of ROC_CARD_TYPE table. This is a stop-gap fix for the buggy ROC card type detection on the XA platform hardware.
 * *
 * *  Revision 1.3  2006/12/13 22:29:40  epm-na\DPrabhakar
 * *  - Suresh modified tableAPI access code - dynamic allocation for
 * *    by passing the table that is being used in the constructor.
 * *    Done for memory usage optimization.
 * *  - All error messages are now logged using the SysLog() API (logLib)
 * *  - Modified code to use the DEBUGx API
 * *
 * *  Revision 1.2  2006/12/08 21:32:41  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _CONFIG_HW_SETUP_H
#define _CONFIG_HW_SETUP_H_

/*-------------------------System header files--------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
/*-------------------------Project header files-------------------------------*/
#include <global.h>
#include <tableapi.hpp>
#include <debug.h>
#include <wrappers.h>
#include <loglib.h>

/*-------------------------Macro Defination-----------------------------------*/
/*
  ROC Card Type enumerations; make sure that they match the table.xml definition 
  in ROC_CARD_CONIFG.roc_card_type column's us/metric format...
*/
enum ROC_CARD_TYPE
{
  ROC_UNKNOWN_CARD = 1,
  ROC_ANALOG_OUTPUT,
  ROC_RS232,
  ROC_RS485,
  ROC_DISCRETE_OUTPUT,
  ROC_DISCRETE_INPUT, 
  ROC_MODEM,
  ROC_ANALOG_INPUT,
} ;

enum ROC_CARD_SLOT
{
  ROC_EXPANSION_SLOT_1,
  ROC_EXPANSION_SLOT_2,
  ROC_EXPANSION_SLOT_3,
  ROC_EXPANSION_SLOT_4,
};

/*******************************************************************************
 ** * Class Name         :       configHWSetup
 ** * Description        :       Functions for configuring the HW_CONFIG table  
 ** * Member variable    :       hwConfig
 ** *      :       xpMaster
 ** * Member function    :       configHWSetup()
 **                      :       ~configHWSetup()
 **      :       SetTablesPointer()
 **                      :       AddDevNameHWConfigTable()
 **      :   PrintHWConfig()
 ******************************************************************************/

class configHWSetup
{
private:
  XP_MASTER_TABLE *xpMaster;
  XPTABLE *hwConfig, *rocTypeConfig;
  // Did we already do out initialization 
  int dbInit;
public:
  configHWSetup();
  ~configHWSetup();
  DB_INT16 SetTablesPointer();
  DB_INT16 AddDevNameHWConfigTable(std::string DevName, DB_INT32 ioFunction, DB_INT32 slotNumber, DB_INT32 reNumber, std::string DevInfo);
  DB_INT16 WriteHWConfigTable();
  DB_INT16 PrintHWConfig();
  ROC_CARD_TYPE GetROCCardType (ROC_CARD_SLOT slot);
};

#endif
