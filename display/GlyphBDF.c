#include "GlyphBDF.h"

extern unsigned char *font_buf;
/**
  *function:description the storing bitmap fonts of the Glyph Bitmap Distribution Format,
  *use the glyph that is taken form the GNU Unifont.
  *
  */
void Glyph_BDF_init(GlyphBDF * Glyph_BDF){
	
	Glyph_BDF->File_offset[0]=19;//
	Glyph_BDF->File_offset[1]=23;//
	Glyph_BDF->File_offset[2]=1;//
	Glyph_BDF->File_offset[3]=6;//
}

void GetBitmapFromGlyphBDF(GlyphBDF * Glyph_BDF,unsigned char Glyph){
	unsigned char *stream = font_buf;
	
	file_seek(&stream,Glyph_BDF->File_offset[0]+Glyph_BDF->File_offset[1]*Glyph);
	file_seek(&stream,Glyph_BDF->File_offset[2]);
	
	if(Glyph<'d'){
	
		(stream)+=9;
		Glyph_BDF->ENCODIN=((*stream)-'0')*10+((stream)[1]-'0');
	}
	else{
		
		(stream)+=9;
		Glyph_BDF->ENCODIN=((*stream)-'0')*100+((stream)[1]-'0')*10+((stream)[2]-'0');
	}
	
	file_seek(&stream,Glyph_BDF->File_offset[3]-Glyph_BDF->File_offset[2]);
	
	for(int i=0;i<16;i++){
	
		for(int j=0;j<2;j++){
		
			Glyph_BDF->BITMAP[i][j]=(*stream);
			(stream)+=1;
			
		}
		(stream)+=1;
	}
}
void file_seek(unsigned char **stream, unsigned int offset){
	for(unsigned int i=0;i< offset; i++ ){
		while((**stream)!='\n'){
			(*stream)+=1;
		}
			(*stream)+=1;
	}
		
}
