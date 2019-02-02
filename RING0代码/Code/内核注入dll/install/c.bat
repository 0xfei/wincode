@echo off
copy ..\drv\objfre_win7_amd64\amd64\inject.sys amd64\inject.sys
copy ..\drv\objfre_win7_x86\i386\inject.sys i386\inject.sys
copy ..\dll\objfre_win7_amd64\amd64\inject.dll amd64\inject.dll
copy ..\dll\objfre_win7_x86\i386\inject.dll i386\inject.dll
copy ..\thunk\objfre_win7_amd64\amd64\thunk.dll amd64\thunk.dll
copy ..\thunk\objfre_win7_x86\i386\thunk.dll i386\thunk.dll
pause
