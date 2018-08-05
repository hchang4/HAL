/*******************************************************************************
 ** *                          Rosemount Analytical 
 ** *                    10241 West Little York, Suite 200 
 ** *                           Houston, TX 77040 
 ** *                                             
 ** *
 ** *  Filename: configHWSetup.hpp
 ** *
 ** *  Description: Analog Input/Output Hardware Setup Header File.
 ** *       Read HW_CONFIG Table, sort out Analog Input/  
 ** *       Output installed hardware and pop up in to 
 ** *       ANALOG_INPUTS/OUTPUTS_SEL table respectively.
 ** *     
 ** *  Copyright:        Copyright (c) 2011-2012, 
 ** *                    Rosemount Analytical 
 ** *                    All Rights Reserved.
 ** *
 ** *  Operating System:  LINUX.
 ** *  Language:          'C++'
 ** *  Target:            Gas Chamatagroph  model GC700XP
 *****************************************************************************/

#ifndef _CONFIG_HW_SETUP_H
#define _CONFIG_HW_SETUP_H_

#define ENABLE_DEBUG
/*-------------------------System header files--------------------------------*/
#include <iostream.h>
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
  XP_MASTER_TABLE xpMaster;
  XPTABLE *hwConfig;
public:
  configHWSetup();
  ~configHWSetup();
  DB_INT16 SetTablesPointer();
  DB_INT16 AddDevNameHWConfigTable(std::string DevName, DB_INT32 ioFunction, DB_INT32 slotNumber, DB_INT32 reNumber);
  DB_INT16 PrintHWConfig();
};

#endif
