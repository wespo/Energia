/*! @file DisplayContents.cpp

  @brief Directory Browse Demo: query directory & subdirectories

  In this demo a file handle to the root directory is obtained. The
  contents of the root directory and its subsequent sub-directories are
  queried recursively and are displayed on the Serial.

   Note: SD Library requires SD card to be formatted in a specific
         format for proper operation. It is recommended to use
         SD formatter from SD card org or HP format tool for
         formatting the SD card
*/

#include "SD.h"

/// recursively query contents of directory, calling display on subdirectories
void display(void *Handle)
{
    File *parentHandle;
    File childHandle;
    char fileName[15];
    int  filecount = 0;
    int  directorycount = 0;

    parentHandle = (File *)Handle;

    while (1)
    {
        /*! Open next file in current directory using File.openNextFile() API */
        childHandle = parentHandle->openNextFile();
        if (childHandle)
        {
            if (childHandle.isDirectory())
            {
				/// If the child is a directory, set up to query the files it contains
                Serial.print("\n\rDirectory Name:");
                childHandle.getName(fileName);
                Serial.println(fileName);
                Serial.print("  Contents of the Directory:");
                Serial.print(fileName);
                Serial.println(" ");

                /*! Since its a directory display the contents under the
                   respective directory */
                display(&childHandle);
                directorycount++;
            }
            else
            {
				/// If the child is a file, print the name and size 
                Serial.print("File Name: ");
                childHandle.getName(fileName);
                Serial.print(fileName);
                Serial.print("\tSize: ");
                Serial.print(childHandle.size());
                Serial.println(" Bytes");
                filecount++;
            }

            childHandle.close();
        }
        else
        {
			/// print the total number of contained files and directories at the end
            Serial.print("  No more Files Under the directory:");
            parentHandle->getName(fileName);
            Serial.println(fileName);
            Serial.print("  File Count: ");
            Serial.print(filecount);
            Serial.print("\tDirectory Count: ");
            Serial.println(directorycount);
            Serial.println(" ");
            break;
        }
    }
}

/// Open SD card, get file handle to Root directory
void setup()
{
    Bool status;
    File fileHandle;
    Serial.begin(9600);
    status = SD.begin();
    if (TRUE == status)
    {
        /*! Get a File handle to the Root directory */
        fileHandle = SD.open("/");
        if (fileHandle)
        {
            Serial.println("  Contents of the Root Directory are:");
			/// query subdirectories
            display(&fileHandle);
        }
    }
    else
    {
        Serial.println("Error in Initializing SD Module");
    }

    fileHandle.close();

    Serial.println("\r\n End of Directory Browse Demo");
}

void loop()
{
}
