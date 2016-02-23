#include <wand/magick_wand.h>
#include <stdio.h>


void main()
{
	MagickWand *m_wand = NULL;
	PixelWand *p_wand = NULL;
	PixelIterator *iterator = NULL;
	PixelWand **pixels = NULL;
	int x,y,gray=0;
	char hex[128];

	MagickWandGenesis();

	p_wand = NewPixelWand();
	PixelSetColor(p_wand,"white");
	m_wand = NewMagickWand();
	// Create a 100x100 image with a default of white
	MagickNewImage(m_wand,16,16,p_wand);
	// Get a new pixel iterator 
	iterator=NewPixelIterator(m_wand);
	for(y=0;y<16;y++) {
		// Get the next row of the image as an array of PixelWands
		pixels=PixelGetNextIteratorRow(iterator,&x);
		// Set the row of wands to a simple gray scale gradient
		for(x=0;x<16;x++) {
			sprintf(hex,"#%02x%02x%02x",255-gray,255-gray,255-gray);
			PixelSetColor(pixels[x],hex);
			gray++;
		}
		// Sync writes the pixels back to the m_wand
		PixelSyncIterator(iterator);
	}
	MagickWriteImage(m_wand,"bits_demo.gif");

	// Clean up
	iterator=DestroyPixelIterator(iterator);
	DestroyMagickWand(m_wand);
	MagickWandTerminus();

}
