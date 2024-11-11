// TNM084 Lab 1
// Procedural textures
// Draws concentric circles on CPU and GPU
// Make something more interesting!

// For the CPU part, you should primarily work in maketexture() below.

// New C++ version!

//uses framework Cocoa
//uses framework OpenGL
#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include <stdio.h>
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"

#include <iostream>

// Lab 1 specific
// Code for you to access to make interesting patterns.
// This is for CPU. For GPU, I provide separate files
// with functions that you can add to your fragment shaders.
#include "noise1234.h"
#include "simplexnoise1234.h"
#include "cellular.h"

#define kTextureSize 512
GLubyte ptex[kTextureSize][kTextureSize][3];
const float ringDensity = 20.0;

enum texture {RadialPattern, BrickPattern, GPU};
texture CurrentTexture = BrickPattern;

// Example: Radial pattern.
void maketexture()
{
    int x, y;
    if(CurrentTexture == texture::RadialPattern){
        // Number of milliseconds since glutInit called (or first call to glutGet(GLUT_ELAPSED_TIME)).
        // * 0.001 to seconds
        float time = ((float)glutGet(GLUT_ELAPSED_TIME)) * 0.001;

        float sinTime = sin(time);
        //std::cout << sinTime<< " " ;


        float fx, fy, fxo, fyo;
    //	char val;

        for (x = 0; x < kTextureSize; x++)
        for (y = 0; y < kTextureSize; y++)
        {
            fx = (float)(x -kTextureSize/2.)/kTextureSize*2.;
            fy = (float)(y -kTextureSize/2.)/kTextureSize*2.;
            fxo = sqrt(fx*fx+fy*fy);
            fyo = sqrt(fx*fx+fy*fy);
            fxo = cos(fxo * ringDensity);
            fyo = sin(fyo * ringDensity);
            if (fxo > 1.0) fxo = 1;
            if (fxo < -1.0) fxo = -1.0;
            if (fyo > 1.0) fyo = 1.0;
            if (fyo < -1.0) fyo = -1.0;
            ptex[x][y][0] = fxo * 127 + 127; // r
            ptex[x][y][1] = fyo * 127 + 127; // g
            ptex[x][y][2] = 128; // b
        }
    } else if(CurrentTexture == texture::BrickPattern){
        for (x = 0; x < kTextureSize; x++) // v
        for (y = 0; y < kTextureSize; y++) // >
        {
            float xx, yy;

            int amountOfSquares = 4;
            float freq = round(kTextureSize / amountOfSquares);
            // Affect x by y
            float xxBeforeFract = x/2/freq + trunc(y / freq)/2;
            xx = xxBeforeFract - floor(xxBeforeFract);
            float yyBeforeFract = y / freq;
            yy = yyBeforeFract - floor(yyBeforeFract);


            float xxPerSquare = x / freq - floor(x / freq);
            float yyPerSquare = yy;

            // whole new cord-system with half the size
            float freq2 = round(kTextureSize / (amountOfSquares / 2));
            float xxBeforeFract2 = x/freq2;
            float xx2 = xxBeforeFract2 - floor(xxBeforeFract2);
            float yyBeforeFract2 = y / freq2;
            float yy2 = yyBeforeFract2 - floor(yyBeforeFract2);

            //std::cout << noise2(xx,yy)<< "  ";

            // checker
            if((xx >= 0) && (xx < 0.5) ){
                ptex[x][y][0] = 255; // r
                ptex[x][y][1] = 255; // g
                ptex[x][y][2] = 255; // b
                if(yy2 < 0.5 && xx2 < 0.5){ //  left && bot
                    // cross
                    float crossWidth = 0.1; // (topleft to botright ||botleft to topright)
                    if( (abs(xxPerSquare + yyPerSquare) < 1 + crossWidth) && (abs(xxPerSquare + yyPerSquare) > 1 - crossWidth) || (abs(xxPerSquare - yyPerSquare) < 0 + crossWidth) && (abs(xxPerSquare - yyPerSquare) > 0 - crossWidth)){
                        ptex[x][y][0] = 64; // r
                        ptex[x][y][1] = 0; // g
                        ptex[x][y][2] = 64; // b){
                    }
                } else {
                    // noisy cross
                    float offset = 1 + noise2(xxPerSquare, yyPerSquare);
                    float crossWidth = 0.1; // (topleft to botright ||botleft to topright)
                    if( (abs(xxPerSquare + yyPerSquare) < 1 + crossWidth) && (abs(xxPerSquare + yyPerSquare) > 1 - crossWidth) || (abs(xxPerSquare - yyPerSquare) < 0 + crossWidth) && (abs(xxPerSquare - yyPerSquare) > 0 - crossWidth)){
                        //std::cout << 10 + (int) (10 * offset) << " ";
                        ptex[x][y][0] = 64 + (int) (10 * offset); // r
                        ptex[x][y][1] = (int) (80 * offset); // g
                        ptex[x][y][2] = 64 + (int) (10 * offset); // b){
                    }
                }

            } else {
                ptex[x][y][0] = 0; // r
                ptex[x][y][1] = 0; // g
                ptex[x][y][2] = 0; // b

                if(yy2 < 0.5 && xx2 > 0.5){ //  left && top
                    // plus
                    float plusWidth = 0.05; // (topleft to botright ||botleft to topright)
                    if( (xxPerSquare < 0.5 + plusWidth) && (xxPerSquare > 0.5 - plusWidth) || (yyPerSquare < 0.5 + plusWidth) && (yyPerSquare > 0.5 - plusWidth) ){
                        ptex[x][y][0] = 64; // r
                        ptex[x][y][1] = 128; // g
                        ptex[x][y][2] = 0; // b){
                    }
                } else {
                    // circle
                    float radius = 0.3;
                    if( pow(xxPerSquare - 0.5, 2) + (pow(yyPerSquare - 0.5, 2)) <= pow(radius, 2) ){
                        ptex[x][y][0] = 64; // r
                        ptex[x][y][1] = 64; // g
                        ptex[x][y][2] = 255; // b){
                    }
                }
            }
            // border
            float borderWidth = 0.1;
            if((xxPerSquare < borderWidth) || (xxPerSquare > 1-borderWidth) || (yyPerSquare < borderWidth) || (yyPerSquare > 1-borderWidth)){
                ptex[x][y][0] = 125; // r
                ptex[x][y][1] = 64; // g
                ptex[x][y][2] = 64; // b){
            }

            // show repeating pattern xx and yy
            borderWidth = 0.01;
            if((xx < borderWidth) || (xx > 1-borderWidth) || (yy < borderWidth) || (yy > 1-borderWidth)){
                ptex[x][y][0] = 0; // r
                ptex[x][y][1] = 64; // g
                ptex[x][y][2] = 64; // b){
            }

            // show repeating pattern xx2 and yy2
            borderWidth = 0.01;
            if((xx2 < borderWidth) || (xx2 > 1-borderWidth) || (yy2 < borderWidth) || (yy2 > 1-borderWidth)){
                ptex[x][y][0] = 0; // r
                ptex[x][y][1] = 0; // g
                ptex[x][y][2] = 64; // b){
            }

        }
        std::cout << "Done";

    }

}

// Globals
// Data would normally be read from files
vec3 vertices[] = { vec3(-1.0f,-1.0f,0.0f),
					vec3(1.0f,-1.0f,0.0f),
					vec3(1.0f,1.0f,0.0f),
					vec3(-1.0f,1.0f,0.0f),
					vec3(1.0f,1.0f,0.0f),
					vec3(1.0f,-1.0f,0.0f) };
vec2 texCoords[] = {vec2(0.0f,0.0f),
					vec2(1.0f,0.0f),
					vec2(1.0f,1.0f),
					vec2(0.0f,1.0f),
					vec2(1.0f,1.0f),
					vec2(1.0f,0.0f) };
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

// vertex array object
//unsigned int vertexArrayObjID;
// Texture reference
GLuint texid;
// Switch between CPU and shader generation
int displayGPUversion = 0;
// Reference to shader program
GLuint program;
// The quad
Model* quad;

void init(void)
{
    glutRepeatingTimer(1);

	// two vertex buffer objects, used for uploading the
    //	unsigned int vertexBufferObjID;
    //	unsigned int texBufferObjID;

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab1.vert", "lab1.frag");
	glUseProgram(program);
	printError("init shader");

	// Upload geometry to the GPU:

	quad = LoadDataToModel(vertices,NULL,texCoords,NULL,indices,4,6);

	// Texture unit
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0

    // Constants common to CPU and GPU
	glUniform1i(glGetUniformLocation(program, "displayGPUversion"), 0); // shader generation off
	glUniform1f(glGetUniformLocation(program, "ringDensity"), ringDensity);

	maketexture();

	// MYCODE send noise to shader


    // Upload texture
	glGenTextures(1, &texid); // texture id
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kTextureSize, kTextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, ptex);

	// End of upload of geometry
	printError("init arrays");
}

// Switch on any key
void key(unsigned char key, int x, int y)
{
	displayGPUversion = !displayGPUversion;
	glUniform1i(glGetUniformLocation(program, "displayGPUversion"), displayGPUversion); // shader generation off
	printf("Changed flag to %d\n", displayGPUversion);
	glutPostRedisplay();
}

void display(void)
{
	printError("pre display");

    // MYCODE
	glUniform1f(glGetUniformLocation(program, "time"), glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	// End Of my code

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawModel(quad, program, "in_Position", NULL, "in_TexCoord");

	printError("display");

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(kTextureSize, kTextureSize);
	glutCreateWindow ("Lab 1");
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	init ();
	glutMainLoop();
}
