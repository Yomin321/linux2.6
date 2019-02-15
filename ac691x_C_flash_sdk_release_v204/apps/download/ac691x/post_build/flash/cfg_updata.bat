copy ac691x.cfg BT_CFG.bin

bfumake.exe -fi BT_CFG.bin -ld 0x0000 -rd 0x0000 -fo updata.bfu

del BT_CFG.bin

REM cp jl_690x.bfu W:\