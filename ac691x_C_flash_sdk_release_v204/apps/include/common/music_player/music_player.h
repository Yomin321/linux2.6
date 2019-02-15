#ifndef __MUSIC_PLAYER_H__
#define __MUSIC_PLAYER_H__
#include "typedef.h"
#include "music_decoder.h"
#include "file_operate/file_operate.h"

//以最大的断点信息存放, flac:68byte, wma:68byte, 其他:8byte
#define MUSIC_DECODER_BREAK_POINT_SIZE (680L)

typedef struct __MMUSIC_PLAYER_BP {
    // DEV_HANDLE dev;
    FOP_BP f_info;
    u32 bp_size;
    u8	buf[MUSIC_DECODER_BREAK_POINT_SIZE];
} MUSIC_PLAYER_BP;




#define MUSIC_PLAYER_TASK	"MusicPlayer"

typedef struct __MUSIC_PLAYER MUSIC_PLAYER;

void music_player_run(MUSIC_PLAYER *obj);
void music_player_destroy(MUSIC_PLAYER **hdl);
MUSIC_PLAYER *music_player_creat(void);
tbool music_player_play(MUSIC_PLAYER *obj, MUSIC_PLAYER_BP *bp_info, u8 is_auto);


tbool music_player_operation(MUSIC_PLAYER *obj, ENUM_FILE_SELECT_MODE op);
tbool music_player_play_spec_file(MUSIC_PLAYER *obj, u32 filenum);
tbool music_player_play_path_file(MUSIC_PLAYER *obj, u8 *path, u32 index);
tbool music_player_play_prev_folder_file(MUSIC_PLAYER *obj);

tbool music_player_play_spec_dev(MUSIC_PLAYER *obj, u32 dev);
tbool music_player_play_next_dev(MUSIC_PLAYER *obj);
tbool music_player_play_prev_dev(MUSIC_PLAYER *obj);
tbool music_player_play_first_dev(MUSIC_PLAYER *obj);
tbool music_player_play_last_dev(MUSIC_PLAYER *obj);

void music_player_set_repeat_mode(MUSIC_PLAYER *obj, ENUM_PLAY_MODE mode);

void music_player_set_decoder_init_sta(MUSIC_PLAYER *obj, MUSIC_DECODER_ST sta);
tbool music_player_delete_playing_file(MUSIC_PLAYER *obj);

void music_player_ff(MUSIC_PLAYER *obj, u8 second);
void music_player_fr(MUSIC_PLAYER *obj, u8 second);
MUSIC_DECODER_ST music_player_pp(MUSIC_PLAYER *obj);

MUSIC_DECODER_ST music_player_get_status(MUSIC_PLAYER *obj);
u32 music_player_get_cur_time(MUSIC_PLAYER *obj);
u32 music_player_get_total_time(MUSIC_PLAYER *obj);

void *music_player_get_cur_dev(MUSIC_PLAYER *obj);
MUSIC_DECODER *music_player_get_dop(MUSIC_PLAYER *obj);
FILE_OPERATE *music_player_get_fop(MUSIC_PLAYER *obj);

void music_tone_play(void *name);
void music_tone_end(void);
void music_tone_stop(void);
#endif// __MUSIC_PLAYER_H__
