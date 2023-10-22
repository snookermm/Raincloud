#include <id.h>
uint8_t *pid_table;
uint8_t *tid_table;
//Simple functions to manipulate PID and TID
id_t pid_alloc(void)
{
	for (int i = 0; i < MAX_PID; i ++)
	{
		if(*(pid_table + i) == 0)
		{
			*(pid_table + i) = 1;
			return i;
		}
	}
	return -1;
}
void pid_free(id_t id)
{
	*(pid_table + id) = 0;
}
id_t tid_alloc(void)
{
	for (int i = 0; i < MAX_PID; i ++)
	{
		if(*(tid_table + i) == 0)
		{
			*(tid_table + i) = 1;
			return i;
		}
	}
	return -1;
}
void tid_free(id_t id)
{
	*(tid_table + id) = 0;
}
void id_init(void)
{
	printf("%s", "ID allocator initializing...\n");
	pid_table = malloc(PID_TABLE_SIZE);
	memset(pid_table, 0, PID_TABLE_SIZE);
	tid_table = malloc(TID_TABLE_SIZE);
	memset(tid_table, 0, TID_TABLE_SIZE);
	printf("ID allocator initializing complete\n");
} 
