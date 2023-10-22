#include <fat.h>
#include <isa.h>
/**
 * Computer the offset of a fat entry in the fat entry table matching specific cluster.
 * fat_index : 0,2,3...
 */
void compute_fat_for_cluster(FATFS_Type *fs, uint8_t fat_index, uint32_t cluster, uint32_t *sec_off, uint32_t *entry_off)
{
	uint32_t FATOffset;
	switch(fs->Type)
	{
		case FAT12:
			FATOffset = cluster + cluster / 2;
			break;
		case FAT16:
			FATOffset = cluster * 2;
			break;
		case FAT32:
			FATOffset = cluster * 4;
			break;
		default:
			bug("Invalid FAT type!\n");
			break;
	}
	*sec_off = fs->FirstFATSec + (FATOffset / fs->BPB->BPB_BytsPerSec) + (fat_index * fs->FATSecs);
	*entry_off = FATOffset % fs->BPB->BPB_BytsPerSec;
}
/**
 * Return fat entry corresponding to specific cluster number
 */
uint32_t extract_fat_entry(FATFS_Type *fs, uint32_t cluster)
{
	char *buf = malloc(512);
	uint32_t sec_off, entry_off;
	uint32_t entry;
	compute_fat_for_cluster(fs, 0, cluster, &sec_off, &entry_off);
	assert(entry_off >= 0 && entry_off < 512);
	pio_read_sector(buf, sec_off + fs->LBAStart);
	switch(fs->Type)
	{
		case FAT12:
			entry = *(uint16_t *)(buf + entry_off);
			if (cluster & 0x0001)
				entry >>= 4;
			else
				entry &= 0x0fff;
			break;
		case FAT16:
			entry = *(uint16_t *)(buf + entry_off);
			break;
		case FAT32:
			entry = *(uint32_t *)(buf + entry_off) & 0x0FFFFFFF;
			break;
		default:
			bug("Invalid FAT type!\n");
			break;
	}
	free(buf);
	return entry;
}
/**
 * Read cluster content into buffer.
 */
void read_cluster(FATFS_Type *fs, void *buf, uint32_t cluster)
{
	uint32_t count = fs->BPB->BPB_SecPerClus;
	uint32_t start = fs->LBAStart + fs->FirstDataSec + (cluster - 2) * fs->BPB->BPB_SecPerClus;
	for (int i = 0; i  < count; i ++)
	{
		pio_read_sector(buf + (i << 9), start + i);
	}
}
/**
 * Computer length of the cluster chain starting with specific cluster
 */
uint32_t compute_cluster_chain_length(FATFS_Type *fs, uint32_t cluster)
{
	uint8_t end = 0;
	uint32_t chain_length = 0;
	do 
	{
		chain_length ++;
		cluster = extract_fat_entry(fs, cluster);
		switch(fs->Type)
		{
			case FAT12:
				if (cluster == FAT12_ENTRY_END_OF_FILE)
					end = 1;
				break;
			case FAT16:
				if (cluster == FAT16_ENTRY_END_OF_FILE)
					end = 1;
				break;
			case FAT32:
				if (cluster == FAT32_ENTRY_END_OF_FILE)
					end = 1;
				break;
			default:
				bug("Invalid FAT type!\n");
				break;
		}
	} while(!end);
	return chain_length;
}
/**
 * Read cluster chain into buffer with a cluster offset.
 */
uint32_t read_cluster_chain(FATFS_Type *fs, void *buf, uint32_t bufsize, uint32_t cluster, uint32_t cluster_offset)
{
	uint8_t end = 0;
	uint32_t read_size = 0;
	do 
	{
		//Avoid overflow
		if (bufsize < read_size + fs->BPB->BPB_SecPerClus * 512)
			break;
		
		if (cluster_offset == 0)
		{
			read_cluster(fs, buf + read_size, cluster);
			read_size += fs->BPB->BPB_SecPerClus * 512;
		}
		else
			cluster_offset --;
		
		cluster = extract_fat_entry(fs, cluster);
		
		switch(fs->Type)
		{
			case FAT12:
				if (cluster == FAT12_ENTRY_END_OF_FILE)
					end = 1;
				break;
			case FAT16:
				if (cluster == FAT16_ENTRY_END_OF_FILE)
					end = 1;
				break;
			case FAT32:
				if (cluster == FAT32_ENTRY_END_OF_FILE)
					end = 1;
				break;
			default:
				bug("Invalid FAT type!\n");
				break;
		}
		
	} while(!end);
	
	return read_size;
}

/**
 * Extract the name of a directory entry starting with the entry provided.
 * 
 * Max is the number of total entries which follow.
 */

int read_name(LongNameDirEntry_Type *dir_entry, char *name, uint32_t max)
{
//Empty directory
	if (((Dir_Struc_Type*)dir_entry)->DIR_Name[0] == 0x00)
		return -1;
//Handle short name directory
	if (((Dir_Struc_Type*)dir_entry)->DIR_Attr != ATTR_LONG_NAME)
	{
		memcpy(name, ((Dir_Struc_Type*)dir_entry)->DIR_Name, 8);
		*(name + 8) = '\0';
		uint8_t str_len = strcspn(name, " ");
		*(name + str_len) = '\0';
		
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || ((Dir_Struc_Type*)dir_entry)->DIR_Name[8] == ' ')
			return 0;
		
		*(name + str_len) = '.';
		memcpy(name + str_len + 1, ((Dir_Struc_Type*)dir_entry)->DIR_Name + 8, 3);
		*(name + str_len + 1 + 3) = '\0';
		str_len = strcspn(name, " ");
		*(name + str_len) = '\0';
		
		return 0;
	}
//Handle long name directory
	int offset;
	char *buf = malloc(512);
	wchar *unicode_lname = (wchar *)buf;
	LongNameDirEntry_Type *entry = dir_entry;
	while(entry->LDIR_Attr == ATTR_LONG_NAME)
	{
		entry ++;
		
		if (entry > max + dir_entry)
		{
			free(buf);
			return -1;
		}
	}
	
	offset = entry - dir_entry;
	entry --;
	
	if (entry->LDIR_Attr != ATTR_LONG_NAME)
	{
		free(buf);
		return -1;
	}
	
	do
	{
		if (entry->LDIR_Name1[0] == 0xffff)
			break;
		unicode_lname = wstrncpy(unicode_lname, entry->LDIR_Name1, 5);
		if (entry->LDIR_Name2[0] == 0xffff)
			break;
		unicode_lname = wstrncpy(unicode_lname, entry->LDIR_Name2, 6);
		if (entry->LDIR_Name3[0] == 0xffff)
			break;
		unicode_lname = wstrncpy(unicode_lname, entry->LDIR_Name3, 2);
		
		if (entry < dir_entry)
		{
			free(buf);
			return -1;
		}
	} while(!((entry--)->LDIR_Ord & 0x40));
	str_unicode_to_utf8((wchar *)buf, name);
	
	free(buf);
	return offset;
}
/**
 * Read a file at the root directory into buffer.
 */
uint32_t read_file(FATFS_Type *fs, const char *name, void *buf, uint64_t bufsize)
{
	uint32_t rootdir_size = compute_cluster_chain_length(fs, fs->BPB->ExtBPB.Ext_BPB_32.BPB_RootClus) * fs->BPB->BPB_SecPerClus << 9;
	void *rootdir = malloc(rootdir_size);
	assert(rootdir != NULL);
	if (read_cluster_chain(fs, rootdir, rootdir_size, fs->BPB->ExtBPB.Ext_BPB_32.BPB_RootClus, 0) != rootdir_size)
		bug("Reading root directory failed unexpectedly");
	
	char *name_buf = malloc(512);
	uint32_t pos = 0;
	int ret;
	do
	{

		if ((ret = read_name((LongNameDirEntry_Type *)rootdir + pos, name_buf, (rootdir_size >> 5) - pos)) == -1)
		{
			bug("Extracting file name from root directory failed unexpectedly");
			break;
		}
		pos += ret + 1;
	} while(strcmp(name_buf, name) != 0);
	
	Dir_Struc_Type *dir_struct = (Dir_Struc_Type *)rootdir + pos - 1;
	
	free(rootdir);
	free(name_buf);
	
	if (bufsize < dir_struct->DIR_FileSize)
		return 0;
	
	return read_cluster_chain(fs, buf, bufsize, dir_struct->DIR_FstClusHI << 16 | dir_struct->DIR_FstClusLO, 0);
}

void fatfs_init(FATFS_Type *fs)
{
	fs->LBAStart = 2048;
	fs->BPB = malloc(sizeof(BPB_Type));
	assert(fs->BPB != NULL);
	
	//The partition begins at 2048 sectors.
	pio_read_sector(fs->BPB, fs->LBAStart);
	assert(fs->BPB->BPB_BytsPerSec == 512);
	
	fs->RootDirSecs = ((fs->BPB->BPB_RootEntCnt * 32) + (fs->BPB->BPB_BytsPerSec - 1)) / fs->BPB->BPB_BytsPerSec;
	fs->FATSecs = fs->BPB->BPB_FATSz16 != 0 ? fs->BPB->BPB_FATSz16 : fs->BPB->ExtBPB.Ext_BPB_32.BPB_FATSz32;

	fs->TotSecs = fs->BPB->BPB_TotSec16 != 0 ? fs->BPB->BPB_TotSec16 : fs->BPB->BPB_TotSec32;
	fs->DataSecs = fs->TotSecs - (fs->BPB->BPB_RsvdSecCnt + (fs->BPB->BPB_NumFATs * fs->FATSecs) + fs->RootDirSecs);
	fs->CountOfClus = fs->DataSecs / fs->BPB->BPB_SecPerClus;
	
	
	fs->FirstFATSec = fs->BPB->BPB_RsvdSecCnt;
	fs->FirstDataSec = fs->BPB->BPB_RsvdSecCnt + (fs->BPB->BPB_NumFATs * fs->FATSecs + fs->RootDirSecs);
	if (fs->CountOfClus < 4085)
	{
		fs->Type = FAT12;
		fs->FirstRootSec = fs->BPB->BPB_RsvdSecCnt + fs->BPB->BPB_NumFATs * fs->BPB->BPB_FATSz16;
	}
	else if (fs->CountOfClus < 65525)
	{
		fs->Type = FAT16;
		fs->FirstRootSec = fs->BPB->BPB_RsvdSecCnt + fs->BPB->BPB_NumFATs * fs->BPB->BPB_FATSz16;
	}
	else
	{
		fs->Type = FAT32;
		fs->FirstRootSec = fs->FirstDataSec + fs->BPB->ExtBPB.Ext_BPB_32.BPB_RootClus * fs->BPB->BPB_SecPerClus;
	}
}
void fatfs_destroy(FATFS_Type *fs)
{
	if (fs->BPB != NULL)
		free(fs->BPB);
}

FATDir_Type *fatfs_opendir(FATFS_Type *fs, const char *path)
{
	FATDir_Type *fatdir;
	char *last_str=NULL;
	char *dir_name;
	char *save_ptr;
	uint32_t pos = 0;
	int ret;
	char *name_buf = malloc(512);
	uint32_t buf_size;
	
	fatdir = malloc(sizeof(FATDir_Type));
	buf_size = fatdir->size = compute_cluster_chain_length(fs, fs->BPB->ExtBPB.Ext_BPB_32.BPB_RootClus) * fs->BPB->BPB_SecPerClus * fs->BPB->BPB_BytsPerSec;
	fatdir->buf = malloc(fatdir->size);
	assert(fatdir->buf != NULL);
	read_cluster_chain(fs, fatdir->buf, buf_size, fs->BPB->ExtBPB.Ext_BPB_32.BPB_RootClus, 0);
	
	fatdir->offset = 0;
	fatdir->file_info = malloc(sizeof(FATFile_Type));
	memset(fatdir->file_info, 0, sizeof(FATFile_Type));
	
	
	char *path_buf = malloc(strlen(path) + 1);
	strcpy(path_buf, path);
	dir_name = strtok_r(path_buf, "/", &last_str);
	
	while(dir_name != NULL)
	{
		pos = 0;
		
		do
		{
			
			if ((ret = read_name(fatdir->buf + pos, name_buf, (fatdir->size - pos) >> 5)) == -1)
			{
				bug("Extracting file name from root directory failed unexpectedly");
				free(name_buf);
				return NULL;
			}
			pos += (ret + 1) << 5;
			
			if (pos >= fatdir->size)
			{
				free(fatdir->buf);
				free(fatdir);
				free(name_buf);
				return NULL;
			}
		} while(strcmp(name_buf, dir_name) != 0);
		
		Dir_Struc_Type *dir_struc = fatdir->buf + (pos - 32);
// 		if (!(dir_struc->DIR_Attr & ATTR_DIRECTORY))
// 		{
// 			free(path_buf);
// 			return fatdir;
// 		}
		
		uint32_t first_cluster = dir_struc->DIR_FstClusHI << 16 | dir_struc->DIR_FstClusLO;
		fatdir->size = compute_cluster_chain_length(fs, first_cluster) * fs->BPB->BPB_SecPerClus * fs->BPB->BPB_BytsPerSec;
		if (fatdir->size > buf_size)
		{
			free(fatdir->buf);
			fatdir->buf = malloc(fatdir->size);
			buf_size = fatdir->size;
			assert(fatdir->buf != NULL);
		}
		
		if(read_cluster_chain(fs, fatdir->buf, buf_size, first_cluster, 0) != fatdir->size)
			bug("Error reading cluster chain");
		
		dir_name = strtok_r(NULL, "/", &last_str);
	}
	free(path_buf);
	free(name_buf);
	return fatdir;
}



FATFile_Type *fatfs_readdir(FATFS_Type *fs, FATDir_Type *dir)
{
	Dir_Struc_Type *dir_struct;
	uint32_t ret;
	if (dir->offset >= dir->size)
		return NULL;
	
	if ((ret = read_name(dir->buf + dir->offset, dir->file_info->Name, (dir->size - dir->offset) >> 5)) == -1)
	{
		return NULL;
	}
	dir->offset += (ret + 1) << 5;
	
	dir_struct = dir->buf + dir->offset - 32;
	dir->file_info->Type = dir_struct->DIR_Attr;
	dir->file_info->First_Cluster = dir_struct->DIR_FstClusHI << 16 | dir_struct->DIR_FstClusLO;
	dir->file_info->Creation_Time = dir_struct->DIR_CtrTIme + dir_struct->DIR_CtrTimeTenth / 10;
	dir->file_info->Creation_Date = dir_struct->DIR_CtrDate;
	dir->file_info->Write_Time = dir_struct->DIR_WriTime;
	dir->file_info->Write_Date = dir_struct->DIR_WritDate;
	dir->file_info->Access_Date = dir_struct->DIR_LstAccDate;
	dir->file_info->FileSize = dir_struct->DIR_FileSize;


	return  dir->file_info;
	
}
void fatfs_rewinddir(FATFS_Type *fs, FATDir_Type *dir)
{
	dir->offset = 0;
	memset(dir->file_info, 0, sizeof(FATFile_Type));
}
void fatfs_closedir(FATFS_Type *fs, FATDir_Type *dir)
{
	free(dir->file_info);
	dir->file_info = NULL;
	free(dir->buf);
	dir->buf = NULL;
	dir->size = 0;
	dir->offset = 0;
	free(dir);
}

int fatfs_searchfile(FATFS_Type *fs, FATFile_Type *buf, const char *path)
{
	char *file_name = strrchr(path, '/') + 1;
	size_t dir_path_len = strlen(path) - strlen(file_name);
	char *dir_path = malloc(dir_path_len + 1);
	memcpy(dir_path, path, dir_path_len);
	*(dir_path + dir_path_len) = '\0';
	
	FATDir_Type *dir = fatfs_opendir(fs, dir_path);
	if (dir == NULL)
	{
		fatfs_closedir(fs, dir);
		return -1;
	}
	
	FATFile_Type *file;
	while((file = fatfs_readdir(fs, dir)) != NULL && strcmp(file->Name, file_name) != 0);
	
	if (file == NULL)
	{
		fatfs_closedir(fs, dir);
		return -1;
	}
	memcpy(buf, file, sizeof(FATFile_Type));
	
	fatfs_closedir(fs, dir);
	return 0;
}

uint32_t fatfs_readfile(FATFS_Type *fs, FATFile_Type *file, void *buf, uint32_t bufsize)
{
	if (bufsize < file->FileSize)
		return 0;
	
	return read_cluster_chain(fs, buf, bufsize, file->First_Cluster, 0);
}
uint32_t computer_needed_buf_size(FATFS_Type *fs, FATFile_Type *file)
{
	return compute_cluster_chain_length(fs, file->First_Cluster) * fs->BPB->BPB_SecPerClus * fs->BPB->BPB_BytsPerSec;
}
