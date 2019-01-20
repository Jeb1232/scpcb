
#ifndef GXFONT_H
#define GXFONT_H

#include <ddraw.h>
#include <d3d.h>

#include "StringType.h"

class gxCanvas;
class gxGraphics;

typedef IDirectDrawSurface7 ddSurf;

class gxFont{
public:
	gxFont(
		gxGraphics *graphics,gxCanvas *canvas,
		int width,int height,int begin_char,int end_char,int def_char,
		int *offs,int *widths );
	~gxFont();

	int charWidth( int c )const;
	void render( gxCanvas *dest,unsigned color_argb,int x,int y,const String& t );

private:
	gxGraphics *graphics;
	gxCanvas *canvas,*t_canvas;
	int width,height,begin_char,end_char,def_char;
	int *offs,*widths;

	/***** GX INTERFACE *****/
public:
	enum{
		FONT_BOLD=1,
		FONT_ITALIC=2,
		FONT_UNDERLINE=4
	};

	//ACCESSORS
	int getWidth()const;							//width of widest char
	int getHeight()const;							//height of font
	int getWidth( const String& text )const;	//width of string
	bool isPrintable( int chr )const;				//printable char?
};

#endif