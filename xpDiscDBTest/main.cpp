/*******************************************************************************
 ** * *                          Rosemount Analytical 
 ** * *                    10241 West Little York, Suite 200 
 ** * *                           Houston, TX 77040 
 ** * *                                             
 ** * *
 ** * *  Filename: main.cpp
 ** * *
 ** * *  Description: Writing to HW_CONFIG table. Test application 
 ** * *  Copyright:        Copyright (c) 2011-2012, 
 ** * *                    Rosemount Analytical 
 ** * *                    All Rights Reserved.
 ** * *
 ** * *  Operating System:  LINUX.
 ** * *  Language:          'C++'
 ** * *  Target:            Gas Chamatagroph Model GC700XP
 ** * *
 ******************************************************************************/

/*---------------------------System header files------------------------------*/
/* None */

/*--------------------------Project header files------------------------------*/
#include "configHWSetup.hpp"
/*-------------------------Macro Defination-----------------------------------*/

/*******************************************************************************
 ** *Function Name :      main
 ** *Description   :    Writes device names to HW_CONFIG table   
 ** *Input         :      None
 ** *Output        :      None
 ** *Return Type   :      int
 ******************************************************************************/
int main(int argc, char *argv[])
{
  std::string devName;
  char szDevName [100];
  //devName = "ANA_IN:SLOT_2:ANA_IN";
  
  configHWSetup devConfig;
  devConfig.SetTablesPointer();
  if (argc == 1)
  {
    for (DB_INT32 count = 0; count < 10; ++count)
    {
      sprintf (szDevName, "%s%d", "ANA_IN:SLOT_2:ANA_IN", count);
      devName = szDevName;
      devConfig.AddDevNameHWConfigTable (devName, count, count, count);
    }
  }
  else
  {
    devConfig.PrintHWConfig();
  }
    
  return(EXIT_SUCCESS);
}
