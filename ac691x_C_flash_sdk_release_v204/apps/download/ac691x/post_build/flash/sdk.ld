
MEMORY
{
    reserved_ram(rw): ORIGIN = 0x0100,              		LENGTH = 0x00100
    mask_ram0       : ORIGIN = 0x0000200,           		LENGTH = 0x200
    ram0            : ORIGIN = 0x0000400,           		LENGTH = 47K-0x400-0x100
    overlay_ram0    : ORIGIN = 47K-0X100,           		LENGTH = 28K
	rom_code(x)     : ORIGIN = 0x0022800,           		LENGTH = 128K-10K
    stack_ram(rw)   : ORIGIN = 0x14000-0x100-5K,            LENGTH = 5K
    code0(x)        : ORIGIN = 0x1000000,           		LENGTH = 16M
}

ENTRY(_start)

#define STR2(x) #x
#define STR(x) STR2(x)
#define QLZ_DATA_EN	1

SECTIONS
{
    . = ORIGIN(reserved_ram);
    .reserved_data ALIGN(4):
    {
        . = 4;
        *(.reserved_info)
        *(.otp_info)
    } > reserved_ram

    . = ORIGIN(ram0);
  .bss (NOLOAD) :SUBALIGN(4)
  {
      *(.bss)
      *(.ram1_bss)

		bredr_base_addr = .;
      *(.bt_classic_base)
      *(.bt_sco_codec_data)
      *(.bt_classic_data)
      *(.bt_stack)

      *(.dac_buf_sec)
  } > ram0

  .data :SUBALIGN(4)
  {
#if QLZ_DATA_EN
	  __begin_of_bank_overlay_0 = .;
#endif
       . = ALIGN(4);
      *(.data*)

      *(.mask_call*)
	   bredr_base_updata_ram1 = .;

	   _bredr_nv_mem_begin = .;
       *(.ram1_data)
	   _bredr_nv_mem_end = .;

	   _ble_nv_mem_begin = .;
       *(.bt_nv_data)
	   _ble_nv_mem_end = .;

	  *(.powerdown_text)
      *(.eq_buf_sec)
      *(.sd_buf)
      *(.usb_buf)

	  *(.common)

      *memcpy.o(.text .rodata*)
      *memset.o(.text .rodata*)
      *icache_hw.o(.text)
	   _system_cfg_begin = .;
      *(.system_cfg)
      _system_cfg_end = .;
#if QLZ_DATA_EN
	  __end_of_bank_overlay_0 = .;
#endif
  } > ram0

  RAM_BEGIN_ADDR = ORIGIN(ram0);
  RAM_END_ADDR  = . ;
  ram_end  = . - ORIGIN(ram0);

  OVERLAY : NOCROSSREFS AT(0x100000) SUBALIGN(4)
  {
	 .overlay1
	  {
	      *(.bt_sbc)
	      *(.bt_sbc_dec)
	      *(.music_sbc_dec)
	  }
	.overlay2
	  {
	      *(.bt_aec)
	  }
	.overlay3
	  {
	      *(.bt_updata)
	  }
	.overlay4
	  {
	      *(.fs_fat)
	      *(.fs_syd)
	      *(.music_mem)
	      *(.music_mp3_dec)
	  }
     .overlay5
     {
     	*(.pc_mem)
     }
  } > overlay_ram0


  .rom_code_seg (NOLOAD) :
  {
	  /* INCLUDE "download/br20/post_build/libs_stubs.ld"; */
      INCLUDE STR(ENTRY_LIST);
  } > rom_code


  . = ORIGIN(stack_ram);
  .stack_data (NOLOAD) :SUBALIGN(4)
    {
        STACK_START = .;
        *(.stack)
        STACK_END = .;
    } > stack_ram

  . = ORIGIN(code0) ;
  .text  ALIGN(4):
  {
	  *startup.o(.text)
      *(.const_tab)

	  *(.ver_magic)
	  *(.version)

      *(.text* .rodata*)
         /*  -----蓝牙的链表接口定义一定要确保对齐,其它代码段不要在中间定义-----  */

	  _LOAD_CACHE_START = . ;
      *(.vm)
      *(.audio_text)
	  _LOAD_CACHE_END = . ;

/*
		_VM_CODE_START = . ;
	 	*(.spi0_drv)
		*(.icache_drv)
		*spi_hw.o(.text)
		*winbond_flash.o(.text)
		*vm_common.o(.text)
		*vm_printf.o(.text)
		*(.vm)
		_VM_CODE_END = . ;
*/

         /*begin*/
        . = ALIGN(4);
	lmp_hci_handler_begin = .;
		*(.lmp_hci_handler)
	lmp_hci_handler_end = .;

        a2dp_sink_media_probe_begin = .;
		*(.a2dp_sink_media_probe)
	a2dp_sink_media_probe_end = .;

	a2dp_sink_media_codec_begin = .;
		*(.a2dp_sink_media_codec)
	a2dp_sink_media_codec_end = .;

	a2dp_source_media_codec_begin = .;
		*(.a2dp_source_media_codec)
	a2dp_source_media_codec_end = .;

	a2dp_event_handler_begin = .;
		KEEP(*(.a2dp_event_handler))
	a2dp_event_handler_end = .;

        clock_switcher_begin = . ;
        *(.clock_switch.fun)
        clock_switcher_end = . ;

        bt_sleep_begin = .;
          *(.bt_sleep)
        bt_sleep_end = .;

       . = (. +3) & ~ 3;
       _sys_initcall_begin = .;
       *(.sys.initcall)
       _sys_initcall_end = .;

        /*  -----end-----  */
        *(.ble)
		*(.bredr)
		*(.bt_power)
        *(.poweroff_flash)

        ble_text_begin = . ;
        *(.ble_irq_text)
        ble_text_end = . ;

        bredr_text_begin = . ;
        *(.bredr_irq_text)
        bredr_text_end = . ;

        *(.sys_cfg)

        poweroff_text_begin = . ;
        *(.poweroff_text)
        poweroff_text_end = . ;

		/*---flash mode----*/
		*(.qlz_init)
		*(.qlz_sco)
		*(.qlz_a2dp)
		*(.qlz_comm)

  } > code0

  text_end = . ;

  /*bt config zone*/

  .version : {
	  *(.ver_magic_start)
	  *(.version)
	  *(.ver_magic_end)
  } > code0

  /* see OTP info.xls */
  jump_addr_base = ORIGIN(code0) + LENGTH(code0) - 128;
  pwr_info_base = ORIGIN(code0) + LENGTH(code0) - 144;
  low_pwr_base = ORIGIN(code0) + LENGTH(code0) - 224;
  code_begin_addr = ORIGIN(code0);

  text_size = SIZEOF(.text);

  data_addr  = ADDR(.data) ;
  data_begin = text_end ;
  data_size =  SIZEOF(.data) ;

  bss_begin = 0x200;
  bss_size  = SIZEOF(.bss) + 0x200;

  /*use maskrom_dict*/
  /* decompressed_dict_begin = 0x20000; */
  /*use otp_dict*/
  decompressed_dict_begin = ORIGIN(code0);

  /**
	 Call maskrom libc functions
   	 MASKROM_VER_A
   */
  /* memcpy = 0x3bed4;
  memset = 0x3bff4;
  strcmp = 0x3c03c;
  strcpy = 0x3c050;
  strlen = 0x3c060;
  memcmp = 0x3c06c; */

 /* ASSERT((text_size + data_size) < CODE_LENGTH, "OTP over flow!")*/
}

