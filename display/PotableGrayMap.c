#include "PotableGrayMap.h"
#include "GlyphBDF.h"
#include "fat.h"
#include "klibc.h"
#include <pci.h>
#include <cpu_intrins.h>
#include <paging.h>
// unsigned char PGMBuffer[Max_Row*16][Max_Column*8];
uint32_t *frame_buffer = NULL;
unsigned char *font_buf = NULL;
/**
 * function:get the bitmap of Glyph to PGM map
 */
void PGM_ASIIC_versionsInit(PGM PGM_Map,unsigned char Glyph){
	GlyphBDF Glyph_BDF;
	Glyph_BDF_init(&Glyph_BDF);
	GetBitmapFromGlyphBDF(&Glyph_BDF,Glyph);
	
	for(int i=0;i<16;i++)
		for(int j=0; j<8; j++)
			PGM_Map->ASIIC_versions[i][j]=0;
		
	for(int i=0;i<16;i++)
		for(int j=0;j<2;j++){
			
			switch(Glyph_BDF.BITMAP[i][j]){
				case '0':break;
				
				case '1':{
					if(j==0)
						PGM_Map->ASIIC_versions[i][3]=255;
					else 
						PGM_Map->ASIIC_versions[i][7]=255;
					break;
				}
				
				case '2':{
					if(j==0)
						PGM_Map->ASIIC_versions[i][2]=255;
					else
						PGM_Map->ASIIC_versions[i][6]=255;
					break;
					
				}
				
				case '3':{
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][2]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][6]=255;
					}
					break;
				}
					
				case '4':{
					if(j==0)
						PGM_Map->ASIIC_versions[i][1]=255;
					else 
						PGM_Map->ASIIC_versions[i][5]=255;
					break;
				}
				
				case '5':{
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][1]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][5]=255;
					}
					break;
				}
				
				case '6':{
					if(j==0){
						PGM_Map->ASIIC_versions[i][2]=255;
						PGM_Map->ASIIC_versions[i][1]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][6]=255;
						PGM_Map->ASIIC_versions[i][5]=255;
					}
					break;	
				}
				
				case '7':{
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][2]=255;
						PGM_Map->ASIIC_versions[i][1]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][6]=255;
						PGM_Map->ASIIC_versions[i][5]=255;
					}
					break;
				}
				
				case '8':{
					if(j==0)
						PGM_Map->ASIIC_versions[i][0]=255;
					else
						PGM_Map->ASIIC_versions[i][4]=255;
					break;
				}
				
				case '9':{
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				
				case 'A':{
					
					if(j==0){
						PGM_Map->ASIIC_versions[i][2]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][6]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				 
				case 'B':{
					
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][2]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][6]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				
				case 'C':{
					
					if(j==0){
						PGM_Map->ASIIC_versions[i][1]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][5]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				
				case 'D':{
					
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][1]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][5]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				
				case 'E':{
					
					if(j==0){
						PGM_Map->ASIIC_versions[i][2]=255;
						PGM_Map->ASIIC_versions[i][1]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][6]=255;
						PGM_Map->ASIIC_versions[i][5]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				
				case 'F':{
					
					if(j==0){
						PGM_Map->ASIIC_versions[i][3]=255;
						PGM_Map->ASIIC_versions[i][2]=255;
						PGM_Map->ASIIC_versions[i][1]=255;
						PGM_Map->ASIIC_versions[i][0]=255;
					}
					else{
						PGM_Map->ASIIC_versions[i][7]=255;
						PGM_Map->ASIIC_versions[i][6]=255;
						PGM_Map->ASIIC_versions[i][5]=255;
						PGM_Map->ASIIC_versions[i][4]=255;
					}
					break;
				}
				
				default: ;
				
			}
		}
}

/**
 * function:initialization the struct of PGM map
 */
void PotableGrayMapASIIC_Init(PGM PGM_Map,int width,int high,unsigned char Glyph){
	
	PGM_Map->Max_value=20;
	PGM_Map->Magic=2;
	PGM_Map->X_axis=width;
	PGM_Map->Y_axis=high;
	PGM_ASIIC_versionsInit(PGM_Map,Glyph);
	
}

void WritePGMData(PGM PGM_Map,int row,int column) {  
    
    for(int i=0;i<16;i++) {   // every row
     			
        for(int j=0;j<8;j++) {   // every column 
        		
//         		PGMBuffer[row*16+i][column*8+j]=PGM_Map->ASIIC_versions[i][j];
// 			frame_buffer[(row*16 + i) * 8 * Max_Column + column * 8 + j] = PGM_Map->ASIIC_versions[i][j];
			frame_buffer[(row * 16 + i) *8* Max_Column + column * 8 + j] = PGM_Map->ASIIC_versions[i][j] | PGM_Map->ASIIC_versions[i][j] << 8 | PGM_Map->ASIIC_versions[i][j] << 16;
        		//PGMBuffer[row*32+2*i][column*16+2*j+1]=PGM_Map->ASIIC_versions[i][j];
        }
    }
} 
/**
 *
 *
 */
void FrameBufferClear(void){
	PotableGrayMap PGM_Map;
	PotableGrayMapASIIC_Init(&PGM_Map,Max_Column*8,Max_Row*16,' ');
	for(int i=0;i<Max_Row;i++)
		for(int j=0;j<Max_Column;j++)
			WritePGMData(&PGM_Map,i,j);
}
void Scroll_Screen(int row)
{
	for (int i = 0; i < (Max_Row - row) * 16; i ++)
	{
		for (int j = 0; j < Max_Column * 8; j ++)
		{
			*(frame_buffer + j + i * Max_Column * 8) = *(frame_buffer + j + (i + row * 16)* Max_Column * 8);
		}
	}
	memset(frame_buffer + (Max_Row - row) * 16 * Max_Column * 8, 0, row * 16 * Max_Column * 8 * 4);
}
void WriteChar(int row, int col, char ch)
{
	PotableGrayMap PGM_Map;
	PotableGrayMapASIIC_Init(&PGM_Map, Max_Column*8, Max_Row*16, ch);
	WritePGMData(&PGM_Map, row, col);
}

void WriteLine(char *str)
{
	asm("pushfq");
	__disable_interrupt__();
	static uint8_t row = 0,column = 0;
	
	char visible;
	char temp;
	while((temp = *str ++) != '\0')
	{
		visible = 0;
		switch(temp)
		{
			case '\n':
			case '\r':
				row ++;
				column = 0;
				break;
			case '\t':
				column = (column + 8) / 8 * 8;
				break;
			default:
				visible = 1;
				break;
			
		}
		if (column >= Max_Column)
		{
			row ++;
			column -= Max_Column;
		}
		if (row >= Max_Row)
		{
			Scroll_Screen(SCROLL_UP_ROW_NUM);
			row -= SCROLL_UP_ROW_NUM;
		}
		if (visible)
			WriteChar(row, column++, temp);
// 		printf("%d,%d\n", row,column);
	}
	asm("popfq");
}
/**
 *
 * Initializing the PGMBuffer and PGM_Map
 *
**/
void DisplayInit(uint64_t *stdout){
	
	uint64_t frame_buffer_addr = get_vga_frame_buffer_addr();
	modify_chunk_mapping(get_current_plm4e(), convert_phy_to_virt(frame_buffer_addr), frame_buffer_addr, 768, DEFAULT_PTE_FLAG);
	frame_buffer = (uint32_t *)convert_phy_to_virt(frame_buffer_addr);
	
	
	FATFS_Type fs;
	fatfs_init(&fs);
	font_buf = malloc(40960);
	memset(font_buf, 0, 40960);
	read_file(&fs,"unifont-asiic.bdf",font_buf,40960);
	fatfs_destroy(&fs);
	FrameBufferClear();

	__stdout = &WriteLine;
	*stdout = (uint64_t)&WriteLine;
	
	printf("Video Display Initializing complete.\n");
}

