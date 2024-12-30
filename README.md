# LiteXDR
## Purpose
This is a demo for an event detection system on Windows that detects when executable programs are loaded 
and sends the information to a main server that keeps track of known/common processes.

The system could have many clients reporting to the server, so an important optimization in this project
is that only hashes of processes and their information are communicated between the client and server. If 
a process loaded is uncommon or has never been detected before, the server inquires the client for more information about it.
In the majority of cases only hashes are sent to the server, so the running cost is kept low. 

## How to use:
This project includes a server, a client program, and a client driver.
You can run the client in a virtual machine to try the system. This was tested on Windows 10.

### Client
**Step 1.** Set up a virtual machine with Windows 10

**Step 2.** Officially signing the Windows driver is currently not in the project's scope, so 
in the command line, you need to run `bcdedit /set testsigning on` and restart to enable Test Mode on windows.

**Step 3.** Installing the driver: `Devcon.exe` is a tool included in WDK, so you can either copy the `devcon.exe` file from your
copy in `WindowsKits/10/Tools/10..../x64/devcon.exe` or use this installer https://github.com/Drawbackz/DevCon-Installer

**Step 4.** Build the project in Visual Studio, transfer the entire Debug/Release folder to the virtual machine. Then, in
command line, run
```
cd KMDF XDR Driver
devcon install KMDFXDRDriver.inf Root/KMDFXDRDriver
cd ../
```
Now the driver is installed, you can find it in the Windows Device Manager.

**Step 5.** Run the client, `DriverController.exe`

### Server
```
cd xdrserver
go run main.go
```

## How it works
### Driver:
The kernel driver registers a callback routine called PsSetLoadImageNotifyRoutine. Callback routines are functions that are called by the kernel when specific events occur. Whenever this happens, the Windows system calls the driver’s custom function for this type of event. This specific callback notifies when an image (DLL or EXE) is mapped into memory, but is invoked before the entrypoint of the image is called.

Using this kernel level callback function, we can report on any process that starts even if it only lasts for a fraction of a second. Potentially, research could be done to attempt to stop known malicious processes before they even run. The driver reports every process that starts and keeps it in a buffer inside kernel memory. When requested by the user-mode program, the driver transfers the buffer using METHOD_BUFFERED. 

METHOD_BUFFERED creates a system buffer and copies from that buffer into the user-mode buffer. The buffer contains a list of all processes that happened with information about them. This information is marshalled with headers and content sections.

### User-Mode Program:
On regular intervals, the user program (client) makes an IRP (IO Request Packet) to receive the buffer of processes that have started since the last call and reads all processes that happened in the buffer. The program then hashes every process with its information and sends it to the server.

The server will respond only if a process is "suspicious" and requests full information about it from the client program.

### Server:
The server keeps a database of known processes and their information. Client programs send hashes of ProcessData to minimize the amount of data sent to the server. Every time a hash gets sent it increases the count of the process in the hashmap, so common processes that happen frequently on every computer have a high count in the map.

If the hash of a process is discovered for the first time, or its count is low in the map, the server requests the user program to send full information about the process in the http response.

## TODO List
- Make easy build scripts for client and server programs
- Make full client program installer
- Prevent unknown processes from running, with an allow or deny list in the kernel driver
- Run more large scale testing
- Pay Microsoft a trillion dollars to sign the driver
