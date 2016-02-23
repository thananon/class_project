CC=gcc
CFLAGS= -g
LDFLAGS1= -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16 -I/opt/local/include/ImageMagick-6 -L/opt/local/lib -lMagickWand-6.Q16 -lMagickCore-6.Q16

LIBPNGFLAGS = -I/opt/local/include -L/opt/local/lib -lpng 

hw1: pngread.c
	$(CC) pngread.c $(LIBPNGFLAGS) $(CFLAGS) -o $@

clean:
	rm -f hw1 gray
