#include<stdio.h>
#include <stdlib.h>
#include "png.h"
#include <math.h>

png_structp png_ptr;
png_infop info_ptr;
png_byte color_type, bit_depth;

int max = 0;
int min = 255;
int hist[255];
int **tmp_free;
float csum[255];
int readmode;
png_bytep *row_ptrs;

#define MALLOC_OUTPUT_INT(out, width, height)					\
	do {									\
		(out) = (int**)malloc(sizeof(int*) * (height)); 		\
		for(int i=0;i<(height);i++)					\
			(out)[i] = (int*) malloc(sizeof(int) * (width));	\
	}									\
	while(0)


#define MALLOC_OUTPUT_FLOAT(out, width, height)					\
	do {									\
		(out) = (float**)malloc(sizeof(float*) * (height)); 		\
		for(int i=0;i<(height);i++)					\
			(out)[i] = (float*) malloc(sizeof(float) * (width));	\
	}									\
	while(0)

#define FREE_PIXEL(out)								\
	do {									\
		tmp_free = (out);						\
		(out) = NULL;							\
		free(tmp_free);							\
	}									\
	while(0)					
int **readPNG(char *filename,int* width, int* height){

	FILE *fp = fopen(filename,"rb");
	char header[8];
	fread(header, 1, 8, fp);
	if(png_sig_cmp((png_const_bytep)header,0 ,8)) {
		printf("not png file\n");
		exit(-1);
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL, NULL,NULL);
	if (!png_ptr) printf("fail create");
	info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	
	*width = png_get_image_width(png_ptr, info_ptr);
	*height = png_get_image_height(png_ptr, info_ptr);
 	color_type = png_get_color_type(png_ptr, info_ptr);
 	bit_depth = png_get_bit_depth(png_ptr, info_ptr);


	setjmp(png_jmpbuf(png_ptr));
	row_ptrs = (png_bytep*) malloc(sizeof(png_bytep) * (*height));
	for(int y=0;y<(*height); y++){
		row_ptrs[y] = (png_bytep) malloc (png_get_rowbytes(png_ptr, info_ptr));
	}
	png_read_image(png_ptr,row_ptrs);
	png_read_end(png_ptr,info_ptr);
	printf("\n\n\n=== Original Photo Information ===============================\n");
	printf("File Name : %s\n",filename);
	printf("%d x %d Pixels\nBit Depth : %d\nColor Type : ",*width,*height,bit_depth);
	fclose(fp);
	int colot_type = png_get_color_type(png_ptr, info_ptr);
	switch(color_type){
		case PNG_COLOR_TYPE_RGB: 
		       printf("RGB\n");
		       readmode = 3;
	       	       break;
		case PNG_COLOR_TYPE_RGBA:
			printf("RGB ALPHA\n");
			readmode = 4;
	 		break;
		case PNG_COLOR_TYPE_GRAY:
			printf("GRAYSCALE\n");
			readmode = 1;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			printf("GRAYSCALE ALPHA\n");
			readmode = 2;
			break;
		case PNG_COLOR_TYPE_PALETTE:
			printf("PALETTE\n");
			readmode = 1;
			break;
		default :
			printf("OTHERS\n");
			break;		
	}
	printf("Row bytes : %d\n",(int)png_get_rowbytes(png_ptr,info_ptr));
	int **pixel;
	pixel = (int**) malloc (sizeof(int*) * (*height));
	if(NULL == pixel) printf("NULL\n");
	for(int i=0;i< (*height);i++){
		pixel[i] = (int*) malloc (sizeof(int) * (*width));
		if(NULL == pixel[i]) printf("NULL\n");
	}

	for(int y = 0; y < (*height); y++){
		png_byte *row = row_ptrs[y];
		for( int x = 0; x< (*width);x++){
			png_byte *ptr = &(row[x*readmode]);
			pixel[y][x] = (int)ptr[0];
			if( (int)ptr[0] > max ) max = (int) ptr[0];
			if( (int)ptr[0] < min ) min = (int) ptr[0];
			hist[pixel[y][x]]++;
		}
		//printf("\n");
	}
	for(int i=0;i<256;i++){
		for(int j=0;j<=i;j++)
			csum[i] += hist[j];
		csum[i] /= ((*width)*(*height));
	}
	printf("Max Intensity : %d\n",max);
	printf("Min Intensity : %d\n",min);
	printf("===============================================================\n\n");
	free(row_ptrs);
	return pixel;

}

void printHist(int **pixel,int width,int height){

	int lHist[256];
	float csum[256];
	for(int i=0;i<256;i++) lHist[i] = csum[i] = 0;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			lHist[pixel[i][j]]++;	
		}
	}
	for(int i=0;i<256;i++) {
		for(int j=0;j<=i;j++) csum[i]+=lHist[j];
	}

	for(int i=0;i<256;i++) printf("%d\t\t%.2f\n",i,csum[i]/(width*height));
}

void printPixel(int **pixel,int width,int height){
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			printf("%d ",pixel[i][j]);
		}
		printf("\n");
	}
}

void writePNG(char *filename,int **pixel,int width,int height){

	png_bytep *row_ptrs;
	row_ptrs = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for(int y=0;y<height; y++){
		//row_ptrs[y] = (png_bytep*) malloc (sizeof(png_bytep) * width);
		row_ptrs[y] = (png_bytep) malloc (width * sizeof(png_byte));
	}

	// write pixel array to row pointers
	for(int y = 0;y<height;y++){
		for(int x = 0;x<width; x++){
			int z = x;
			row_ptrs[y][x]   = pixel[y][x];
			//row_ptrs[y][z+1]   = 255;
		}
	}
	
	FILE *fp = fopen(filename,"wb");
	if(!fp) 
		exit(-1);

	png_structp wpng_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if(!wpng_ptr)
		exit(-1);



	png_infop winfo_ptr = png_create_info_struct(wpng_ptr);

	setjmp(png_jmpbuf(wpng_ptr));
	png_init_io(wpng_ptr, fp);

	setjmp(png_jmpbuf(wpng_ptr));
	png_set_IHDR(wpng_ptr, winfo_ptr, width, height,
			bit_depth, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(wpng_ptr, winfo_ptr);


	setjmp(png_jmpbuf(wpng_ptr));
	png_write_image(wpng_ptr, row_ptrs);

	setjmp(png_jmpbuf(wpng_ptr));
	png_write_end(wpng_ptr, NULL);

	fclose(fp);
	free(row_ptrs);

}

int **IRescale(int **pixel, int width, int height,int min,int max,int tomin,int tomax){

	int **out;
	MALLOC_OUTPUT_INT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x =0;x<width;x++){
			out[y][x] = (int)((float)((pixel[y][x] - min)) / (max-min) * (tomax-tomin))+tomin;
		}
	}
	return out;

}


int **Normalize(float **pixel, int width, int height){

	int **out;
	float min = pixel[0][0];
	float max = pixel[0][0];
	MALLOC_OUTPUT_INT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x =0;x<width;x++){
			if(pixel[y][x] < min) min = pixel[y][x];
			if(pixel[y][x] > max) max = pixel[y][x];
		}
	}
	printf("mix = %f\nmax=%f\n",min,max);
	// Scale them up
	for(int y=0;y<height;y++){
		for(int x =0;x<width;x++){
			out[y][x] = (int)((float)((pixel[y][x] - min)) / (max-min) * 255);
		}
	}
	
	return out;

}

int **ParseInt(float **pixel, int width, int height){

	int **out;
	float min = pixel[0][0];
	float max = pixel[0][0];
	MALLOC_OUTPUT_INT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x =0;x<width;x++){
			out[y][x] = (int)pixel[y][x];
		}
	}
	
	return out;

}



int **histEQ(int **pixel, int width, int height){

	int T[256];
	int **out;
	MALLOC_OUTPUT_INT(out, width, height);

	for(int i=0;i<256;i++){
		T[i] = 255 * (csum[i]);
	}

	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			out[y][x] = T[pixel[y][x]];
		}
	}
	return out;
}

int **halfIntensity(int **pixel, int width, int height){
	
	int **out;
	MALLOC_OUTPUT_INT(out, width, height);
	
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			out[y][x] = pixel[y][x]/2;
		}
	}
	return out;
}


int **getNegative(int **pixel, int width, int height){
	
	int **out;
	MALLOC_OUTPUT_INT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			out[y][x] = 255-pixel[y][x];
		}
	}
	return out;
}

float **getLog(int **pixel, int width, int height,float c){
	
	float **out;
	MALLOC_OUTPUT_FLOAT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			out[y][x] = c * log(1+pixel[y][x]);
		}
	}
	return out;
}

float **getExp(int **pixel, int width, int height,float c,float b){
	
	float **out;
	MALLOC_OUTPUT_FLOAT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			out[y][x] = c * (pow(b,pixel[y][x])-1);
		}
	}
	return out;
}
float **getPower(int **pixel, int width, int height,float c,float gamma){
	
	float **out;
	MALLOC_OUTPUT_FLOAT(out, width, height);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			out[y][x] = c * pow(pixel[y][x],gamma);
		}
	}
	return out;
}


