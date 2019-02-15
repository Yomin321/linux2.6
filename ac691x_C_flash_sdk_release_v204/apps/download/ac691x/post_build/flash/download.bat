@echo off

@echo ********************************************************************************
@echo 			BR20 FLASH SDK download...			
@echo ********************************************************************************
@echo %date%

cd  %~dp0

if exist %1.bin del %1.bin 
if exist %1.lst del %1.lst 

REM  echo %1
REM  echo %2
REM  echo %3

REM %2 -disassemble %1.exe > %1.lst 
%3 -O binary -j .text  %1.exe  %1.bin 
%3 -O binary -j .data  %1.exe  data.bin 
%3 -O binary -j .nvdata %1.exe  nvdata.bin 
%2 -section-headers  %1.exe 

REM û��ѹ��
REM copy %1.bin/b + data.bin/b + nvdata.bin/b sdk.app

REM codeѹ��
%4 -rom %1.bin -o compressed.bin -elf %1.exe
copy %1.bin/b + compressed.bin/b sdk.app

REM if exist %1.exe del %1.exe
del %1.bin data.bin nvdata.bin

if exist uboot.boot del uboot.boot
type uboot.bin > uboot.boot

cd tone_resource
copy *.mp3 ..\
cd ..

isd_download.exe -tonorflash -dev br20 -boot 0x2000 -div6 -wait 300  -f uboot.boot sdk.app ac691x.cfg bt.mp3 music.mp3  linein.mp3 radio.mp3 pc.mp3  connect.mp3 disconnect.mp3 ring.mp3 power_off.mp3 0.mp3 1.mp3 2.mp3 3.mp3 4.mp3 5.mp3 6.mp3 7.mp3 8.mp3 9.mp3 -maskrom ..\libs_stubs.ld 

IF EXIST jl_691x.bin del jl_691x.bin
IF EXIST jl_691x.bfu del jl_691x.bfu
rename jl_isd.bin jl_691x.bin
bfumake.exe -fi jl_691x.bin -ld 0x0000 -rd 0x0000 -fo updata.bfu


if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.sty del *.sty

IF EXIST no_isd_file del jl_670x.bin jl_cfg.bin
del no_isd_file


@rem format vm        //����VM 68K����
@rem format cfg       //����BT CFG 4K����
@rem format 0x3f0-2  //��ʾ�ӵ� 0x3f0 �� sector ��ʼ�������� 2 �� sector(��һ������Ϊ16���ƻ�10���ƶ��ɣ��ڶ�������������10����)



