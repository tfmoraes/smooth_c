
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "smooth.h"

void save_image(Image_d image, char* filename){
	int RANK=3;
    int SRANK=1;
	hid_t       file_id;
	hsize_t     dims[3]={image.dz, image.dy, image.dx};
    hsize_t     dims_spacing[1] = {3};
	herr_t      status;

	/* create a HDF5 file */
	file_id = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	/* create and write an integer type dataset named "dset" */
	status = H5LTmake_dataset(file_id,"/dset",RANK,dims,H5T_NATIVE_DOUBLE, image.data);
	status = H5LTmake_dataset(file_id,"/spacing",SRANK,dims_spacing,H5T_NATIVE_FLOAT, image.spacing);

	/* close file */
	status = H5Fclose (file_id);
}

Image open_image(char* filename){
	Image out;
	hid_t file, space, dset, spacing;
	hsize_t dims[3];
	file = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT);
	dset = H5Dopen (file, "dset", H5P_DEFAULT);
	spacing = H5Dopen (file, "spacing", H5P_DEFAULT);

    /* Reading dset*/
	space = H5Dget_space(dset);    /* dataspace identifier */
	H5Sget_simple_extent_dims(space, dims, NULL);
	printf("%d %d %d", dims[0], dims[1], dims[2]);

	out.dz = dims[0];
	out.dy = dims[1];
	out.dx = dims[2];
	out.data = (unsigned char *)malloc(out.dx*out.dy*out.dz*sizeof(unsigned char));

	H5Dread(dset, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, out.data);

    /* reading spacing */
    space = H5Dget_space(spacing);
	out.spacing = (float *)malloc(3*sizeof(float));
	H5Dread(spacing, H5T_NATIVE_FLOAT , H5S_ALL, H5S_ALL, H5P_DEFAULT, out.spacing);

	return out;
}

Image build_ball(int s, int r){
	int i, j, k, X, Y, Z, c;
	Image ball;
	X = Y = Z = s;
	c = s / 2;
	ball.dx = X;
	ball.dy = Y;
	ball.dz = Z;
	ball.data = (unsigned char*)malloc(X*Y*Z*sizeof(unsigned char));
	for (k=0; k < Z; k++){
		for (j=0; j < Y; j++){
			for (i=0; i < X; i++){
				if ((i-c)*(i-c) + (j-c)*(j-c) + (k-c)*(k-c) <= r*r){
					G(ball, k, j, i) = 1;
				}
				else{
					G(ball, k, j, i) = 0;
				}
			}
		}
	}
	return ball;
}

void print_slice(Image image, int n){
	int x, y;
	for(y=0; y < image.dy; y++){
		printf("\n");
		for(x=0; x < image.dx; x++){
			printf("%d", G(image, n, y, x));
		}
	}
}

int main(int argc, const char *argv[])
{
	int r, size;
	/*char* filename = argv[3];*/
	/*size = atoi(argv[1]);*/
	/*r = atoi(argv[2]);*/
	Image test;
	char* filename = argv[1];
	char* output = argv[2];
	test = open_image(filename);
	/*Image ball = build_ball(size, r);*/
	Image_d smoothed = smooth(test, 500);
	/*print_slice(ball, size/2);*/
	/*print_slice(A1, 25);*/
	/*printf("\n");*/
	/*print_slice(A2, 25);*/
	/*printf("\n");*/
	/*print_slice(A3, 25);*/
	/*printf("\n");*/
	/*print_slice(A4, 25);*/
	/*printf("\n");*/
	/*print_slice(Band, 25);*/
	/*printf("\n");*/
	save_image(smoothed, output);
	return 0;
}
