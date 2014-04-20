// Homework #1 for Computer Graphic, UFL
// 
// Authoer: Ruijin Wu <ruijin@cise.ufl.edu>


#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

#ifdef __APPLE__
#include <GLEW/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include "utils.h"

// CONTSTANTS
const int window_width = 500, window_height = 500;
const char * window_title = "HW1";

const int ncpoints =  8;
const float Pi = 3.1415926536f;


struct Vertex
{
    float pos[4];
    float color[4];
};

// runtime variable
char buf[1024];


//XXX
int count = 0;	
const int n=10;				
float delta_t= 1.0/n;		

int timefrenet = 0;

Vertex frenetT[ncpoints][20];
Vertex frenetN[ncpoints][20];
Vertex frenetB[ncpoints][20];
Vertex frenetT2[ncpoints][20];
Vertex frenetN2[ncpoints][20];
Vertex frenetB2[ncpoints][20];

bool flagSubpolygon = true;
bool flagBezier =false;
bool flagView =false;

bool flagDrag =false;
bool flagFrenet =false;

Vertex cpoints[ncpoints];
Vertex cpoints2[ncpoints];
Vertex cstrips[ncpoints+1];
Vertex cstrips2[ncpoints+1];
Vertex *substrips = (Vertex *) malloc(sizeof(Vertex) * (ncpoints+1));
Vertex lel1[2*ncpoints+1];
Vertex lel2[4*ncpoints+1];
Vertex lel3[8*ncpoints+1];
Vertex lel4[16*ncpoints+1];

Vertex ctrpoints1[ncpoints][5];
Vertex ctrpoints2[ncpoints][5];
Vertex bezierpoints[ncpoints][n+1];
Vertex bezierpoints2[ncpoints][n+1];

GLuint vertex_shader, fragment_shader, program;
GLuint linesT_vbo;
GLuint linesN_vbo;
GLuint linesB_vbo;

GLuint linesT2_vbo;
GLuint linesN2_vbo;
GLuint linesB2_vbo;

GLuint position_location;
GLuint color_location;

// function Prototype 
void graphics_init();
void draw_objects(int mod);

void green(){
        for(int i = 0; i < ncpoints; i++){
                cpoints[i].color[0] = frand();
                cpoints[i].color[1] = float(i)/ncpoints*0.8+0.2;
                cpoints[i].color[2] = frand();
                cpoints[i].color[3] = 1.0f;
                
                cpoints2[i].color[0] = frand();
                cpoints2[i].color[1] = float(i)/ncpoints*0.8+0.2;
                cpoints2[i].color[2] = frand();
                cpoints2[i].color[3] = 1.0f;
         }
}

void blue(){
        for(int i = 0; i < ncpoints; i++){
                cpoints[i].color[0] = 0;
                cpoints[i].color[1] = 0;
                cpoints[i].color[2] = 1;
                cpoints[i].color[3] = 1.0f;
                
                cpoints2[i].color[0] = 0;
                cpoints2[i].color[1] = 0;
                cpoints2[i].color[2] = 1;
                cpoints2[i].color[3] = 1.0f;
         }
}

//generate cpoints, initialize 8 points
void initialize_points(){
	cpoints[0].pos[0] = -0.1;
    cpoints[0].pos[1] = 0.1;
	cpoints[0].pos[2] = 0;
	cpoints[0].pos[3] = 1.0;
		
	for(int i = 1; i < ncpoints; i++){
        cpoints[i].pos[0] = 0.8*sin(2*Pi*i/ncpoints);
       	cpoints[i].pos[1] = 0.8*cos(2*Pi*i/ncpoints);
		cpoints[i].pos[2] = 0;
		cpoints[i].pos[3] = 1.0;
	}
	
}

//generate cpoints, initialize 8 points of second picture
void initialize2_points(){
	cpoints2[0].pos[0] = 0;
    cpoints2[0].pos[1] = 0.1;
	cpoints2[0].pos[2] = -0.1;
	cpoints2[0].pos[3] = 1.0;
		
	for(int i = 1; i < ncpoints; i++){
        cpoints2[i].pos[0] = 0;
       	cpoints2[i].pos[1] = 0.8*cos(2*Pi*i/ncpoints);
		cpoints2[i].pos[2] = 0.8*sin(2*Pi*i/ncpoints);
		cpoints2[i].pos[3] = 1.0;
		}	
}

//generate cstrips, initialize polygon
void initialize_polygon(Vertex cp[]){
	for(int i = 0; i< ncpoints; i++){
		cstrips[i] = cp[i];
	}
	cstrips[ncpoints] = cstrips[0];

	for(int i = 0; i < ncpoints+1; i++){
                cstrips[i].color[0] = 1;
                cstrips[i].color[1] = 1;
                cstrips[i].color[2] = 1;
                cstrips[i].color[3] = 1.0f;
         }     
}

//cstrips2
void initialize2_polygon(Vertex cp[]){
	for(int i = 0; i< ncpoints; i++){
		cstrips2[i] = cp[i];
	}
	cstrips2[ncpoints] = cstrips2[0];

	for(int i = 0; i < ncpoints+1; i++){
                cstrips2[i].color[0] = 1;
                cstrips2[i].color[1] = 1;
                cstrips2[i].color[2] = 1;
                cstrips2[i].color[3] = 1.0f;
         }     
}

//generate ctrpoints, return control points, by cstrips
void ctrPoints1(Vertex csps[]){
	Vertex ctrpoints[ncpoints][5];
	

	
	//C[i][0]=(P[i-2]+11*P[i-1]+11*P[i]+P[i+1])/24
	//C[i][1]=(8*P[i-1]+14*P[i]+2*P[i+1])/24
	//C[i][2]=(4*P[i-1]+16*P[i]+4*P[i+1])/24
	//C[i][3]=(2*P[i-1]+14*P[i]+8*P[i+1])/24
	//C[i][4]=C[i+1][0]
	
	//generate C[0][j], C[1][j],j<-0 to 3
	ctrpoints[0][0].pos[0] = (csps[6].pos[0]+11*csps[7].pos[0]+11*csps[0].pos[0]+csps[1].pos[0])/24;
	ctrpoints[0][0].pos[1] = (csps[6].pos[1]+11*csps[7].pos[1]+11*csps[0].pos[1]+csps[1].pos[1])/24;
	ctrpoints[0][1].pos[0] = (8*csps[7].pos[0]+14*csps[0].pos[0]+2*csps[1].pos[0])/24;
	ctrpoints[0][1].pos[1] = (8*csps[7].pos[1]+14*csps[0].pos[1]+2*csps[1].pos[1])/24;	
	ctrpoints[0][2].pos[0] = (4*csps[7].pos[0]+16*csps[0].pos[0]+4*csps[1].pos[0])/24;
	ctrpoints[0][2].pos[1] = (4*csps[7].pos[1]+16*csps[0].pos[1]+4*csps[1].pos[1])/24;	
	ctrpoints[0][3].pos[0] = (2*csps[7].pos[0]+14*csps[0].pos[0]+8*csps[1].pos[0])/24;
	ctrpoints[0][3].pos[1] = (2*csps[7].pos[1]+14*csps[0].pos[1]+8*csps[1].pos[1])/24;

	ctrpoints[1][0].pos[0] = (csps[7].pos[0]+11*csps[0].pos[0]+11*csps[1].pos[0]+csps[2].pos[0])/24;
	ctrpoints[1][0].pos[1] = (csps[7].pos[1]+11*csps[0].pos[1]+11*csps[1].pos[1]+csps[2].pos[1])/24;
	ctrpoints[1][1].pos[0] = (8*csps[0].pos[0]+14*csps[1].pos[0]+2*csps[2].pos[0])/24;
	ctrpoints[1][1].pos[1] = (8*csps[0].pos[1]+14*csps[1].pos[1]+2*csps[2].pos[1])/24;	
	ctrpoints[1][2].pos[0] = (4*csps[0].pos[0]+16*csps[1].pos[0]+4*csps[2].pos[0])/24;
	ctrpoints[1][2].pos[1] = (4*csps[0].pos[1]+16*csps[1].pos[1]+4*csps[2].pos[1])/24;	
	ctrpoints[1][3].pos[0] = (2*csps[0].pos[0]+14*csps[1].pos[0]+8*csps[2].pos[0])/24;
	ctrpoints[1][3].pos[1] = (2*csps[0].pos[1]+14*csps[1].pos[1]+8*csps[2].pos[1])/24;
	
	
	//generate C[2][j],C[3][j],C[4][j],C[5][j],C[6][j],C[7][j]
	for(int i=2;i<ncpoints;i++){
		ctrpoints[i][0].pos[0] = (csps[i-2].pos[0]+11*csps[i-1].pos[0]+11*csps[i].pos[0]+csps[i+1].pos[0])/24;
		ctrpoints[i][0].pos[1] = (csps[i-2].pos[1]+11*csps[i-1].pos[1]+11*csps[i].pos[1]+csps[i+1].pos[1])/24;
		ctrpoints[i][1].pos[0] = (8*csps[i-1].pos[0]+14*csps[i].pos[0]+2*csps[i+1].pos[0])/24;
		ctrpoints[i][1].pos[1] = (8*csps[i-1].pos[1]+14*csps[i].pos[1]+2*csps[i+1].pos[1])/24;	
		ctrpoints[i][2].pos[0] = (4*csps[i-1].pos[0]+16*csps[i].pos[0]+4*csps[i+1].pos[0])/24;
		ctrpoints[i][2].pos[1] = (4*csps[i-1].pos[1]+16*csps[i].pos[1]+4*csps[i+1].pos[1])/24;	
		ctrpoints[i][3].pos[0] = (2*csps[i-1].pos[0]+14*csps[i].pos[0]+8*csps[i+1].pos[0])/24;
		ctrpoints[i][3].pos[1] = (2*csps[i-1].pos[1]+14*csps[i].pos[1]+8*csps[i+1].pos[1])/24;
		}
	//set pos[2],pos[3]
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<4;j++){
			ctrpoints[i][j].pos[2] = 0;
			ctrpoints[i][j].pos[3] = 1;
			}
		}
	//draw color
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<4;j++){
			ctrpoints[i][j].color[0] = 1;
			ctrpoints[i][j].color[1] = 0;
			ctrpoints[i][j].color[2] = 0;
			ctrpoints[i][j].color[3] = 1;
			}
		}
	ctrpoints[0][4] = ctrpoints[1][0];
	ctrpoints[1][4] = ctrpoints[2][0];
	ctrpoints[2][4] = ctrpoints[3][0];
	ctrpoints[3][4] = ctrpoints[4][0];
	ctrpoints[4][4] = ctrpoints[5][0];
	ctrpoints[5][4] = ctrpoints[6][0];
	ctrpoints[6][4] = ctrpoints[7][0];
	ctrpoints[7][4] = ctrpoints[0][0];
	
	
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<5;j++){
			ctrpoints1[i][j]=ctrpoints[i][j];
			}
		}
	}


void ctrPoints2(Vertex csps[]){
	Vertex ctrpoints[ncpoints][5];
	
	//C[i][0]=(P[i-2]+11*P[i-1]+11*P[i]+P[i+1])/24
	//C[i][1]=(8*P[i-1]+14*P[i]+2*P[i+1])/24
	//C[i][2]=(4*P[i-1]+16*P[i]+4*P[i+1])/24
	//C[i][3]=(2*P[i-1]+14*P[i]+8*P[i+1])/24
	//C[i][4]=C[i+1][0]
	
	//generate C[0][j], C[1][j],j<-0 to 3
	ctrpoints[0][0].pos[0] = (csps[6].pos[0]+11*csps[7].pos[0]+11*csps[0].pos[0]+csps[1].pos[0])/24;
	ctrpoints[0][0].pos[1] = (csps[6].pos[1]+11*csps[7].pos[1]+11*csps[0].pos[1]+csps[1].pos[1])/24;
	ctrpoints[0][1].pos[0] = (8*csps[7].pos[0]+14*csps[0].pos[0]+2*csps[1].pos[0])/24;
	ctrpoints[0][1].pos[1] = (8*csps[7].pos[1]+14*csps[0].pos[1]+2*csps[1].pos[1])/24;	
	ctrpoints[0][2].pos[0] = (4*csps[7].pos[0]+16*csps[0].pos[0]+4*csps[1].pos[0])/24;
	ctrpoints[0][2].pos[1] = (4*csps[7].pos[1]+16*csps[0].pos[1]+4*csps[1].pos[1])/24;	
	ctrpoints[0][3].pos[0] = (2*csps[7].pos[0]+14*csps[0].pos[0]+8*csps[1].pos[0])/24;
	ctrpoints[0][3].pos[1] = (2*csps[7].pos[1]+14*csps[0].pos[1]+8*csps[1].pos[1])/24;

	ctrpoints[1][0].pos[0] = (csps[7].pos[0]+11*csps[0].pos[0]+11*csps[1].pos[0]+csps[2].pos[0])/24;
	ctrpoints[1][0].pos[1] = (csps[7].pos[1]+11*csps[0].pos[1]+11*csps[1].pos[1]+csps[2].pos[1])/24;
	ctrpoints[1][1].pos[0] = (8*csps[0].pos[0]+14*csps[1].pos[0]+2*csps[2].pos[0])/24;
	ctrpoints[1][1].pos[1] = (8*csps[0].pos[1]+14*csps[1].pos[1]+2*csps[2].pos[1])/24;	
	ctrpoints[1][2].pos[0] = (4*csps[0].pos[0]+16*csps[1].pos[0]+4*csps[2].pos[0])/24;
	ctrpoints[1][2].pos[1] = (4*csps[0].pos[1]+16*csps[1].pos[1]+4*csps[2].pos[1])/24;	
	ctrpoints[1][3].pos[0] = (2*csps[0].pos[0]+14*csps[1].pos[0]+8*csps[2].pos[0])/24;
	ctrpoints[1][3].pos[1] = (2*csps[0].pos[1]+14*csps[1].pos[1]+8*csps[2].pos[1])/24;
	
	
	//generate C[2][j],C[3][j],C[4][j],C[5][j],C[6][j],C[7][j]
	for(int i=2;i<ncpoints;i++){
		ctrpoints[i][0].pos[0] = (csps[i-2].pos[0]+11*csps[i-1].pos[0]+11*csps[i].pos[0]+csps[i+1].pos[0])/24;
		ctrpoints[i][0].pos[1] = (csps[i-2].pos[1]+11*csps[i-1].pos[1]+11*csps[i].pos[1]+csps[i+1].pos[1])/24;
		ctrpoints[i][1].pos[0] = (8*csps[i-1].pos[0]+14*csps[i].pos[0]+2*csps[i+1].pos[0])/24;
		ctrpoints[i][1].pos[1] = (8*csps[i-1].pos[1]+14*csps[i].pos[1]+2*csps[i+1].pos[1])/24;	
		ctrpoints[i][2].pos[0] = (4*csps[i-1].pos[0]+16*csps[i].pos[0]+4*csps[i+1].pos[0])/24;
		ctrpoints[i][2].pos[1] = (4*csps[i-1].pos[1]+16*csps[i].pos[1]+4*csps[i+1].pos[1])/24;	
		ctrpoints[i][3].pos[0] = (2*csps[i-1].pos[0]+14*csps[i].pos[0]+8*csps[i+1].pos[0])/24;
		ctrpoints[i][3].pos[1] = (2*csps[i-1].pos[1]+14*csps[i].pos[1]+8*csps[i+1].pos[1])/24;
		}
	//set pos[2],pos[3]
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<4;j++){
			ctrpoints[i][j].pos[2] = 0;
			ctrpoints[i][j].pos[3] = 1;
			}
		}
	//draw color
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<4;j++){
			ctrpoints[i][j].color[0] = 1;
			ctrpoints[i][j].color[1] = 0;
			ctrpoints[i][j].color[2] = 0;
			ctrpoints[i][j].color[3] = 1;
			}
		}
	ctrpoints[0][4] = ctrpoints[1][0];
	ctrpoints[1][4] = ctrpoints[2][0];
	ctrpoints[2][4] = ctrpoints[3][0];
	ctrpoints[3][4] = ctrpoints[4][0];
	ctrpoints[4][4] = ctrpoints[5][0];
	ctrpoints[5][4] = ctrpoints[6][0];
	ctrpoints[6][4] = ctrpoints[7][0];
	ctrpoints[7][4] = ctrpoints[0][0];
	
	
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<5;j++){
			ctrpoints2[i][j]=ctrpoints[i][j];
			}
		}
	}

//get a point on Bezier using de Casteljau's algorithm
Vertex pointOnBezier1(int j, int i, int k, float t){
	if(k == 0){
		return ctrpoints1[j][i];
		}else{
			Vertex point;
			Vertex iPrev = pointOnBezier1(j,i,k-1,t);
			Vertex iNext = pointOnBezier1(j,i+1,k-1,t);
			
			point.pos[0] = (1.0-t)*iPrev.pos[0] + t*iNext.pos[0];
			point.pos[1] = (1.0-t)*iPrev.pos[1] + t*iNext.pos[1];
			
			return point;
			}
	}

Vertex pointOnBezier2(int j, int i, int k, float t){
	if(k == 0){
		return ctrpoints2[j][i];
		}else{
			Vertex point;
			Vertex iPrev = pointOnBezier2(j,i,k-1,t);
			Vertex iNext = pointOnBezier2(j,i+1,k-1,t);
			
			point.pos[0] = (1.0-t)*iPrev.pos[0] + t*iNext.pos[0];
			point.pos[1] = (1.0-t)*iPrev.pos[1] + t*iNext.pos[1];
			
			return point;
			}
	}
	
//generate bezierpoints on Bezier curve by control points, k degree
void genBezierPoints(){
	float t=0;
	for(int j=0;j<ncpoints;j++){
		t=0;
		for(int i=0;i<n+1;i++){
			bezierpoints[j][i] = pointOnBezier1(j,0,4,t);
			t = t+ delta_t;
			}
		}
	for(int j=0;j<ncpoints;j++){
		for(int i=0;i<n+1;i++){
			bezierpoints[j][i].pos[2] = 0;
			bezierpoints[j][i].pos[3] = 1;
			}
		}
		
	for(int j=0;j<ncpoints;j++){
		for(int i=0;i<n+1;i++){
			bezierpoints[j][i].color[0] = 1;
			bezierpoints[j][i].color[1] = 1;
			bezierpoints[j][i].color[2] = 0;
			bezierpoints[j][i].color[3] = 1;
			}
		}
	}
	
void genBezierPoints2(){
	float t=0;
	for(int j=0;j<ncpoints;j++){
		t=0;
		for(int i=0;i<n+1;i++){
			bezierpoints2[j][i] = pointOnBezier2(j,0,4,t);
			t = t+ delta_t;
			}
		}
	for(int j=0;j<ncpoints;j++){
		for(int i=0;i<n+1;i++){
			bezierpoints2[j][i].pos[2] = 0;
			bezierpoints2[j][i].pos[3] = 1;
			}
		}
		
	for(int j=0;j<ncpoints;j++){
		for(int i=0;i<n+1;i++){
			bezierpoints2[j][i].color[0] = 1;
			bezierpoints2[j][i].color[1] = 1;
			bezierpoints2[j][i].color[2] = 0;
			bezierpoints2[j][i].color[3] = 1;
			}
		}
	}

//subdivision
void subdivision(Vertex s1[], Vertex s2[],int n){
		//s1[n],s2[2n-1]
		//e.g. level1 p0 = (old p6 + 10* old p7 + 5* old p0)/16
		s2[0].pos[0] = (s1[n-3].pos[0]+10*s1[n-2].pos[0]+5*s1[0].pos[0])/16;
		s2[0].pos[1] = (s1[n-3].pos[1]+10*s1[n-2].pos[1]+5*s1[0].pos[1])/16;
		s2[1].pos[0] = (5*s1[n-2].pos[0]+10*s1[0].pos[0]+s1[1].pos[0])/16;
 		s2[1].pos[1] = (5*s1[n-2].pos[1]+10*s1[0].pos[1]+s1[1].pos[1])/16;
 		s2[2].pos[0] = (s1[n-2].pos[0]+10*s1[0].pos[0]+5*s1[1].pos[0])/16;
 		s2[2].pos[1] = (s1[n-2].pos[1]+10*s1[0].pos[1]+5*s1[1].pos[1])/16;
 		s2[3].pos[0] = (5*s1[0].pos[0]+10*s1[1].pos[0]+s1[2].pos[0])/16;
 		s2[3].pos[1] = (5*s1[0].pos[1]+10*s1[1].pos[1]+s1[2].pos[1])/16;
		 
		//i<- 2 to 7
		for(int i=2; i<n-1;i++){
			s2[2*i].pos[0] = (s1[i-2].pos[0]+10*s1[i-1].pos[0]+5*s1[i].pos[0])/16;
			s2[2*i].pos[1] = (s1[i-2].pos[1]+10*s1[i-1].pos[1]+5*s1[i].pos[1])/16;
			s2[2*i+1].pos[0] = (5*s1[i-1].pos[0]+10*s1[i].pos[0]+s1[i+1].pos[0])/16;
			s2[2*i+1].pos[1] = (5*s1[i-1].pos[1]+10*s1[i].pos[1]+s1[i+1].pos[1])/16;
			}
		//i<- 0 to 16, set pos[2],pos[3],and color
		for(int i=0; i<2*n-1;i++){
			s2[i].pos[2] = 0;
			s2[i].pos[3] = 1;
			s2[i].color[0] = 0;
			s2[i].color[1] = 1;
			s2[i].color[2] = 0;
			s2[i].color[3] = 1;
			}
		s2[2*n-2] = s2[0];
	}

//initialize level1 to 4
void smooth_curve(){

	subdivision(cstrips,lel1,ncpoints+1);	//generate level1
	subdivision(lel1,lel2,2*ncpoints+1);	//generate level2
	subdivision(lel2,lel3,4*ncpoints+1);    //generate level3
	subdivision(lel3,lel4,8*ncpoints+1);    //generate level4

	}

//malloc substrips by steps
void genstripsbyCount(){
	free(substrips);
	switch(count){
			case 1:
				substrips = (Vertex *) malloc(sizeof(Vertex) * (2*ncpoints+1));	
				for(int i=0;i<2*ncpoints+1;i++){
					substrips[i] = lel1[i];
				}
				break;
			case 2:
				substrips = (Vertex *) malloc(sizeof(Vertex) * (4*ncpoints+1));	
				for(int i=0;i<4*ncpoints+1;i++){
					substrips[i] = lel2[i];
				}
				break;
			case 3:
				substrips = (Vertex *) malloc(sizeof(Vertex) * (8*ncpoints+1));	
				for(int i=0;i<8*ncpoints+1;i++){
					substrips[i] = lel3[i];
				}
				break;
			case 4:
				substrips = (Vertex *) malloc(sizeof(Vertex) * (16*ncpoints+1));	
				for(int i=0;i<16*ncpoints+1;i++){
					substrips[i] = lel4[i];
				}
				break;
			}
}

//generate frenet vectors
void genTpoints(){
	float vectorT[3];
	Vertex m1,m2;
	float unitlength;
	for(int i = 0; i<ncpoints;i++){
		float t = 0;
		for(int m = 0; m<10; m++){
			for(int k = 0; k <3; k++) {
				frenetT[i][m*2].pos[k]=frenetN[i][m*2].pos[k]=ctrpoints1[i][0].pos[k]*pow((1-t),4)+4*ctrpoints1[i][1].pos[k]*pow((1-t),3)*t+6*ctrpoints1[i][2].pos[k]*pow((1-t),2)*t*t+4*ctrpoints1[i][3].pos[k]*(1-t)*t*t*t+ctrpoints1[i][4].pos[k]*pow(t,4);		
				t += 0.001;
				m1.pos[k]=ctrpoints1[i][0].pos[k]*pow((1-t),4)+4*ctrpoints1[i][1].pos[k]*pow((1-t),3)*t+6*ctrpoints1[i][2].pos[k]*pow((1-t),2)*t*t+4*ctrpoints1[i][3].pos[k]*(1-t)*t*t*t+ctrpoints1[i][4].pos[k]*pow(t,4);
				vectorT[k]=(m1.pos[k]-frenetT[i][m*2].pos[k])/0.001;
				t -= 0.001;
				}
			unitlength = sqrt(vectorT[0]*vectorT[0]+vectorT[1]*vectorT[1]+vectorT[2]*vectorT[2]);
			for(int p = 0; p<3;p++){
				frenetT[i][m*2+1].pos[p] = frenetT[i][m*2].pos[p] + 0.2*vectorT[p]/unitlength;
				}
			frenetT[i][m*2].pos[3] = frenetT[i][m*2+1].pos[3] = 1;	
			t+=delta_t;
		}
		
		for(int a = 0;a<20; a++){
			frenetT[i][a].color[0] = 1;
			frenetT[i][a].color[1] = 0;
			frenetT[i][a].color[2] = 0;
			frenetT[i][a].color[3] = 1;
			}
		}		  
		glGenBuffers(1, &linesT_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, linesT_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*20) * sizeof(Vertex), &frenetT[0][0], GL_STATIC_DRAW);

	}

void genBpoints(){
	
	float vectorT[3],vectorT1[3],vectorB[3];
	Vertex m1,m2;
	float t1,t2;
	float unitlength;
	for(int i=0;i<ncpoints;i++){
		float t = 0.1;
		for(int m = 1; m<10; m++){
			for(int k = 0; k <3; k++) {				
				frenetB[i][m*2].pos[k]=ctrpoints1[i][0].pos[k]*pow((1-t),4)+4*ctrpoints1[i][1].pos[k]*pow((1-t),3)*t+6*ctrpoints1[i][2].pos[k]*pow((1-t),2)*t*t+4*ctrpoints1[i][3].pos[k]*(1-t)*t*t*t+ctrpoints1[i][4].pos[k]*pow(t,4);		
				t1 = t+0.001;
				m1.pos[k]=ctrpoints1[i][0].pos[k]*pow((1-t1),4)+4*ctrpoints1[i][1].pos[k]*pow((1-t1),3)*t1+6*ctrpoints1[i][2].pos[k]*pow((1-t1),2)*t1*t1+4*ctrpoints1[i][3].pos[k]*(1-t1)*t1*t1*t1+ctrpoints1[i][4].pos[k]*pow(t1,4);
				t2 = t-0.001;
				m2.pos[k]=ctrpoints1[i][0].pos[k]*pow((1-t2),4)+4*ctrpoints1[i][1].pos[k]*pow((1-t2),3)*t2+6*ctrpoints1[i][2].pos[k]*pow((1-t2),2)*t2*t2+4*ctrpoints1[i][3].pos[k]*(1-t2)*t2*t2*t2+ctrpoints1[i][4].pos[k]*pow(t2,4);							
//				printf("m1=%f,m2=%f,testN=%f,vectorN=%f,t=%f \n",m1.pos[k],m2.pos[k],testN[i][m*2].pos[k],vectorN[k],t);
				vectorT[k]=(m1.pos[k]-frenetB[i][m*2].pos[k])/0.001;
				vectorT1[k]=(m2.pos[k]-frenetB[i][m*2].pos[k])/0.001;
				}
			vectorB[0] = vectorT[1]*vectorT1[2] - vectorT[2]*vectorT1[1];
			vectorB[1] = vectorT[2]*vectorT1[0] - vectorT[0]*vectorT1[2];
			vectorB[2] = vectorT[0]*vectorT1[1] - vectorT[1]*vectorT1[0];
			unitlength = sqrt(vectorB[0]*vectorB[0]+vectorB[1]*vectorB[1]+vectorB[2]*vectorB[2]);
			t+=delta_t;	
			for(int p = 0; p<3;p++){
				frenetB[i][m*2+1].pos[p] = frenetB[i][m*2].pos[p] + 0.2*vectorB[p]/unitlength;
				}
			frenetB[i][m*2].pos[3] = frenetB[i][m*2+1].pos[3] = 1;								
			}
		//blue
		for(int b = 0;b<20; b++){
			frenetB[i][b].color[0] = 0;
			frenetB[i][b].color[1] = 0;
			frenetB[i][b].color[2] = 1;
			frenetB[i][b].color[3] = 1;
			}	
		}
		
//frenetB[i][0]		
		for(int i=0;i<ncpoints;i++){
			int j;
			if(i==0){
				j=7;
				}else{
					j=i-1;
					}
			for(int k = 0; k <3; k++) {			
				float tt=0.001;
				frenetB[i][0].pos[k]=ctrpoints1[i][0].pos[k];			
				m1.pos[k]=ctrpoints1[i][0].pos[k]*pow((1-tt),4)+4*ctrpoints1[i][1].pos[k]*pow((1-tt),3)*tt+6*ctrpoints1[i][2].pos[k]*pow((1-tt),2)*tt*tt+4*ctrpoints1[i][3].pos[k]*(1-tt)*tt*tt*tt+ctrpoints1[i][4].pos[k]*pow(tt,4);
				m2.pos[k]=ctrpoints1[j][0].pos[k]*pow(tt,4)+4*ctrpoints1[j][1].pos[k]*pow(tt,3)*(1-tt)+6*ctrpoints1[j][2].pos[k]*pow(tt,2)*(1-tt)*(1-tt)+4*ctrpoints1[j][3].pos[k]*tt*(1-tt)*(1-tt)*(1-tt)+ctrpoints1[j][4].pos[k]*pow((1-tt),4);				
				vectorT[k]=(m1.pos[k]-frenetB[i][0].pos[k])/0.001;
				vectorT1[k]=(m2.pos[k]-frenetB[i][0].pos[k])/0.001;
				}
				vectorB[0] = vectorT[1]*vectorT1[2] - vectorT[2]*vectorT1[1];
				vectorB[1] = vectorT[2]*vectorT1[0] - vectorT[0]*vectorT1[2];
				vectorB[2] = vectorT[0]*vectorT1[1] - vectorT[1]*vectorT1[0];
				unitlength = sqrt(vectorB[0]*vectorB[0]+vectorB[1]*vectorB[1]+vectorB[2]*vectorB[2]);
			for(int p = 0; p<3;p++){
				frenetB[i][1].pos[p] = frenetB[i][0].pos[p] + 0.2*vectorB[p]/unitlength;
				}
			frenetB[i][0].pos[3] = frenetB[i][1].pos[3] = 1;	
			}
		
//			printf("x0=%f,y0=%f,z0=%f,x1=%f,y1=%f,z1=%f \n",frenetB[0][0].pos[0],frenetB[0][0].pos[1],frenetB[0][0].pos[2],frenetB[0][1].pos[0],frenetB[0][1].pos[1],frenetB[0][1].pos[2]);
//			printf("x0=%f,y0=%f,z0=%f,x1=%f,y1=%f,z1=%f \n",frenetB[1][6].pos[0],frenetB[1][6].pos[1],frenetB[1][6].pos[2],frenetB[1][7].pos[0],frenetB[1][7].pos[1],frenetB[1][7].pos[2]);
		glGenBuffers(1, &linesB_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, linesB_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*20) * sizeof(Vertex), &frenetB[0][0], GL_STATIC_DRAW);
	}
	
void genNpoints(){
		float vectorN[3],vectorN1[3],vectorN2[3];
		Vertex m1,m2;
		float unitlength;
		for(int i = 0; i<ncpoints; i++){
			for(int m = 0;m<10;m++){
				for(int k=0;k<3;k++){
					vectorN1[k] = frenetT[i][m*2].pos[k] - frenetT[i][m*2+1].pos[k];
					vectorN2[k] = frenetB[i][m*2+1].pos[k] - frenetB[i][m*2].pos[k];				 
					}
				vectorN[0] = vectorN1[1]*vectorN2[2] - vectorN1[2]*vectorN2[1];
				vectorN[1] = vectorN1[2]*vectorN2[0] - vectorN1[0]*vectorN2[2];
				vectorN[2] = vectorN1[0]*vectorN2[1] - vectorN1[1]*vectorN2[0];
				unitlength = sqrt(vectorN[0]*vectorN[0]+vectorN[1]*vectorN[1]+vectorN[2]*vectorN[2]);
				for(int p = 0; p<3;p++){
					frenetN[i][m*2+1].pos[p] = frenetN[i][m*2].pos[p] + 0.2*vectorN[p]/unitlength;
					}
				frenetN[i][m*2].pos[3] = frenetN[i][m*2+1].pos[3] = 1;
				}
			for(int b = 0;b<20; b++){
				frenetN[i][b].color[0] = 0;
				frenetN[i][b].color[1] = 1;
				frenetN[i][b].color[2] = 0;
				frenetN[i][b].color[3] = 1;
				}				
			}
			
		glGenBuffers(1, &linesN_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, linesN_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*20) * sizeof(Vertex), &frenetN[0][0], GL_STATIC_DRAW);
	}

//gen TBN y-z view
void genTBN2view(){
	for(int i=0;i<ncpoints;i++){
		for(int j=0;j<20;j++){
			frenetT2[i][j].pos[0] = frenetT[i][j].pos[2];
			frenetT2[i][j].pos[1] = frenetT[i][j].pos[1];
			frenetT2[i][j].pos[2] = frenetT[i][j].pos[0];
			frenetT2[i][j].pos[3] = frenetT[i][j].pos[3];
			
			frenetB2[i][j].pos[0] = frenetB[i][j].pos[2];
			frenetB2[i][j].pos[1] = frenetB[i][j].pos[1];
			frenetB2[i][j].pos[2] = frenetB[i][j].pos[0];
			frenetB2[i][j].pos[3] = frenetB[i][j].pos[3];
			
			frenetN2[i][j].pos[0] = frenetN[i][j].pos[2];
			frenetN2[i][j].pos[1] = frenetN[i][j].pos[1];
			frenetN2[i][j].pos[2] = frenetN[i][j].pos[0];
			frenetN2[i][j].pos[3] = frenetN[i][j].pos[3];
			
			frenetT2[i][j].color[0] = 1;
			frenetT2[i][j].color[1] = 0;
			frenetT2[i][j].color[2] = 0;
			frenetT2[i][j].color[3] = 1;
			
			frenetB2[i][j].color[0] = 0;
			frenetB2[i][j].color[1] = 0;
			frenetB2[i][j].color[2] = 1;
			frenetB2[i][j].color[3] = 1;
			
			frenetN2[i][j].color[0] = 0;
			frenetN2[i][j].color[1] = 1;
			frenetN2[i][j].color[2] = 0;
			frenetN2[i][j].color[3] = 1;
			}
		}
		
		glGenBuffers(1, &linesT2_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, linesT2_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*20) * sizeof(Vertex), &frenetT2[0][0], GL_STATIC_DRAW);
		
		glGenBuffers(1, &linesN2_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, linesN2_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*20) * sizeof(Vertex), &frenetN2[0][0], GL_STATIC_DRAW);
		
		glGenBuffers(1, &linesB2_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, linesB2_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*20) * sizeof(Vertex), &frenetB2[0][0], GL_STATIC_DRAW);
	}

// return the index of the picked point
int color_code_pick(int x,int y){
      
	green();

    // draw the scene
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_objects(1);
	draw_objects(2);
	glFlush();

    // read back the green channel of pixel under the cursor into data
    // data is in range [0,255]
	GLubyte data;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glReadPixels(x,viewport[3]-y,1,1,GL_GREEN,GL_UNSIGNED_BYTE,&data);
    printf("color: %d\n", data);
     



    // TODO: identify the selected point and return its index
    // Identify the clicked point based on the color 
       if(data == 0){
		return -1;
       }else{
		int idx;
		idx = ceil((float(data)/256 - 0.2)*10);
		return idx;
       }

     return -1;  
}

// MOUSE handling *******************************************

int last_x, last_y;
int selected_idx = -1;

void mouse(int button, int state, int x, int y ){
	if(state == GLUT_DOWN){
		selected_idx = color_code_pick(x,y);
		printf("pick point #%d\n ", selected_idx);
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
			flagDrag = true;	
		}		
	}else{
		flagDrag = false;
		}

    last_x = x;
    last_y = y;
    
}


void motion( int x, int y){
    GLint viewport[4];
    float dx,dy;
    
	glGetIntegerv(GL_VIEWPORT,viewport);
    
    dx = (x - last_x)/(float)viewport[2] * 2;
    dy = -(y - last_y)/(float)viewport[3] * 2;

    // TODO: move the picked point
    // change the position of the selected point 
    // (dy, dy) is the mouse movement transferred
    // back from screen space to object space 

    last_x = x;
    last_y = y;
	
	if(selected_idx!=-1){
    cpoints[selected_idx].pos[0] = cpoints[selected_idx].pos[0] + dx;
    cpoints[selected_idx].pos[1] = cpoints[selected_idx].pos[1] + dy;
    
    cpoints2[selected_idx].pos[1] = cpoints2[selected_idx].pos[1] + dy;
    if(flagDrag==true ){
		initialize_polygon(cpoints);
		smooth_curve();
		ctrPoints1(cstrips);
		genBezierPoints();
		
		initialize2_polygon(cpoints2);
		ctrPoints2(cstrips2);
		genBezierPoints2();
		
		genTpoints();
		genBpoints();
		genNpoints();
		genTBN2view();
		}
		
	glutPostRedisplay();
	}
}


// KEYBOARD handling *******************************************

void keyboard(unsigned char key, int x, int y){
//	if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
//		cout<<"shift active"<<endl;
//		flagDrag = true;
//	}else{
//		flagDrag = false;
		
	switch (key) {
    case 49:
		if(count<4){
			count++;	
		}else{
			count=0;	
		}
		glutPostRedisplay();
		break;
    case 50:
        if(flagBezier == false){
			flagBezier = true;
			cout<<"Bezier Curves show"<<endl;
			}else{
				flagBezier = false;
				cout<<"Bezier Curves hide"<<endl;
				}
		glutPostRedisplay();
        break;
    case 51:
        if(flagView == false){
			flagView = true;
			}else{
				flagView = false;
				}
		glutPostRedisplay();
        break;
    case 52:
        if(flagFrenet == false){
			flagFrenet = true;
			}else{
				flagFrenet = false;
				}
		glutPostRedisplay();
        break;
    case 'r':
        graphics_init();
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    }
//		}
	
}

// DISPLAY and RENDERING functions *************************

//mod=1,x-y view; mod=2,y-z view
void draw_objects(int mod){

    GLuint vao;
    GLuint points_vbo;

    GLuint polygon_vbo;    
    GLuint subpolygon_vbo;
    GLuint ctrpoints_vbo;
    GLuint bezierpoints_vbo;

    // specify the shaders we want to use
    glUseProgram(program);

    // get the input variable location in this shader
    position_location = glGetAttribLocation(program, "in_position");
    color_location = glGetAttribLocation(program, "in_color");

    // create and bind a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // enable the input locations we want to use
    glEnableVertexAttribArray(position_location);
    glEnableVertexAttribArray(color_location);

    
	if(mod == 1){
		
	// draw points

    // generate and bind a vertex buffer to hold the vertex data on GPU
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);

    // initialize the vertex buffer with the vertex data
    glBufferData(GL_ARRAY_BUFFER, ncpoints * sizeof(Vertex), &cpoints[0] , GL_STATIC_DRAW);

    // specify the input data format
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

    // draw points
    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, ncpoints);

    //XXX
    //draw polygon

    glGenBuffers(1, &polygon_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
    glBufferData(GL_ARRAY_BUFFER, (ncpoints+1) * 4 * sizeof(Vertex), &cstrips[0], GL_STATIC_DRAW);

    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

    glDrawArrays(GL_LINE_STRIP, 0, ncpoints+1);
	
    //draw subpolygon
    if(count>0 && flagSubpolygon == true){
		genstripsbyCount();
		glGenBuffers(1, &subpolygon_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, subpolygon_vbo);
		glBufferData(GL_ARRAY_BUFFER, ((int)pow(2,count)*ncpoints+1) * 4 * sizeof(Vertex), &substrips[0], GL_STATIC_DRAW);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINE_STRIP, 0, ((int)pow(2,count)*ncpoints+1));
     }
	
	//draw control points and Bezier curve
		if(flagBezier == true){
		glGenBuffers(1, &ctrpoints_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, ctrpoints_vbo);
		glBufferData(GL_ARRAY_BUFFER, (8*5) * sizeof(Vertex), &ctrpoints1[0][0] , GL_STATIC_DRAW);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glPointSize(4);
		glDrawArrays(GL_POINTS, 0, (8*5));

  
    	glGenBuffers(1, &bezierpoints_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, bezierpoints_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*(n+1)) * sizeof(Vertex), &bezierpoints[0][0] , GL_STATIC_DRAW);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glPointSize(3);
		glDrawArrays(GL_LINE_STRIP, 0, (ncpoints*(n+1)));
		}
		
		if(flagFrenet == true){		
		glBindBuffer(GL_ARRAY_BUFFER, linesT_vbo);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINES, timefrenet, 2);
		
		glBindBuffer(GL_ARRAY_BUFFER, linesB_vbo);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINES, timefrenet, 2);
		
		glBindBuffer(GL_ARRAY_BUFFER, linesN_vbo);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINES, timefrenet, 2);
		}
	}else{
			// draw points

    // generate and bind a vertex buffer to hold the vertex data on GPU
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);

    // initialize the vertex buffer with the vertex data
    glBufferData(GL_ARRAY_BUFFER, ncpoints * sizeof(Vertex), &cpoints2[0] , GL_STATIC_DRAW);

    // specify the input data format
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

    // draw points
    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, ncpoints);

    //XXX
    //draw polygon

    glGenBuffers(1, &polygon_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, polygon_vbo);
    glBufferData(GL_ARRAY_BUFFER, (ncpoints+1) * 4 * sizeof(Vertex), &cstrips2[0], GL_STATIC_DRAW);

    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

    glDrawArrays(GL_LINE_STRIP, 0, ncpoints+1);
	
	
	//draw control points and Bezier curve
		if(flagBezier == true){
		glGenBuffers(1, &ctrpoints_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, ctrpoints_vbo);
		glBufferData(GL_ARRAY_BUFFER, (8*5) * sizeof(Vertex), &ctrpoints2[0][0] , GL_STATIC_DRAW);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glPointSize(4);
		glDrawArrays(GL_POINTS, 0, (8*5));

  
    	glGenBuffers(1, &bezierpoints_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, bezierpoints_vbo);
		glBufferData(GL_ARRAY_BUFFER, (ncpoints*(n+1)) * sizeof(Vertex), &bezierpoints2[0][0] , GL_STATIC_DRAW);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glPointSize(3);
		glDrawArrays(GL_LINE_STRIP, 0, (ncpoints*(n+1)));
		}
		
		if(flagFrenet == true){
		
		glBindBuffer(GL_ARRAY_BUFFER, linesT2_vbo);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINES, timefrenet, 2);
		
		glBindBuffer(GL_ARRAY_BUFFER, linesB2_vbo);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINES, timefrenet, 2);
		
		glBindBuffer(GL_ARRAY_BUFFER, linesN2_vbo);
		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glDrawArrays(GL_LINES, timefrenet, 2);
		
		}
		
		
	}
    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Delete VAO and VBO
    glDeleteBuffers(1, &points_vbo);
    glDeleteBuffers(1, &polygon_vbo);
    glDeleteBuffers(1, &subpolygon_vbo);
    glDeleteBuffers(1, &ctrpoints_vbo);
    glDeleteBuffers(1, &bezierpoints_vbo);

    glDeleteVertexArrays(1, &vao);
  
}

void timer(int value){
    // do the update you want, e.g. update the frame position
	if(timefrenet == ncpoints*20){
		timefrenet=0;
		}
	timefrenet+=2;
    // reset timer to be trigger every 60 ms
    glutTimerFunc(60, &timer, 0);

    // redraw the screen
    glutPostRedisplay();
}

void display(){

	blue();

    // Clear Viewport

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(flagView == true){
	//top, xy view
    glViewport(window_width/4, window_height/2, window_width/2, window_height/2); 
    		
    draw_objects(1);

    //bottom, yz view
    glViewport(window_width/4, 0, window_width/2, window_height/2);
	
    draw_objects(2);
		}else{

		glViewport(0, 0, window_width, window_height);
		
		draw_objects(1);
	}
    glFlush();
    glutSwapBuffers();
}

void reshape(int width, int height){
    // Clip the view port to match our ratio
    glViewport(0,0, width, height); 
    glutPostRedisplay();
}

void graphics_init(){

    // init vertex shader
    cout << "compiling vertex shader" << endl;
    read_shader_source_code("vs.glsl", buf, 1024);
    int vertex_shader_source_length = strlen(buf);
    const char *vertex_shader_sources[] = { buf };

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_sources, &vertex_shader_source_length);
    glCompileShader(vertex_shader);
    check_shader_compile_error(vertex_shader);
    
    // init fragment shader 
    cout << "compiling fragment shader" << endl;
    read_shader_source_code("fs.glsl", buf, 1024); 
    int fragment_shader_source_length = strlen(buf);
    const char *fragment_shader_sources[] = { buf };
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_sources, &fragment_shader_source_length);
    glCompileShader(fragment_shader);
    check_shader_compile_error(fragment_shader);

    // init program
    cout << "linking program" << endl;
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    check_program_linking_error(program);
  
    // enable depth test
    glEnable(GL_DEPTH_TEST);


}

void print_info(){
    fprintf(
        stdout,
        "INFO: OpenGL Version: %s\n",
        glGetString(GL_VERSION)
        );
    fprintf(stdout, "INFO: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    if(glewIsSupported("GL_ARB_debug_output"))
    {
        fprintf(stdout, "INFO: Support ARB_debug_output\n");
    }
    else
    {
        fprintf(stdout, "INFO: Not support ARB_debug_output\n");
    }
}

int main(int argc,char * argv[]){

    // Setup GLUT
    glutInit(&argc,argv);

    glutInitContextVersion (3, 0);
    glutInitContextFlags (GLUT_DEBUG);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB     | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize (window_width,window_height);
    glutCreateWindow("First step - OpenGL 3");
  
    // Setup GLEW
    glewExperimental = true;
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    print_info();
  
    // Initialize OpenGL
    graphics_init ();
    
		initialize_points();
		initialize_polygon(cpoints);
		smooth_curve();
		ctrPoints1(cstrips);
		genBezierPoints();
		
		initialize2_points();
		initialize2_polygon(cpoints2);
		ctrPoints2(cstrips2);
		genBezierPoints2();
		
	genTpoints();
	genBpoints();
	genNpoints();
	genTBN2view();
    // set up callbacks
    glutReshapeFunc(&reshape);
    glutDisplayFunc(&display);
    glutKeyboardFunc(&keyboard);
    glutMouseFunc(&mouse);
    glutMotionFunc(&motion);
	glutTimerFunc(60, &timer, 0);
    // main loop
    glutMainLoop();
    return 0;
}


