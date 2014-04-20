// Homework #2 for Computer Graphic, UFL
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
#include <vector>

using namespace std;

#ifdef __APPLE__
#include <GLEW/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include "utils.h"
#include "debuglib.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



// CONTSTANTS
const float PI = 3.1415926;
const int window_width = 500, window_height = 500;
const char * window_title = "HW2";
glm::vec4 lightPosition = glm::vec4(10.0f,10.0f,10.0f,1.0f);
glm::vec4 lightDiffuse = glm::vec4(1.0f,1.0f,1.0f,1.0f);
glm::vec4 lightSpecular = glm::vec4(1.0f,1.0f,1.0f,1.0f);

bool cameraFlag = false;
bool selectBase = false;
bool selectTop = false;
bool selectFirstArm = false;
bool selectSecondArm = false;
bool selectPen = false;
int lightOn = 1; //1 turn off, 0 turn on

float angle_blue = 0;
float angle_red = 0;
float sphere_x = 0;
float sphere_y = 0;
float sphere_z = 0;

float angle_top = 0;
float angle_firstarm = 0;
float angle_secondarm = 0;
float angle_X = 0;
float angle_Y = 0;
float angle_pen_longitude = 0;
float angle_pen_latitude = 0;

struct Vertex
{
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec4 normal;
};


// runtime variable
char buf[1024];
vector<Vertex> grid_vertices;

vector<Vertex> test_vertices;
glm::mat4 Model_test = glm::mat4(1.0f);
vector<Vertex> testN_vertices;

vector<Vertex> base_vertices;
vector<Vertex> top_vertices;
vector<Vertex> firstarm_vertices;
vector<Vertex> joint_vertices;
vector<Vertex> secondarm_vertices;
vector<Vertex> pen_vertices;
vector<Vertex> buttom_vertices;

Vertex cube[8];
Vertex cube_triangles_points[36]; 
Vertex disk1[360];
Vertex disk2[360];
Vertex diskCenter[2];
Vertex disk_triangles_points[360*3*2*2];
Vertex stick_triangles_points[360*3*2*2];
Vertex littlecube_triangles_points[360*3*2*2];

glm::mat4 Model_base, Model_top, Model_firstarm, Model_joint, Model_secondarm, Model_pen, Model_buttom;
glm::mat4 NormalMatrix_base, NormalMatrix_top, NormalMatrix_firstarm, NormalMatrix_joint, NormalMatrix_secondarm, NormalMatrix_pen, NormalMatrix_buttom;
glm::mat4 transBase = glm::mat4(1.0f);
glm::mat4 rotateTop = glm::mat4(1.0f);
glm::mat4 rotateFirstarm = glm::mat4(1.0f);
glm::mat4 rotateSecondarm = glm::mat4(1.0f);
glm::mat4 rotatePenLatitude = glm::mat4(1.0f);
glm::mat4 rotatePenLongitude = glm::mat4(1.0f);
glm::mat4 rotatePenSelf = glm::mat4(1.0f);

glm::mat4 scaleInitBase, transInitBase, scaleInitTop, transInitTop, rotateInitTop, scaleInitFirstarm, transInitFirstarm;
glm::mat4 scaleInitJoint, transInitJoint, rotateInitJoint, scaleInitSecondarm, rotateInitSecondarm, transInitSecondarm;
glm::mat4 scaleInitPen, transInitPen, rotateInitPen, scaleInitButtom, transInitButtom;
glm::mat4 transInitJoint_x, transInitJoint_y, transInitSecondarm_x1, transInitPen_x1, transInitPen_x2, transInitButtom_x1, transInitButtom_y, transInitButtom_x2,transInitButtom_x3;

GLuint vertex_shader, fragment_shader, program;

// function Prototype 
void graphics_init();

void updateModels();


void initialize_points(){
    // create the grid

    for(int i = 0; i < 11 ; i++){
        Vertex start,end;
        start.color = glm::vec4(1,1,1,1);
        end.color = glm::vec4(1,1,1,1);

        start.pos = glm::vec4(i - 5.0, 0, -5.0, 1.0); 
        end.pos = glm::vec4(i - 5.0, 0, 5.0, 1.0);
        grid_vertices.push_back(start);
        grid_vertices.push_back(end);
            
        start.pos = glm::vec4(-5.0, 0, i  - 5.0, 1.0); 
        end.pos = glm::vec4(5.0f, 0, i - 5.0, 1.0); 
        grid_vertices.push_back(start);
        grid_vertices.push_back(end);
    }
    // create the frame at origin

    Vertex vo, vx,vy,vz;
    vo.pos = glm::vec4(0,0.01,0,1);
    
    vx.pos = glm::vec4(3,0.01,0,1);
    vx.color = glm::vec4(1,0,0,1); // red
    
    vy.pos = glm::vec4(0,3.01,0,1);
    vy.color = glm::vec4(0,1,0,1); // green
    
    vz.pos = glm::vec4(0,0.01,3,1);
    vz.color = glm::vec4(0,0,1,1); // green

    vo.color = vx.color;
    grid_vertices.push_back(vo);
    grid_vertices.push_back(vx);

    vo.color = vy.color;
    grid_vertices.push_back(vo);
    grid_vertices.push_back(vy);
    
    vo.color = vz.color;
    grid_vertices.push_back(vo);
    grid_vertices.push_back(vz);    
    
}

void initialize_cube(){
    //pos
    cube[0].pos = glm::vec4(1,1,1,1);
    cube[1].pos = glm::vec4(1,1,-1,1);
    cube[2].pos = glm::vec4(1,-1,1,1);
    cube[3].pos = glm::vec4(1,-1,-1,1);
    cube[4].pos = glm::vec4(-1,1,1,1);
    cube[5].pos = glm::vec4(-1,1,-1,1);
    cube[6].pos = glm::vec4(-1,-1,1,1);
    cube[7].pos = glm::vec4(-1,-1,-1,1);
    //translate to x+
    glm::mat4 transCube = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f, 0.0f, 0.0f));
    for(int i = 0; i<8; i++){
		cube[i].pos = transCube * cube[i].pos;
		}
    
    //color
	for(int i =0;i<8;i++){
		cube[i].color = glm::vec4(1,1,0,1);
		}	
	//normal	
	float x10 = cube[1].pos.x - cube[0].pos.x; 
	float y10 = cube[1].pos.y - cube[0].pos.y;
	float z10 = cube[1].pos.z - cube[0].pos.z;
	float x20 = cube[2].pos.x - cube[0].pos.x; 
	float y20 = cube[2].pos.y - cube[0].pos.y;
	float z20 = cube[2].pos.z - cube[0].pos.z;
	float x40 = cube[4].pos.x - cube[0].pos.x; 
	float y40 = cube[4].pos.y - cube[0].pos.y;
	float z40 = cube[4].pos.z - cube[0].pos.z;
	float x37 = cube[3].pos.x - cube[7].pos.x; 
	float y37 = cube[3].pos.y - cube[7].pos.y;
	float z37 = cube[3].pos.z - cube[7].pos.z;
	float x57 = cube[5].pos.x - cube[7].pos.x; 
	float y57 = cube[5].pos.y - cube[7].pos.y;
	float z57 = cube[5].pos.z - cube[7].pos.z;
	float x67 = cube[6].pos.x - cube[7].pos.x; 
	float y67 = cube[6].pos.y - cube[7].pos.y;
	float z67 = cube[6].pos.z - cube[7].pos.z;	
	glm::vec3 normal012 = glm::normalize(glm::vec3((y20*z10-z20*y10),(z20*x10-x20*z10),(x20*y10-y20*x10)));//plane 0123
	glm::vec3 normal014 = glm::normalize(glm::vec3((y10*z40-z10*y40),(z10*x40-x10*z40),(x10*y40-y10*x40)));//plane 0145
	glm::vec3 normal024 = glm::normalize(glm::vec3((y40*z20-z40*y20),(z40*x20-x40*z20),(x40*y20-y40*x20)));//plane 0246
	glm::vec3 normal735 = glm::normalize(glm::vec3((y57*z37-z57*y37),(z57*x37-x57*z37),(x57*y37-y57*x37)));//plane 1357
	glm::vec3 normal736 = glm::normalize(glm::vec3((y37*z67-z37*y67),(z37*x67-x37*z67),(x37*y67-y37*x67)));//plane 2367	
	glm::vec3 normal756 = glm::normalize(glm::vec3((y67*z57-z67*y57),(z67*x57-x67*z57),(x67*y57-y67*x57)));//plane 4567
	
	cube[0].normal = glm::vec4(glm::normalize(normal012 + normal014 + normal024),1.0f);
	cube[1].normal = glm::vec4(glm::normalize(normal012 + normal014 + normal735),1.0f);
	cube[2].normal = glm::vec4(glm::normalize(normal012 + normal024 + normal736),1.0f);
	cube[3].normal = glm::vec4(glm::normalize(normal012 + normal735 + normal736),1.0f);
	cube[4].normal = glm::vec4(glm::normalize(normal756 + normal014 + normal024),1.0f);
	cube[5].normal = glm::vec4(glm::normalize(normal756 + normal014 + normal735),1.0f);
	cube[6].normal = glm::vec4(glm::normalize(normal756 + normal024 + normal736),1.0f);
	cube[7].normal = glm::vec4(glm::normalize(normal756 + normal736 + normal735),1.0f);	 
	
	cube_triangles_points[0] = cube_triangles_points[3] = cube_triangles_points[6] = cube_triangles_points[9] = cube_triangles_points[12] = cube_triangles_points[15] = cube[0];
	cube_triangles_points[1] = cube_triangles_points[7] = cube_triangles_points[20] = cube_triangles_points[23] = cube[1];
	cube_triangles_points[4] = cube_triangles_points[16] = cube_triangles_points[32] = cube_triangles_points[35] =cube[2];
	cube_triangles_points[2] = cube_triangles_points[5] = cube_triangles_points[22] = cube_triangles_points[31] =cube[3];
	cube_triangles_points[10] = cube_triangles_points[13] = cube_triangles_points[26] = cube_triangles_points[29] =cube[4];
	cube_triangles_points[8] = cube_triangles_points[11] = cube_triangles_points[19] = cube_triangles_points[25] =cube[5];
	cube_triangles_points[14] = cube_triangles_points[17] = cube_triangles_points[28] = cube_triangles_points[34] =cube[6];
	cube_triangles_points[18] = cube_triangles_points[21] = cube_triangles_points[24] = cube_triangles_points[27] = cube_triangles_points[30] = cube_triangles_points[33] = cube[7];
	
	for(int i = 0; i<36; i++){
		    glm::mat4 scale1 = glm::scale(glm::mat4(1.0f),glm::vec3(0.05f, 0.05f ,0.05f));   		
			glm::mat4 trans1 = glm::translate(glm::mat4(1.0f),glm::vec3(1.05, 0.1, 0.0f));
			littlecube_triangles_points[i].pos = trans1 * scale1 * cube_triangles_points[i].pos;
			littlecube_triangles_points[i].normal = cube_triangles_points[i].normal;
 		}
 	for(int i = 0; i<18; i++){
//		printf("%d,%f,%f,%f\n", i,littlecube_triangles_points[i].normal.x,littlecube_triangles_points[i].normal.y,littlecube_triangles_points[i].normal.z);
//		printf("%d,%f,%f,%f\n", i,cube_triangles_points[i].normal.x,cube_triangles_points[i].normal.y,cube_triangles_points[i].normal.z);
	}
	}
	
void initialize_disk(){
	
	diskCenter[0].pos = glm::vec4(0,1,0,1);
	diskCenter[0].color = glm::vec4(0,0.5,1,1);
	diskCenter[1].pos = glm::vec4(0,-1,0,1);
	diskCenter[1].color = glm::vec4(0,0.5,1,1);
	
	float angle_disk = 0;
	
	for(int i=0; i<360; i++){
		disk1[i].pos = glm::vec4(sin(angle_disk),1,cos(angle_disk),1);
		disk1[i].color = glm::vec4(0,0.5,1,1);
		disk2[i].pos = glm::vec4(sin(angle_disk),-1,cos(angle_disk),1);
		disk2[i].color = glm::vec4(0,0.5,1,1);
		angle_disk += PI/180.0f;
		}	
	//normal	
	float x10 = disk1[0].pos.x - diskCenter[0].pos.x;
	float y10 = disk1[0].pos.y - diskCenter[0].pos.y;
	float z10 = disk1[0].pos.z - diskCenter[0].pos.z;
	float x11 = disk1[1].pos.x - diskCenter[0].pos.x;
	float y11 = disk1[1].pos.y - diskCenter[0].pos.y;
	float z11 = disk1[1].pos.z - diskCenter[0].pos.z;
	float x20 = disk2[0].pos.x - diskCenter[1].pos.x;
	float y20 = disk2[0].pos.y - diskCenter[1].pos.y;
	float z20 = disk2[0].pos.z - diskCenter[1].pos.z;
	float x21 = disk2[1].pos.x - diskCenter[1].pos.x;
	float y21 = disk2[1].pos.y - diskCenter[1].pos.y;
	float z21 = disk2[1].pos.z - diskCenter[1].pos.z;	

	diskCenter[0].normal = glm::vec4(glm::normalize(glm::vec3((y10*z11-z10*y11),(z10*x11-x10*z11),(x10*y11-y10*x11))),1.0f);
	diskCenter[1].normal = glm::vec4(glm::normalize(glm::vec3((y21*z20-z21*y20),(z21*x20-x21*z20),(x21*y20-y21*x20))),1.0f);

	glm::vec4 normalsPlane[360];
	for(int i=0; i<360; i++){
		int i_next = i+1;
		if(i==359) i_next=0;
		float xright = disk1[i_next].pos.x - disk1[i].pos.x;
		float yright = disk1[i_next].pos.y - disk1[i].pos.y;
		float zright = disk1[i_next].pos.z - disk1[i].pos.z;
		float xmiddle = disk2[i].pos.x - disk1[i].pos.x;
		float ymiddle = disk2[i].pos.y - disk1[i].pos.y;
		float zmiddle = disk2[i].pos.z - disk1[i].pos.z;
		glm::vec4 normal0 = glm::vec4(glm::normalize(glm::vec3((ymiddle*zright-zmiddle*yright),(zmiddle*xright-xmiddle*zright),(xmiddle*yright-ymiddle*xright))),1.0f);
		normalsPlane[i] = normal0;
	}
	
	for(int i=0; i<360; i++){
		int i_before = i-1;
		if(i==0) i_before=359;
		disk1[i].normal = glm::normalize(diskCenter[0].normal + normalsPlane[i] + normalsPlane[i_before]);
		disk2[i].normal = glm::normalize(diskCenter[1].normal + normalsPlane[i] + normalsPlane[i_before]);
		}
	//disk triangles
	for(int i=0;i<360;i++){
		int i_next = i+1;
		if(i==359) i_next=0;
		disk_triangles_points[i*3] = disk1[i];
		disk_triangles_points[i*3+1] = diskCenter[0];
		disk_triangles_points[i*3+2] = disk1[i_next];
		
		disk_triangles_points[360*3+i*3] = disk2[i];
		disk_triangles_points[360*3+i*3+1] = diskCenter[1];
		disk_triangles_points[360*3+i*3+2] = disk2[i_next];
		
		disk_triangles_points[360*6+i*3] = disk1[i];
		disk_triangles_points[360*6+i*3+1] = disk2[i];
		disk_triangles_points[360*6+i*3+2] = disk2[i_next];
		
		disk_triangles_points[360*9+i*3] = disk1[i];
		disk_triangles_points[360*9+i*3+1] = disk1[i_next];
		disk_triangles_points[360*9+i*3+2] = disk2[i_next];
		}
	
//	for(int i=0; i<360*12; i++){
//		disk_triangles_points[i].normal = glm::vec4(glm::normalize(glm::vec3((0-disk_triangles_points[i].normal.x),(0-disk_triangles_points[i].normal.y),(0-disk_triangles_points[i].normal.z))),1.0f);
//		}
	
	glm::mat4 transhalf = glm::translate(glm::mat4(1.0f),glm::vec3(0.6, 0.0, 0.0f));
	glm::mat4 rotateStick = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(0,0,1.0));
    glm::mat4 scaleStick = glm::scale(glm::mat4(1.0f),glm::vec3(0.1,0.6,0.1));
	for(int i=0; i<360*12; i++){
		stick_triangles_points[i].pos = transhalf * rotateStick * scaleStick * disk_triangles_points[i].pos;
		stick_triangles_points[i].normal = rotateStick * disk_triangles_points[i].normal;
		}	
	

	for(int i=0; i<360*12; i++){
		Vertex test;
		test.pos = disk_triangles_points[i].pos;
		test.color = glm::vec4(0,0.5,1,1);
		test.normal = disk_triangles_points[i].normal;
		test_vertices.push_back(test);
		}	
	for(int i=0; i<360*12; i++){
		Vertex testN1,testN2;
		testN1.pos = disk_triangles_points[i].pos;
		testN1.color = glm::vec4(1,0,0,1);
		testN_vertices.push_back(testN1);	
		testN2.pos = testN1.pos + disk_triangles_points[i].normal;
		testN2.color = glm::vec4(1,0,0,1);
		testN_vertices.push_back(testN2);
		}

	}

void init_base_by_disk(){  //height = 1.0
	scaleInitBase = glm::scale(glm::mat4(1.0f),glm::vec3(1.5f, 0.5f ,1.5f));		
	transInitBase = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.5f, 0.0f));	
	Model_base = transInitBase * scaleInitBase;
	NormalMatrix_base = glm::mat4(1.0f);
	for(int i=0; i<360*12; i++){
		Vertex basePoint;
		basePoint.pos = disk_triangles_points[i].pos;
		basePoint.color = glm::vec4(0.5,0.5,0.5,1);	
		basePoint.normal = disk_triangles_points[i].normal;
		base_vertices.push_back(basePoint);
		}
	}

void init_top_by_disk(){	//radius = 1
    scaleInitTop = glm::scale(glm::mat4(1.0f),glm::vec3(1, 0.5,1));		
    rotateInitTop = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0,0,0));
	transInitTop = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 2.0f, 0.0f));
	Model_top = transInitTop * rotateInitTop * scaleInitTop;
	NormalMatrix_top = rotateInitTop;
	for(int i=0; i<360*12; i++){
		Vertex topPoint;
		topPoint.pos = disk_triangles_points[i].pos;
		topPoint.normal = rotateInitTop * disk_triangles_points[i].normal;
		topPoint.color = glm::vec4(0,0,0.5,1);	
		top_vertices.push_back(topPoint);
		}
	}
	
void init_firstarm_by_cube(){ //length = 2
    scaleInitFirstarm = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f, 0.2f ,0.2f));
	transInitFirstarm = glm::translate(glm::mat4(1.0f),glm::vec3(0.0, 2.0, 0.0f));
	Model_firstarm = transInitFirstarm * scaleInitFirstarm;
	NormalMatrix_firstarm = glm::mat4(1.0f);
	for(int i=0; i<36; i++){
		Vertex firstarmPoint;
		firstarmPoint.pos = cube_triangles_points[i].pos;
		firstarmPoint.normal = cube_triangles_points[i].normal;
		firstarmPoint.color = glm::vec4(1,1,0,1);
		firstarm_vertices.push_back(firstarmPoint);
		}
	}	

void init_joint_by_disk(){  
	transInitJoint_x = glm::translate(glm::mat4(1.0f),glm::vec3(2.0, 0.0, 0.0f));
	transInitJoint_y = glm::translate(glm::mat4(1.0f),glm::vec3(0.0, 2.0, 0.0f));
    scaleInitJoint = glm::scale(glm::mat4(1.0f),glm::vec3(0.3, 0.21,0.3));	
    rotateInitJoint = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0,0,0));
	transInitJoint = glm::translate(glm::mat4(1.0f),glm::vec3(2.0, 2.0, 0.0f));
	Model_joint = transInitJoint * rotateInitJoint * scaleInitJoint;
	NormalMatrix_joint = rotateInitJoint;
	for(int i=0; i<360*12; i++){
		Vertex jointPoint;
		jointPoint.pos = disk_triangles_points[i].pos;
		jointPoint.normal = rotateInitJoint * disk_triangles_points[i].normal;
		jointPoint.color = glm::vec4(0,1,1,1);
		joint_vertices.push_back(jointPoint);
		}
	}
	
void init_secondarm_by_disk(){ //length = 1.2
	transInitSecondarm_x1 = glm::translate(glm::mat4(1.0f),glm::vec3(2.0, 0.0, 0.0f));
	transInitSecondarm = glm::translate(glm::mat4(1.0f),glm::vec3(2.0, 2.0, 0.0f));
	Model_secondarm = transInitSecondarm;
	NormalMatrix_secondarm = glm::mat4(1.0f); 
	for(int i=0; i<360*12; i++){
		Vertex secondarmPoint;
		secondarmPoint.pos = stick_triangles_points[i].pos;
		secondarmPoint.normal = stick_triangles_points[i].normal;
		secondarmPoint.color = glm::vec4(1,0,1,1);
		secondarm_vertices.push_back(secondarmPoint);
		}	
	}
	
void init_pen_by_disk(){ //length = 1.2
	transInitPen_x1 = glm::translate(glm::mat4(1.0f),glm::vec3(3.2, 0.0, 0.0f));
	transInitPen_x2 = glm::translate(glm::mat4(1.0f),glm::vec3(1.2, 0.0, 0.0f));
	transInitPen = glm::translate(glm::mat4(1.0f),glm::vec3(3.2, 2.0, 0.0f));
	Model_pen = transInitPen;
	NormalMatrix_pen = glm::mat4(1.0f); 
	for(int i=0; i<360*12; i++){
		Vertex penPoint;
		penPoint.pos = stick_triangles_points[i].pos;
		penPoint.normal = stick_triangles_points[i].normal;
		penPoint.color = glm::vec4(0,0.8,0,1);
		pen_vertices.push_back(penPoint);
		}
		
	}
	
void init_buttom_by_cube(){
	transInitButtom_x1 = glm::translate(glm::mat4(1.0f),glm::vec3(3.2, 0.0, 0.0f));
	transInitButtom_y = glm::translate(glm::mat4(1.0f),glm::vec3(0.0, 2.0, 0.0f));
	transInitButtom_x2 = glm::translate(glm::mat4(1.0f),glm::vec3(1.1, 0.0, 0.0f));	
	transInitButtom = glm::translate(glm::mat4(1.0f),glm::vec3(3.2, 2.0, 0.0f));
	Model_buttom = transInitButtom;
	NormalMatrix_buttom = glm::mat4(1.0f); 
	for(int i=0; i<36; i++){
		Vertex buttomPoint;
		buttomPoint.pos = littlecube_triangles_points[i].pos;
		buttomPoint.normal = littlecube_triangles_points[i].normal;
		buttomPoint.color = glm::vec4(1,1,0,1);
		buttom_vertices.push_back(buttomPoint);
		}
	}

// MOUSE handling *******************************************

int last_x, last_y;
int selected_idx = -1;

void mouse(int button, int state, int x, int y ){
	if(state == GLUT_DOWN){
        
	}else{
        
	}

    last_x = x;
    last_y = y;
    
}

void motion( int x, int y){
    GLint viewport[4];

    glutPostRedisplay();
    
}

// KEYBOARD handling *******************************************

void keyboard(unsigned char key, int x, int y){
    switch (key) {
	case 49:
		if(selectBase == false){
			selectBase = true;
			selectTop = false;
			selectFirstArm = false;
			selectSecondArm = false;
			selectPen = false;
			}else
				selectBase = false;
		break;	
	case 50:
		if(selectTop == false){
			selectBase = false;
			selectTop = true;
			selectFirstArm = false;
			selectSecondArm = false;
			selectPen = false;
			}else
			selectTop = false;
		break;
	case 51:
		if(selectFirstArm == false){
			selectBase = false;
			selectTop = false;
			selectFirstArm = true;
			selectSecondArm = false;
			selectPen = false;
			}else
				selectFirstArm = false;
		break;
	case 52:
		if(selectSecondArm == false){
			selectBase = false;
			selectTop = false;
			selectFirstArm = false;
			selectSecondArm = true;
			selectPen = false;
			}else
				selectSecondArm = false;
		break;
	case 53:
		if(selectPen == false){
			selectBase = false;
			selectTop = false;
			selectFirstArm = false;
			selectSecondArm = false;
			selectPen = true;
			}else
				selectPen = false;
		break;
	case 99:
		if(cameraFlag == false)
			cameraFlag = true;
		else
			cameraFlag = false;
		break;
	case 108:
		if(lightOn == 1){
			lightOn = 0;
			}
		else
			lightOn = 1;
		break;
    case 'r':
        graphics_init();
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void special_key(int key, int x, int y){
    if(cameraFlag == true){
		switch (key){
			case GLUT_KEY_RIGHT:
				angle_red += PI/180.0f;
				break;
			case GLUT_KEY_LEFT:
				angle_red -= PI/180.0f;
				break;
			case GLUT_KEY_UP:
				angle_blue += PI/180.0f;
				break;
			case GLUT_KEY_DOWN:
				angle_blue -= PI/180.0f;
				break;
			}
		}else if(selectBase == true){ //move the whole model
				switch (key){
					case GLUT_KEY_RIGHT:						
						transBase = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f, 0.0f, 0.0f)) * transBase;
						break;
					case GLUT_KEY_LEFT:
						transBase = glm::translate(glm::mat4(1.0f),glm::vec3(-1.0f, 0.0f, 0.0f)) * transBase;
						break;
					case GLUT_KEY_UP:						
						transBase = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, 1.0f)) * transBase;
						break;
					case GLUT_KEY_DOWN:
						transBase = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -1.0f)) * transBase;						
						break;
				}	
				updateModels();
			}else if(selectTop == true){ //rotate up or down from top
					switch (key){
						case GLUT_KEY_RIGHT:{
								rotateTop = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0,1.0,0)) * rotateTop;		
								}
							break;
						case GLUT_KEY_LEFT:{
								rotateTop = glm::rotate(glm::mat4(1.0f), - 1.0f, glm::vec3(0,1.0,0)) * rotateTop;										
								}
							break;
						}			
						updateModels();
				}else if(selectFirstArm == true){  //rotate from first arm
					switch (key){
						case GLUT_KEY_UP:{
								rotateFirstarm = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0,0,1.0)) * rotateFirstarm;											
								}
							break;
						case GLUT_KEY_DOWN:{
								rotateFirstarm = glm::rotate(glm::mat4(1.0f), - 1.0f, glm::vec3(0,0,1.0)) * rotateFirstarm;															
								}
							break;
						}
						updateModels();
					}else if(selectSecondArm == true){ //rotate from second arm						
						switch (key){
							case GLUT_KEY_UP:{
									rotateSecondarm = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0,0,1.0)) * rotateSecondarm;									
									}
								break;
							case GLUT_KEY_DOWN:{
									rotateSecondarm = glm::rotate(glm::mat4(1.0f), -1.0f, glm::vec3(0,0,1.0)) * rotateSecondarm;								
									}
								break;
							}
							updateModels();
						}else if(selectPen == true){  //rotate & twist pen							
							
							if(glutGetModifiers() & GLUT_ACTIVE_SHIFT){
								switch (key){
									case GLUT_KEY_UP:{
											rotatePenSelf = glm::rotate(glm::mat4(1.0f), 10.0f, glm::vec3(1.0,0,0)) * rotatePenSelf;
											}
										break;
									case GLUT_KEY_DOWN:{
											rotatePenSelf = glm::rotate(glm::mat4(1.0f), -10.0f, glm::vec3(1.0,0,0)) * rotatePenSelf;
											}
										break;
									}
								}else{
									switch (key){
										case GLUT_KEY_UP:{
												rotatePenLatitude = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(1.0,0,0)) * rotatePenLatitude;
												}
											break;
										case GLUT_KEY_DOWN:{
												rotatePenLatitude = glm::rotate(glm::mat4(1.0f), -1.0f, glm::vec3(1.0,0,0)) * rotatePenLatitude;
												}
											break;
										case GLUT_KEY_RIGHT:{
												rotatePenLongitude = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0,0,1.0)) * rotatePenLongitude;
												}
											break;
										case GLUT_KEY_LEFT:{
												rotatePenLongitude = glm::rotate(glm::mat4(1.0f), -1.0f, glm::vec3(0,0,1.0)) * rotatePenLongitude;
												}
											break;
										}
									}
									updateModels();	
								}
													
    glutPostRedisplay();
}

void updateModels(){
	Model_base = transBase * transInitBase * scaleInitBase;
	Model_top = transBase * rotateTop * transInitTop * rotateInitTop * scaleInitTop;
	Model_firstarm = transBase * rotateTop * transInitFirstarm * rotateFirstarm * scaleInitFirstarm;
	Model_joint = transBase *rotateTop * transInitJoint_y * rotateFirstarm * transInitJoint_x * rotateInitJoint * scaleInitJoint;
	Model_secondarm = transBase *rotateTop * transInitJoint_y * rotateFirstarm * transInitSecondarm_x1 * rotateSecondarm;
	Model_pen = transBase *rotateTop * transInitJoint_y * rotateFirstarm * transInitSecondarm_x1 * rotateSecondarm * transInitPen_x2 * rotatePenLatitude * rotatePenLongitude * rotatePenSelf;
	Model_buttom = transBase *rotateTop * transInitJoint_y * rotateFirstarm * transInitSecondarm_x1  * rotateSecondarm * transInitPen_x2 * rotatePenLatitude * rotatePenLongitude * rotatePenSelf; 	
														
	NormalMatrix_base = NormalMatrix_base;
	NormalMatrix_top = rotateTop * rotateInitTop;
	NormalMatrix_firstarm = rotateTop * rotateFirstarm;
	NormalMatrix_joint = rotateTop * rotateFirstarm * rotateInitJoint;
	NormalMatrix_secondarm = rotateTop * rotateFirstarm * rotateSecondarm;
	NormalMatrix_pen = rotateTop * rotateFirstarm * rotateSecondarm * rotatePenLatitude * rotatePenLongitude * rotatePenSelf;
	NormalMatrix_buttom = rotateTop * rotateFirstarm * rotateSecondarm * rotatePenLatitude * rotatePenLongitude * rotatePenSelf; 
	
	}

// DISPLAY and RENDERING functions *************************

void draw_grid(int lignt_turn){
    
    GLuint vao;
    GLuint grid_vbo;
    GLuint points_vbo;
    GLuint lighton;
    GLuint position_location;
    GLuint color_location;
    GLuint MVP_location;
	
	sphere_x = - cos(angle_blue) * sin(angle_red) * 10.0f;
	sphere_y = sin(angle_blue) * 10.0f; 
	sphere_z = cos(angle_blue) * cos(angle_red) * 10.0f;
    
    //Projection
    glm::mat4 Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View;

	View  = glm::lookAt(glm::vec3(sphere_x,sphere_y,sphere_z), glm::vec3(0,0,0), glm::vec3(0,1.0f,0));

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);  
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; 
	
    // specify the shaders we want to use
    glUseProgram(program);

    // get the input variable location in this shader
    position_location = glGetAttribLocation(program, "in_position");
    color_location = glGetAttribLocation(program, "in_color");

    MVP_location = glGetUniformLocation(program, "MVP");
    lighton = glGetUniformLocation(program, "lighton");
    
    // create and bind a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // enable the input locations we wan to use
    glEnableVertexAttribArray(position_location);
    glEnableVertexAttribArray(color_location);
    
	//MVP
	glUniformMatrix4fv(MVP_location, 1, GL_FALSE, &MVP[0][0]);
    glUniform1i(lighton,lignt_turn);
    // draw points

    // generate and bind a vertex buffer to hold the vertex data on GPU
    glGenBuffers(1, &grid_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);

    // initialize the vertex buffer with the vertex data
    glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(Vertex), &grid_vertices[0] , GL_STATIC_DRAW);

    // specify the input data format
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	
    // draw points
    glPointSize(10);
    glDrawArrays(GL_LINES, 0, grid_vertices.size());

    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Delete VAO and VBO
//    glDeleteBuffers(1, &cubetriangle_vbo);
    glDeleteBuffers(1, &grid_vbo);
    glDeleteVertexArrays(1, &vao);
  
}

void draw_modelparts(glm::mat4 Model_part, vector<Vertex> part_vertices, glm::mat4 ModelNormal, int lignt_turn){
	GLuint vao;
	GLuint part_vbo;
    
    GLuint position_location;
    GLuint color_location;
    GLuint normal_location;
    GLuint MVP_location;
    GLuint ModelN_location;
    GLuint light_location;
    GLuint diffuse_location;
    GLuint specular_location;
    GLuint lighton;
    
    sphere_x = - cos(angle_blue) * sin(angle_red) * 10.0f;
	sphere_y = sin(angle_blue) * 10.0f; 
	sphere_z = cos(angle_blue) * cos(angle_red) * 10.0f;
    
    glm::mat4 Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(sphere_x,sphere_y,sphere_z), glm::vec3(0,0,0), glm::vec3(0,1.0f,0));
	  
	glm::mat4 MVP = Projection * View * Model_part; 
	
    glUseProgram(program);

    position_location = glGetAttribLocation(program, "in_position");
    color_location = glGetAttribLocation(program, "in_color");
    normal_location = glGetAttribLocation(program, "in_normal");
    
    light_location = glGetUniformLocation(program, "light");
    diffuse_location = glGetUniformLocation(program, "diffuse");
    specular_location = glGetUniformLocation(program, "specular");
    
    MVP_location = glGetUniformLocation(program, "MVP");
	lighton = glGetUniformLocation(program, "lighton");
	ModelN_location = glGetUniformLocation(program, "ModelN");//
	
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(position_location);
    glEnableVertexAttribArray(color_location);
    glEnableVertexAttribArray(normal_location);
    
	glUniformMatrix4fv(MVP_location, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelN_location, 1, GL_FALSE, &ModelNormal[0][0]);
    glUniform4fv(light_location,1, &lightPosition[0]);
    glUniform4fv(diffuse_location,1, &lightDiffuse[0]);
	glUniform4fv(specular_location,1, &lightSpecular[0]);
    glUniform1i(lighton,lignt_turn);
    
    glGenBuffers(1, &part_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, part_vbo);
    glBufferData(GL_ARRAY_BUFFER, part_vertices.size() * sizeof(Vertex), &part_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glDrawArrays(GL_TRIANGLES, 0, part_vertices.size());

    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Delete VAO and VBO
    glDeleteVertexArrays(1, &vao);
    
	}

void draw_test(){
	GLuint vao;
	GLuint part_vbo;
    GLuint points_vbo;
    GLuint position_location;
    GLuint color_location;
    GLuint MVP_location;      
    sphere_x = - cos(angle_blue) * sin(angle_red) * 10.0f;
	sphere_y = sin(angle_blue) * 10.0f; 
	sphere_z = cos(angle_blue) * cos(angle_red) * 10.0f;
    
    glm::mat4 Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(sphere_x,sphere_y,sphere_z), glm::vec3(0,0,0), glm::vec3(0,1.0f,0));
	glm::mat4 Model = glm::mat4(1.0f);  
	glm::mat4 MVP = Projection * View * Model; 
	
    glUseProgram(program);

    position_location = glGetAttribLocation(program, "in_position");
    color_location = glGetAttribLocation(program, "in_color");
    
    MVP_location = glGetUniformLocation(program, "MVP");
	
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(position_location);
    glEnableVertexAttribArray(color_location);
    
	glUniformMatrix4fv(MVP_location, 1, GL_FALSE, &MVP[0][0]);

    glGenBuffers(1, &part_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, part_vbo);
    glBufferData(GL_ARRAY_BUFFER, testN_vertices.size() * sizeof(Vertex), &testN_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glDrawArrays(GL_LINES, 0, testN_vertices.size());

	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, testN_vertices.size() * sizeof(Vertex), &testN_vertices[0] , GL_STATIC_DRAW);
	glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glPointSize(4);
	glDrawArrays(GL_POINTS, 0, testN_vertices.size());

    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Delete VAO and VBO
    glDeleteVertexArrays(1, &vao);
    
	}

void draw_obj(){
	
	draw_grid(1);
//	draw_modelparts(Model_test, test_vertices,lightOn);
//	draw_test();

	draw_modelparts(Model_base, base_vertices,NormalMatrix_base,lightOn);
	draw_modelparts(Model_top, top_vertices,NormalMatrix_top,lightOn);
	draw_modelparts(Model_firstarm, firstarm_vertices,NormalMatrix_firstarm,lightOn);
	draw_modelparts(Model_joint, joint_vertices,NormalMatrix_joint,lightOn);
	draw_modelparts(Model_secondarm, secondarm_vertices,NormalMatrix_secondarm,lightOn);
	draw_modelparts(Model_pen, pen_vertices,NormalMatrix_pen,lightOn);
	draw_modelparts(Model_buttom, buttom_vertices,NormalMatrix_buttom,lightOn);
	}

void display(){
    // Clear Viewport
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
//    draw_grid();
	draw_obj();
    glFlush();
    glutSwapBuffers();

}

void reshape(int width, int height){
    // Clip the view port to match our ratio
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

void graphics_init(){

    // init vertex shader
    read_shader_source_code("vs.glsl", buf, 1024);
    cout << buf << endl;
    int vertex_shader_source_length = strlen(buf);
    const char *vertex_shader_sources[] = { buf };

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_sources, &vertex_shader_source_length);
    glCompileShader(vertex_shader);
    
    // init fragment shader 
    read_shader_source_code("fs.glsl", buf, 1024); 
    int fragment_shader_source_length = strlen(buf);
    const char *fragment_shader_sources[] = { buf };
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_sources, &fragment_shader_source_length);
    glCompileShader(fragment_shader);

    // init program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
  
    // enable depth test
    glEnable(GL_DEPTH_TEST);
    
    //NEW ADD
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


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

    if(glewIsSupported("GL_ARB_debug_output"))
        VSDebugLib::init();
    // Initialize OpenGL
    graphics_init ();

    initialize_points();
	initialize_cube();
	initialize_disk();

	init_base_by_disk();
	init_top_by_disk();
	init_firstarm_by_cube();
    init_joint_by_disk();
    init_secondarm_by_disk();
    init_pen_by_disk();
    init_buttom_by_cube(); 
       
//	reCalc();
	
    // set up callbacks
    glutReshapeFunc(&reshape);
    glutDisplayFunc(&display);
    glutKeyboardFunc(&keyboard);
    glutMouseFunc(&mouse);
    glutMotionFunc(&motion);
    glutSpecialFunc(&special_key);

    // main loop
    glutMainLoop();
    return 0;
}
