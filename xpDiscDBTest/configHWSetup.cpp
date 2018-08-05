/*******************************************************************************
 ** *                          Rosemount Analytical 
 ** *                    10241 West Little York, Suite 200 
 ** *                           Houston, TX 77040 
 ** *                                             
 ** *
 ** *  Filename: configHWSetup.cpp
 ** *
 ** *  Description: Routines for configuring the HW_Config Table
 ** *
 ** *  Copyright:        Copyright (c) 2011-2012, 
 ** *                    Rosemount Analytical 
 ** *                    All Rights Reserved.
 ******************************************************************************/                  

/*---------------------------System header files------------------------------*/
/* None */

/*--------------------------Project header files------------------------------*/
#include"configHWSetup.hpp"

#define REMOTE_ID_STR_LENGTH  255
#define SEND_RECV_TIME_OUT    400

/*******************************************************************************
 * *Function Name :      configHWSetup()
 * *Description   :      Constructor 
 * *Input         :      None
 * *Output        :      None
 * *Return Type   :      None
 * ****************************************************************************/
configHWSetup :: configHWSetup()
{
  hwConfig  = NULL;
}
/*******************************************************************************
 * * Function Name :      ~configHWSetup()

 * * Description   :      Destuctor
 * * Input         :      None
 * * Output        :      None
 * * Return Type   :      None
 * * **************************************************************************/
configHWSetup :: ~configHWSetup()
{
}

/*******************************************************************************
 * *Function Name :      SetTablesPointer()
 * *Description   :      get pointer for the HW_CONFIG table 
 * *Input         :      None
 * *Output        :      None
 * *Return Type   :      None
 * ****************************************************************************/
DB_INT16 configHWSetup :: SetTablesPointer()
{
  char remoteId[REMOTE_ID_STR_LENGTH];
  strcpy (remoteId , "");
  DB_INT32 sendTimeout = SEND_RECV_TIME_OUT;
  DB_INT32 recvTimeout = SEND_RECV_TIME_OUT;
  
  // Initialize XP Master Table container
  xpMaster.init_data_table(remoteId, TASK_ID_GC, SYSTEM, sendTimeout, recvTimeout);
    
  // Get Pointer to "HW_CONFIG" table
  hwConfig  = xpMaster.get_class_pointer("HW_CONFIG");
  if(NULL == hwConfig)
  {
    DEBUG1("Table Not found : HW_CONFIG");
    SysLog((ERR_SEVERITY_ENUM)ERR_FATAL,
           (ERR_NUM_ENUM)(ERR_INVALID_DATA),__FILE__,__LINE__, 
           "::Table not found : HW_CONFIG");
    return -1;
  } 
  
  return 0;
}
/*******************************************************************************
 * * Function Name :      AddDevNameHWConfigTable()
 * * Description   :      Adds a new device name to the HW_CONFIG table.
 *        When invoked for the first time, this function clears
 *        the contents of the HW_CONFIG table. This is done
 *        because this function should be called when the GC is 
 *        started and the various hardware devices are pinged to
 *        determine supported device functions / enumerations.
 * * Input         :      None
 * * Output        :      None
 * * Return Type   :      None
 * * **************************************************************************/
DB_INT16 configHWSetup ::AddDevNameHWConfigTable(std::string DeviceName, DB_INT32 ioFunction, DB_INT32 slotNumber, DB_INT32 revNumber)
{
  static int init = 0; // Did we already do out initialization?
  static DB_INT32 numRows = 0; // Number of devices in HW_CONFIG table

  // Read table contents
  hwConfig->read_table_data();
  
  // The first time that we pass through this function, 
  // clear the contents of the HW_CONFIG table. 
  // The device discovery task runs once during the startup
  // and populates the HW configuration for all other system 
  // tasks to determine what HW is connected where.
  if (!init)
  {
    // Clear the contents the first time 
    // this function is called
    hwConfig->clear_table_data();
    init = 1;
  }
    
  // Write device details to the table
  hwConfig->set ("IO_name", numRows, DeviceName);
  hwConfig->set ("IO_function", numRows, ioFunction);
  hwConfig->set ("Slot_number", numRows, slotNumber);
  hwConfig->set ("rev_number", numRows, revNumber);

  numRows++; // Increment the number of rows in the table
  
  // Set number of rows
  hwConfig->set_stored_rows(numRows);
  
  // Update the table
  hwConfig->write_table_data();
  
  return 0;
}

/*******************************************************************************
 * * Function Name :      PrintHWConfig()
 * * Description   :      Print contents of HW_CONFIG table.
 * * Input         :      None
 * * Output        :      None
 * * Return Type   :      None
 *******************************************************************************/
DB_INT16 configHWSetup :: PrintHWConfig ()
{
  DB_INT32 noStoredRows = -1, HWFunction = -1, SlotNum = -1, RevNum = -1;
  std::string HWname;

  hwConfig->read_table_data();
  noStoredRows = hwConfig->get_stored_rows();

  if (noStoredRows)
  {
    // Header
    printf ("%5s\t%30s\t%5s\t%5s\t%5s\n", 
            "#", 
            "Dev Name",
            "Fn", 
            "Slot", 
            "Rev");

    // Get the details of each entry in the HW_CONFIG table
    for (DB_INT32 cnt = 0; cnt < noStoredRows; ++cnt)
    {
      hwConfig->get ("IO_name", cnt, HWname);

      hwConfig->get ("IO_function", cnt, HWFunction);
      hwConfig->get ("Slot_number", cnt, SlotNum);
      hwConfig->get ("rev_number", cnt, RevNum);

      printf ("%5d\t%30s\t%5d\t%5d\t%5d\n", 
              cnt,
              HWname.c_str(), 
              HWFunction, 
              SlotNum, 
              RevNum);
    }
  }

  return 0; 
}

