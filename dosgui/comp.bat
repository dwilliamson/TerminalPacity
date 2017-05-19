@echo off
echo NOTE: You are not compiling "DOS.ASM"!
echo NOTE: You are not compiling "DOSG.C"!
del pacsetup.exe
rem wcc dosg -ml -zq
wcc test -ml -zq
rem wasm dos -zq -ml
wlink @dos.lk1 op q
atf dos.exe extract.lzh e
copy tempatf.exe dos.exe
del tempatf.exe
ren dos.exe pacsetup.exe
