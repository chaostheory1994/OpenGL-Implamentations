#ifndef __MY_GL__
#define __MY_GL__


void my_frustum(float, float, float, float, float, float);
void my_perspective(float, float, float, float);
void my_scale(float, float, float);
void my_rotate(float, float, float, float);
void my_lookat(float, float, float, float, float, float, float, float, float);
void my_ortho(float, float, float, float, float, float);
void my_translate(float, float, float);
void print_mat(float*, int, int);


#endif // __MY_GL__