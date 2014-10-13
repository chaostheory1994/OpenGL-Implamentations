#include "GL.h"
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float matrix[16];

void my_translate(float x, float y, float z){
	// Setup Translation Matrix
	float trans_mat[16] = { 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							x, y, z, 1 };
	// Multiply Matrix
	glMultMatrixf(trans_mat);
}

void my_scale(float x, float y, float z){
	float scale_mat[16] =
	{ x, 0, 0, 0,
	0, y, 0, 0,
	0, 0, z, 0,
	0, 0, 0, 1 };
	// Multiply Matrix
	glMultMatrixf(scale_mat);
}

void my_rotate(float theta, float x, float y, float z){
	// Prerequisites to speed up the process of multiplying these values over and over.
	float cos_t = cos((theta * M_PI) / 180);
	float om_cos_t = 1 - cos_t;
	float sin_t = sin((theta * M_PI) / 180);
	float om_sin_t = 1 - sin_t;
	// Rotation Matrix
	float rot_matrix[16] =
	{ cos_t + x*x*om_cos_t, y*x*om_cos_t + z*sin_t, z*x*om_cos_t - y*sin_t, 0,
	x*y*om_cos_t - z*sin_t, cos_t + y*y*om_cos_t, z*y*om_cos_t + x*sin_t, 0,
	x*z*om_cos_t + y*sin_t, y*z*om_cos_t - x*sin_t, cos_t + z*z*om_cos_t, 0,
	0, 0, 0, 1 };

	// MultiplyMatrixf
	glMultMatrixf(rot_matrix);

}

void my_lookat(float eyex, float eyey, float eyez,
	float centerx, float centery, float centerz,
	float upx, float upy, float upz){
	int i;
	float xaxis[3];
	float yaxis[3];
	// We can easily figure z axis out.
	float zaxis[3] = {
		centerx - eyex,
		centery - eyey,
		centerz - eyez
	};
	float up[3] = {
		upx,
		upy,
		upz
	};
	float temp;
	float lookat_mat[16];
	// Now we need to setup the rotation.
	// We kno z axis and up vector. Lets cross that product.
	// However, they need to be normalized first.
	temp = 0;
	for (i = 0; i < 3; i++) temp += zaxis[i] * zaxis[i];
	temp = sqrt(temp);
	for (i = 0; i < 3; i++) zaxis[i] = zaxis[i] / temp;

	temp = 0;
	for (i = 0; i < 3; i++) temp += up[i] * up[i];
	temp = sqrt(temp);
	for (i = 0; i < 3; i++) up[i] = up[i] / temp;

	xaxis[0] = zaxis[1] * up[2] - zaxis[2] * up[1];
	xaxis[1] = zaxis[2] * up[0] - zaxis[0] * up[2];
	xaxis[2] = zaxis[0] * up[1] - zaxis[1] * up[0];

	// Now we find the y axis.
	yaxis[0] = xaxis[1] * zaxis[2] - xaxis[2] * zaxis[1];
	yaxis[1] = xaxis[2] * zaxis[0] - xaxis[0] * zaxis[2];
	yaxis[2] = xaxis[0] * zaxis[1] - xaxis[1] * zaxis[0];

	// Setup the matrix.
	for (i = 0; i < 3; i++) lookat_mat[i*4] = xaxis[i];
	for (i = 0; i < 3; i++) lookat_mat[i*4 + 1] = yaxis[i];
	for (i = 0; i < 3; i++) lookat_mat[i*4 + 2] = -zaxis[i];
	// Fill in gaps.
	for (i = 0; i < 3; i++) lookat_mat[i + 12] = 0;
	lookat_mat[3] = 0;
	lookat_mat[7] = 0;
	lookat_mat[11] = 0;
	lookat_mat[15] = 1;
	
	// Change model view
	glMultMatrixf(lookat_mat);
	my_translate(-eyex, -eyey, -eyez);
}

void my_ortho(float left, float right, float bottom, float top, float n, float f){
	my_scale(2 / (right - left), 2 / (top - bottom), -2 / (f - n));
	my_translate(-(right + left) / 2, -(top + bottom) / 2, (f + n) / 2);
}

void my_perspective(float fovy, float aspect, float n, float f){
	float cotan = 1 / tan((fovy * M_PI) / 360);
	float perspect_mat[16] = {
		cotan / aspect, 0, 0, 0,
		0, cotan, 0, 0,
		0, 0, (f + n) / (n - f), -1,
		0, 0, 2 * f*n / (n - f), 1
	};
	glMultMatrixf(perspect_mat);
}

void my_frustum(float left, float right, float bottom, float top, float n, float f){
	float frust_mat[16] = {
		2 * n / (right - left), 0, 0, 0,
		0, 2 * n / (top - bottom), 0, 0,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -(f + n) / (f - n), -1,
		0, 0, -2 * f*n / (f - n), 0
	};
	glMultMatrixf(frust_mat);
}


/* This method is for debug purposes.
 * Will display the the values in the matrix to the terminal.
*/
void print_mat(float* mat, int row, int col){
	int i, j;
	if (row < 0 || col < 0) return;
	int size = row * col;
	for (i = 0; i < col; i++){
		for (j = 0; j < row; j++){
			printf("%f, ", mat[j * 4 + i]);
		}
		puts("");
	}
	puts("");
}
