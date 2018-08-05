/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: hwfileif.cpp
 * *
 * *  Description: Hardware file interface library. Use this library
 * *               to create, access and modify 'tags', which represent
 * *               specific device functions.
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
 * *  $Id: hwfileif.cpp,v 1.6 2015/05/04 14:50:46 EMRSN\sboyidi Exp $
 * *  $Log: hwfileif.cpp,v $
 * *  Revision 1.6  2015/05/04 14:50:46  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.5  2014/06/26 09:58:58  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.4  2012/03/21 06:48:31  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2007/03/01 07:10:41  EPM-NA\ssatpute
 * *  Increased tag value length to 255 characters. This is universal and applies to all data types (int/float/char *).
 * *
 * *
 * *************************************************************************/


#include <string.h>
#include <malloc.h>

#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "hwfileif.h"

extern int errno;

//Constructor
CHwFileIf::CHwFileIf(char *pszFileName)
{
  int nStrLen = 0;

  //Init vars...
  m_nErrorCode = FILEIF_NO_ERROR;

  m_fdFile = 0;

  m_eIsTagSelected = TAG_FUNC_NAME;

  memset(&m_szFileName, 0, 
         sizeof(m_szFileName[0]) * MAX_FILE_PATH_LEN);

  m_ulTagsAccessedCount = 0;

  m_pstTagsAccessed = NULL;

  m_nEnableIncCount = 0;

  //See if the user has supplied a file name
  if (pszFileName != NULL)
  {
    nStrLen = strlen(pszFileName);

    //Check if the file path is too long
    if (nStrLen > MAX_FILE_PATH_LEN)
    {
      m_nErrorCode = FILEIF_ERR_PATH_LONG;
    }
    else
    {
      //Save the user supplied file name
      //TODO - probably don't need it... may be for debugging...
      strncpy(m_szFileName, pszFileName, nStrLen);

      //NULL terminate the file path
      m_szFileName[nStrLen] = '\0';

//!!!We shouldn't be deleting the tag file on start-up
#if 0
      //Check if the file already exists.
      //  Delete the file if it already exists.
      //  Set an error if unable to delete
      if (0 == access(m_szFileName, F_OK))
      {
        if (unlink(m_szFileName) < 0)
        {
          m_nErrorCode = FILEIF_ERR_FILE_ACCESS;
        }
      }
#endif

      //If no error occurred previously, create & open the 
      //  user specified file in read+write mode.
      if (FILEIF_NO_ERROR == m_nErrorCode)
      {
        m_fdFile = open(m_szFileName, O_RDWR | O_CREAT, 0777);

        if (m_fdFile < 0)
        {
          m_nErrorCode = FILEIF_ERR_FILE_OPEN;
        }
      }
    }
  }
  else
  {
    m_nErrorCode = FILEIF_ERR_NO_FILENAME;
  }
}

//Destructor
CHwFileIf::~CHwFileIf()
{
  if (FILEIF_NO_ERROR == m_nErrorCode)
  {
    close(m_fdFile);
  }

  if (m_pstTagsAccessed)
  {
    free(m_pstTagsAccessed);
  }
}

//Create & initialize a tag
int CHwFileIf::CreateDevice(char *pszTag, int nInitVal)
{
  //Create a new tag with value of type Integer
  return CreateTag(pszTag, &nInitVal, DTYPE_INT);
}

//Create & initialize a tag
int CHwFileIf::CreateDevice(char *pszTag, float fInitVal)
{
  //Create a new tag with value of type Float
  return CreateTag(pszTag, &fInitVal, DTYPE_FLOAT);
}

//Create & initialize a tag
int CHwFileIf::CreateDevice(char *pszTag, char *pszInitVal)
{
  //Create a new tag with value of type String
  return CreateTag(pszTag, pszInitVal, DTYPE_STRING);
}

//Write to the specified tag
int CHwFileIf::WriteDevice(char *pszTag, int nValue)
{
  //Update the value field with the user supplied value
  return SetValue(pszTag, &nValue, DTYPE_INT);
}

//Write to the specified tag
int CHwFileIf::WriteDevice(char *pszTag, float fValue)
{
  return SetValue(pszTag, &fValue, DTYPE_FLOAT);
}

//Write to the specified tag
int CHwFileIf::WriteDevice(char *pszTag, char *pszValue)
{
  //Update the value field with the user supplied value
  return SetValue(pszTag, pszValue, DTYPE_STRING);
}

//Read from the specified tag
int CHwFileIf::ReadDevice(char *pszTag, int *pnValue, int *pnIsNew)
{
  //Update the value field with the user supplied value
  return GetValue(pszTag, pnValue, DTYPE_INT, pnIsNew);
}

//Read from the specified tag
int CHwFileIf::ReadDevice(char *pszTag, float *pfValue, int *pnIsNew)
{
  //Update the value field with the user supplied value
  return GetValue(pszTag, pfValue, DTYPE_FLOAT, pnIsNew);
}

//Read from the specified tag
int CHwFileIf::ReadDevice(char *pszTag, char *pszValue, int *pnIsNew)
{
  //Update the value field with the user supplied value
  return GetValue(pszTag, pszValue, DTYPE_STRING, pnIsNew);
}

//Check if the tag is valid
int CHwFileIf::CheckTagValidity(char *pszTag)
{
  int nColonCnt = 0;
  int nStrLen = 0;

  if ( (NULL == pszTag) )
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  nStrLen = strlen(pszTag);

  //Return with error if the tag is too long
  if (nStrLen > MAX_TAG_LEN)
  {
    return FILEIF_ERR_LONG_TAG;
  }

  //Count the number of Tag field separators
  for (int nCnt = 0; nCnt < nStrLen; nCnt++)
  {
    if (TAG_FIELD_SEPR == pszTag[nCnt])
    {
      nColonCnt++;
    }
  }

  //Return with error if more than 1 separator is found
  if (nColonCnt != 1)
  {
    return FILEIF_ERR_TAG_INVALID;
  }

  //Return with error if the last character is a separator
  if (TAG_FIELD_SEPR == pszTag[nStrLen - 1])
  {
    return FILEIF_ERR_TAG_INVALID; 
  }

  return FILEIF_NO_ERROR;
}

//Create and initialize a tag with value and count
int CHwFileIf::CreateTag(char *pszTag, void *pData, E_DATA_TYPE eDataType)
{
  int nStrLen = 0;
  int nLocalErr = FILEIF_NO_ERROR;
  //Length of the tag itself + length of value field + 
  //  length of count field + new line + null termination
  char szTagVal[MAX_TAG_LEN + MAX_TAG_VAL_LEN + 1 + MAX_TAG_CNT_LEN + 1 + 1];

  if ( (NULL == pszTag) || (NULL == pData) || (eDataType >= MAX_DATA_TYPE) )
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  if (m_nErrorCode)
  {
    return m_nErrorCode;
  }

  //Lock file
  nLocalErr = FileLock(TAG_WR_DEV, FLOCK_OPER_LOCK);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    return nLocalErr;
  }

  //Check tag name validity
  nLocalErr = CheckTagValidity(pszTag);

  //Return with error for an invalid tag
  if (nLocalErr != FILEIF_NO_ERROR)
  {
    goto exit_CreateTag;
    //return nLocalErr;
  }

  //Duplicate tag check...
  //If the tag already exists, return with an error
  //  TODO - alternate - just update the value and return?
  if (FILEIF_NO_ERROR == SelectTagField(pszTag, TAG_FIELD_VALUE))
  {
    nLocalErr = FILEIF_ERR_DUP_TAG;
    goto exit_CreateTag;
    //return FILEIF_ERR_DUP_TAG;
  }

  memset(szTagVal, 0, sizeof(szTagVal));

  //Form the tag name, value and count string for the specified data type
  switch (eDataType)
  {
  case DTYPE_INT:
    sprintf(szTagVal, "%s:%+*d,%+*lld\n", pszTag, MAX_TAG_VAL_LEN, 
            *(int *)pData, MAX_TAG_CNT_LEN, TAG_INIT_COUNT);
    break;

  case DTYPE_FLOAT:
    sprintf(szTagVal, "%s:%+*.*e,%+*lld\n", pszTag, MAX_TAG_VAL_LEN, 
            TAG_VAL_DECML_SEPR, *(float *)pData, MAX_TAG_CNT_LEN, TAG_INIT_COUNT);
    break;

  default:
  case DTYPE_STRING:
    sprintf(szTagVal, "%s:%*s,%+*lld\n", pszTag, MAX_TAG_VAL_LEN, 
            (char *)pData, MAX_TAG_CNT_LEN, TAG_INIT_COUNT);
    break;
  }

  //Go to the end of the file.
  if (lseek(m_fdFile, 0, SEEK_END) < 0)
  {
    nLocalErr = FILEIF_ERR_FILE_ACCESS;
    //return FILEIF_ERR_FILE_ACCESS;
    goto exit_CreateTag;
  }
              
  nStrLen = strlen(szTagVal);

  //Write this new tag to the file
  if (write(m_fdFile, szTagVal, nStrLen) < nStrLen)
  {
    nLocalErr = FILEIF_ERR_FILE_WR;
    goto exit_CreateTag;
    //return FILEIF_ERR_FILE_WR;
  }

  //Sync data to file immediately
  if (fsync(m_fdFile) < 0)
  {
    nLocalErr = FILEIF_ERR_FILE_WR;
    goto exit_CreateTag;
  }

exit_CreateTag:
  //If an error occurred in any of the prev operation, 
  //  try to unlock and return the original error code
  if (nLocalErr)
  {
    FileLock(TAG_WR_DEV, FLOCK_OPER_UNLOCK);
    return nLocalErr;
  }
  else
  {
    //Unlock file
    nLocalErr = FileLock(TAG_WR_DEV, FLOCK_OPER_UNLOCK);

    if (nLocalErr != FILEIF_NO_ERROR)
    {
      return nLocalErr;
    }

    //Store this tag in the list of tags
    return UpdateLocalTagList(pszTag);
  }
}

//Go to the specified tag - to be called before 
// calling SetValue(...) or GetValue(...) or GetCount(...)
int CHwFileIf::SelectTagField(char *pszTag, E_TAG_FIELD eField)
{
  int nRetVal = 0;
  char szReadTag[TAG_STRING_LEN];
  char cData;
  E_TAG_FIELD eFieldCount = TAG_FUNC_NAME;
  int nWrIx = 0;
  int nLocalErr = FILEIF_NO_ERROR;

  if ( (NULL == pszTag) || (eField >= MAX_TAG_FIELDS) || 
       (eField <= TAG_TYPE_NAME) )
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  if (m_nErrorCode)
  {
    return m_nErrorCode;
  }

  //Check tag name validity
  nLocalErr = CheckTagValidity(pszTag);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    return nLocalErr;
  }

  memset(szReadTag, 0, sizeof (szReadTag[0] * TAG_STRING_LEN));

  //Start from the beginning of the file
  if (lseek(m_fdFile, 0, SEEK_SET) < 0)
  {
    return FILEIF_ERR_FILE_ACCESS;
  }

  m_eIsTagSelected = TAG_FUNC_NAME;

  //Parse through the tag name and point to the specified 
  //  field (value or count) of the specified tag
  while ( (nRetVal >= 0) && (FILEIF_NO_ERROR == nLocalErr) 
          && (m_eIsTagSelected <= TAG_TYPE_NAME) )
  {
    nRetVal = read(m_fdFile, &cData, 1);

    //End of file indication
    if (0 == nRetVal)
    {
      //Tag not found
      nLocalErr = FILEIF_ERR_NO_TAG;
    }
    //Read error
    else if (nRetVal < 0)
    {
      nLocalErr = FILEIF_ERR_FILE_RD;
    }
    //Save tag info and compare
    else
    {
      switch (cData)
      {
        //Field separator... 
      case TAG_FIELD_SEPR:
        switch (eFieldCount)
        {
          //This part of the tag contains the name of the 'function'
          //  accumulate this into an array
        case TAG_FUNC_NAME:
          eFieldCount = TAG_TYPE_NAME;

          szReadTag[nWrIx] = cData;
          nWrIx++;
          if (nWrIx >= MAX_TAG_LEN)
          {
            nLocalErr = FILEIF_ERR_LONG_TAG;
          }
          break;

          //This part of the tag contains the feature information.
        case TAG_TYPE_NAME:
          //Terminate the tag string
          szReadTag[nWrIx] = '\0';
          //Compare with the user specified tag
          if (0 == strcmp(szReadTag, pszTag))
          {
            //Reset stuff
            nWrIx = 0;
            eFieldCount = TAG_FIELD_VALUE;
            //If the caller requested the value field, break out of the loop right here
            if (TAG_FIELD_VALUE == eField)
            {
              m_eIsTagSelected = TAG_FIELD_VALUE;
            }
            //If the caller requested the count field, offset to the start of the
            //  count field for this tag and then break out of the loop
            else if (TAG_FIELD_COUNT == eField)
            {
              if (lseek(m_fdFile, (MAX_TAG_VAL_LEN + 1), SEEK_CUR) < 0)
              {
                nLocalErr = FILEIF_ERR_FILE_RD;
              }
              else 
              {
                m_eIsTagSelected = TAG_FIELD_COUNT;
              }
            }
          }
          else
          {
            //Hop to the next tag: Go to the end of line 
            //  (value length + ',' + count length)
            if (lseek(m_fdFile, (MAX_TAG_VAL_LEN + 1 + MAX_TAG_CNT_LEN), SEEK_CUR) < 0)
            {
              nLocalErr = FILEIF_ERR_FILE_ACCESS;
            }
          }
          break;

        default:
          break;
        }
        break;

        //New line and carriage return
      case '\n':
      case '\r':
        nWrIx = 0;
        eFieldCount = TAG_FUNC_NAME;
        break;

        //This should only execute when we are accumulating the tag name
      default:
        switch (eFieldCount)
        {
          //Store tag name...
        case TAG_TYPE_NAME:
        case TAG_FUNC_NAME:
          szReadTag[nWrIx] = cData;
          nWrIx++;
          if (nWrIx >= MAX_TAG_LEN)
          {
            nLocalErr = FILEIF_ERR_LONG_TAG;
          }
          break;

        default:
          nLocalErr = FILEIF_ERR_PARSE;
          break;
        }

        break;
      }
    }
  }

  return nLocalErr;
}

//Store value for the specified tag
int CHwFileIf::SetValue(char *pszTag, void *pData, E_DATA_TYPE eDataType)
{
  char szValue[TAG_VAL_STRING_LEN];
  int nLocalErr = FILEIF_NO_ERROR;
  char *pszStr = NULL;

  if ( (NULL == pData) || (eDataType >= MAX_DATA_TYPE) )
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  if (m_nErrorCode)
  {
    return m_nErrorCode;
  }

  //Lock file
  nLocalErr = FileLock(TAG_WR_DEV, FLOCK_OPER_LOCK);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    return nLocalErr;
  }

  //Select the value field for the specified tag
  nLocalErr = SelectTagField(pszTag, TAG_FIELD_VALUE);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    goto exit_SetValue;
    //return nLocalErr;
  }

  //Verify that the value field has been selected
  if (m_eIsTagSelected != TAG_FIELD_VALUE)
  {
    nLocalErr = FILEIF_ERR_INT_ERR;
    goto exit_SetValue;
    //return FILEIF_ERR_INT_ERR;
  }

  memset(szValue, 0, sizeof(szValue[0]) * MAX_TAG_VAL_LEN);

  //Convert value to string based on data type
  switch (eDataType)
  {
  case DTYPE_INT:
    sprintf(szValue, "%+*d", MAX_TAG_VAL_LEN, *(int *)pData);
    break;

  case DTYPE_FLOAT:
    sprintf(szValue, "%+*.*e", MAX_TAG_VAL_LEN, TAG_VAL_DECML_SEPR, *(float *)pData);
    break;

  default:
  case DTYPE_STRING:
    pszStr = (char *) pData;
    if (strlen(pszStr) > MAX_TAG_VAL_LEN)
    {
      nLocalErr = FILEIF_ERR_LONG_VAL;
      goto exit_SetValue;
    }

    sprintf(szValue, "%*s", MAX_TAG_VAL_LEN, pszStr);
    break;
  }

  //Update tag value
  if (write(m_fdFile, szValue, MAX_TAG_VAL_LEN) < MAX_TAG_VAL_LEN)
  {
    nLocalErr = FILEIF_ERR_FILE_WR;
    goto exit_SetValue;
    //return FILEIF_ERR_FILE_WR;
  }

  //Sync data to file immediately
  if (fsync(m_fdFile) < 0)
  {
    nLocalErr = FILEIF_ERR_FILE_WR;
    goto exit_SetValue;
  }


  //Store this tag in the list of tags
  nLocalErr = UpdateLocalTagList(pszTag);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    goto exit_SetValue;
    //return nLocalErr;
  }


exit_SetValue:
  //If an error occurred in any of the prev operation, 
  //  try to unlock and return the original error code
  if (nLocalErr)
  {
    FileLock(TAG_WR_DEV, FLOCK_OPER_UNLOCK);
    return nLocalErr;
  }
  else
  {
    //Unlock file
    nLocalErr = FileLock(TAG_WR_DEV, FLOCK_OPER_UNLOCK);

    if (nLocalErr != FILEIF_NO_ERROR)
    {
      return nLocalErr;
    }

    m_nEnableIncCount = 1;

    //Increment the count
    return IncCount();
  }
}

//Get value of the specified tag
int CHwFileIf::GetValue(char *pszTag, void *pData, E_DATA_TYPE eDataType, int *pnIsNew)
{
  char szValue[TAG_VAL_STRING_LEN];
  char szFormat[20];
  unsigned int unCnt = 0, unCpCnt = 0;
  int nFoundValidChar = 0;
  int nLocalErr = FILEIF_NO_ERROR;

  if ( (NULL == pData) || (eDataType >= MAX_DATA_TYPE) || (NULL == pnIsNew) )
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  if (m_nErrorCode)
  {
    return m_nErrorCode;
  }

  //Lock file
  nLocalErr = FileLock(TAG_RD_DEV, FLOCK_OPER_LOCK);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    return nLocalErr;
  }

  //Select the value field for the specified tag
  nLocalErr = SelectTagField(pszTag, TAG_FIELD_VALUE);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    goto exit_GetValue;
    //return nLocalErr;
  }

  //Verify that the value field has been selected
  if (m_eIsTagSelected != TAG_FIELD_VALUE)
  {
    nLocalErr = FILEIF_ERR_INT_ERR;
    goto exit_GetValue;
    //return FILEIF_ERR_INT_ERR;
  }

  memset(szValue, 0, TAG_VAL_STRING_LEN);

  //Read the entire value field
  if (read(m_fdFile, szValue, MAX_TAG_VAL_LEN) < MAX_TAG_VAL_LEN)
  {
    nLocalErr = FILEIF_ERR_FILE_RD;
    goto exit_GetValue;
    //return FILEIF_ERR_FILE_RD;
  }

  //Convert string to value, based on data type
  switch (eDataType)
  {
  case DTYPE_INT:
    sprintf(szFormat, "%%%dd", MAX_TAG_VAL_LEN);

    //sscanf(szValue, "%*d", MAX_TAG_VAL_LEN, (int *)pData);
    sscanf(szValue, szFormat, (int *)pData);
    break;

  case DTYPE_FLOAT:
    sscanf(szValue, "%e", (float *)pData);
    break;

  default:
  case DTYPE_STRING:
    //For string type, remove leading white spaces
    for (unCnt = 0, unCpCnt = 0; unCnt < strlen(szValue); unCnt++)
    {
      if ( (szValue[unCnt] != ' ') || (nFoundValidChar) )
      {
        nFoundValidChar = 1;
        * ((char *) pData + unCpCnt) = szValue[unCnt];
        unCpCnt++;
      }
    }
    * ((char *) pData + unCpCnt) = '\0';
    //sscanf(szValue, "%s", (char *)pData);
    break;
  }

  //Store this tag in the list of tags
  nLocalErr = UpdateLocalTagList(pszTag);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    goto exit_GetValue;
    //return nLocalErr;
  }

exit_GetValue:
  //If an error occurred in any of the prev operation, 
  //  try to unlock and return the original error code
  if (nLocalErr)
  {
    FileLock(TAG_RD_DEV, FLOCK_OPER_UNLOCK);
    return nLocalErr;
  }
  else
  {
    //Unlock file
    nLocalErr = FileLock(TAG_RD_DEV, FLOCK_OPER_UNLOCK);

    if (nLocalErr != FILEIF_NO_ERROR)
    {
      return nLocalErr;
    }

    //Store this tag in the list of tags
    return CheckNewData(pszTag, pnIsNew);
  }
}

//Check and updated the local list of successfully accessed tags
int CHwFileIf::UpdateLocalTagList(char *pszTag)
{
  void *pTagsRealloc = NULL;

  if (NULL == pszTag)
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  //Check if the tag already exists in our list
  for (unsigned long ulCnt = 0; ulCnt < m_ulTagsAccessedCount; ulCnt++)
  {
    //If the tag already exists, return with no error
    if (0 == strcmp(m_pstTagsAccessed[ulCnt].szTagName, pszTag))
    {
      return FILEIF_NO_ERROR;
    }
  }

//This is a new tag... so, add it to the list:
  //Increment count of tags
  m_ulTagsAccessedCount++;

  //Allocate memory for the new tag
  pTagsRealloc = realloc(m_pstTagsAccessed, 
                         sizeof(STagsAccessed) * m_ulTagsAccessedCount);

  //Return with error if allocation failed
  if (NULL == pTagsRealloc)
  {
    return FILEIF_ERR_MEMORY;
  }

  //Update pointer to the array of tags
  m_pstTagsAccessed = (STagsAccessed *) pTagsRealloc;

  //Store the tag name
  strcpy(m_pstTagsAccessed[m_ulTagsAccessedCount - 1].szTagName, pszTag);
  //Init count to zero
  m_pstTagsAccessed[m_ulTagsAccessedCount - 1].llLastReadCount = 0;

  return FILEIF_NO_ERROR;
}

//Check if the data being read is new (by comparing the count field with the
//  local count).
int CHwFileIf::CheckNewData(char *pszTag, int *pnIsNew)
{
  tagcount llCrntCnt = 0;
  unsigned long ulTagListCnt = 0;
  int nTagFound = 0;
  int nLocalErr = FILEIF_NO_ERROR;

  nLocalErr = GetCount(pszTag, &llCrntCnt);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    return nLocalErr;
  }

  //Check for the tag in the internal list of tags
  for (ulTagListCnt = 0; ulTagListCnt < m_ulTagsAccessedCount; ulTagListCnt++)
  {
    //On finding the tag, break out of the loop
    if (0 == strcmp(m_pstTagsAccessed[ulTagListCnt].szTagName, pszTag))
    {
      nTagFound = 1;
      break;
    }
  }

  if (nTagFound)
  {
    if (m_pstTagsAccessed[ulTagListCnt].llLastReadCount < llCrntCnt)
    {
      m_pstTagsAccessed[ulTagListCnt].llLastReadCount = llCrntCnt;
      *pnIsNew = 1;
    }
    else if (m_pstTagsAccessed[ulTagListCnt].llLastReadCount == llCrntCnt)
    {
      *pnIsNew = 0;
    }
    else
    {
      return FILEIF_ERR_COUNT;
    }
  }
  else
  {
    return FILEIF_ERR_INT_ERR;
  }

  return FILEIF_NO_ERROR;
}


//Get write count for the selected tag
int CHwFileIf::GetCount(char *pszTag, tagcount *llCount)
{
  int nLocalErr = FILEIF_NO_ERROR;
  char szCount[TAG_CNT_STRING_LEN];
  char szFormat[20];
  tagcount llTagCount = 0;

  if (NULL == llCount)
  {
    return FILEIF_ERR_INVALID_ARGS;
  }

  //Select the value field for the specified tag
  nLocalErr = SelectTagField(pszTag, TAG_FIELD_COUNT);

  if (nLocalErr != FILEIF_NO_ERROR)
  {
    return nLocalErr;
  }

  //Verify that the the count field has been selected
  if (m_eIsTagSelected != TAG_FIELD_COUNT)
  {
    return FILEIF_ERR_INT_ERR;
  }

  //Read the entire count field
  if (read(m_fdFile, szCount, MAX_TAG_CNT_LEN) < MAX_TAG_CNT_LEN)
  {
    return FILEIF_ERR_FILE_RD;
  }

  //Setup the format for reading the count
  sprintf(szFormat, "%%%dlld", MAX_TAG_CNT_LEN);

  //sscanf(szCount, "%*lld", MAX_TAG_CNT_LEN, &llTagCount);
  sscanf(szCount, szFormat, &llTagCount);

  if (llTagCount < 0)
  {
    return FILEIF_ERR_COUNT;
  }

  *llCount = llTagCount;

  return nLocalErr;
}

//Get write count for the selected tag (to be called from SetValue(...))
//IMPORTANT: THIS IS NOT A GENERIC FUNCTION. THIS FUNCTION EXPECTS TO BE
//  CALLED FROM SetValue(...).
int CHwFileIf::IncCount()
{
  char szCount[TAG_CNT_STRING_LEN];
  char szFormat[20];
  tagcount llTagCount = 0;

  //Validate if called in correct sequence
  if (!m_nEnableIncCount)
  {
    return FILEIF_ERR_SEQ;
  }

  //Reset after validation
  m_nEnableIncCount = 0;

  memset(szCount, 0, TAG_CNT_STRING_LEN);

  //Go past the TAG_VALUE_SEPR
  if (lseek(m_fdFile, 1, SEEK_CUR) < 0)
  {
    return FILEIF_ERR_FILE_ACCESS;
  }

  //Read the entire count field
  if (read(m_fdFile, szCount, MAX_TAG_CNT_LEN) < MAX_TAG_CNT_LEN)
  {
    return FILEIF_ERR_FILE_RD;
  }

  //Setup the format for reading the count
  sprintf(szFormat, "%%%dlld", MAX_TAG_CNT_LEN);

  //sscanf(szCount, "%*lld", MAX_TAG_CNT_LEN, &llTagCount);
  sscanf(szCount, szFormat, &llTagCount);

  if (llTagCount < 0)
  {
    return FILEIF_ERR_COUNT;
  }

  llTagCount++;

  //Rewind to the beginning to the count field
  if (lseek(m_fdFile, -MAX_TAG_CNT_LEN, SEEK_CUR) < 0)
  {
    return FILEIF_ERR_FILE_ACCESS;
  }

  //Convert count to string
  sprintf(szCount, "%+*lld", MAX_TAG_CNT_LEN, llTagCount);

  //Update tag count
  if (write(m_fdFile, szCount, MAX_TAG_CNT_LEN) < MAX_TAG_CNT_LEN)
  {
    return FILEIF_ERR_FILE_WR;
  }

  //Sync data to file immediately
  if (fsync(m_fdFile) < 0)
  {
    return FILEIF_ERR_FILE_WR;
  }

  //printf("%s: Count %lld\n", __FUNCTION__, llTagCount);

  return FILEIF_NO_ERROR;
}

//File access lock enable routine
int CHwFileIf::FileLock(E_TAG_OPER eTagOper, E_LOCK_OPER eLockOper)
{
  struct flock stFileLock;

  int nLockCmd = F_SETLKW;
  int nLocErrno = 0;

  //If locking the file, decide what type of lock to place
  if (FLOCK_OPER_LOCK == eLockOper)
  {
    //Place a WRITE lock when writing to a tag
    if (TAG_WR_DEV == eTagOper)
    {
      stFileLock.l_type = F_WRLCK;
    }
    //Place a READ lock when reading from a tag
    else
    {
      stFileLock.l_type = F_RDLCK;
    }
  }
  //Otherwise, unlock the file
  else
  {
    stFileLock.l_type = F_UNLCK;
  }
  
  stFileLock.l_whence = SEEK_SET;
  stFileLock.l_start = 0;
  stFileLock.l_len = 0;

  do 
  {
    //Obtain the lock - wait till the lock is available
    if (fcntl(m_fdFile, nLockCmd, &stFileLock) < 0)
    {
      nLocErrno = errno;
      perror("FileLock:");
      if (nLocErrno != EINTR)
      {
        return FILEIF_ERR_LOCK;
      }
      else
      {
        printf("Retrying...\n");
      }
    }
    else
    {
      break;
    }
  } while (1);

  return FILEIF_NO_ERROR;
}
