#ifndef _ID_H_
#define _ID_H_

#include <klibc.h>

#define MAX_PID		256
#define PID_TABLE_SIZE	(MAX_PID * sizeof(uint8_t))

#define MAX_TID		1024
#define TID_TABLE_SIZE	(MAX_TID * sizeof(uint8_t))

id_t pid_alloc(void);
void pid_free(id_t id);
id_t tid_alloc(void);
void tid_free(id_t id);

void id_init(void);

#endif
