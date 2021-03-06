// GameBooster.cpp : Defines the entry point for the console application.
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
#include <winreg.h>

#include <stdio.h>
#pragma comment(lib, "user32.lib")  


//#include "nvapi.h"
//#include "NvApiDriverSettings.h"

using namespace std;

LONG DeleteDirectoryAndAllSubfolders(LPCWSTR wzDirectory)
{
	//DeleteDirectoryAndAllSubfolders(L"C:\\Users\\Hristo-PC\\AppData\\Local\\Temp"); // how to use this method just in case!!!!
	WCHAR szDir[MAX_PATH + 1];  // +1 for the double null terminate
	SHFILEOPSTRUCTW fos = { 0 };

	StringCchCopy(szDir, MAX_PATH, wzDirectory);
	int len = lstrlenW(szDir);
	szDir[len + 1] = 0; // double null terminate for SHFileOperation

						// delete the folder and everything inside
	fos.wFunc = FO_DELETE;
	fos.pFrom = szDir;
	fos.fFlags = FOF_NO_UI;
	return SHFileOperation(&fos);
}

bool is_file_exist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}


HKEY OpenRegistryKey(HKEY hRootKey, const wchar_t* strSubKey)
{
	HKEY hKey;
	LONG lError = RegOpenKeyEx(hRootKey, strSubKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (ERROR_FILE_NOT_FOUND == lError)
	{
		//Create a key
		lError = RegCreateKeyEx(hRootKey, strSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (lError != ERROR_SUCCESS)
	{
		std::cout << "\nSomething is wrong. Could not Open or Create the Key";
	}

	return hKey;
}

void SetRegistryValues(HKEY hRootKey, LPCTSTR lpVal, DWORD data)
{
	LONG nErr = RegSetValueEx(hRootKey, lpVal, NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));
	if (nErr != ERROR_SUCCESS)
	{
		std::cout << "\nNot able to set the Registry Value.";
	}
}

DWORD GetValueFromRegistry(HKEY hRootKey, LPCTSTR lpValue)
{
	DWORD data = 0;
	DWORD dtype = REG_DWORD;
	DWORD dSize = sizeof(data);
	LONG lErr = RegQueryValueEx(hRootKey, lpValue, NULL, &dtype, (LPBYTE)&data, &dSize);

	if (ERROR_SUCCESS != lErr)
	{
		std::cout << "\nCould Not find any Value.";
	}

	return data;
}

void mouseLeftClick()
{
	INPUT Input = { 0 };

	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}
void mouseMoveTo(int toX, int toY)
{
	// get system information regarding screen size / resolution
	double screenRes_width = ::GetSystemMetrics(SM_CXSCREEN) - 1;
	double screenRes_height = ::GetSystemMetrics(SM_CYSCREEN) - 1;
	// scale the function inputs 'toX and 'toY' appropriately by a
	// factor of 65535
	double dx = toX * (65535.0f / screenRes_width);
	double dy = toY * (65535.0f / screenRes_height);
	// we now have variables 'dx' and 'dy' with the user-desired
	// coordinates in a form that the SendInput function can understand

	// set up INPUT Input, assign values, and move the cursor
	INPUT Input = { 0 };
	// we want to be sending a MOUSE-type input
	Input.type = INPUT_MOUSE;
	// the mouse input is of the 'MOVE ABSOLUTE' variety
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx = LONG(dx);
	Input.mi.dy = LONG(dy);
	// we have given information about the magnitude of the new
	// cursor position; here we will send it
	::SendInput(1, &Input, sizeof(INPUT));
}

void SetHighPerformanceMode()
{
	SystemParametersInfo(SPI_SETCLIENTAREAANIMATION, FALSE, NULL, SPIF_UPDATEINIFILE); //  Animate controls and elements inside windows
	SystemParametersInfo(SPI_SETANIMATION, FALSE, NULL, SPIF_UPDATEINIFILE); //  Animate windows when minimizing and maximizing
	SystemParametersInfo(SPI_SETMENUANIMATION, FALSE, NULL, SPIF_UPDATEINIFILE); // Fade or slide menus into view
	SystemParametersInfo(SPI_SETCURSORSHADOW, FALSE, NULL, SPIF_UPDATEINIFILE); // Show shadows under mouse pointer       
	SystemParametersInfo(SPI_SETDROPSHADOW, FALSE, NULL, SPIF_UPDATEINIFILE); // Show shadows under windows
	SystemParametersInfo(SPI_SETCOMBOBOXANIMATION, FALSE, NULL, SPIF_UPDATEINIFILE); // Slide open combo boxes
	SystemParametersInfo(SPI_SETCLEARTYPE, FALSE, NULL, SPIF_UPDATEINIFILE); // Smooth edges of screen fonts
	SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, FALSE, NULL, SPIF_UPDATEINIFILE); // Smooth-scroll list boxes
	SystemParametersInfo(SPI_SETUIEFFECTS, FALSE, NULL, SPIF_UPDATEINIFILE); // A whole bunch of settings
	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, SPIF_UPDATEINIFILE); // Show window content while dragging
	SystemParametersInfo(SPI_SETFONTSMOOTHING, FALSE, NULL, SPIF_UPDATEINIFILE); // Disables Smooth edges of screen fonts
	
}

void SetNormalMode()
{
	SystemParametersInfo(SPI_SETCLIENTAREAANIMATION, TRUE, NULL, SPIF_UPDATEINIFILE); //  Animate controls and elements inside windows
	SystemParametersInfo(SPI_SETANIMATION, TRUE, NULL, SPIF_UPDATEINIFILE); //  Animate windows when minimizing and maximizing
	SystemParametersInfo(SPI_SETMENUANIMATION, TRUE, NULL, SPIF_UPDATEINIFILE); // Fade or slide menus into view    
	SystemParametersInfo(SPI_SETCURSORSHADOW, TRUE, NULL, SPIF_UPDATEINIFILE); // Show shadows under mouse pointer
	SystemParametersInfo(SPI_SETDROPSHADOW, TRUE, NULL, SPIF_UPDATEINIFILE); // Show shadows under windows
	SystemParametersInfo(SPI_SETCOMBOBOXANIMATION, TRUE, NULL, SPIF_UPDATEINIFILE); // Slide open combo boxes
	SystemParametersInfo(SPI_SETCLEARTYPE, TRUE, NULL, SPIF_UPDATEINIFILE); // Smooth edges of screen fonts
	SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, TRUE, NULL, SPIF_UPDATEINIFILE); // Smooth-scroll list boxes 
	SystemParametersInfo(SPI_SETUIEFFECTS, TRUE, NULL, SPIF_UPDATEINIFILE); // A whole bunch of settings
	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, NULL, SPIF_UPDATEINIFILE); // Show window content while dragging
	SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, NULL, SPIF_UPDATEINIFILE); // Disables Smooth edges of screen fonts
}
//string::find_last_of
#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>         // std::size_t



std::string getexepath()
{
	char result[MAX_PATH];
	std::string(result, GetModuleFileNameA(NULL, result, MAX_PATH));

	const std::string& str = "";

	std::size_t found = str.find_last_of("/\\");
	std::cout << str.substr(0, found) << '\n';
	
	return result;
}



int main(int argc, char* argv[])
{
	int choiceOfmenu = 0;
	WORD mouseX = 10;// x coord of mouse
	WORD mouseY = 10;// y coord of mouse

	string Check_If_Boosted_String = "PC Boosted for Game Performance!";
	string Check_If_NormalStage_String = "PC Reversed Back to Normal stage!";
	string GameBoost_Info_File_String = ""; // string for the text containing in the txt file

	ofstream gameboost_info_txt;

	std::string command = "del /Q ";
	std::string delete_path1 = "C:\\Users\\Hristo-PC\\AppData\\Local\\Temp\\*";
	std::string delete_path2 = "C:\\Windows\\Temp\\*";

	DWORD visual_effects_registry_value;

	DWORD TaskbarAnimationsV;
	DWORD ListviewAlphaSelectV;
	DWORD ListviewShadowV;
	DWORD ThemeActiveV;

	do
	{
		HKEY hKey = OpenRegistryKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VisualEffects");

		HKEY TaskbarAnimations = OpenRegistryKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
		HKEY ListviewAlphaSelect = OpenRegistryKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
		HKEY ListviewShadow = OpenRegistryKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
		HKEY ThemeActive = OpenRegistryKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager");
		
		visual_effects_registry_value = GetValueFromRegistry(hKey, L"VisualFXSetting");

		TaskbarAnimationsV = GetValueFromRegistry(TaskbarAnimations, L"TaskbarAnimations");
		ListviewAlphaSelectV = GetValueFromRegistry(ListviewAlphaSelect, L"ListviewAlphaSelect");
		ListviewShadowV = GetValueFromRegistry(ListviewShadow, L"ListviewShadow");
		ThemeActiveV = GetValueFromRegistry(ThemeActive, L"ThemeActive");
		
		if (!is_file_exist("GameBoost_Info.txt")) // create txt file if not created
		{
			gameboost_info_txt.open("GameBoost_Info.txt");
			gameboost_info_txt.close();
		}
		else
		{
			ifstream myReadFile; // open txt file
			myReadFile.open("GameBoost_Info.txt");
			if (myReadFile.is_open())
			{
				while (getline(myReadFile, GameBoost_Info_File_String)) // To get you all the lines.
				{
					//getline(myReadFile, GameBoost_Info_File_String); // Saves the line in STRING.
					myReadFile >> GameBoost_Info_File_String;
				}
				myReadFile.close();
			}
			else
			{
				cout << "Unable to open and read file";
			}
		}

		cout
			<< "\n1 - Boost PC for better Performence and higher fps\n"
			<< "2 - Revert PC back to its normal state\n"
			<< "3 - Exit\n"
			<< "Please enter number in rage of 1 to 3:";
		cin >> choiceOfmenu;

		switch (choiceOfmenu)
		{
		default:
			break;

			 ////////////////////////////////
			//----------BOOST PC----------//
		   ////////////////////////////////
		case 1: 
			if (GameBoost_Info_File_String == "" || GameBoost_Info_File_String == Check_If_NormalStage_String)
			{


				/*01*/ PowerSetActiveScheme(0, &GUID_MIN_POWER_SAVINGS); // change battery to high performance

				///*02*/ system(command.append(delete_path1).c_str()); // delete all files from temp directory
				///*02*/ system(command.append(delete_path2).c_str()); // delete all files from temp directory
				
				/*2*/ system("U:\\Programming-Stuffs\\C++\\Projects\\GameBooster\\x64\\Debug\\tempdeleter.bat");
				/*2*/ system("U:\\Programming-Stuffs\\C++\\Projects\\GameBooster\\x64\\Debug\\stop_services.bat"); 

				system("pause");
				
				/*03*/ visual_effects_registry_value = 3;
				/*03*/ SetRegistryValues(hKey, L"VisualFXSetting", visual_effects_registry_value); // set the value to regestry

				/*03*/ TaskbarAnimationsV = 0;
				/*03*/ ListviewAlphaSelectV = 0;
				/*03*/ ListviewShadowV = 0;
				/*03*/ ThemeActiveV = 0;

				/*03*/ SetRegistryValues(TaskbarAnimations, L"TaskbarAnimations", TaskbarAnimationsV); // set the value to regestry
				/*03*/ SetRegistryValues(ListviewAlphaSelect, L"ListviewAlphaSelect", ListviewAlphaSelectV); // set the value to regestry
				/*03*/ SetRegistryValues(ListviewShadow, L"ListviewShadow", ListviewShadowV); // set the value to regestry
				/*03*/ SetRegistryValues(ThemeActive, L"ThemeActive", ThemeActiveV); // set the value to regestry

				/*03*/ //RegFlushKey(HKEY_CURRENT_USER); // update registry to take effect of visual effects performance
				/*03*/ RegCloseKey(hKey);

				/*03*/ RegCloseKey(TaskbarAnimations);
				/*03*/ RegCloseKey(ListviewAlphaSelect);
				/*03*/ RegCloseKey(ListviewShadow);
				/*03*/ RegCloseKey(ThemeActive);

				SetHighPerformanceMode();
				//system("U:\\Programming-Stuffs\\C++\\Projects\\GameBooster\\Debug\\Turn_off_thumbnail_previews_in_explorer.bat");
				/*04*/ //EnableOrDisableService(1, argv[1]);
				/*04*/ //EnableOrDisableService("SysMain", 1);

				

				gameboost_info_txt.open("GameBoost_Info.txt");

				gameboost_info_txt << "PC Boosted for Game Performance!" << endl;

				gameboost_info_txt.close();
				cout << "\nPC Boosted for Game Performance!" << endl;
			}
			else if(GameBoost_Info_File_String == "PC Boosted for Game Performance!")
			{
				cout << "\nPC is already boosted! There is no need!" << endl;
			}
			break;

			 /////////////////////////////////////////////////
			//----------REVERT PC TO NORMAL STATE----------//
		   /////////////////////////////////////////////////
		case 2: 
			if (GameBoost_Info_File_String == "" || GameBoost_Info_File_String == Check_If_Boosted_String)
			{
				/*01*/ PowerSetActiveScheme(0, &GUID_TYPICAL_POWER_SAVINGS); // change battery to balanced 

				/*2*/system("U:\\Programming-Stuffs\\C++\\Projects\\GameBooster\\x64\\Debug\\start_services.bat");
				/*02*/ visual_effects_registry_value = 0;
				/*02*/ SetRegistryValues(hKey, L"VisualFXSetting", visual_effects_registry_value);
				/*02*/ //RegFlushKey(HKEY_CURRENT_USER); // update registry to take effect of visual effects performance
				/*02*/ RegCloseKey(hKey);

				/*03*/ TaskbarAnimationsV = 1;
				/*03*/ ListviewAlphaSelectV = 1;
				/*03*/ ListviewShadowV = 1;
				/*03*/ ThemeActiveV = 1;

				/*03*/ SetRegistryValues(TaskbarAnimations, L"TaskbarAnimations", TaskbarAnimationsV); // set the value to regestry
				/*03*/ SetRegistryValues(ListviewAlphaSelect, L"ListviewAlphaSelect", ListviewAlphaSelectV); // set the value to regestry
				/*03*/ SetRegistryValues(ListviewShadow, L"ListviewShadow", ListviewShadowV); // set the value to regestry
				/*03*/ SetRegistryValues(ThemeActive, L"ThemeActive", ThemeActiveV); // set the value to regestry

				/*03*/ RegCloseKey(TaskbarAnimations);
				/*03*/ RegCloseKey(ListviewAlphaSelect);
				/*03*/ RegCloseKey(ListviewShadow);
				/*03*/ RegCloseKey(ThemeActive);

				SetNormalMode();
				//system("U:\\Programming-Stuffs\\C++\\Projects\\GameBooster\\Debug\\Turn_on_thumbnail_previews_in_explorer.bat");

				/*03*/ //EnableOrDisableService("WSearch", true);
				/*03*/ //EnableOrDisableService("SysMain", true);


				gameboost_info_txt.open("GameBoost_Info.txt");

				gameboost_info_txt << "PC Reversed Back to Normal stage!" << endl;

				gameboost_info_txt.close();
				cout << "\nPC Reversed Back to Normal stage!" << endl;
			}
			else if(GameBoost_Info_File_String == "PC Reversed Back to Normal stage!")
			{
				cout << "\nPC is already in Normal stage! There is no need!" << endl;
			}
			break;

			 /////////////////////////////////////////////
			//----------EXIT FROM APPLICATION----------//
		   /////////////////////////////////////////////
		case 3:
			exit(0);
			break;
		}		
	} while (choiceOfmenu != 3);


    return 0;
}
