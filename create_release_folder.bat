@echo off
mkdir for_release
robocopy "Win32 Release/" for_release/ Obj2Geom.exe
robocopy ./ for_release/ collision_flags-default.txt
cd for_release
rename collision_flags-default.txt collision_flags.txt