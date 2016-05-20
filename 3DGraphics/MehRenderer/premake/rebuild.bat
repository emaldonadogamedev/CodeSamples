@echo off
.\premake4.exe clean
pause

@echo on

echo CLEANING DONE...NOW RE-BUILDING PROJECT

.\premake4.exe vs2013

echo RE-BUILD PROJECT
pause