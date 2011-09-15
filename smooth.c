#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct image {
	unsigned char ***data;
	int dx, dy, dz;
} Image;

Image build_ball(int s, int r){
	int i, j, k, X, Y, Z, c;
	Image ball;
	X = Y = Z = s;
	c = s / 2;
	ball.dx = X;
	ball.dy = Y;
	ball.dz = Z;
	ball.data = (unsigned char***)malloc(X*Y*Z*sizeof(unsigned char));
	for (k=0; k < Z; k++){
		ball.data[k] = (unsigned char**)malloc(X*Y*sizeof(unsigned char));
		for (j=0; j < Y; j++){
			ball.data[k][j] = (unsigned char*)malloc(X*sizeof(unsigned char));
			for (i=0; i < X; i++){
				if ((i-c)*(i-c) + (j-c)*(j-c) + (k-c)*(k-c) <= r*r){
					ball.data[k][j][i] = 1;
				}
				else{
					ball.data[k][j][i] = 0;
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
			printf("%d", image.data[n][y][x]);
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
	out.data = (unsigned char ***)malloc(dx*dy*dz*sizeof(unsigned char));
	for(z = 0; z < dz; z++){
		out.data[z] = (unsigned char**)malloc(dx*dy*sizeof(unsigned char));
		for(y = 0; y < dy; y++){
			out.data[z][y] = (unsigned char*)malloc(dx*sizeof(unsigned char));
			for(x = 0; x < dx; x++){
				out.data[z][y][x] = 0;
				for(z_=z-1; z_ <= z+1; z_++){
					for(y_=y-1; y_ <= y+1; y_++){
						for(x_=x-1; x_ <= x+1; x_++){
							if ((x_ >= 0) && (x_ < dx) && (y_ >= 0) && (y_ < dy) && (z_ >= 0) && (z_ < dz) && (image.data[z][y][x] != image.data[z_][y_][x_])){
								out.data[z][y][x] = 1;
							}
						}
					}
				}
			}
		}
	}

	return out;
}


int main(int argc, const char *argv[])
{
	int r, size;
	size = atoi(argv[1]);
	r = atoi(argv[2]);
	Image ball = build_ball(size, r);
	Image A1 = perim(ball);
	Image A2 = perim(A1);
	Image A3 = perim(A2);
	Image A4 = perim(A3);
	print_slice(A1, 25);
	printf("\n");
	print_slice(A2, 25);
	printf("\n");
	print_slice(A3, 25);
	printf("\n");
	print_slice(A4, 25);
	printf("\n");
	return 0;
}
