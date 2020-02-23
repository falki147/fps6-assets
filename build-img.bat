@echo off
setlocal enabledelayedexpansion

if exist "build/fps6-builder.exe" (
	set builder="build/fps6-builder.exe"
) else (
	set builder=fps6-builder
)

set files=
for /r assets %%i In (*) DO set files=!files! "%%i"
%builder% %files%
echo Done
