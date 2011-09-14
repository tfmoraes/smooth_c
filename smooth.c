#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned char ***build_ball(void){
	int x = 50, y = 50, z = 50;
	int i, j, k;
	int r = 20;
	int c = 25;
	unsigned char ***ball = (unsigned char***)malloc(x*y*z*sizeof(unsigned char));
	for (k=0; k < z; k++){
		ball[k] = (unsigned char**)malloc(x*y*sizeof(unsigned char));
		for (j=0; j < y; j++){
			ball[k][j] = (unsigned char*)malloc(x*sizeof(unsigned char));
			for (i=0; i < x; i++){
				if ((i-c)*(i-c) + (j-c)*(j-c) + (k-c)*(k-c) <= r*r){
					ball[k][j][i] = 1;
				}
				else{
					ball[k][j][i] = 0;
				}

			}
		}
	}
	return ball;

}

int main(void)
{
	unsigned char ***ball = build_ball();
	return 0;
}

