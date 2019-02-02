
//
// Execute.C
// Craig.Peacock@beyondlogic.org
// http://www.beyondlogic.org
//
// Usage : AllowIO <ExecutableName.exe>       eg. AllowIO Beep.exe
//
// When executed with a executable as an argument will create the process
// and send the newly created process's ID to the Device Driver so it is
// granted excusive access to the IO Ports. Ie. the driver will modify 
// the IOPM (I/O Permission Bitmap) of the created process.
//
// Monday 12th June 2000
//

#include <stdio.h>
#include <windows.h>

int __cdecl main(int argc, char ** argv)
{
	HANDLE h;  			// Handle for PortTalk Driver
	int Error; 		 	// Error Handling for DeviceIoControl()
 	int count;			// Temp Variable to process Auguments
	char filename[80] = {""};	// Filename of Executable
 	DWORD BytesReturned;		// Bytes Returned for DeviceIoControl()
	DWORD IoControlCode = 0x04;	// IOCTL for IOPM 
  	DWORD InputBufferSize = 0x04;   // Buffer Size for IOPM Call
	STARTUPINFO si;			// Startup Info Structure
    	PROCESS_INFORMATION pi;		// Process Info Structure - Contains Process ID Information

	int value;
	int offset;

        printf("BeyondLogic AllowIO\n");

	// No Arguments ?
	if (argc <= 1)
		{
		 printf("HTTP://www.beyondlogic.org\n");
		 printf("Grants the specified executable exclusive access to specified \nI/O Ports under Windows NT/Windows 2000\n");
		 printf("Usage : AllowIO <executable> <addresses(Hex)> <switches>\n");
		 printf("Switches : /a - Grants exclusive access to ALL Ports\n");
		 return 0;
		}

	// Open PortTalk Driver
    	h = CreateFile("\\\\.\\PortTalk", 
			GENERIC_READ, 
			0, 
			NULL,
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

    	if(h == INVALID_HANDLE_VALUE) 
		{
	       	printf("Couldn't access PortTalk Driver, Please ensure driver is loaded.\n\n");
        	return -1;
    		}

	Error = DeviceIoControl(
                          	h,
                          	0x10,   // RTL Fill Memory,
	                        NULL,
        	                0,	// Send Zero Bytes
                	        NULL,
                        	0,
                          	&BytesReturned,
                          	NULL
                         	);

	if (!Error) printf("Error Occured talking to Device Driver %d\n",GetLastError());
	
	// Process Auguments	
	for (count = 1; count < argc; count++)

           { // If argu starts with '0x'
            if (argv[count][0] == '0' & argv[count][1] =='x') 
		{
		sscanf(argv[count],"%x", &value);
		offset = value / 8;
		
		Error = DeviceIoControl(
                          		h,
                          		0x18,   // IoControlCode,
  	                        	&offset,
        	                  	3,	// Send Zero Bytes
                	          	NULL,
                        	  	0,
                          		&BytesReturned,
                          		NULL
                         		);

  		if (!Error) printf("Error %d granting access to Address 0x%03X\n",GetLastError(),value);
		else        printf("Address 0x%03X (IOPM Offset 0x%02X) has been granted access.\n",value,offset);

		}
            else if (argv[count][0] == '/' & argv[count][1] =='a')
		{
		 // Set Entire IOPM
		printf("Granting exclusive access to all I/O Ports\n");
		Error = DeviceIoControl(
                          		h,
                          		0x14,   // IoControlCode,
  	                        	NULL,
        	                  	0,	// Send Zero Bytes
                	          	NULL,
                        	  	0,
                          		&BytesReturned,
                          		NULL
                         		);

		}
	    else
		// Filename
            	{
	     	 if (strlen(filename) + strlen(argv[count]) > 80)
		 	{ 
                 	printf("Command line exceeds 80 Characters\n");
		 	return 0;
			}
	     	 strcat(filename,argv[count]);
	     	 strcat(filename," ");
            	}
	   }

	// Start Executable
        printf("Executing %swith a ",filename);

	ZeroMemory( &si, sizeof(si) ); 	// Zero Startup Info
    	si.cb = sizeof(si);		// Set Size

	if( !CreateProcess( NULL, 	// No module name (use command line). 
        		    filename, 	// Command line. 
        		    NULL,      	// Process handle not inheritable. 
        		    NULL,       // Thread handle not inheritable. 
        		    FALSE,      // Set handle inheritance to FALSE. 
        		    0,          // No creation flags. 
        		    NULL,       // Use parent's environment block. 
        		    NULL,       // Use parent's starting directory. 
        		    &si,        // Pointer to STARTUPINFO structure.
       	 		    &pi )       // Pointer to PROCESS_INFORMATION structure.
    		       ) printf("Error in CreateProcess\n\n");    

	printf("ProcessID of %d\n",pi.dwProcessId);

	Error = DeviceIoControl(
                          	h,
                          	IoControlCode,
                          	&pi.dwProcessId,
                          	InputBufferSize,
                          	NULL,
                          	0,
                          	&BytesReturned,
                          	NULL
                         	);

  	if (!Error) printf("Error Occured talking to Device Driver %d\n",GetLastError());
	else        printf("PortTalk Device Driver has set IOPM for ProcessID %d.\n",pi.dwProcessId);

	CloseHandle(h);

	return 0;

}

