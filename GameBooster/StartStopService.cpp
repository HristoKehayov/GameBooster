// StartStopService.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <powrprof.h> // library for power management
#include <iostream>
#include <fstream>
#include <string>
#include <atlstr.h>

#pragma comment(lib, "powrprof.lib")
#include <winuser.h>
#include <sys/stat.h>

#include <filesystem>
#include <experimental/filesystem> 
#include <strsafe.h>


int _tmain(int argc, _TCHAR* argv[])
{
	SC_HANDLE serviceDbHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE serviceHandle = OpenService(serviceDbHandle, argv[1], SC_MANAGER_ALL_ACCESS);

	SERVICE_STATUS_PROCESS status;
	DWORD bytesNeeded;
	QueryServiceStatusEx(serviceHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded);

	if (status.dwCurrentState == SERVICE_RUNNING)
	{// Stop it
		BOOL b = ControlService(serviceHandle, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&status);
		if (b)
		{
			std::cout << "Service stopped." << std::endl;
		}
		else { std::cout << "Service failed to stop." << std::endl; }
	}
	else {
		/*Start it*/ BOOL b = StartService(serviceHandle, NULL, NULL); 
		if (b) 
		{ 
			std::cout << "Service started." << std::endl; 
		} else { std::cout << "Service failed to start." << std::endl; } 
	}
	CloseServiceHandle(serviceHandle); 
	CloseServiceHandle(serviceDbHandle);
	return 0; 
}