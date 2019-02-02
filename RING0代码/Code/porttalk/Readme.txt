
                Beyond Logic Port Talk I/O Port Driver
                      http://www.beyondlogic.org

The PortTalk driver combined with allowIO.exe, grants certain programs 
exclusive access to IO Ports on a Windows NT / Windows 2000 system.


Porttalk Driver Installation.
_____________________________

Copy porttalk.sys to winnt/system32/drivers and then double click 
porttalk.reg to enter the required keys into your registry. This 
will load the driver automatically at next reboot.

For the more power hungry users, try the Windows NT/2000 driver 
install utility at http://www.beyondlogic.org/dddtools/dddtools.htm
This tool can be used to install, start, stop and remove the driver 
at run time without needing to reboot your machine.


Allowio - Grants programs access to IO Ports.
_____________________________________________

Usage : AllowIO <executable.exe> <Hex Addresse(s)> <Switches>

eg. Using allowio to grant access to IO ports 0x42, 0x43 and 0x61

 C:\porttalk\AllowIO>allowio 0x42 0x43 0x61 beep.exe
 Beyond Logic AllowIO.EXE
 Address 0x042 (IOPM Offset 0x08) has been granted access.
 Address 0x043 (IOPM Offset 0x08) has been granted access.
 Address 0x061 (IOPM Offset 0x0C) has been granted access.
 Executing beep.exe with a ProcessID of 1096
 PortTalk Device Driver has set IOPM for ProcessID 1096.

This is more secure than allowing all programs and processes access
to all IO ports.

If you don't know what addresses a certain program uses, you can
grant access to all ports.

 C:\porttalk>allowio beep /a
 BeyondLogic AllowIO
 Granting exclusive access to all I/O Ports
 Executing beep with a ProcessID of 524
 PortTalk Device Driver has set IOPM for ProcessID 524.

This is less secure. For example the program can talk to COM1 
and lock up your mouse.


Source Code Included
____________________

This driver demonstrates the use of the following undocumented 
windows NT calls,
PsLookupProcessByProcessId()
Ke386SetIoAccessMap();
Ke386QueryIoAccessMap();
Ke386IoSetAccessProcess();


Craig Peacock
11th June 2000

