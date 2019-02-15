#include "dev_mg_api.h"
#include "fs_io.h"

#include "mbr.h"

#include "syd_io.h"
#include "fat_io.h"

#include "dev_cache.h"
#include "dev_usb.h"

#include "string.h"

#include "uart.h"

#define dev_dbg  printf
#define mbr_deg  printf

extern int mbr_example(MBR_DRIVE_INFO *mbr_inf, void *hdev, void *read_fun, void *write_fun);
extern int mbr_exit(MBR_DRIVE_INFO *mbr);
extern void *malloc(unsigned int size);
extern void  free(void *mem);


#define FLASH_BASE_ADDR		0x1000000

u32 read_api(DEV_HANDLE hdev, u8 *buf, u32 addr)
{
    if (512 == dev_read(hdev, buf, addr, 1)) {
        return 0;//SUCC
    } else {
        return 1;
    }
}

u32 write_api(DEV_HANDLE hdev, u8 *buf, u32 addr)
{
    if (512 == dev_write(hdev, buf, addr, 1)) {
        return 0;//SUCC
    } else {
        return 1;
    }
}

static void vfs_test2(void *dev, void *read_fun, void *write_fun)
{
    MBR_DRIVE_INFO mbr_inf;
    u32 first_sect = 0;
    _FS_HDL fs_hdl;
    _FIL_HDL f_hdl;

    s32 err;
    u8 *buf;

    memset(&fs_hdl, 0x00, sizeof(fs_hdl));
    memset(&f_hdl, 0x00, sizeof(f_hdl));
    memset(&mbr_inf, 0x00, sizeof(mbr_inf));

#if 1	//syd 不需要扫描mbr
    puts("mbr_example_init\n");
    err = mbr_scan_parition(&mbr_inf, dev, read_fun, write_fun);
    ASSERT(!err);
    first_sect = mbr_inf.drive_boot_sect[0];
    mbr_scan_end(&mbr_inf);
#else
    first_sect = 0;
#endif

    printf("first_sect = 0x%x\n", first_sect);
    err = fs_drive_open(&fs_hdl, dev, first_sect, read_fun, write_fun);
    if (err) {
        puts("fs_drive_open err\n");
        return;
    }
    ASSERT(!err);

    printf("file_total = 0x%x\n", fs_get_file_total(&fs_hdl, 0, 0));

#if 0
    puts("fs_get_file_byindex\n");
    err = fs_get_file_byindex(&fs_hdl, &f_hdl, 1, NULL);
    if (err) {
        puts("fs_get_file_byindex err\n");
        return;
    }
    /* ASSERT(!err); */
#else
    puts("fs_get_file_bypath\n");
    /* err = fs_get_file_bypath(&fs_hdl, &f_hdl, (void*)"test.iso", NULL); */
    err = fs_get_file_bypath(&fs_hdl, &f_hdl, (void *)"/SLEEP.LRC", NULL);
    if (err) {
        printf("fs_get_file_bypath err = %d\n", err);
        return;
    }
    /* ASSERT(!err); */
#endif

    buf = malloc(512);
    ASSERT(buf);

    puts("fs_read\n");
    err = fs_read(&f_hdl, buf, 512);
    if (err != 512) {
        puts("fs_read err\n");
        return;
    }
    /* ASSERT(err == 512); */

    /* printf_buf(buf, 512); */

    free(buf);

    puts("fs_close\n");
    err = fs_close(&f_hdl);
    ASSERT(err == 0);

    puts("fs_drive_close\n");
    err = fs_drive_close(&fs_hdl);
    ASSERT(err == 0);
}

#define FS_TEST_TYPE	1

DEV_HANDLE usb;
DEV_HANDLE cache;

void vfs_example_init(void)
{
    u32 parm;
    dev_mg_init();

    vfs_init();
    fat_init();
    syd_init();

#if FS_TEST_TYPE
    DEVICE_REG(cache, NULL);
    parm = FLASH_BASE_ADDR;
    cache = dev_open("cache", &parm);
    ASSERT(cache);
    /* #else */
    DEVICE_REG(usb, NULL);
    parm = 512;
    usb = dev_open("usb", &parm);
    ASSERT(usb);
#endif
}

void vfs_example(void)
{
    u32 parm;
    /* DEV_HANDLE dev = cache; */
    DEV_HANDLE dev = usb;
    u32 ret;

    while (1) {
        ret = dev_detect(dev, (void *)&parm);
        if (dev_get_online_status(dev, (void *)&parm) == DEV_ERR_NONE) {
            if (parm == DEV_ONLINE) {
                puts("dev_online\n");

                ret = dev_mount(dev, NULL);
                ASSERT(!ret);

                vfs_test(dev, read_api, write_api);

                ret = dev_unmount(dev);
                ASSERT(!ret);

                puts("test_ok\n");
                while (1);
            }
        }
    }
}

