@echo off
wcc ndosg -ml -zq
wcc test -ml -zq
rem wasm dos -zq -ml
wlink name test file ndosg,dos,test op q
