#include<stdio.h>
#include <stdlib.h>
#include "png.h"
#include "process.h"

int main(int argc,char **argv){
	int width,height;
	int **pixel;
	int **out;
	float **tmp;
	pixel = readPNG(argv[1],&width,&height);
	
	writePNG("res_original.png",pixel,width,height);

	out = histEQ(pixel,width,height);
	writePNG("res_Equalized.png",out,width,height);
	FREE_PIXEL(out);

	out = getNegative(pixel,width,height);
	writePNG("res_Negative.png",out,width,height);
	FREE_PIXEL(out);

	out = halfIntensity(pixel,width,height);
	writePNG("res_HalfI.png",out,width,height);
	FREE_PIXEL(out);
	
	out = IRescale(pixel,width,height,min,max,0,100);
	writePNG("res_scaleto0100.png",out,width,height);
	FREE_PIXEL(out);

	out = IRescale(pixel,width,height,min,max,200,255);
	writePNG("res_scaleto200255.png",out,width,height);
	FREE_PIXEL(out);

	tmp = getExp(pixel,width,height,20,1.01);
	out = Normalize(tmp,width,height);
	writePNG("res_exp.png",out,width,height);
	FREE_PIXEL(out);

	tmp = getLog(pixel,width,height,20.2);
	out = Normalize(tmp,width,height);
	writePNG("res_log.png",out,width,height);
	FREE_PIXEL(out);
	
	tmp = getPower(pixel,width,height,1,0.5);
	out = Normalize(tmp,width,height);
	writePNG("res_powerg05.png",out,width,height);
	FREE_PIXEL(out);
	
	tmp = getPower(pixel,width,height,1,2);
	out = Normalize(tmp,width,height);
	writePNG("res_powerg2.png",out,width,height);
	FREE_PIXEL(out);

	tmp = getPower(pixel,width,height,1,2.5);
	out = Normalize(tmp,width,height);
	writePNG("res_powerg25.png",out,width,height);
	FREE_PIXEL(out);
	//printHist(out,width,height);

	free(pixel);
return 0;
}
