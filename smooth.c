#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "hdf5.h"
#include "hdf5_hl.h"

#define G(I, z, y, x) *(I.data + z*I.dy*I.dx + y*I.dx + x)

typedef struct image {
	unsigned char *data;
	int dx, dy, dz;
} Image;

typedef struct d_image {
	double *data;
	int dx, dy, dz;
} dImage;

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

Image perim(Image image){
	Image out;
	int dx, dy, dz;
	int x,y,z,x_,y_,z_;
	dx = image.dx;
	dy = image.dy;
	dz = image.dz;
	out.dx = image.dx;
	out.dy = image.dy;
	out.dz = image.dz;
	out.data = (unsigned char *)malloc(dx*dy*dz*sizeof(unsigned char));
	for(z = 0; z < dz; z++){
		for(y = 0; y < dy; y++){
			for(x = 0; x < dx; x++){
				*(out.data + z*out.dy*dx + y*out.dx + x)= 0;
				for(z_=z-1; z_ <= z+1; z_++){
					for(y_=y-1; y_ <= y+1; y_++){
						for(x_=x-1; x_ <= x+1; x_++){
							if ((x_ >= 0) && (x_ < dx) && (y_ >= 0) && (y_ < dy) && (z_ >= 0) && (z_ < dz) \
									&& (G(image, z, y, x) != G(image, z_, y_, x_))){
								G(out, z, y, x) = 1;
							}
						}
					}
				}
			}
		}
	}

	return out;
}

Image sum_bands(int n, ...){
	int x, y, z, i;
	Image out, aux;
	va_list bands;
	
	va_start(bands, n);
	aux = va_arg(bands, Image);
	
	out.dx = aux.dx;
	out.dy = aux.dy;
	out.dz = aux.dz;
	out.data = (unsigned char *)malloc(aux.dx*aux.dy*aux.dz*sizeof(unsigned char));
	
	for(z=0; z < out.dz; z++){
		for(y=0; y < out.dy; y++){
			for(x=0; x < out.dx; x++){
				G(out, z, y, x) = G(aux, z, y, x);
			}
		}
	}

	for(i=1; i < n; i++){
		aux = va_arg(bands, Image);
		for(z=0; z < out.dz; z++){
			for(y=0; y < out.dy; y++){
				for(x=0; x < out.dx; x++){
					G(out, z, y, x) = G(aux, z, y, x);
				}
			}
		}
	}

	va_end(bands);
	return out;
}

double calculate_H(dImage I, int z, int y, int x){
    double fx, fy, fz, fxx, fyy, fzz, fxy, fxz, fyz, H;
    int h, k, l;

    h = 1;
    k = 1;
    l = 1;

    fx = (G(I, z, y, x + h) - G(I, z, y, x - h)) / (2.0*h);

    fy = (G(I, z, y + k, x) - G(I, z, y - k, x)) / (2.0*k);

    fz = (G(I, z + l, y, x) - G(I, z - l, y, x)) / (2.0*l);

    fxx = (G(I, z, y, x + h) - 2*G(I, z, y, x) + G(I, z, y, x - h)) / (h*h);

    fyy = (G(I, z, y + k, x) - 2*G(I, z, y, x) + G(I, z, y - k, x)) / (k*k);

    fzz = (G(I, z + l, y, x) - 2*G(I, z, y, x) + G(I, z - l, y, x)) / (l*l);

    fxy = (G(I, z, y + k, x + h) - G(I, z, y - k, x + h) \
            - G(I, z, y + k, x - h) + G(I, z, y - k, x - h)) \
            / (4.0*h*k);

    fxz = (G(I, z + l, y, x + h) - G(I, z + l, y, x - h) \
            - G(I, z - l, y, x + h) + G(I, z - l, y, x - h)) \
            / (4.0*h*l);

    fyz = (G(I, z + l, y + k, x) - G(I, z + l, y - k, x) \
            - G(I, z - l, y + k, x) + G(I, z - l, y - k, x)) \
            / (4.0*k*l);

    if (fx*fx + fy*fy + fz*fz > 0)
        H = ((fy*fy + fz*fz)*fxx + (fx*fx + fz*fz)*fyy \
                + (fx*fx + fy*fy)*fzz - 2*(fx*fy*fxy \
                + fx*fz*fxz + fy*fz*fyz)) \
                / (fx*fx + fy*fy + fz*fz);
	else
        H = 0.0;

    return H;
}

void replicate(dImage source, dImage dest){
	int x, y, z;
	for(z=0; z < source.dz; z++)
		for(y=0; y < source.dy; y++)
			for(x=0; x < source.dx; x++)
				G(dest, z, y, x) = G(source, z, y, x);
}

dImage smooth(Image image, int n){
	int i, x, y, z, S;
	double H, diff=0, dt=1/6.0, v, cn;
	dImage out, aux;

	Image A1 = perim(image);
	Image A2 = perim(A1);
	Image A3 = perim(A2);
	Image A4 = perim(A3);
	Image Band = sum_bands(4, A1, A2, A3, A4);
	free(A1.data);
	free(A2.data);
	free(A3.data);
	free(A4.data);

	out.data = (double *) malloc(image.dz*image.dy*image.dx*sizeof(double));
	aux.data = (double *) malloc(image.dz*image.dy*image.dx*sizeof(double));

	S = 0;
	for(z=0; z < image.dz; z++){
		for(y=0; y < out.dy; y++){
			for(x=0; x < out.dx; x++){
				G(out, z, y, x) = G(image, z, y, x);
				if (G(Band, z, y, x))
					S += 1;
			}
		}
	}

	for(i=0; i < n; i++){
		replicate(out, aux);
		diff = 0.0;
		for(z=0; z < image.dz; z++){
			for(y=0; y < out.dy; y++){
				for(x=0; x < out.dx; x++){
					if (G(Band, z, y, x)){
						H = calculate_H(aux, z, y, x);
						v = G(aux, z, y, x) + dt*H;
						if(G(image, z, y, x)){
							G(out, z, y, x) = v > 0.5 ? v: 0.5;
						} else {
							G(out, z, y, x) = v < 0.5 ? v: 0.5;
						}
						diff += (G(out, z, y, x) - G(out, z, y, x));
					}
				}
			}
		}
		cn = sqrt(1.0/S * diff);
		printf("%.8f", cn);
	}

}


void save_image(Image image, char* filename){
	int RANK=3;
	hid_t       file_id;
	hsize_t     dims[3]={image.dz, image.dy, image.dx};
	herr_t      status;

	/* create a HDF5 file */
	file_id = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	/* create and write an integer type dataset named "dset" */
	status = H5LTmake_dataset(file_id,"/dset",RANK,dims,H5T_NATIVE_UCHAR, image.data);

	/* close file */
	status = H5Fclose (file_id);
}

int main(int argc, const char *argv[])
{
	int r, size;
	char* filename = argv[3];
	size = atoi(argv[1]);
	r = atoi(argv[2]);
	Image ball = build_ball(size, r);
	dImage smoothed = smooth(ball, 5);
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
	/*save_image(Band, filename);*/
	return 0;
}
