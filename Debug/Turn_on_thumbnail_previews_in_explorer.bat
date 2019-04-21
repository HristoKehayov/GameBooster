@echo off

:: Created by: Shawn Brink
:: http://www.tenforums.com
:: Tutorial: http://www.tenforums.com/tutorials/18834-thumbnail-previews-file-explorer-enable-disable-windows-10-a.html


REG ADD "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced" /V IconsOnly /T REG_DWORD /D 0 /F

taskkill /f /im explorer.exe
start explorer.exe