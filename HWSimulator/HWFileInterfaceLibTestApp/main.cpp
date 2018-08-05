#include <stdio.h>

#include "hwfileif.h"

int main(int argc, char* argv[])
{
  int nRetVal = 0;
  CHwFileIf obFileIf("tags.txt");
  int nVal = 0;
  float fVal = 0.0;
  char szVal[100];
  int nStatus = 0;

  //Create a tag - float value
  if ( (nRetVal = obFileIf.CreateDevice("TEST:TAG", (float)1.115)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Create a tag - int
  if ( (nRetVal = obFileIf.CreateDevice("TEST:TAG1", 20)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Create a tag - int
  if ( (nRetVal = obFileIf.CreateDevice("TEST:TAG2", 20)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Create a tag - duplicate - error
  if ( (nRetVal = obFileIf.CreateDevice("TEST:TAG1", 20)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }


  //Update an existing tag
  if ( (nRetVal = obFileIf.WriteDevice("TEST:TAG1", 30)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Update an existing tag - string (diff data type)
  if ( (nRetVal = obFileIf.WriteDevice("TEST:TAG2", "/dev/can0")) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Read value from tag - float
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG", &fVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Read value from tag - float - repeat read
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG", &fVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Read value from tag - int
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG1", &nVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Read value from tag - int - repeat read
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG1", &nVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Update an existing tag - to check if new data status is set
  if ( (nRetVal = obFileIf.WriteDevice("TEST:TAG1", 30)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Read value from tag - int - updated value read - should indicate new
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG1", &nVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }


  //Read value from tag - string
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG2", szVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  //Read value from tag - string - repeat read
  if ( (nRetVal = obFileIf.ReadDevice("TEST:TAG2", szVal, &nStatus)) < 0)
  {
    printf("Tag access error: %d\n", nRetVal);
    //return -1;
  }

  return 0;
}
