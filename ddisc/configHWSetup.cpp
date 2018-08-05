/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: configHWSetup.cpp
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
 * *  $Id: configHWSetup.cpp,v 1.9 2015/05/04 14:31:44 EMRSN\sboyidi Exp $
 * *  $Log: configHWSetup.cpp,v $
 * *  Revision 1.9  2015/05/04 14:31:44  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.8  2014/06/26 09:59:49  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.7  2012/03/21 06:48:42  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.6  2010/10/29 13:23:25  emrsn\rsaha
 * *  Revised to build ddiscfallback ( with no DbManager interface and HAL support ).
 * *  This task is launched from Mk3upgradefallback when no firmware exists in CPU board.
 * *  This is to facilitate logging of IO firmware version details in current_software_info.txt file, so to display in MON2020.
 * *
 * *  Revision 1.5  2010/06/02 00:26:42  emrsn\rrthomas
 * *  Add device enumeration entries to HW_CONFIG table (Installed Hardware database) based on contents of ROC_CARD_TYPE table. This is a stop-gap fix for the buggy ROC card type detection on the XA platform hardware.
 * *
 * *  Revision 1.4  2008/01/03 18:27:00  epm-na\lratner
 * *  Resolved most of the compiler warnings that are not caused by '#warning' pragmas.
 * *
 * *  Revision 1.3  2006/12/13 22:29:40  epm-na\DPrabhakar
 * *  - Suresh modified tableAPI access code - dynamic allocation for
 * *    by passing the table that is being used in the constructor.
 * *    Done for memory usage optimization.
 * *  - All error messages are now logged using the SysLog() API (logLib)
 * *  - Modified code to use the DEBUGx API
 * *
 * *  Revision 1.2  2006/12/08 21:27:35  epm-na\DPrabhakar
 * *  - Added Danile file headers.
 * *  - Added README file containing application execution information.
 * *  - Modified device discovery to update the database with board revision
 * *    information.
 * *  - Added device description field to the database containing information
 * *    provided by the device during device discovery.
 * *
 * *
 * *************************************************************************/


/*---------------------------System header files------------------------------*/
/* None */

/*--------------------------Project header files------------------------------*/
#include"configHWSetup.h"

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
#ifndef MAKE_FALLBACK
  char remoteId[REMOTE_ID_STR_LENGTH];
  DB_INT32 sendTimeout = SEND_RECV_TIME_OUT;
  DB_INT32 recvTimeout = SEND_RECV_TIME_OUT;
  std::vector<std::string> vTables;
  
  hwConfig  = NULL; rocTypeConfig = NULL;
  dbInit = 0;
  vTables.push_back((std::string)"HW_CONFIG");
  vTables.push_back((std::string)"ROC_CARD_CONFIG");
  
  xpMaster = new XP_MASTER_TABLE(vTables);

  strcpy (remoteId , "");

  // Initialize XP Master Table container
  xpMaster->init_data_table(remoteId, TASK_ID_GC, SYSTEM, sendTimeout, recvTimeout);
#endif //#ifndef MAKE_FALLBACK
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
#ifndef MAKE_FALLBACK
  if(xpMaster)
    delete xpMaster;
#endif //#ifndef MAKE_FALLBACK
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
#ifndef MAKE_FALLBACK   
  // Get Pointer to "HW_CONFIG" table
  hwConfig  = xpMaster->get_class_pointer("HW_CONFIG");
  if(NULL == hwConfig)
  {
    DEBUG1("Table Not found : HW_CONFIG");
    SysLog((ERR_SEVERITY_ENUM)ERR_FATAL,
           (ERR_NUM_ENUM)(ERR_INVALID_DATA),__FILE__,__LINE__, 
           "::Table not found : HW_CONFIG");
    return -1;
  } 

  // Read table contents
  hwConfig->read_table_data();
  
  // The first time that we pass through this function, 
  // clear the contents of the HW_CONFIG table. 
  // The device discovery task runs once during the startup
  // and populates the HW configuration for all other system 
  // tasks to determine what HW is connected where.
  if (!dbInit)
  {
    // Clear the contents the first time 
    // this function is called
    hwConfig->clear_table_data();
    dbInit = 1;
  }

  rocTypeConfig = xpMaster->get_class_pointer("ROC_CARD_CONFIG");
  if(NULL == rocTypeConfig)
  {
    DEBUG1("Table Not found : ROC_CARD_CONFIG");
    SysLog((ERR_SEVERITY_ENUM)ERR_FATAL,
           (ERR_NUM_ENUM)(ERR_INVALID_DATA),__FILE__,__LINE__, 
           "::Table not found : ROC_CARD_CONFIG");
    return -1;
  } 

  // Read table contents
  rocTypeConfig->read_table_data(); 
#endif //#ifndef MAKE_FALLBACK

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
DB_INT16 configHWSetup ::AddDevNameHWConfigTable(std::string DeviceName, DB_INT32 ioFunction, DB_INT32 slotNumber, DB_INT32 revNumber, std::string DevInfo)
{
#ifndef MAKE_FALLBACK
  static DB_INT32 numRows = 0; // Number of devices in HW_CONFIG table

  // Write device details to the table
  hwConfig->set ("IO_name", numRows, DeviceName);
  hwConfig->set ("IO_function", numRows, ioFunction);
  hwConfig->set ("Slot_number", numRows, slotNumber);
  hwConfig->set ("rev_number", numRows, revNumber);
  hwConfig->set ("device_description", numRows, DevInfo);

  numRows++; // Increment the number of rows in the table

  // Set number of rows
  hwConfig->set_stored_rows(numRows);
#endif //#ifndef MAKE_FALLBACK

  return 0;
}

/*******************************************************************************
 * * Function Name :      WriteHWConfigTable()
 * * Description   :      Commits all the data added in 'AddDevNameHWConfigTable'
 * *                      to the database
 * * Input         :      None
 * * Output        :      None
 * * Return Type   :      None
 * * **************************************************************************/
DB_INT16 configHWSetup::WriteHWConfigTable()
{
#ifndef MAKE_FALLBACK
  if (dbInit)
  {
    // Update the table
    hwConfig->write_table_data();
    return 0;
  }

  return -1;
#else
  return 0;
#endif //#ifndef MAKE_FALLBACK
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
#ifndef MAKE_FALLBACK
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

      printf ("%5ld\t%30s\t%5ld\t%5ld\t%5ld\n", 
              cnt,
              HWname.c_str(), 
              HWFunction, 
              SlotNum, 
              RevNum);
    }
  }
#endif //#ifndef MAKE_FALLBACK

  return 0; 
}

/*******************************************************************************
 * * Function Name :      GetROCCardType (ROC_CARD_SLOT)
 * * Description   :      Given the ROC Slot Number, determine the card type that is 
 * *                      supposed to be installed at that location. 
 * * Input         :      None
 * * Output        :      None
 * * Return Type   :      None
 *******************************************************************************/
ROC_CARD_TYPE configHWSetup :: GetROCCardType (ROC_CARD_SLOT slot)
{
  ROC_CARD_TYPE cardType = ROC_UNKNOWN_CARD; // By default, set to "Unknown" or "None"
#ifndef MAKE_FALLBACK
  DB_INT32 noStoredRows = -1;

  noStoredRows = rocTypeConfig->get_stored_rows();
  
  // Check if there are enough rows in the table that stores the card types... Remember that 
  // slot numbers and row indices are zero-based.
  if ( noStoredRows >= (slot + 1) )
  {
    rocTypeConfig->get ((std::string)"roc_card_type" , (int)slot, (DB_INT32&)cardType);
    //printf ("Slot = %d, Type = %d\n", slot + 1, cardType);
  }
#endif //#ifndef MAKE_FALLBACK

  return cardType;
}
