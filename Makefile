.PHONY: all clean

all: smooth

clean:
	rm -rf smooth

smooth: smooth.c
	gcc smooth.c -I/usr/include/mpi -lhdf5_hl -lhdf5 -lz -lm  -o smooth
