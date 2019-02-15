#!bin/bash

if [ -f $1.bin ]; then rm $1.bin; fi
if [ -f $1.lst ]; then rm $1.lst; fi

${OBJDUMP} -disassemble $1.exe > $1.lst
${OBJCOPY} -O binary -j .text  $1.exe  $1.bin
${OBJCOPY} -O binary -j .data  $1.exe  data.bin
${OBJCOPY} -O binary -j .nvdata $1.exe  nvdata.bin
${OBJDUMP} -section-headers  $1.exe
# ${OBJDUMP} -t $1.or32 > $1.symbol.txt

if [ $QLZ_TYPE = "OTP_DICT" ] ;then
	echo qlz_type:OTP_DICT
	
#OTP_Dict
${BANKZIP} -rom $1.bin -o compressed.bin -elf $1.exe

#Maskrom_Dict
#	br20=`svn info --show-item wc-root`
#	${BANKZIP} -rom $br20/tags/maskrom/maskrom_ver_A/libs/post_build/br20/rom.image -o \
#							compressed.bin -elf $1.exe

	cat $1.bin  compressed.bin > sdk.app 

else
	cat $1.bin data.bin nvdata.bin > sdk.app 
fi
# host-client -project sdk_br16 -f uboot.boot sdram.app
	echo ${NICKNAME}_${CODE_SPACE}
host-client -project ${NICKNAME}_${CODE_SPACE} -f uboot.bin sdk.app ${PLATFORM}loader.bin

