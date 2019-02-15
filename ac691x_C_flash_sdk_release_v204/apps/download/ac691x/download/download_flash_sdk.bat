cd  %~dp0

set EXE_NAME=ac691x
set OBJDUMP=C:\JL\pi32\bin\llvm-objdump
   
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy

set BANKZIP=C:\JL\pi32\bin\bankzip

cd ../post_build/flash

call download.bat %EXE_NAME% %OBJDUMP% %OBJCOPY% %BANKZIP%



pause
