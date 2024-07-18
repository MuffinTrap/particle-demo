
:: This file is a template to copy the needed .dll files to the build folder
:: Copy this file to the build folder

:: Copy from here
SET src=C:\msys64\ucrt64\bin
:: and here: This is the github repo folder. eg. C:\Dev\Project\particle-demo
SET repo=D:\Dev\Wii\Projects\particle-demo

:: To this folder
SET dest=.

xcopy %src%\libgcc_s_seh-1.dll %dest% /y
xcopy %src%\libfreeglut.dll %dest% /y
xcopy %src%\libopenal-1.dll %dest% /y
xcopy %src%\libpng16-16.dll %dest% /y
xcopy %src%\libsndfile-1.dll %dest% /y
xcopy %src%\libstdc++-6.dll %dest% /y
xcopy %src%\libwinpthread-1.dll %dest% /y
xcopy %src%\zlib1.dll %dest% /y
xcopy %src%\libFLAC.dll %dest% /y
xcopy %src%\libmpg123-0.dll %dest% /y
xcopy %src%\libmp3lame-0.dll %dest% /y
xcopy %src%\libogg-0.dll %dest% /y
xcopy %src%\libvorbis-0.dll %dest% /y
xcopy %src%\libopus-0.dll %dest% /y
xcopy %src%\libvorbisenc-2.dll %dest% /y

:: Copy exe
xcopy %repo%\gldemo.exe %dest% /y 
:: Copy all sd_card files
xcopy %repo%\sd_card\ %dest% /s /e /y 
:: Copy music
xcopy %repo%\*.wav %dest% /y 
