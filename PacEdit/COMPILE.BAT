@echo ===================
@echo Compiling PACEDIT.C
@echo ===================
@echo off
wcc pacedit1.c -i%include% -w4 -e25 -zq -otexan -d2 -bt=dos -mc
wlink d all SYS dos op m op maxe=25 op q op symf @pacedit.lk1
