#ifndef __TASK_MUSIC_H__
#define __TASK_MUSIC_H__
#include "typedef.h"
#include "task_manager.h"

extern const TASK_APP task_music_info;

extern tbool mutex_resource_apply(char *resource, int prio, void (*apply_response)(void *priv), void (*release_request)(void *priv), void *priv);
extern tbool mutex_resource_release(char *resource);


#endif//__TASK_MUSIC_H__

