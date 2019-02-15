#include "dev_mg_api.h"
#include "flash_cfg.h"

#include "sydf/syd_file.h"

#include "string.h"
#include "uart.h"


extern void *malloc(unsigned int size);
extern void  free(void *mem);

/**
 * @brief syd_file_system - example
 */
#if 0
#include "sydf/syd_file.h"
#include "icache_interface.h"

extern u32 args[3];

static void sydf_init(void)
{
    FLASH_SYS_CFG *sys_cfg = (void *)(args[0] + FLASH_BASE_ADDR);
    set_sydf_header_base(sys_cfg->flash_cfg.sdfile_head_addr);
}

static tbool syd_test(void *p_hdev, void *read_fun, void *write_fun)
{
    SYDFS *syd_fs = NULL  ;
    SDFILE *f_p = NULL;
    u8 *buff;
    u32 ret;
    u32 i;

    printf("fun:%s\n", __FUNCTION__);

    //prepare
    printf("prepare\r");
    buff = malloc(512);
    ASSERT(buff);
    memset(buff, 0, 512);

    syd_fs = malloc(sizeof(SYDFS));
    ASSERT(syd_fs);
    memset((u8 *)syd_fs, 0, sizeof(SYDFS));

    f_p = malloc(sizeof(SDFILE));
    ASSERT(f_p);
    memset((u8 *)syd_fs, 0, sizeof(SDFILE));

    //init
    printf("init\r");
    syd_fs->win.fs = syd_fs;
    syd_fs->win.sector = 0xffffffff;

    syd_fs->disk_read = (u32(*)(void *, u8 _xdata *, u32))read_fun; //fs_api_read;
    syd_fs->disk_write = (u32(*)(void *, u8 _xdata *, u32))write_fun; //fs_api_write;
    syd_fs->hdev = p_hdev;

    sydf_init();

    //test
    printf("test\r");
    if (SR_OK == check_syd(syd_fs, 0)) {
        printf("check_syd succ \r");

        printf("sydf_openbyindex\r");
        if (!sydf_openbyindex(syd_fs, f_p, 1, NULL)) {
            printf("open file fail \r");
        }
        ret = sydf_read(f_p, buff, 512);
        ASSERT(ret);
        printf_buf(buff, ret);
        sydf_close(f_p);

        printf("sydf_openbyname\r");
        if (!sydf_openbyname(syd_fs, f_p, "sdram.app")) {
            printf("open file fail \r");
        }
        ret = sydf_read(f_p, buff, 512);
        ASSERT(ret);
        printf_buf(buff, ret);
        sydf_close(f_p);
    } else {
        puts("check_syd fail \r");
    }
    free(buff);
}
#endif


/**
 * @brief mbr - example
 */
#include "mbr.h"
#include "fat/ff_api.h"

#define MBR_DRV_MAX				23		//扫描最大的分区数

#define dev_dbg  printf
#define mbr_deg  printf

int mbr_example(MBR_DRIVE_INFO *mbr_inf, void *hdev, void *read_fun, void *write_fun)
{
    MRESULT res;
    MBR_FS *mbr = NULL;
    u8 i;

    mbr_deg("func = %s\n", __FUNCTION__);

    mbr = malloc(sizeof(*mbr));
    ASSERT(mbr);
    memset(mbr, 0, sizeof(*mbr));

    mbr->win.mbr = mbr;
    mbr->win.sector = 0xffffffff;
    mbr->disk_read = read_fun;  //需要补齐代码
    mbr->disk_write = write_fun; //需要补齐代码
    mbr->hdev = hdev;

    mbr->inf  = mbr_inf;

    mbr->inf->drive_cnt = 0;
    mbr->inf->drive_max = MBR_DRV_MAX;
    mbr->inf->drive_boot_sect = malloc(MBR_DRV_MAX * sizeof(u32));

    u32 parm;
    if (DEV_ERR_NONE != dev_io_ctrl(hdev, DEV_GET_BLOCK_SIZE, &parm)) {
        printf("DEV_GET_BLOCK_SIZE\r");
    }
    printf("DEV_GET_BLOCK_SIZE = %d\r", parm);
    mbr->sector_512size = get_powerof2(parm);

    res = mbr_scan(mbr, 0, (u8)255);//,查找第一个盘

    free(mbr);

    if (MBR_FAIL == res) {
        mbr_deg("no mbr\n");
        if ((NULL != mbr_inf->drive_boot_sect)) {
            mbr_inf->drive_cnt = 1;
            mbr_inf->drive_boot_sect[0] = 0;
        } else {
            mbr_deg("mbr inf err\n");
        }
    } else {
        for (i = 0; i < mbr_inf->drive_cnt; i++) {
            printf("%d	addr:0x%x\n", mbr_inf->drive_boot_sect[i]);
        }
    }

    return MBR_OK;
}

int mbr_exit(MBR_DRIVE_INFO *mbr)
{
    if (mbr->drive_boot_sect) {
        free(mbr->drive_boot_sect);
        mbr->drive_boot_sect = 0;
    }
    mbr->drive_cnt = 0;
    return MBR_OK;
}


#if 0
/**
 * @brief fat_file_system - example
 */
#include "fat/ff_api.h"

extern u32 args[3];

static tbool fat_test(void *p_hdev, void *read_fun, void *write_fun)
{
    MBR_DRIVE_INFO mbr_inf;

    FATFS *fs_p = NULL  ;
    FIL *f_p = NULL;

    u8 *buff;
    u32 ret;
    u32 i;

    printf("fun:%s\n", __FUNCTION__);

    mbr_example(&mbr_inf, p_hdev, read_fun, write_fun);

    //prepare
    printf("prepare\r");
    buff = malloc(512);
    ASSERT(buff);
    memset(buff, 0, 512);

    fs_p = malloc(sizeof(*fs_p));
    ASSERT(fs_p);
    memset((u8 *)fs_p, 0, sizeof(*fs_p));

    f_p = malloc(sizeof(*f_p));
    ASSERT(f_p);
    memset((u8 *)f_p, 0, sizeof(*f_p));

    //init
    printf("init\r");
    fs_p->win.fs = fs_p;
    fs_p->win.sector = 0xffffffff;

    fs_p->disk_read = (u32(*)(void *, u8 _xdata *, u32))read_fun; //fs_api_read;
    fs_p->disk_write = (u32(*)(void *, u8 _xdata *, u32))write_fun; //fs_api_write;
    fs_p->hdev = p_hdev;

    //get_dev_att
    u32 parm;
    if (DEV_ERR_NONE != dev_io_ctrl(p_hdev, DEV_GET_BLOCK_SIZE, &parm)) {
        printf("DEV_GET_BLOCK_SIZE\r");
    }

    printf("DEV_GET_BLOCK_SIZE = %d\r", parm);
    fs_p->sector_512size = get_powerof2(parm);
    fs_p->s_size = fs_p->sector_512size + 9;


    //test
    printf("test\r");
    if (FR_OK == check_fs(fs_p, mbr_inf.drive_boot_sect[0])) {
        printf("check_syd succ \r");

        fs_ext_setting("MP3");

        /* fs_scan_disk(fs_p, NULL); */
        /* printf("fs_scan_disk total = 0x%x\r", fs_p->total_file); */

#if 1
        printf("fat_openbyindex\r");
        if (fs_getfile_bynumber(fs_p, f_p, 1, NULL)) {
            printf("open file succ\r");
            ret = f_read(f_p, buff, 512);
            ASSERT(ret);
            printf_buf(buff, ret);
            /* sydf_close(f_p); */
        }
#else
        printf("fat_openbyname\r");
        if (fs_getfile_bypath(fs_p, f_p, "/sleep.mp3", NULL)) {
            printf("open file succ\r");
            ret = f_read(f_p, buff, 512);
            ASSERT(ret);
            printf_buf(buff, ret);
            /* sydf_close(f_p); */
        }
#endif

    } else {
        puts("check_fat fail \r");
    }
    free(buff);

    return 0;
}


#define FS_TEST_TYPE	1	//0:fat	1:syd

void fs_example(void)
{
    DEV_HANDLE dev;
    u32 parm;
    u32 ret;
    u8 *buf;

    dev_manage_init();

#if FS_TEST_TYPE
    FLASH_SYS_CFG *sys_cfg = (void *)(args[0] + FLASH_BASE_ADDR);
    DEVICE_REG(cache, NULL);
    parm = FLASH_BASE_ADDR;
    dev = dev_open("cache", &parm);
    ASSERT(dev);
#else
    DEVICE_REG(usb, NULL);
    parm = 512;
    dev = dev_open("usb", &parm);
    ASSERT(dev);
#endif

    buf = malloc(512);
    ASSERT(buf);

    while (1) {
        ret = dev_detect(dev);
        if (ret == DEV_ONLINE) {
            puts("dev_online\n");
            ret = dev_mount(dev, NULL);
            ASSERT(!ret);
#if 0
            ret = dev_read(dev, buf, 0, 1);
            ASSERT(ret);

            put_buf(buf, ret);
            free(buf);
#endif

#if FS_TEST_TYPE
            syd_test(dev, read_api, write_api);
#else
            fat_test(dev, read_api, write_api);
#endif

            ret = dev_unmount(dev);
            ASSERT(!ret);
            return;
        }
    }
}
#endif
