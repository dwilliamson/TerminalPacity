@echo ==================
@echo Compiling PACMAN.C
@echo ==================
@echo off
wcc pacman1.c -i%include% -w4 -e25 -zq -otexan -d2 -bt=dos -ml -3
wlink d all SYS dos op m op maxe=25 op q op symf @pacman.lk1
