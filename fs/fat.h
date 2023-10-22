#ifndef _FAT_H_
#define _FAT_H_

#include <klibc.h>

typedef struct __attribute__((packed)) Ext_BPB_12_16
{
	uint8_t BS_DrvNum;
	uint8_t BS_Reserved1; //Set value to 0x0
	uint8_t BS_BootSig;
	uint32_t BS_VolID;
	uint8_t BS_VolLab[11];
	uint8_t BS_FilSysType[8];
	uint8_t _reserved_1[448]; //Set to 0x00
	uint8_t Signature_word[2]; //0x55(at 510) 0xAA(at 511)
}Ext_BPB_12_16_Type;

typedef struct  __attribute__((packed)) Ext_BPB_32 
{
	uint32_t BPB_FATSz32;
	uint16_t BPB_ExtFlags;
	uint16_t BPB_FSVer;
	uint32_t BPB_RootClus;
	uint16_t BPB_FSInfo;
	uint16_t BPB_BkBootSec;
	uint8_t BPB_Reserved[12]; //Must be set to 0x00
	uint8_t BS_DrvNum;
	uint8_t BS_Reserved1; //Set value to 0x0
	uint8_t BS_BootSig;
	uint32_t BS_VOlID;
	uint8_t BS_VolLab[11];
	uint8_t BS_FilSysType;
	uint8_t _reserved_1[420]; //Set to 0x00
	uint8_t Signature_word[2]; //0x55(at 510) 0xAA(at 511)
}Ext_BPB_32_Type;

//BPB structure common to FAT12, FAT16 and FAT32 implementations
typedef struct __attribute__((packed)) BPB 
{
	uint8_t BS_jmpBoot[3];
	uint8_t BS_OEMName[8];
	uint16_t BPB_BytsPerSec;
	uint8_t BPB_SecPerClus;
	uint16_t BPB_RsvdSecCnt;
	uint8_t BPB_NumFATs;
	uint16_t BPB_RootEntCnt;
	uint16_t BPB_TotSec16;
	uint8_t BPB_Media;
	uint16_t BPB_FATSz16;
	uint16_t BPB_SecPerTrk;
	uint16_t BPB_NumHeads;
	uint32_t BPB_HiddSec;
	uint32_t BPB_TotSec32;
	union {
		struct Ext_BPB_12_16 Ext_BPB_12_16;
		struct Ext_BPB_32 Ext_BPB_32;
	} ExtBPB;
}BPB_Type;



typedef enum FAT_TYPE
{
	FAT12,
	FAT16,
	FAT32
}FAT_TYPE;

#define FAT12_ENTRY_FREE		0x000
#define FAT12_ENTRY_END_OF_FILE		0xFFF

#define FAT16_ENTRY_FREE		0x0000
#define FAT16_ENTRY_END_OF_FILE		0xFFFF

#define FAT32_ENTRY_FREE		0x0000000
#define FAT32_ENTRY_END_OF_FILE		0xFFFFFFF



typedef struct __attribute__((packed)) FSInfo 
{
	uint8_t FSI_LeadSig[4];
	uint8_t FSI_Reserved1[480];
	uint8_t FSI_StrucSig[4];
	uint8_t FSI_Free_Count[4];
	uint8_t FSI_Nxt_Free[4];
	uint8_t FSI_Reserved2[12];
	uint8_t FSI_TrailSig[4];
}FSInfo_Type;

typedef struct __attribute__((packed)) Dir_Struc
{
	uint8_t DIR_Name[11];
	uint8_t DIR_Attr;
	uint8_t DIR_NTRes;
	uint8_t DIR_CtrTimeTenth;
	uint16_t DIR_CtrTIme;
	uint16_t DIR_CtrDate;
	uint16_t DIR_LstAccDate;
	uint16_t DIR_FstClusHI;
	uint16_t DIR_WriTime;
	uint16_t DIR_WritDate;
	uint16_t DIR_FstClusLO;
	uint32_t DIR_FileSize;
}Dir_Struc_Type;

#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN	0x02
#define ATTR_SYSTEM	0x04
#define ATTR_VOLUMN_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20


#define ATTR_LONG_NAME	0x0F

typedef struct __attribute__((packed)) LongNameDirEntry 
{
	uint8_t LDIR_Ord;
	uint16_t LDIR_Name1[5];
	uint8_t LDIR_Attr;
	uint8_t LDIR_Type;
	uint8_t LDIR_Chksum;
	uint16_t LDIR_Name2[6];
	uint16_t FstClusLO; //Must be set to 0.
	uint16_t LDIR_Name3[2];
}LongNameDirEntry_Type;

typedef struct FATFS
{
	uint32_t LBAStart;
	
	BPB_Type *BPB;
	
	FAT_TYPE Type;
	uint32_t RootDirSecs;
	uint32_t FATSecs;
	uint32_t DataSecs;
	uint32_t TotSecs;
	uint32_t CountOfClus;
	
	uint32_t FirstRootSec;
	uint32_t FirstFATSec;
	uint32_t FirstDataSec;
}FATFS_Type;

typedef struct FATFile
{
	char Name[256];
	
	uint8_t Type;
	uint32_t First_Cluster;
	
	uint32_t Creation_Time;//In tenth seconds
	uint32_t Creation_Date;
	uint32_t Write_Time;
	uint32_t Write_Date;
	uint32_t Access_Date;
	uint32_t FileSize;
}FATFile_Type;

typedef struct FATDir
{
	void *buf;
	uint32_t size;
	uint32_t offset;//In bytes.
	FATFile_Type *file_info;
}FATDir_Type;

void fatfs_init(FATFS_Type *fs);
void fatfs_destroy(FATFS_Type *fs);

FATDir_Type *fatfs_opendir(FATFS_Type *fs, const char *path);
FATFile_Type *fatfs_readdir(FATFS_Type *fs, FATDir_Type *dir);
void fatfs_rewinddir(FATFS_Type *fs, FATDir_Type *dir);
void fatfs_closedir(FATFS_Type *fs, FATDir_Type *dir);
uint32_t fatfs_readfile(FATFS_Type *fs, FATFile_Type *file, void *buf, uint32_t bufsize);
int fatfs_searchfile(FATFS_Type *fs, FATFile_Type *buf, const char *path);
uint32_t computer_needed_buf_size(FATFS_Type *fs, FATFile_Type *file);
#endif
