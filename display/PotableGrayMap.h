#ifndef _POTABLEGRAYMAP_H_
#define _POTABLEGRAYMAP_H_
//#include "stdio.h"
#include <klibc.h>

#define Max_Row 48
#define Max_Column 128
#define SCROLL_UP_ROW_NUM 1
typedef struct PotableGreyMap{
	
	unsigned char Magic;//magic number identifies the type of file it is(PBM,PGM,and PPM)and its encoding(ASCII or binary)
	int Max_value;
	int X_axis;
	int Y_axis;
	unsigned char ASIIC_versions[16][8];//bitmap of Glyph
}PotableGrayMap;

//typedef struct PotableGreyMap PotableGreyMap;
typedef  struct PotableGreyMap * PGM;

//unsigned char PGMBuffer[Max_range*32][Max_line*16];//the buffer of the file.pgm


void PotableGrayMapASIIC_Init(PGM PGM_Map,int width,int high,unsigned char Glyph);
//int WritePGMHeader(FILE *stream,PGM PGM_Map);
// void WritePGMBufferInit(PGM PGM_Map,int row,int col);
//void UpdataPGMBuffer(FILE* stream,PGM PGM_Map);
void WritePGMData(PGM PGM_Map,int row,int col);
void DisplayInit(uint64_t *stdout);
#endif


/**************************endfile************************/

