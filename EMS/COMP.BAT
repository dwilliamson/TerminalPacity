@echo ===============
@echo Compiling EMS.C
@echo ===============
@echo off
wcc ems.c -i%include% -w4 -e25 -zq -bt=dos -ml -3 -s
wlink d all SYS dos op maxe=25 op q op symf @ems.lk1
