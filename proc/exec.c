#include <proc.h>
#include <sched.h>
#include <paging.h>
#include <vm.h>
#include <fat.h>
#include <queue.h>
#include <klibc.h>
extern thread_t *current_thread;
extern proc_t *current_proc;
extern queue_t *ready_queue;

void exec(const char *path, char *const argv[])
{
	printf("Exec\n");
	if (current_proc == NULL)
	{
		vm_map_t *new_map = malloc(sizeof(vm_map_t));
		vm_map_init(new_map);
		
		vm_map_entry_t *new_entry = malloc(sizeof(vm_map_entry_t));
		new_entry->virt_addr = 0;
		new_entry->phy_addr = page_alloc(1);
		new_entry->flags = DEFAULT_USER_TEXT_PTE_FLAGS;
		new_entry->refs = 1;
		new_entry->page_nums = 1;
		vm_map_add(new_map, new_entry);
		vm_map_do_mapping(new_map);
		
		FATFS_Type *fs = malloc(sizeof(FATFS_Type));
		fatfs_init(fs);

		FATFile_Type *file = malloc(sizeof(FATFile_Type));
		if(fatfs_searchfile(fs, file, path))
			bug("Error reading file %s", path);
		void *ptr = (void *)convert_phy_to_virt(new_entry->phy_addr);
		printf("Load file succeed, file size:%d bytes\n", fatfs_readfile(fs, file, ptr, 4096));
		printf("File Size %d\n", file->FileSize);
		fatfs_destroy(fs);
		free(file);
		free(fs);
		
		
		pargs_t *pargs = malloc(sizeof(pargs_t));
		pargs->argv = malloc(sizeof(char*));
		pargs->argc = 1;
		pargs->argv[0] = malloc(strlen(path) + 1);
		strcpy(pargs->argv[0], path);
		
		penvs_t *penvs = malloc(sizeof(penvs_t));
		penvs->pwd = malloc(strlen(path) + 1);
		strcpy(penvs->pwd, path);
		

		proc_t *new_proc = malloc(sizeof(proc_t));
		proc_init(new_proc, current_proc, pargs, penvs, new_map);
		thread_t *new_thread = malloc(sizeof(thread_t));
		thread_init(new_thread, new_proc, 0, 4096);
		queue_enqueue(ready_queue, new_thread);
	}
	else
	{
		FATFS_Type *fs = malloc(sizeof(FATFS_Type));
		fatfs_init(fs);

		FATFile_Type *file = malloc(sizeof(FATFile_Type));
		if(fatfs_searchfile(fs, file, path))
			bug("Error reading file %s", path);

		vm_map_t *new_map = malloc(sizeof(vm_map_t));
		vm_map_init(new_map);
		vm_map_entry_t *new_entry = malloc(sizeof(vm_map_entry_t));
		new_entry->virt_addr = 0;
		new_entry->phy_addr = page_alloc(1);
		new_entry->flags = DEFAULT_USER_TEXT_PTE_FLAGS;
		new_entry->refs = 1;
		new_entry->page_nums = 1;
		vm_map_add(new_map, new_entry);
		vm_map_do_mapping(new_map);
		
		void *ptr = (void *)convert_phy_to_virt(new_entry->phy_addr);
		printf("Load file succeed, file size:%d bytes\n", fatfs_readfile(fs, file, ptr, 4096));
		printf("File Size %d\n", file->FileSize);
		fatfs_destroy(fs);
		free(file);
		free(fs);
		
		proc_t *proc = get_current_proc();
		assert(proc != NULL);
// 		vm_map_clear(proc->vm_map);
		proc->vm_map = new_map;
		vm_map_do_mapping(new_map);
		thread_t *thread = get_current_thread();
		assert(thread != NULL);
		thread_init(thread, proc, 0, 4096);
// 		sched();
	}

}
