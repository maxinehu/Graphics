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
#include "tga.h"
#include "debuglib.h"
#include "ray_casting.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

// CONTSTANTS
const float PI = 3.1415926;
const int window_width = 500, window_height = 500;
const char * window_title = "HW2";

int camera_i; 
int camera_j;

int green=1;

struct Vertex
{
    glm::vec4 pos;
    glm::vec4 color;
	glm::vec4 normal;
	glm::vec2 tex_coord;
};


// runtime variable
char buf[4096];
vector<glm::vec3> mesh_pos;
vector<glm::vec3> mesh_normals;
vector<GLuint> mesh_elements;
//
glm::mat4 view,projection;
glm::mat4 model_grid,model_cpoints, model_cpoints3, model_surface, model_mesh, model_eyel,model_eyer;
glm::mat4 modelView_grid, modelView_cpoints, modelView_cpoints3,modelView_surface,modelView_mesh, modelView_eyel, modelView_eyer;
glm::mat3 normalMatrix_surface,normalMatrix_mesh,normalMatrix_eyel,normalMatrix_eyer;
//
bool cameraMoving=true;
bool show_cpoints=true;
bool show_surface=true;
bool show_model=true;
bool show_eyes=false;
bool smile = false;
bool autofit = false;
bool centerMoving = false;
//bool fragment_disgard=true;
int show_texture=1;

Vertex cpoints[20][20];
Vertex cpoints1[39][39];
Vertex cpoints2[77][77];
Vertex cpoints3[153][153];

vector<Vertex> grid_vertices;
vector<Vertex> spoints;
vector<Vertex> mesh_vertices;
vector<Vertex> eyeR;
vector<Vertex> eyeL;
vector<Vertex> eyeMR;
vector<Vertex> eyeML;

Vertex topCenter;

int ncpoints=400;

GLuint vertex_shader, fragment_shader, program;

// function Prototype 
void graphics_init();
void set_mvp();
void draw_grid();
void reset_state();
void initialize_cpoints();
void draw_cpoints();
void draw_surface();
int color_code_pick_r(int x,int y);
int color_code_pick_g(int x,int y);
void cpoints_color_code();
void color_cpoints_green();
void generate_surface_points();
void load_obj(const char* filename, vector<glm::vec3> &vertices, vector<glm::vec3> &normals, vector<GLuint> &elements);
void initialize_mesh();
void draw_mesh();
void auto_fit();
void createEyes();
void createSphere(vector<Vertex> &sphere, double R, double X, double Y, double Z);
void draw_eyes();
void timer(int value);
void Generate_circle();
	
void initialize_points(){
	topCenter.pos = glm::vec4(0,3,0,1);
    topCenter.color = glm::vec4(0,1,0,1);
    // create the grid

    for(int i = 0; i < 11 ; i++)
    {
        Vertex start,end;
        start.color = glm::vec4(1,1,1,1);
        end.color = glm::vec4(1,1,1,1);
		start.normal = glm::vec4(0,1,0,0);
		end.normal = glm::vec4(0,1,0,0);

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
	vo.normal=glm::vec4(0,1,0,0);
	vx.normal=glm::vec4(0,1,0,0);
	vy.normal=glm::vec4(0,1,0,0);
	vz.normal=glm::vec4(0,1,0,0);
    
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

int num_of_Cpoints = 152;
vector<Vertex> Circle_Vertices;
vector<Vertex> round1;
vector<Vertex> second_circle; //after the second subdivision
vector<Vertex> second_line;
vector<Vertex> third_circle;  //after the third subdivision
vector<Vertex> first_line; //the first line after the second subdivision
vector<Vertex> first_line_3; //the first line after the third subdivision
vector<Vertex> second_line_3; //the second line after the third subdivision
vector<Vertex> third_line;   //the third line after the third subdivision
vector<Vertex> fourth_line; //the fourth line after the fourth subdivision
vector<Vertex> fifth_line; //the fourth line after the fifth subdivision
vector<Vertex> sixth_line; //the fourth line after the sixth subdivision
vector<Vertex> fourth_circle; 
	
vector<Vertex> checknormal;

void Generate_circle()
{
	Circle_Vertices.resize(0);
	round1.resize(0);
	second_circle.resize(0); //after the second subdivision
	second_line.resize(0);
	third_circle.resize(0);  //after the third subdivision
	first_line.resize(0); //the first line after the second subdivision
	first_line_3.resize(0); //the first line after the third subdivision
	second_line_3.resize(0); //the second line after the third subdivision
	third_line.resize(0);   //the third line after the third subdivision
	fourth_line.resize(0); //the fourth line after the fourth subdivision
	fifth_line.resize(0); //the fourth line after the fifth subdivision
	sixth_line.resize(0); //the fourth line after the sixth subdivision
	fourth_circle.resize(0); 
	
	Vertex temp;
	Vertex center;
	vector<Vertex> old; 
	glm::vec4 a;
	glm::vec4 b;
	glm::vec3 norm;
///////////////////////////////////////////////////////////The first circle without subdivision/////////////////////////////////////
    
	temp.color = glm::vec4(0,1,0,1);
	temp.pos = cpoints3[0][13*8].pos;
	temp.normal = glm::vec4(0,1,0,0);
	Circle_Vertices.push_back(temp);
	
    for (int i = 1;i < num_of_Cpoints; i++){

		temp.color = glm::vec4(0,1,0,1);
		temp.pos = cpoints3[num_of_Cpoints-i][13*8].pos;
		temp.normal = glm::vec4(0,1,0,0);
		Circle_Vertices.push_back(temp);
	}
	center.color = glm::vec4(0,1,0,1);
	center.pos = topCenter.pos;
	center.normal = glm::vec4(0,1,0,0);
//////////////////////////////////////////////The second circle after first subdivision//////////////////////////////////////////////////
	glm::vec4 c = center.pos * 0.75;
	int n = Circle_Vertices.size();
	for (int i = 0;i < n; i++){   //generate new center point	    
		c = c + 0.25 * Circle_Vertices[i].pos / num_of_Cpoints;
	}
	for (int i = 0;i < n; i++){  //generate new circle     
		temp.pos = 0.5f * center.pos;
		for (int j = 0;j < n; j++){ 
			float value_cos = cos(2*PI*j/n);
			temp.pos = temp.pos + Circle_Vertices.at((i+j)%n).pos * ((float)(5.0f/8.0f) * value_cos + value_cos * value_cos + (float)0.5f * value_cos * value_cos * value_cos)/n;
		}
		second_circle.push_back(temp);
	}
	center.pos = c;
///////////////////////////////////////////////////The second subdivision//////////////////////////////////////////////////
	for (int i = 0;i < n; i++){  //generate new circle      
		temp.pos = 0.5f * center.pos;
		for (int j = 0;j < n; j++){ 
			float value_cos = cos(2*PI*j/n);
			temp.pos = temp.pos + second_circle.at((i+j)%n).pos * ((float)(5.0f/8.0f) * value_cos + value_cos * value_cos + (float)0.5f * value_cos * value_cos * value_cos)/n;
		}
		third_circle.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate first line     
		temp.pos = (float)(1.0f/8.0f) * center.pos + (float)(6.0f/8.0f) * second_circle.at(i).pos + (float)(1.0f/8.0f) * Circle_Vertices.at(i).pos;
		first_line.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate second line     
		temp.pos = (float)(1.0f/2.0f) * second_circle.at(i).pos + (float)(1.0f/2.0f) * Circle_Vertices.at(i).pos;
		second_line.push_back(temp);
	}
	center.pos = center.pos * 0.75;
	for (int i = 0;i < n; i++){     //generate new center point	    
		center.pos = center.pos + 0.25 * second_circle[i].pos / n;
	}
/////////////////////////////////////////////////////The third subdivision///////////////////////////////////
	for (int i = 0;i < n; i++){  //generate new circle     
		temp.pos = 0.5f * center.pos;
		for (int j = 0;j < n; j++){ 
			float value_cos = cos(2*PI*j/n);
			temp.pos = temp.pos + third_circle.at((i+j)%n).pos * ((float)(5.0f/8.0f) * value_cos + value_cos * value_cos + (float)0.5f * value_cos * value_cos * value_cos)/n;
		}
		fourth_circle.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate first line      
		temp.pos = (float)(1.0f/8.0f) * center.pos + (float)(6.0f/8.0f) * third_circle.at(i).pos + (float)(1.0f/8.0f) * first_line.at(i).pos;
		first_line_3.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate second line      
		temp.pos = (float)(1.0f/2.0f) * third_circle.at(i).pos + (float)(1.0f/2.0f) * first_line.at(i).pos;
		second_line_3.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate third line     
		temp.pos = (float)(1.0f/8.0f) * third_circle.at(i).pos + (float)(6.0f/8.0f) * first_line.at(i).pos + (float)(1.0f/8.0f) * second_line.at(i).pos;
		third_line.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate fourth line      
		temp.pos = (float)(1.0f/2.0f) * first_line.at(i).pos + (float)(1.0f/2.0f) * second_line.at(i).pos;
		fourth_line.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate fifth line      
		temp.pos = (float)(1.0f/8.0f) * first_line.at(i).pos + (float)(6.0f/8.0f) * second_line.at(i).pos + (float)(1.0f/8.0f) * Circle_Vertices.at(i).pos;
		fifth_line.push_back(temp);
	}
	for (int i = 0;i < n; i++){  //generate sixth line     
		temp.pos = (float)(1.0f/2.0f) * Circle_Vertices.at(i).pos + (float)(1.0f/2.0f) * second_line.at(i).pos;
		sixth_line.push_back(temp);
	}
	center.pos = center.pos * 0.75;
	for (int i = 0;i < n; i++){     //generate new center point	    
		center.pos = center.pos + 0.25 * third_circle[i].pos / n;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0;i < num_of_Cpoints; i++){   //compute the normal for each face

		a = glm::normalize(fourth_circle.at((i+1)%num_of_Cpoints).pos - fourth_circle.at(i).pos);
		b = glm::normalize(center.pos - fourth_circle.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		fourth_circle.at(i).normal = glm::normalize(glm::vec4(norm,0));

		a = glm::normalize(first_line_3.at((i+1)%num_of_Cpoints).pos - first_line_3.at(i).pos);
		b = glm::normalize(fourth_circle.at(i).pos - first_line_3.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		first_line_3.at(i).normal = glm::normalize(glm::vec4(norm,0));

		a = glm::normalize(second_line_3.at((i+1)%num_of_Cpoints).pos - second_line_3.at(i).pos);
		b = glm::normalize(first_line_3.at(i).pos - second_line_3.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		second_line_3.at(i).normal = glm::normalize(glm::vec4(norm,0));

		a = glm::normalize(third_line.at((i+1)%num_of_Cpoints).pos - third_line.at(i).pos);
		b = glm::normalize(second_line_3.at(i).pos - third_line.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		third_line.at(i).normal = glm::normalize(glm::vec4(norm,0));

		a = glm::normalize(fourth_line.at((i+1)%num_of_Cpoints).pos - fourth_line.at(i).pos);
		b = glm::normalize(third_line.at(i).pos - fourth_line.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		fourth_line.at(i).normal = glm::normalize(glm::vec4(norm,0));

		a = glm::normalize(fifth_line.at((i+1)%num_of_Cpoints).pos - fifth_line.at(i).pos);
		b = glm::normalize(fourth_line.at(i).pos - fifth_line.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		fifth_line.at(i).normal = glm::normalize(glm::vec4(norm,0));

		a = glm::normalize(sixth_line.at((i+1)%num_of_Cpoints).pos - sixth_line.at(i).pos);
		b = glm::normalize(fifth_line.at(i).pos - sixth_line.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		sixth_line.at(i).normal = glm::normalize(glm::vec4(norm,0));
 
		a = glm::normalize(Circle_Vertices.at((i+1)%num_of_Cpoints).pos - Circle_Vertices.at(i).pos);
		b = glm::normalize(sixth_line.at(i).pos - Circle_Vertices.at(i).pos);
		norm = glm::cross(glm::vec3(a),glm::vec3(b));
		Circle_Vertices.at(i).normal = glm::normalize(glm::vec4(norm,0));
	}

	for (int i = 0;i < num_of_Cpoints; i++){
		round1.push_back(center);
		temp = fourth_circle.at(i);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (fourth_circle.at(i).normal + fourth_circle.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal 
			+ first_line_3.at(i).normal + first_line_3.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = fourth_circle.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (fourth_circle.at(i).normal + fourth_circle.at((i+1)%num_of_Cpoints).normal 
			+ first_line_3.at(i).normal + first_line_3.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);
 
		temp = first_line_3.at(i);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (first_line_3.at(i).normal + first_line_3.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal 
			+ second_line_3.at(i).normal + second_line_3.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = first_line_3.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (first_line_3.at(i).normal + first_line_3.at((i+1)%num_of_Cpoints).normal 
			+ second_line_3.at(i).normal + second_line_3.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = third_line.at(i);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (second_line_3.at(i).normal + second_line_3.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal 
			+ third_line.at(i).normal + third_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = third_line.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (second_line_3.at(i).normal + second_line_3.at((i+1)%num_of_Cpoints).normal 
			+ third_line.at(i).normal + third_line.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = fourth_line.at(i);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (third_line.at(i).normal + third_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal 
			+ fourth_line.at(i).normal + fourth_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = fourth_line.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (third_line.at(i).normal + third_line.at((i+1)%num_of_Cpoints).normal 
			+ fourth_line.at(i).normal + fourth_line.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = fifth_line.at(i);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (fourth_line.at(i).normal + fourth_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal 
			+ fifth_line.at(i).normal + fifth_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = fifth_line.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (fourth_line.at(i).normal + fourth_line.at((i+1)%num_of_Cpoints).normal 
			+ fifth_line.at(i).normal + fifth_line.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = sixth_line.at(i);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (fifth_line.at(i).normal + fifth_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal 
			+ sixth_line.at(i).normal + sixth_line.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = sixth_line.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/4.0f) * (fifth_line.at(i).normal + fifth_line.at((i+1)%num_of_Cpoints).normal 
			+ sixth_line.at(i).normal + sixth_line.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = Circle_Vertices.at(i);
		temp.normal = glm::normalize((float)(1.0f/2.0f) * (Circle_Vertices.at(i).normal + Circle_Vertices.at((i-1+num_of_Cpoints)%num_of_Cpoints).normal));
		round1.push_back(temp);

		temp = Circle_Vertices.at((i+1)%num_of_Cpoints);
		temp.normal = glm::normalize((float)(1.0f/2.0f) * (Circle_Vertices.at(i).normal + Circle_Vertices.at((i+1)%num_of_Cpoints).normal));
		round1.push_back(temp);
 
	}

	for (int i = 0;i < round1.size(); i++){
		round1.at(i).color = glm::vec4(0.9,0.9,0.9,1);
	}
	
	
//		if(i!=0){
//			round1.at(i).normal = glm::vec4(-glm::vec3(round1.at(i).normal),1);
//		}
	//}
}

void initialize_cpoints()
{
	for(int j = 0; j < 20 ; j++)
    {
        for( int i = 0; i < 20 ; i++)
		{
			Vertex temp;
			temp.color=glm::vec4(0,1,0,1);
			temp.pos=glm::vec4((-9+i)*0.3,j*0.2,0,1);
			temp.normal=glm::vec4(0.f,0.f,-1.f,1.f);
			temp.tex_coord = glm::vec2(j*1.f/19,(19-i)*1.f/19);
			cpoints[i][j]=temp;

		}
    }
    
    //bend		
		for(int j = 0; j < 20; j++){
			cpoints[0][j].pos = glm::vec4(0, j*0.2, 0.8, 1);
			cpoints[0][j].normal = glm::vec4(0.f,0.f,1.f,1.f);
			
		}

		for(int j = 0; j < 20; j++){
			for(int i = 1; i < 20; i++){ 
				glm::mat4 rotateCP = glm::rotate(glm::mat4(1.0f), -(float)i*360.f/19.f, glm::vec3(0,1.0,0));
				cpoints[i][j].pos = rotateCP * cpoints[0][j].pos;
				cpoints[i][j].normal = rotateCP * cpoints[0][j].normal;
			}
		}
		
		for(int j = 0; j < 20; j++){ 
			cpoints[0][j].pos = cpoints[19][j].pos;
			cpoints[0][j].normal = cpoints[19][j].normal;
			}

}



void generate_surface_points()
{
	//generate cpoints1
	//column
	for(int j=0; j<20; j++)
	{
		for(int i=0;i<20;i++)
		{
			if(j==0)
			{
				cpoints1[2*i][0].pos = cpoints[i][0].pos;
				cpoints1[2*i][1].pos = (4.f*cpoints[i][0].pos+4.f*cpoints[i][1].pos)/8.f;
				cpoints1[2*i][0].tex_coord= cpoints[i][0].tex_coord;
				cpoints1[2*i][1].tex_coord = (4.f*cpoints[i][0].tex_coord+4.f*cpoints[i][1].tex_coord)/8.f;
			}else if(j==19){
				cpoints1[2*i][2*j].pos=cpoints[i][j].pos;
				cpoints1[2*i][2*j].tex_coord=cpoints[i][j].tex_coord;
			}else{
				cpoints1[2*i][2*j].pos=(cpoints[i][j-1].pos + 6.f*cpoints[i][j].pos + cpoints[i][j+1].pos)/8.f;
				cpoints1[2*i][2*j+1].pos=(4.f*cpoints[i][j].pos + 4.f*cpoints[i][j+1].pos)/8.f;
				cpoints1[2*i][2*j].tex_coord=(cpoints[i][j-1].tex_coord + 6.f*cpoints[i][j].tex_coord + cpoints[i][j+1].tex_coord)/8.f;
				cpoints1[2*i][2*j+1].tex_coord=(4.f*cpoints[i][j].tex_coord + 4.f*cpoints[i][j+1].tex_coord)/8.f;
			}
		}
	}
	//row
	for(int j=0; j<39; j++)
	{
		for(int i=0;i<20;i++)
		{
			if(i==0)
			{
				cpoints1[0][j].pos = cpoints1[0][j].pos;
				cpoints1[1][j].pos = (4.f*cpoints1[0][j].pos+4.f*cpoints1[2][j].pos)/8.f;
				cpoints1[0][j].tex_coord = cpoints1[0][j].tex_coord;
				cpoints1[1][j].tex_coord = (4.f*cpoints1[0][j].tex_coord+4.f*cpoints1[2][j].tex_coord)/8.f;
			}else if(i==19){
				cpoints1[2*i][j].pos=cpoints1[2*i][j].pos;
				cpoints1[2*i][j].tex_coord=cpoints1[2*i][j].tex_coord;
			}else{
				cpoints1[2*i][j].pos=(cpoints1[2*i-2][j].pos + 6.f*cpoints1[2*i][j].pos + cpoints1[2*i+2][j].pos)/8.f;
				cpoints1[2*i+1][j].pos=(4.f*cpoints1[2*i][j].pos + 4.f*cpoints1[2*i+2][j].pos)/8.f;
				cpoints1[2*i][j].tex_coord=(cpoints1[2*i-2][j].tex_coord + 6.f*cpoints1[2*i][j].tex_coord + cpoints1[2*i+2][j].tex_coord)/8.f;
				cpoints1[2*i+1][j].tex_coord=(4.f*cpoints1[2*i][j].tex_coord + 4.f*cpoints1[2*i+2][j].tex_coord)/8.f;
			}
		}
	}

	//generate cpoints2
	//column
	for(int j=0; j<39; j++)
	{
		for(int i=0;i<39;i++)
		{
			if(j==0)
			{
				cpoints2[2*i][0].pos = cpoints1[i][0].pos;
				cpoints2[2*i][1].pos = (4.f*cpoints1[i][0].pos+4.f*cpoints1[i][1].pos)/8.f;
				cpoints2[2*i][0].tex_coord = cpoints1[i][0].tex_coord;
				cpoints2[2*i][1].tex_coord = (4.f*cpoints1[i][0].tex_coord+4.f*cpoints1[i][1].tex_coord)/8.f;
			}else if(j==38){
				cpoints2[2*i][2*j].pos=cpoints1[i][j].pos;
				cpoints2[2*i][2*j].tex_coord=cpoints1[i][j].tex_coord;
			}else{
				cpoints2[2*i][2*j].pos=(cpoints1[i][j-1].pos + 6.f*cpoints1[i][j].pos + cpoints1[i][j+1].pos)/8.f;
				cpoints2[2*i][2*j+1].pos=(4.f*cpoints1[i][j].pos + 4.f*cpoints1[i][j+1].pos)/8.f;
				cpoints2[2*i][2*j].tex_coord=(cpoints1[i][j-1].tex_coord + 6.f*cpoints1[i][j].tex_coord + cpoints1[i][j+1].tex_coord)/8.f;
				cpoints2[2*i][2*j+1].tex_coord=(4.f*cpoints1[i][j].tex_coord + 4.f*cpoints1[i][j+1].tex_coord)/8.f;
			}
		}
	}
	//row
	for(int j=0; j<77; j++)
	{
		for(int i=0;i<39;i++)
		{
			if(i==0)
			{
				cpoints2[0][j].pos = cpoints2[0][j].pos;
				cpoints2[1][j].pos = (4.f*cpoints2[0][j].pos+4.f*cpoints2[2][j].pos)/8.f;
				cpoints2[0][j].tex_coord = cpoints2[0][j].tex_coord;
				cpoints2[1][j].tex_coord = (4.f*cpoints2[0][j].tex_coord+4.f*cpoints2[2][j].tex_coord)/8.f;
			}else if(i==38){
				cpoints2[2*i][j].pos=cpoints2[2*i][j].pos;
				cpoints2[2*i][j].tex_coord=cpoints2[2*i][j].tex_coord;
			}else{
				cpoints2[2*i][j].pos=(cpoints2[2*i-2][j].pos + 6.f*cpoints2[2*i][j].pos + cpoints2[2*i+2][j].pos)/8.f;
				cpoints2[2*i+1][j].pos=(4.f*cpoints2[2*i][j].pos + 4.f*cpoints2[2*i+2][j].pos)/8.f;
				cpoints2[2*i][j].tex_coord=(cpoints2[2*i-2][j].tex_coord + 6.f*cpoints2[2*i][j].tex_coord + cpoints2[2*i+2][j].tex_coord)/8.f;
				cpoints2[2*i+1][j].tex_coord=(4.f*cpoints2[2*i][j].tex_coord + 4.f*cpoints2[2*i+2][j].tex_coord)/8.f;
			}
		}
	}
	
	//generate cpoints3
	//column
	for(int j=0; j<77; j++)
	{
		for(int i=0;i<77;i++)
		{
			if(j==0)
			{
				cpoints3[2*i][0].pos = cpoints2[i][0].pos;
				cpoints3[2*i][1].pos = (4.f*cpoints2[i][0].pos+4.f*cpoints2[i][1].pos)/8.f;
				cpoints3[2*i][0].tex_coord = cpoints2[i][0].tex_coord;
				cpoints3[2*i][1].tex_coord = (4.f*cpoints2[i][0].tex_coord+4.f*cpoints2[i][1].tex_coord)/8.f;
			}else if(j==76){
				cpoints3[2*i][2*j].pos=cpoints2[i][j].pos;
				cpoints3[2*i][2*j].tex_coord=cpoints2[i][j].tex_coord;
			}else{
				cpoints3[2*i][2*j].pos=(cpoints2[i][j-1].pos + 6.f*cpoints2[i][j].pos + cpoints2[i][j+1].pos)/8.f;
				cpoints3[2*i][2*j+1].pos=(4.f*cpoints2[i][j].pos + 4.f*cpoints2[i][j+1].pos)/8.f;
				cpoints3[2*i][2*j].tex_coord=(cpoints2[i][j-1].tex_coord + 6.f*cpoints2[i][j].tex_coord + cpoints2[i][j+1].tex_coord)/8.f;
				cpoints3[2*i][2*j+1].tex_coord=(4.f*cpoints2[i][j].tex_coord + 4.f*cpoints2[i][j+1].tex_coord)/8.f;
			}
		}
	}
	//row
	for(int j=0; j<153; j++)
	{
		for(int i=0;i<77;i++)
		{
			if(i==0)
			{
				cpoints3[0][j].pos = cpoints3[0][j].pos;
				cpoints3[1][j].pos = (4.f*cpoints3[0][j].pos+4.f*cpoints3[2][j].pos)/8.f;
				cpoints3[0][j].tex_coord = cpoints3[0][j].tex_coord;
				cpoints3[1][j].tex_coord = (4.f*cpoints3[0][j].tex_coord+4.f*cpoints3[2][j].tex_coord)/8.f;
			
			}else if(i==76){
				cpoints3[2*i][j].pos=cpoints3[2*i][j].pos;
				cpoints3[2*i][j].tex_coord=cpoints3[2*i][j].tex_coord;
			}else{
				cpoints3[2*i][j].pos=(cpoints3[2*i-2][j].pos + 6.f*cpoints3[2*i][j].pos + cpoints3[2*i+2][j].pos)/8.f;
				cpoints3[2*i+1][j].pos=(4.f*cpoints3[2*i][j].pos + 4.f*cpoints3[2*i+2][j].pos)/8.f;
				cpoints3[2*i][j].tex_coord=(cpoints3[2*i-2][j].tex_coord + 6.f*cpoints3[2*i][j].tex_coord + cpoints3[2*i+2][j].tex_coord)/8.f;
				cpoints3[2*i+1][j].tex_coord=(4.f*cpoints3[2*i][j].tex_coord + 4.f*cpoints3[2*i+2][j].tex_coord)/8.f;
			}
		}
	}

	/*for(int j=0;j<153;j++)
		for(int i=0;i<153;i++)
			cpoints3[i][j].color=glm::vec4(0.7f,0.7f,0.7f,1.f);*/


	//clear vector
	spoints.clear();
	spoints.resize(0);
	//spoints.swap(vector<Vertex>());

	//color the surface points light gray and make their normal points to -z axis
	Vertex temp;
	temp.color = glm::vec4(0.9f,0.9f,0.9f,1.f);
	//temp.normal = glm::vec4(0.f,0.f,1.f,0.f);

	for(int j=0; j<152;j++)
	{
		for(int i=0;i<152;i++)
		{
			if(i != 0 && i !=152  && j != 0 && j!=152 )
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos))),0.f);
			}
			else if(i == 0 && j != 0  && j != 152)
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos))),0.f);
			}
			else if(i == 152 && j != 0  && j != 152)
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos))),0.f);
			}
			else if(j==0 && i!=0 && i!=152)
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos))),0.f);
			}
			else if(j==152 && i!=0 && i!=152)
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos))),0.f);
			}
			else if(i ==0 && j==0)
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j+1].pos-cpoints3[i][j].pos))),0.f);
			}
			else if( i == 0 && j== 152 )
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j-1].pos),glm::vec3(cpoints3[i][j].pos-cpoints3[i][j-1].pos))
													  +glm::cross(glm::vec3(cpoints3[i+1][j-1].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i+1][j].pos-cpoints3[i][j].pos))),0.f);
			}
			else if( i == 152 && j == 0 )
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i][j].pos-cpoints3[i-1][j].pos),glm::vec3(cpoints3[i-1][j+1].pos-cpoints3[i-1][j].pos))
													  +glm::cross(glm::vec3(cpoints3[i][j].pos-cpoints3[i-1][j+1].pos),glm::vec3(cpoints3[i][j+1].pos-cpoints3[i-1][j+1].pos))),0.f);
			}
			else if( i == 152 && j == 152)
			{
				temp.normal = -glm::vec4(glm::normalize(glm::cross(glm::vec3(cpoints3[i-1][j].pos-cpoints3[i][j].pos),glm::vec3(cpoints3[i][j-1].pos-cpoints3[i][j].pos))),0.f);
			}

			temp.pos=cpoints3[i][j].pos;
			temp.tex_coord=cpoints3[i][j].tex_coord;
			spoints.push_back(temp);

			temp.pos=cpoints3[i+1][j].pos;
			temp.tex_coord=cpoints3[i+1][j].tex_coord;
			spoints.push_back(temp);

			temp.pos=cpoints3[i][j+1].pos;
			temp.tex_coord=cpoints3[i][j+1].tex_coord;
			spoints.push_back(temp);

			temp.pos=cpoints3[i+1][j+1].pos;
			temp.tex_coord=cpoints3[i+1][j+1].tex_coord;
			spoints.push_back(temp);
			
			temp.pos=cpoints3[i][j+1].pos;
			temp.tex_coord=cpoints3[i][j+1].tex_coord;
			spoints.push_back(temp);
			
			temp.pos=cpoints3[i+1][j].pos;
			temp.tex_coord=cpoints3[i+1][j].tex_coord;
			spoints.push_back(temp);
		}
	}
	
	Generate_circle();
}

void cpoints_color_code()
{
	for( int j = 0; j < 20 ; j++)
	{
		for( int i = 0; i < 20 ; i++)
		{
			cpoints[i][j].color[0]=(float)i/20.f*0.8f+0.2;
			cpoints[i][j].color[1]=(float)j/20.f*0.8f+0.2;
			cpoints[i][j].color[2]=0.f;
			cpoints[i][j].color[3]=1.f;
		}
	}
}

void color_cpoints_green()
{
	for( int j = 0; j < 20 ; j++)
	{
		for( int i = 0; i < 20 ; i++)
		{
			cpoints[i][j].color=glm::vec4(0,1,0,1);
		}
	}
}
int color_code_pick_r(int x,int y){
    
    // draw the scene
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cpoints_color_code();
	draw_grid();
	glFlush();

    // read back the green channel of pixel under the cursor into data
    // data is in range [0,255]
	GLubyte data_r;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glReadPixels(x,viewport[3]-y,1,1,GL_RED,GL_UNSIGNED_BYTE,&data_r);;
    printf("color: %d\n", data_r);

    // TODO: identify the selected point and return its index
    // Di
	if(data_r)
		for(int j=0;j<20;j++)
		{
			for(int i=0;i<20;i++)
			{
				if (abs(cpoints[i][j].color[0]*255 - data_r)<1)
					return i;
			}
		}

	return -1;
}

int color_code_pick_g(int x,int y){
    
    // draw the scene
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cpoints_color_code();
	draw_grid();
	glFlush();

    // read back the green channel of pixel under the cursor into data
    // data is in range [0,255]
	GLubyte data_g;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glReadPixels(x,viewport[3]-y,1,1,GL_GREEN,GL_UNSIGNED_BYTE,&data_g);;
    printf("color: %d\n", data_g);

    // TODO: identify the selected point and return its index
    // Di
	if(data_g)
		for(int j=0;j<20;j++)
		{
			for(int i=0;i<20;i++)
			{
				if (abs(cpoints[i][j].color[1]*255 - data_g)<1)
					return j;
			}
		}

	return -1;
}
void load_obj(const char* filename, vector<glm::vec3> &vertices, vector<glm::vec3> &normals, vector<GLuint> &elements) {
	ifstream in(filename, ios::in);
	if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }
	 
	string line;
	while (getline(in, line)) {
		if (line.substr(0,2) == "v ") {
		  istringstream s(line.substr(2));
		  glm::vec3 v; 
		  s >> v.x; s >> v.y; s >> v.z; 
		  vertices.push_back(v);
		} else if (line.substr(0,2) == "f ") {
		  istringstream s(line.substr(2));
		  GLuint a,b,c;
		  s >> a; s >> b; s >> c;
		  a--; b--; c--;
		  elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
	  }
	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < elements.size(); i+=3) {
		GLuint ia = elements[i];
		GLuint ib = elements[i+1];
		GLuint ic = elements[i+2];
		glm::vec3 normal = glm::normalize(glm::cross( vertices[ib] - vertices[ia],vertices[ic] - vertices[ia]));
		normals[ia]+=normal;normals[ib]+=normal;normals[ic]+=normal;
	  }
  
}

void initialize_mesh(){	
	glm::mat4 scaleM = glm::scale(glm::mat4(1.0f),glm::vec3(15.f, 12.f ,15.f));
	glm::mat4 rotateM = glm::rotate(glm::mat4(1.0f), -60.f, glm::vec3(0,1.0,0));
	glm::mat4 transM = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8,0,0.8));
	
	for(int i=0; i< mesh_elements.size(); i++){
		Vertex temp;
		temp.pos=transM * rotateM * scaleM * glm::vec4(mesh_pos[mesh_elements[i]],1.f);
		temp.color = glm::vec4(0.8f,0.8f,0.8f,1.0f);
		temp.normal=glm::vec4(glm::normalize(mesh_normals[mesh_elements[i]]),0.0f);
		mesh_vertices.push_back(temp);
	}
}

void auto_fit(){
	printf("auto fitting...please wait.\n");
	for(int i = 0; i < 20; i++){
		for(int j = 0; j < 20; j++){
			int count = 0;
			for(int m = 0; m < mesh_vertices.size(); m+=3){
				float v1[3] = {mesh_vertices[m].pos.x,mesh_vertices[m].pos.y,mesh_vertices[m].pos.z};
				float v2[3] = {mesh_vertices[m+1].pos.x,mesh_vertices[m+1].pos.y,mesh_vertices[m+1].pos.z};
				float v3[3] = {mesh_vertices[m+2].pos.x,mesh_vertices[m+2].pos.y,mesh_vertices[m+2].pos.z};				
				float p[3] = {cpoints[i][j].pos.x,cpoints[i][j].pos.y,cpoints[i][j].pos.z};
				float l[3] = {cpoints[i][j].pos.x,0,cpoints[i][j].pos.z};//direct from {0,y,0} to p{x,y,z}
				float cg[3] = {0,0,0};
				
				ray_cast(&v1[0],&v2[0],&v3[0],&p[0],&l[0],&cg[0]);
				if((cg[0]>=0&&cg[0]<=1)&&(cg[1]>=0&&cg[1]<=1)&&(cg[2]>=0&&cg[2]<=1)){
					//ray and triangle intersects,P=sA+qB+rC
					glm::vec3 newPos = cg[0]*glm::vec3(mesh_vertices[m].pos) + cg[1]*glm::vec3(mesh_vertices[m+1].pos) + cg[2]*glm::vec3(mesh_vertices[m+2].pos);										
					if(newPos.x*p[0]>=0){//cg on the segment[{0,y,0},{x,y,z}]							
							count++;
							cpoints[i][j].pos = glm::vec4(newPos,1.f);
							
						}
					}
				if(count>0)
						break;
					}
			}
		}
	for(int j = 0; j < 20; j++){
		cpoints[0][j].pos = cpoints[18][j].pos;
		}
		
	generate_surface_points();
	printf("auto fitting over \n");
}


void createEyes(){
	double r = 0.1;
	
	double model_rightx = 0.8;
	double model_righty = 1.66;
	double model_rightz = -0.7;

	double model_leftx = 0;
	double model_lefty =1.66;
	double model_leftz = -1.1;
	
	createSphere(eyeMR,r,model_rightx,model_righty,model_rightz);
	createSphere(eyeML,r,model_leftx,model_lefty,model_leftz);
	
	double texture_rightx = 0.7;
	double texture_righty = 1.66;
	double texture_rightz = -0.8;

	double texture_leftx = 0.2;
	double texture_lefty = 1.65;
	double texture_leftz = -1;

	createSphere(eyeR,r,texture_rightx,texture_righty,texture_rightz);
	createSphere(eyeL,r,texture_leftx,texture_lefty,texture_leftz);
	
	double rightx = -0.55;
	double righty = 2.15;
	double rightz = -1.15;

	double leftx = -1;
	double lefty = 2.15;
	double leftz = -0.85;
	
	for(int i=0;i<eyeR.size();i++){
			eyeR[i].color=glm::vec4(1,1,0,1);
			eyeL[i].color=glm::vec4(1,1,0,1);
			eyeMR[i].color=glm::vec4(1,1,0,1);
			eyeML[i].color=glm::vec4(1,1,0,1);
		}
	}
const double space = 10;
void createSphere(vector<Vertex> &sphere, double R, double X, double Y, double Z){
	int n;
	double a;
	double b;
	
	for( b = 0; b <= 90 - space; b+=space)
	{
		for( a = 0; a <= 360 - space; a+=space)
		{
			Vertex temp;
			temp.pos.x = R * sin((a) / 180 * PI) * sin((b) / 180 * PI) + X;
			temp.pos.y = R * cos((a) / 180 * PI) * sin((b) / 180 * PI) + Y;
			temp.pos.z = R * cos((b) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);

			temp.pos.x = R * sin((a) / 180 * PI) * sin((b + space) / 180 * PI) + X;
			temp.pos.y = R * cos((a) / 180 * PI) * sin((b + space) / 180 * PI) + Y;
			temp.pos.z = R * cos((b + space) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);

			temp.pos.x = R * sin((a + space) / 180 * PI) * sin((b) / 180 * PI) + X;
			temp.pos.y = R * cos((a + space) / 180 * PI) * sin((b) / 180 * PI) + Y;
			temp.pos.z = R * cos((b) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);

			temp.pos.x = R * sin((a + space) / 180 * PI) * sin((b + space) / 180 * PI) + X;
			temp.pos.y = R * cos((a + space) / 180 * PI) * sin((b + space) / 180 * PI) + Y;
			temp.pos.z = R * cos((b + space) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);
		}
	}

	for( b = 0; b <= 90 - space; b+=space)
	{
		for( a = 0; a <= 360 - space; a+=space)
		{
			Vertex temp;
			temp.pos.x = R * sin((a) / 180 * PI) * sin((b) / 180 * PI) + X;
			temp.pos.y = R * cos((a) / 180 * PI) * sin((b) / 180 * PI) + Y;
			temp.pos.z = -R * cos((b) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);

			temp.pos.x = R * sin((a) / 180 * PI) * sin((b + space) / 180 * PI) + X;
			temp.pos.y = R * cos((a) / 180 * PI) * sin((b + space) / 180 * PI) + Y;
			temp.pos.z = -R * cos((b + space) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);

			temp.pos.x = R * sin((a + space) / 180 * PI) * sin((b) / 180 * PI) + X;
			temp.pos.y = R * cos((a + space) / 180 * PI) * sin((b) / 180 * PI) + Y;
			temp.pos.z = -R * cos((b) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);

			temp.pos.x = R * sin((a + space) / 180 * PI) * sin((b + space) / 180 * PI) + X;
			temp.pos.y = R * cos((a + space) / 180 * PI) * sin((b + space) / 180 * PI) + Y;
			temp.pos.z = -R * cos((b + space) / 180 * PI) + Z;
			temp.pos.w = 1.0f;
			temp.normal = temp.pos-glm::vec4(X,Y,Z,1.f);
			sphere.push_back(temp);
		}
	}
}

// MOUSE handling *******************************************

int last_x, last_y;
int selected_idx_i = -1;
int selected_idx_j = -1;

void mouse(int button, int state, int x, int y ){
	if(state == GLUT_DOWN){
        selected_idx_i = color_code_pick_r(x,y);
		selected_idx_j = color_code_pick_g(x,y);
		printf("pick point #%d %d\n", selected_idx_i,selected_idx_j );
	}else{
        
	}

    last_x = x;
    last_y = y;
	
}


void motion( int x, int y){
    GLint viewport[5];
	 float dx,dy;
    
	glGetIntegerv(GL_VIEWPORT,viewport);
    
    dx = (x - last_x)/(float)viewport[2]*5.f;
    dy = -(y - last_y)/(float)viewport[3]*5.f;

    // TODO: move the picked point
    // Di
//	cpoints[selected_idx_i][selected_idx_j].pos[2]-=dx;
//	cpoints[selected_idx_i][selected_idx_j].pos[1]+=dy;
    last_x = x;
    last_y = y;
	if(selected_idx_i ==0 || selected_idx_i == 19){
		cpoints[0][selected_idx_j].pos[0] = cpoints[0][selected_idx_j].pos[0] - cos(camera_j)*dx;
		cpoints[0][selected_idx_j].pos[1] = cpoints[0][selected_idx_j].pos[1] - cos(camera_i)*dy;
		cpoints[0][selected_idx_j].pos[2] = cpoints[0][selected_idx_j].pos[2] - sin(camera_j)*dx + sin(camera_i)*dy;
		cpoints[19][selected_idx_j].pos[0] = cpoints[0][selected_idx_j].pos[0];
		cpoints[19][selected_idx_j].pos[1] = cpoints[0][selected_idx_j].pos[1];
		cpoints[19][selected_idx_j].pos[2] = cpoints[0][selected_idx_j].pos[2];
		}else{
			cpoints[selected_idx_i][selected_idx_j].pos[0] = cpoints[selected_idx_i][selected_idx_j].pos[0] - cos(camera_j)*dx;
			cpoints[selected_idx_i][selected_idx_j].pos[1] = cpoints[selected_idx_i][selected_idx_j].pos[1] - cos(camera_i)*dy;
			cpoints[selected_idx_i][selected_idx_j].pos[2] = cpoints[selected_idx_i][selected_idx_j].pos[2] - sin(camera_j)*dx + sin(camera_i)*dy;			
			}
//	printf("camera_i= %d,camera_j= %d\n",camera_i,camera_j);		
	generate_surface_points();  

    glutPostRedisplay();    
}

float smile_value = 0.f;
int dir = 1;
void timer(int value){
	if(smile){
		if(smile_value>0.1||smile_value<=0.f){
			dir = -dir;
			}
		smile_value += 0.01*dir;
		cpoints[10][4].pos[1] += 0.02*dir;
		cpoints[12][4].pos[1] += 0.02*dir;
		
		cpoints[10][4].pos[0] -= 0.01*dir;
		cpoints[12][4].pos[0] += 0.01*dir;		
		cpoints[10][4].pos[2] -= 0.01*dir;
		cpoints[12][4].pos[2] += 0.01*dir;
		}
		generate_surface_points();
		glutPostRedisplay();
		glutTimerFunc(60,&timer,0);
	}


void set_mvp() 
{ 
    modelView_grid = model_grid; 
	modelView_cpoints = model_cpoints;
	modelView_cpoints3 = model_cpoints3;
	modelView_surface = model_surface;
	modelView_mesh=model_mesh;
	modelView_eyel=model_eyel;
	modelView_eyer=model_eyer;
}

void reset_state()
{
	camera_i=45; 
	camera_j=15;
	initialize_cpoints();
	generate_surface_points(); 
	model_grid = glm::mat4(1);
	model_cpoints = glm::mat4(1);
	model_cpoints3 = glm::mat4(1);
	model_surface = glm::mat4(1);
	model_mesh = glm::mat4(1);
	model_eyer = glm::mat4(1);
	model_eyel = glm::mat4(1);
    view = glm::lookAt(glm::vec3(10*sqrt(3.f)*cos(camera_i*PI/180.f)*cos(camera_j*PI/180.f),10*sqrt(3.f)*sin(camera_j*PI/180), -10*sqrt(3.f)*sin(camera_i*PI/180)*cos(camera_j*PI/180)), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
    projection = glm::perspective(28.f, 4.f/3.f,0.1f,100.0f); 
    
}

// KEYBOARD handling *******************************************

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':
        graphics_init();
		reset_state();
        break;
	case 'c':
		cameraMoving=!cameraMoving;
		centerMoving = false;
		break;
	case 'd':
		show_cpoints=!show_cpoints;
		break;
	case 's':
		show_surface=!show_surface;
		break;
	case 't':
		if(show_texture==1)
			show_texture=0;
		else
			show_texture=1;
		break;
	case 'f':
		show_model=!show_model;
		break;
	case 'e':
		show_eyes=!show_eyes;
		break;
	case 'm':{
		auto_fit();
		autofit = !autofit;
		break;}
	case 'l':
		smile = !smile;
		break;
	case 'p':
		centerMoving = !centerMoving;
		cameraMoving = false;
		break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void special_key(int key, int x, int y)
{
    // TODO: capture the arrow key here 

	float a=10.f; 
	float b=4.f; 
	float c=10.f; 
	if(cameraMoving){
		switch (key){ 
		case GLUT_KEY_LEFT: 
			camera_i++; 
			break; 
		case GLUT_KEY_RIGHT: 
			camera_i--; 
			break; 
		case GLUT_KEY_UP: 
			camera_j--; 
			break; 
		case GLUT_KEY_DOWN: 
			camera_j++; 
			break; 
		} 	
		
		float thetaZ,thetaY; 
		thetaZ=camera_i*PI/180.f; 
		thetaY=camera_j*PI/180.f; 
		a=10*sqrt(3.f)*cos(thetaZ)*cos(thetaY); 
		b=10*sqrt(3.f)*sin(thetaY); 
		c=-10*sqrt(3.f)*sin(thetaZ)*cos(thetaY); 

		view = glm::lookAt(glm::vec3(a,b,c), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
		
		set_mvp(); 	
		
		}else if(centerMoving){
			switch (key){ 
			case GLUT_KEY_UP: 
				topCenter.pos+=glm::vec4(0.f,0.1f,0.f,0.f); 
				break; 
			case GLUT_KEY_DOWN: 
				topCenter.pos-=glm::vec4(0.f,0.1f,0.f,0.f);
				break; 
			} 			
			Generate_circle();
		}else{}




	glutPostRedisplay();

}

// DISPLAY and RENDERING functions *************************


void draw_grid(){
    
    GLuint vao;
    GLuint grid_vbo;
	GLuint top_vbo;
	
    GLuint position_location;
    GLuint color_location;
	GLuint normal_location;
	GLuint texcoord_location;

    GLuint MVP_location;
	GLuint p_location; 
    GLuint v_location; 
    GLuint m_location; 

    // specify the shaders we want to use
    glUseProgram(program);

    // get the input variable location in this shader
    position_location = glGetAttribLocation(program, "in_position");
    color_location = glGetAttribLocation(program, "in_color");
	normal_location = glGetAttribLocation(program, "in_normal");
	texcoord_location = glGetAttribLocation(program, "in_texcoord");


    MVP_location = glGetUniformLocation(program, "MVP"); 
	p_location = glGetUniformLocation(program, "P"); 
    v_location = glGetUniformLocation(program, "V"); 
    m_location = glGetUniformLocation(program, "M"); 
	
    // create and bind a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // enable the input locations we wan to use
    glEnableVertexAttribArray(position_location);
    glEnableVertexAttribArray(color_location);
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(texcoord_location);

	//glUniformMatrix4fv(MVP_location, 1, GL_FALSE, &mvp_grid[0][0]); 

	glUniformMatrix4fv(p_location, 1, GL_FALSE, &projection[0][0]); 
    glUniformMatrix4fv(v_location, 1, GL_FALSE, &view[0][0]); 
    glUniformMatrix4fv(m_location, 1, GL_FALSE, &modelView_grid[0][0]); 

    // draw points
	glPointSize(5);
	if(show_cpoints) draw_cpoints();
	if(show_surface) draw_surface();
	if(show_model) draw_mesh();
	if(show_eyes) draw_eyes();
	
	GLuint light_location;
	light_location = glGetUniformLocation(program, "light");
	glUniform1i(light_location,0);

	GLuint texture_location;
	texture_location = glGetUniformLocation(program, "texture_flag");
	glUniform1i(texture_location,0);

	/*GLuint green_location;
	green_location = glGetUniformLocation(program, "green");
	glUniform1i(green_location,0);*/
	

    // generate and bind a vertex buffer to hold the vertex data on GPU
    glGenBuffers(1, &grid_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);

    // initialize the vertex buffer with the vertex data
    
    glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(Vertex), &grid_vertices[0] , GL_STATIC_DRAW);

    // specify the input data format
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    // draw points
    //glPointSize(10);
    glDrawArrays(GL_LINES, 0, grid_vertices.size());
/*
	//check
	GLuint ck_vbo;
	glGenBuffers(1, &ck_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, ck_vbo);   
    glBufferData(GL_ARRAY_BUFFER, checknormal.size() * sizeof(Vertex), &checknormal[0] , GL_STATIC_DRAW);
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glDrawArrays(GL_LINES, 0, checknormal.size());
	*/
	
    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Delete VAO and VBO
    glDeleteBuffers(1, &grid_vbo);
    glDeleteVertexArrays(1, &vao);
  
}

void draw_cpoints()
{
	GLuint vao; 
    glUseProgram(program); 
  
    GLuint position_location; 
    GLuint color_location; 
	GLuint normal_location;
	GLuint texcoord_location;
    GLuint p_location; 
    GLuint v_location; 
    GLuint m_location; 
  
    position_location = glGetAttribLocation(program, "in_position"); 
    color_location = glGetAttribLocation(program, "in_color"); 
	normal_location = glGetAttribLocation(program, "in_normal"); 
	texcoord_location = glGetAttribLocation(program, "in_texcoord"); 
  
    p_location = glGetUniformLocation(program, "P"); 
    v_location = glGetUniformLocation(program, "V"); 
    m_location = glGetUniformLocation(program, "M"); 
  
    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao); 
  
    glEnableVertexAttribArray(position_location); 
    glEnableVertexAttribArray(color_location); 
	glEnableVertexAttribArray(normal_location); 
	glEnableVertexAttribArray(texcoord_location); 

    glUniformMatrix4fv(p_location, 1, GL_FALSE, &projection[0][0]); 
    glUniformMatrix4fv(v_location, 1, GL_FALSE, &view[0][0]); 
    glUniformMatrix4fv(m_location, 1, GL_FALSE, &modelView_cpoints[0][0]); 
  
	/*GLuint green_location;
	green_location = glGetUniformLocation(program, "green");
	glUniform1i(green_location,1);*/

	GLuint light_location;
	light_location = glGetUniformLocation(program, "light");
	glUniform1i(light_location,0);

	GLuint texture_location;
	texture_location = glGetUniformLocation(program, "texture_flag");
	glUniform1i(texture_location,0);

    GLuint cpoints_vbo; 
  
    glGenBuffers(1, &cpoints_vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, cpoints_vbo); 
    glBufferData(GL_ARRAY_BUFFER, ncpoints*sizeof(Vertex), &cpoints[0][0] , GL_STATIC_DRAW); 
  
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); 
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord)); 

    glDrawArrays(GL_POINTS,0,ncpoints); 
	
	GLuint topP_vbo;
	glGenBuffers(1, &topP_vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, topP_vbo); 
    glBufferData(GL_ARRAY_BUFFER, 1*sizeof(Vertex), &topCenter , GL_STATIC_DRAW); 
  
    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); 
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord)); 

    glDrawArrays(GL_POINTS,0,1); 
	
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glDeleteBuffers(1, &cpoints_vbo); 
    glDeleteBuffers(1, &topP_vbo); 
}

void draw_eyes(){
	
	normalMatrix_eyel = glm::transpose(glm::inverse(glm::mat3(modelView_eyel)));
	glm::mat4 view_inv = glm::inverse(view);

	GLuint vao; 
    glUseProgram(program); 
  
    GLuint position_location; 
    GLuint color_location; 
	GLuint normal_location;
	GLuint texcoord_location;
	GLuint normalMatrix_location;

    GLuint p_location; 
    GLuint v_location; 
    GLuint m_location; 
	GLuint v_inv_location;
  
    position_location = glGetAttribLocation(program, "in_position"); 
    color_location = glGetAttribLocation(program, "in_color"); 
	normal_location = glGetAttribLocation(program, "in_normal");
	texcoord_location = glGetAttribLocation(program, "in_texcoord");

	normalMatrix_location = glGetUniformLocation(program, "normalmatrix");
    p_location = glGetUniformLocation(program, "P"); 
    v_location = glGetUniformLocation(program, "V"); 
    m_location = glGetUniformLocation(program, "M"); 
	v_inv_location = glGetUniformLocation(program, "v_inv");
  
    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao); 
  
    glEnableVertexAttribArray(position_location); 
    glEnableVertexAttribArray(color_location); 
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(texcoord_location);

    glUniformMatrix4fv(p_location, 1, GL_FALSE, &projection[0][0]); 
    glUniformMatrix4fv(v_location, 1, GL_FALSE, &view[0][0]); 
    glUniformMatrix4fv(m_location, 1, GL_FALSE, &modelView_eyel[0][0]); 
	glUniformMatrix3fv(normalMatrix_location, 1, GL_FALSE, &normalMatrix_eyel[0][0]);
	glUniformMatrix4fv(v_inv_location, 1, GL_FALSE, &view_inv[0][0]);

	GLuint light_location;
	light_location = glGetUniformLocation(program, "light");
	glUniform1i(light_location,1);

	GLuint texture_location;
	texture_location = glGetUniformLocation(program, "texture_flag");
	glUniform1i(texture_location,0);

	/*GLuint green_location;
	green_location = glGetUniformLocation(program, "green");
	glUniform1i(green_location,0);*/

    GLuint eyel_vbo, eyer_vbo; 
	
	if(autofit){
		glGenBuffers(1, &eyel_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, eyel_vbo); 
		glBufferData(GL_ARRAY_BUFFER, eyeL.size()*sizeof(Vertex), &eyeL[0] , GL_STATIC_DRAW); 

		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
		glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));  
		glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));  

		glDrawArrays(GL_TRIANGLE_STRIP,0,eyeL.size()); 

		glGenBuffers(1, &eyer_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, eyer_vbo); 
		glBufferData(GL_ARRAY_BUFFER, eyeR.size()*sizeof(Vertex), &eyeR[0] , GL_STATIC_DRAW); 

		glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
		glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
		glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));  
		glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));  

		glDrawArrays(GL_TRIANGLE_STRIP,0,eyeR.size()); 
		}else if(autofit==false && show_model){	
			glGenBuffers(1, &eyel_vbo); 
			glBindBuffer(GL_ARRAY_BUFFER, eyel_vbo); 
			glBufferData(GL_ARRAY_BUFFER, eyeL.size()*sizeof(Vertex), &eyeML[0] , GL_STATIC_DRAW); 

			glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
			glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
			glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));  
			glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));  

			glDrawArrays(GL_TRIANGLE_STRIP,0,eyeL.size()); 

			glGenBuffers(1, &eyer_vbo); 
			glBindBuffer(GL_ARRAY_BUFFER, eyer_vbo); 
			glBufferData(GL_ARRAY_BUFFER, eyeR.size()*sizeof(Vertex), &eyeMR[0] , GL_STATIC_DRAW); 

			glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
			glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
			glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));  
			glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));  

			glDrawArrays(GL_TRIANGLE_STRIP,0,eyeR.size()); 
		}
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glDeleteBuffers(1, &eyel_vbo); 
    glDeleteBuffers(1, &eyer_vbo); 
}

void draw_surface(){
	normalMatrix_surface = glm::transpose(glm::inverse(glm::mat3(modelView_surface)));
	glm::mat4 view_inv = glm::inverse(view);

	GLuint vao; 
    glUseProgram(program); 
  
    GLuint position_location; 
    GLuint color_location; 
	GLuint normal_location;
	GLuint texcoord_location;
	GLuint normalMatrix_location;
	GLuint topSurface_vbo;
	
    GLuint p_location; 
    GLuint v_location; 
    GLuint m_location; 
	GLuint v_inv_location;
  
    position_location = glGetAttribLocation(program, "in_position"); 
    color_location = glGetAttribLocation(program, "in_color"); 
	normal_location = glGetAttribLocation(program, "in_normal");
	texcoord_location = glGetAttribLocation(program, "in_texcoord");

	normalMatrix_location = glGetUniformLocation(program, "normalmatrix");
    p_location = glGetUniformLocation(program, "P"); 
    v_location = glGetUniformLocation(program, "V"); 
    m_location = glGetUniformLocation(program, "M"); 
	v_inv_location = glGetUniformLocation(program, "v_inv");
  
    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao); 
  
    glEnableVertexAttribArray(position_location); 
    glEnableVertexAttribArray(color_location); 
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(texcoord_location);

    glUniformMatrix4fv(p_location, 1, GL_FALSE, &projection[0][0]); 
    glUniformMatrix4fv(v_location, 1, GL_FALSE, &view[0][0]); 
    glUniformMatrix4fv(m_location, 1, GL_FALSE, &modelView_surface[0][0]); 
	glUniformMatrix3fv(normalMatrix_location, 1, GL_FALSE, &normalMatrix_surface[0][0]);
	glUniformMatrix4fv(v_inv_location, 1, GL_FALSE, &view_inv[0][0]);

	GLuint light_location;
	light_location = glGetUniformLocation(program, "light");
	glUniform1i(light_location,1);

	GLuint texture_location;
	texture_location = glGetUniformLocation(program, "texture_flag");
	glUniform1i(texture_location,show_texture);


	glUniform1i(glGetUniformLocation(program, "face_tex"), 0);

	/*GLuint green_location;
	green_location = glGetUniformLocation(program, "green");
	glUniform1i(green_location,0);*/

    GLuint surface_vbo; 
  
    glGenBuffers(1, &surface_vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, surface_vbo); 
    glBufferData(GL_ARRAY_BUFFER, spoints.size()*sizeof(Vertex), &spoints[0] , GL_STATIC_DRAW); 
	//glBufferData(GL_ARRAY_BUFFER, 153*153*sizeof(Vertex), &cpoints3[0][0] , GL_STATIC_DRAW); 
	//glBufferData(GL_ARRAY_BUFFER, 39*39*sizeof(Vertex), &cpoints1[0][0] , GL_STATIC_DRAW); 

    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));  
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));  

	glDrawArrays(GL_TRIANGLES,0,spoints.size()); 
	//glDrawArrays(GL_POINTS,0,153*153);
	//glDrawArrays(GL_POINTS,0,39*39);
	
	glDeleteBuffers(1, &surface_vbo); 

	glGenBuffers(1, &topSurface_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, topSurface_vbo);
	glBufferData(GL_ARRAY_BUFFER,  round1.size()* sizeof(Vertex), &round1[0] , GL_STATIC_DRAW);
	glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
	glPointSize(2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, round1.size());	
//	Vertex temp = round1.at(1);
	//printf("%f %f %f \n",temp.color[0],temp.color[1],temp.color[2]);
	
	
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
    glDeleteBuffers(1, &topSurface_vbo);
}
void draw_mesh()
{
	normalMatrix_mesh = glm::transpose(glm::inverse(glm::mat3(modelView_mesh)));
	glm::mat4 view_inv = glm::inverse(view);

	GLuint vao; 
    glUseProgram(program); 
  
    GLuint position_location; 
    GLuint color_location; 
	GLuint normal_location;
	GLuint texcoord_location;
	GLuint normalMatrix_location;

    GLuint p_location; 
    GLuint v_location; 
    GLuint m_location; 
	GLuint v_inv_location;
  
    position_location = glGetAttribLocation(program, "in_position"); 
    color_location = glGetAttribLocation(program, "in_color"); 
	normal_location = glGetAttribLocation(program, "in_normal");
	texcoord_location = glGetAttribLocation(program, "in_texcoord");

	normalMatrix_location = glGetUniformLocation(program, "normalmatrix");
    p_location = glGetUniformLocation(program, "P"); 
    v_location = glGetUniformLocation(program, "V"); 
    m_location = glGetUniformLocation(program, "M"); 
	v_inv_location = glGetUniformLocation(program, "v_inv");
  
    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao); 
  
    glEnableVertexAttribArray(position_location); 
    glEnableVertexAttribArray(color_location); 
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(texcoord_location);

    glUniformMatrix4fv(p_location, 1, GL_FALSE, &projection[0][0]); 
    glUniformMatrix4fv(v_location, 1, GL_FALSE, &view[0][0]); 
    glUniformMatrix4fv(m_location, 1, GL_FALSE, &modelView_mesh[0][0]); 
	glUniformMatrix3fv(normalMatrix_location, 1, GL_FALSE, &normalMatrix_mesh[0][0]);
	glUniformMatrix4fv(v_inv_location, 1, GL_FALSE, &view_inv[0][0]);

	GLuint light_location;
	light_location = glGetUniformLocation(program, "light");
	glUniform1i(light_location,1);

	GLuint texture_location;
	texture_location = glGetUniformLocation(program, "texture_flag");
	glUniform1i(texture_location,0);

	/*GLuint green_location;
	green_location = glGetUniformLocation(program, "green");
	glUniform1i(green_location,0);*/

    GLuint mesh_vbo; 
  
    glGenBuffers(1, &mesh_vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo); 
    glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size()*sizeof(Vertex), &mesh_vertices[0] , GL_STATIC_DRAW); 

    glVertexAttribPointer(position_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); 
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
	glVertexAttribPointer(normal_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));  
	glVertexAttribPointer(texcoord_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));  

	glDrawArrays(GL_TRIANGLES,0,mesh_vertices.size()); 

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glDeleteBuffers(1, &mesh_vbo); 
}

void display(){
    // Clear Viewport
    glClearColor(0.f,0.f,0.f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	color_cpoints_green();
    draw_grid();

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
    read_shader_source_code("vs.glsl", buf, 4096);
    cout << buf << endl;
    int vertex_shader_source_length = strlen(buf);
    const char *vertex_shader_sources[] = { buf };

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_sources, &vertex_shader_source_length);
    glCompileShader(vertex_shader);
    check_shader_compile_error(vertex_shader);

    // init fragment shader 
    read_shader_source_code("fs.glsl", buf, 4096); 
    int fragment_shader_source_length = strlen(buf);
    const char *fragment_shader_sources[] = { buf };
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_sources, &fragment_shader_source_length);
    glCompileShader(fragment_shader);
	check_shader_compile_error(fragment_shader);

    // init program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
	check_program_linking_error(program);

	// load and bind the texture
	GLuint texture_id = load_texture_TGA( "qian.tga", NULL, NULL, GL_CLAMP, GL_CLAMP );
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
  
    // enable depth test
    glEnable(GL_DEPTH_TEST);


}

void print_info()
{
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
	reset_state();
    initialize_points();
 	generate_surface_points();
	Generate_circle();
	load_obj("qian.obj", mesh_pos, mesh_normals, mesh_elements);
	initialize_mesh();
	createEyes();
	
	//
/*	checknormal.resize(0);
	for (int i = 0;i < spoints.size(); i++){
			Vertex temp1,temp2;
			temp1.pos = spoints.at(i).pos+spoints.at(i).normal;
			temp1.color = glm::vec4(1,0,0,1);
			checknormal.push_back(temp1);
			temp2.pos = spoints.at(i).pos;
			temp2.color = glm::vec4(1,0,0,1);
			checknormal.push_back(temp2);
			
		}
*/		
	
    // set up callbacks
    glutReshapeFunc(&reshape);
    glutDisplayFunc(&display);
    glutKeyboardFunc(&keyboard);
    glutMouseFunc(&mouse);
    glutMotionFunc(&motion);
    glutSpecialFunc(&special_key);
    glutTimerFunc(60,&timer,0);
    // main loop
    glutMainLoop();
    return 0;
}
