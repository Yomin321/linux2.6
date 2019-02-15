


CC_ARG = \
-DNOFLOAT \
-DHAVE_MALLOC \

ifeq ($(CODE_SPACE),flash)
CC_ARG += \
-DFLASH_MODE
endif


INCLUDES = \
-I../include_lib \
-I../include_lib/cpu \
-I../include_lib/cpu/$(RELEASE) \
-I../include_lib/power_manage \
-I../include_lib/mcpwm \
-I../include_lib/rtc \
-I../include_lib/common \
-I../include_lib/dev_mg \
-I../include_lib/sdmmc \
-I../include_lib/aec \
-I../include_lib/dec \
-I../include_lib/vm \
-I../include_lib/fs \
-Iinclude/ac691x_cfg \
-Iinclude/bluetooth \
-Iinclude \
-Iinclude/common \
-Iinclude/common/music_player \
-Iinclude/common/file_operate \
-Iinclude/common/fs \
-Iinclude/cpu \
-Iinclude/cpu/$(RELEASE) \
-Iinclude/cpu/$(RELEASE)/audio \
-Iinclude/cpu/$(RELEASE)/power \
-Iinclude/cpu/$(RELEASE)/key_drv \
-Iinclude/cpu/$(RELEASE)/led \
-Iinclude/cpu/$(RELEASE)/nv_mem \
-Iinclude/cpu/$(RELEASE)/iic \
-Itask_manager \
-Itask_manager/task_bt \
-Itask_manager/task_idle \
-Itask_manager/task_music \
-Iinclude/cpu/ac691x_cfg \

SRCS_C = \
btstack/trunk/fcc_test.c \
btstack/trunk/bt_trunk_main.c \
btstack/trunk/aec_main.c \
btstack/trunk/audio_improve_main.c \
btstack/trunk/a2dp_sync.c \
btstack/user/bt_configs_user.c \
btstack/user/aec_user.c \
btstack/user/a2dp_decode.c \
cpu/$(RELEASE)/clock.c \
cpu/$(RELEASE)/board_init.c \
cpu/$(RELEASE)/wdt.c \
cpu/$(RELEASE)/irq.c \
cpu/$(RELEASE)/timer.c \
cpu/$(RELEASE)/updata.c \
cpu/$(RELEASE)/rotate_dec.c \
cpu/$(RELEASE)/key_drv/key.c \
cpu/$(RELEASE)/key_drv/key_drv_io.c \
cpu/$(RELEASE)/key_drv/key_drv_ad_rtcvdd.c \
cpu/$(RELEASE)/key_drv/key_drv_ad_vddio.c \
cpu/$(RELEASE)/key_drv/key_drv_ir.c \
cpu/$(RELEASE)/key_drv/key_drv_tch.c \
cpu/$(RELEASE)/key_drv/key_drv_uart.c \
cpu/$(RELEASE)/board/board.c \
cpu/$(RELEASE)/uart/uart.c \
cpu/$(RELEASE)/power/power.c \
cpu/$(RELEASE)/power/charge.c \
cpu/$(RELEASE)/adc_api.c \
cpu/$(RELEASE)/audio/dac.c \
cpu/$(RELEASE)/audio/ladc.c \
cpu/$(RELEASE)/audio/tone.c \
cpu/$(RELEASE)/audio/eq.c \
cpu/$(RELEASE)/audio/src.c \
cpu/$(RELEASE)/audio/sin_tab.c \
cpu/$(RELEASE)/nv_mem/nv_mem.c \
cpu/$(RELEASE)/led/led.c \
cpu/$(RELEASE)/iic/iic_hw.c \
cpu/$(RELEASE)/iic/iic.c \
cpu/$(RELEASE)/memory_api.c \
common/common.c \
common/msg.c \
common/file_operate/file_operate.c \
common/file_operate/file_io.c \
common/fs/fs.c \
common/dev_manage.c \
common/compress.c \
common/flash_api.c \
common/sys_cfg.c \
common/music_player/music_decoder.c \
common/music_player/music_player.c \
common/music_player/music_decoder_config.c \
common/music_player/warning_tone.c \
task_manager/task_common.c \
task_manager/task_manager.c \
task_manager/task_bt/task_bt.c \
task_manager/task_bt/task_bt_key.c \
task_manager/task_idle/task_idle.c \
task_manager/task_idle/task_idle_key.c \
task_manager/task_music/task_music.c \
task_manager/task_music/task_music_key.c \




SRCS_LS = \


SRCS_BS = \
cpu/$(RELEASE)/version.S\

LIBS = \
--start-group  \
../include_lib/bluetooth_lib$(SUFFIX).a  \
../include_lib/rtc_lib$(SUFFIX).a  \
../include_lib/power_manage_lib$(SUFFIX).a  \
../include_lib/periphy_lib$(SUFFIX).a  \
../include_lib/aec_lib$(SUFFIX).a  \
../include_lib/audio_lib$(SUFFIX).a   \
../include_lib/dev_mg_lib.a       \
../include_lib/fs_lib.a       \
../include_lib/sdmmc_lib.a       \
../include_lib/usb_lib.a       \
../include_lib/flash_lib.a       \
../include_lib/dec/mp3_dec_lib.a       \
../include_lib/dec/wav_dec_lib.a 		  \
../include_lib/encode/lib_adpcm_encode.a  \
../include_lib/encode/lib_mp2_encode.a  \
../include_lib/encode_lib.a  \
--end-group  \


# -----------------------LINEIN_TASK
INCLUDES += \
-Itask_manager/task_linein \


SRCS_C += \
task_manager/task_linein/task_linein.c \
task_manager/task_linein/task_linein_key.c \




# -----------------------PC_TASK
INCLUDES += \
-I../include_lib/usb \
-Iinclude/common/pc_device \
-Itask_manager/task_pc \


SRCS_C += \
task_manager/task_pc/task_pc.c \
task_manager/task_pc/task_pc_key.c \

LIBS += \
../include_lib/usb/lib_usb_syn.a \

# PC_DRIVER
SRCS_C += \
common/pc_device/dev_pc.c \
common/pc_device/card_reader_io.c \

LIBS += \
../include_lib/usb/lib_usb_syn.a \


# -----------------------REC_TASK
# REC_TASK
INCLUDES += \
-Itask_manager/task_rec \

SRCS_C += \
task_manager/task_rec/task_rec.c \
task_manager/task_rec/task_rec_key.c \

# REC_FUCNTION
SRCS_C += \
common/record/rec_file_op.c \
common/record/rec_api.c \
common/record/rec_input.c \
common/record/rec_output.c \
common/record/rec_play.c \

INCLUDES += \
-I../include_lib/encode \
-Iinclude/common/record \

LIBS += \

-include $(MAKE_RULE)
