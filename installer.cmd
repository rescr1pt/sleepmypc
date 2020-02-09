set SLEEPMYPC_SRC_EXE=msvc/x64/Release/sleepmypc.exe
set SLEEPMYPC_ICO=src/sleepmypc.ico
set SLEEPMYPC_COPYING=COPYING
set SLEEPMYPC_README=README

"C:\Program Files (x86)\Inno Setup 6\iscc.exe" /O"sleepmypc" installer.script
pause