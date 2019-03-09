/*
 View.cpp
 CPSC 8170 Physically Based Animation
 Donald H. House     8/23/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 8/31/2018

 Implementation for Particle System Viewer
*/

#include "View.h"
#include "StateVector.h"

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cstdlib>
#include <iostream>

// Screen width and height, proportions match 1080P
#define WIDTH 960
#define HEIGHT 540

// Distance of near and far clipping planes, and
// camera vertical field-of-view in degrees
#define NEAR  1.0
#define FAR   1000.0
#define FOV   60.0

// Scale of the model, and its initial distance from the camera
#define MODELSIZE   20.0
#define MODELDEPTH  30.0

// Shading parameters
#define DIFFUSE_FRACTION  0.8
#define SPECULAR_FRACTION 0.2
#define SHININESS         60.0

// Light colors
#define WHITE     0.8, 0.8, 0.8, 1
#define DIM_WHITE 0.4, 0.4, 0.4, 1

// Screen background color
#define GREY_BACKGROUND 0.839, 0.839, 0.839, 1

// Material colors
#define BASE_COLOR  0.874, 0.901, 0.925     // diffuse color
#define HIGHLIGHT_COLOR 1.0, 1.0, 1.0   // specular color
#define BOX_COLOR 0.561, 0.737, 0.561 // used to color box

using namespace std;

//
// Constructor: set parameters for viewing, initialize camera, and
// establish link to Model
//
View::View(Model *model):
  camera(NULL), themodel(model),
  width(WIDTH), height(HEIGHT),
  near(NEAR), far(FAR), fov(FOV),
  modelsize(MODELSIZE), modeldepth(MODELDEPTH),
  diffuse_fraction(DIFFUSE_FRACTION), specular_fraction(SPECULAR_FRACTION), shininess(SHININESS),
  white{WHITE}, dim_white{DIM_WHITE}, grey_background{GREY_BACKGROUND},
  base_color{BASE_COLOR}, highlight_color{HIGHLIGHT_COLOR}, box_color{BOX_COLOR}{
    
  // Set up camera: parameters are eye point, aim point, up vector,
  // near and far clip plane distances, and camera vertical FOV in degrees
  camera = new Camera(Vector3d(0, 0, modeldepth), Vector3d(0, 0, 0), Vector3d(0, 1, 0),
                      near, far, fov);

  // point to the model
  themodel = model;

  // initialize current window dimensions to match default
  Width = width;
  Height = height;

  // load in obj model
  objloader = ObjLoader();
  objloader.LoadObj("skeleton.obj");
  obj = objloader.ReturnObj();

  // compute bounding box of the obj
  float minX, maxX, minY, maxY, minZ, maxZ;
  minX = obj.VertexArray[0].X;
  minY = obj.VertexArray[0].Y;
  minZ = obj.VertexArray[0].Z;
  maxX = minX;
  maxY = minY;
  maxZ = minZ;
  for (int i = 1; i < obj.NumVertex; i ++)
  {
      if (obj.VertexArray[i].X < minX){minX = obj.VertexArray[i].X;}
      if (obj.VertexArray[i].Y < minY){minY = obj.VertexArray[i].Y;}
      if (obj.VertexArray[i].Z < minZ){minZ = obj.VertexArray[i].Z;}
      if (obj.VertexArray[i].X > maxX){maxX = obj.VertexArray[i].X;}
      if (obj.VertexArray[i].Y > maxY){maxY = obj.VertexArray[i].Y;}
      if (obj.VertexArray[i].Z > maxZ){maxZ = obj.VertexArray[i].Z;}
  }

  float thresh = 0.02;

  themodel->setBoundingBox(minX - thresh, minY - thresh, minZ - thresh, maxX + thresh, maxY + thresh, maxZ + thresh);
  themodel->constructLattice();
  Lattice* L = themodel->getLPointer();
  Particle* P = themodel->getParticles();

  meshVertices = new MeshVertex[obj.NumVertex];

  for (int j = 0; j < obj.NumVertex; j ++)
  {
     int index = L->searchCellIndex(obj.VertexArray[j].X, obj.VertexArray[j].Y, obj.VertexArray[j].Z);
     meshVertices[j].cellIndex = index;
     int p0 = L->cells[index].vertIndices[0];
     int p1 = L->cells[index].vertIndices[1];
     int p2 = L->cells[index].vertIndices[2];
     int p4 = L->cells[index].vertIndices[4];

     meshVertices[j].u = (obj.VertexArray[j].X - P[p0].position.x)/(P[p1].position.x - P[p0].position.x);
     meshVertices[j].v = (obj.VertexArray[j].Y - P[p0].position.y)/(P[p2].position.y - P[p0].position.y);
     meshVertices[j].w = (obj.VertexArray[j].Z - P[p0].position.z)/(P[p4].position.z - P[p0].position.z);
  }
}

//
// Routine to initialize the state of the viewer to start-up defaults
//
void View::setInitialView(){
  // return camera to its default settings
  camera->Reset();
  
  // window background color is opaque grey initially
  BackgroundGrey = false;
  glClearColor(0, 0, 0, 1);

  ShowLattice = false;

  // smooth shade across triangles if vertex normals are present
  glShadeModel(GL_SMOOTH);
  
  // make sure that all surface normal vectors are unit vectors
  glEnable(GL_NORMALIZE);
  
  // enable dept testing for hidden surfaces
  glEnable(GL_DEPTH_TEST);
  glDepthRange(0.0, 1.0);
  
  // set the colors of the key, fill, and back lights
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  
  glLightfv(GL_LIGHT1, GL_DIFFUSE, dim_white);
  glLightfv(GL_LIGHT1, GL_SPECULAR, dim_white);
  
  glLightfv(GL_LIGHT2, GL_DIFFUSE, dim_white);
  glLightfv(GL_LIGHT2, GL_SPECULAR, dim_white);
  
  // turn on lighting
  glEnable(GL_LIGHT0);      // key light
  KeyOn = true;
  glEnable(GL_LIGHT1);      // fill light
  FillOn = true;
  glEnable(GL_LIGHT2);      // back light
  BackOn = true;
  
  // turn on shading
  glEnable(GL_LIGHTING);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // consider light position for specular
  
  // define the diffuse and specular colors
  // and set its specular exponent
  float diffuse_color[4], specular_color[4];
  for(int i = 0; i < 3; i++){
    diffuse_color[i] = diffuse_fraction * base_color[i];
    specular_color[i] = specular_fraction * highlight_color[i];
  }
  diffuse_color[3] = specular_color[3] = 1;
  
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

//
// Position the 3 lights
//
void View::setLights(){
  // key is point light above and behind camera to the left
  const float key_light_position[4] = {-modeldepth / 2, modeldepth / 2, modeldepth / 2, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, key_light_position);
  
  // fill is point light at eye level to right
  const float fill_light_position[4] = {modeldepth / 2, 0, 0, 1};
  glLightfv(GL_LIGHT1, GL_POSITION, fill_light_position);

  // back is parallel light coming from behind object, and above and to left
  const float back_light_direction[4] = {-2 * modeldepth, 2 * modeldepth, -2 * modeldepth, 0};
  glLightfv(GL_LIGHT2, GL_POSITION, back_light_direction);
}

// Toggle the lights on/off
void View::toggleKeyLight(){
  KeyOn = !KeyOn;
  if(KeyOn)
    glEnable(GL_LIGHT0);
  else
    glDisable(GL_LIGHT0);
}

void View::toggleFillLight(){
  FillOn = !FillOn;
  if(FillOn)
    glEnable(GL_LIGHT1);
  else
    glDisable(GL_LIGHT1);
}

void View::toggleBackLight(){
  BackOn = !BackOn;
  if(BackOn)
    glEnable(GL_LIGHT2);
  else
    glDisable(GL_LIGHT2);
}

// toggle window background color between grey and black
void View::toggleBackColor(){
  BackgroundGrey = !BackgroundGrey;
  if(BackgroundGrey)
    glClearColor(grey_background[0], grey_background[1], grey_background[2], grey_background[3]);
  else
    glClearColor(0, 0, 0, 1);
}

void View::toggleLattice(){
   ShowLattice = !ShowLattice;
}
// draw the box, and also the ball, if the simulation is running
void View::drawModel(){

  // nothing to do if the simulation is not running
  if(themodel->isSimRunning()){

     StateVector* S = themodel->getSPointer();
     int np = S->getNumParticles();
     int ns = themodel->getNumStruts();
     Strut* ST = themodel->getStruts();
     Particle* P = themodel->getParticles();
     Lattice* L = themodel->getLPointer();

    
     
     glBegin(GL_TRIANGLES);
        for (int t = 0; t < obj.NumTriangle; t++){
           int v0 = obj.TriangleArray[t].Vertex[0]; 
           int v1 = obj.TriangleArray[t].Vertex[1]; 
           int v2 = obj.TriangleArray[t].Vertex[2];

           int n0 = obj.TriangleArray[t].Normal[0];
           int n1 = obj.TriangleArray[t].Normal[1];
           int n2 = obj.TriangleArray[t].Normal[2];

           int cell_i_v0 = meshVertices[v0].cellIndex;
           Particle p0_v0 = P[L->cells[cell_i_v0].vertIndices[0]];
           Particle p1_v0 = P[L->cells[cell_i_v0].vertIndices[1]];
           Particle p2_v0 = P[L->cells[cell_i_v0].vertIndices[2]];
           Particle p3_v0 = P[L->cells[cell_i_v0].vertIndices[3]];
           Particle p4_v0 = P[L->cells[cell_i_v0].vertIndices[4]];
           Particle p5_v0 = P[L->cells[cell_i_v0].vertIndices[5]];
           Particle p6_v0 = P[L->cells[cell_i_v0].vertIndices[6]];
           Particle p7_v0 = P[L->cells[cell_i_v0].vertIndices[7]];
           float u_v0 = meshVertices[v0].u;
           float v_v0 = meshVertices[v0].v;
           float w_v0 = meshVertices[v0].w;

           Vector3d v0prime;
           v0prime.set(((1 - u_v0) * (1 - v_v0) * w_v0 * p4_v0.position) + (u_v0 * (1 - v_v0) * w_v0 * p5_v0.position)
                        + ((1 - u_v0) * v_v0 * w_v0 * p6_v0.position) + (u_v0 * v_v0 * w_v0 * p7_v0.position)
                        + ((1 - u_v0) * (1 - v_v0) * (1 - w_v0) * p0_v0.position) + (u_v0 * (1 - v_v0) * (1 - w_v0) * p1_v0.position)
                        + ((1 - u_v0) * v_v0 * (1 - w_v0) * p2_v0.position) + (u_v0 * v_v0 * (1 - w_v0) * p3_v0.position));
           Vector3d v0Trans;
           v0Trans.set(v0prime.x - obj.VertexArray[v0].X, v0prime.y - obj.VertexArray[v0].Y, v0prime.z - obj.VertexArray[v0].Z);

           //===========================================================================

	   int cell_i_v1 = meshVertices[v1].cellIndex;
           Particle p0_v1 = P[L->cells[cell_i_v1].vertIndices[0]];
           Particle p1_v1 = P[L->cells[cell_i_v1].vertIndices[1]];
           Particle p2_v1 = P[L->cells[cell_i_v1].vertIndices[2]];
           Particle p3_v1 = P[L->cells[cell_i_v1].vertIndices[3]];
           Particle p4_v1 = P[L->cells[cell_i_v1].vertIndices[4]];
           Particle p5_v1 = P[L->cells[cell_i_v1].vertIndices[5]];
           Particle p6_v1 = P[L->cells[cell_i_v1].vertIndices[6]];
           Particle p7_v1 = P[L->cells[cell_i_v1].vertIndices[7]];
           float u_v1 = meshVertices[v1].u;
           float v_v1 = meshVertices[v1].v;
           float w_v1 = meshVertices[v1].w;

           Vector3d v1prime;
           v1prime.set(((1 - u_v1) * (1 - v_v1) * w_v1 * p4_v1.position) + (u_v1 * (1 - v_v1) * w_v1 * p5_v1.position)
                        + ((1 - u_v1) * v_v1 * w_v1 * p6_v1.position) + (u_v1 * v_v1 * w_v1 * p7_v1.position)
                        + ((1 - u_v1) * (1 - v_v1) * (1 - w_v1) * p0_v1.position) + (u_v1 * (1 - v_v1) * (1 - w_v1) * p1_v1.position)
                        + ((1 - u_v1) * v_v1 * (1 - w_v1) * p2_v1.position) + (u_v1 * v_v1 * (1 - w_v1) * p3_v1.position));
           Vector3d v1Trans;
           v1Trans.set(v1prime.x - obj.VertexArray[v1].X, v1prime.y - obj.VertexArray[v1].Y, v1prime.z - obj.VertexArray[v1].Z);

           //===========================================================================

	   int cell_i_v2 = meshVertices[v2].cellIndex;
           Particle p0_v2 = P[L->cells[cell_i_v2].vertIndices[0]];
           Particle p1_v2 = P[L->cells[cell_i_v2].vertIndices[1]];
           Particle p2_v2 = P[L->cells[cell_i_v2].vertIndices[2]];
           Particle p3_v2 = P[L->cells[cell_i_v2].vertIndices[3]];
           Particle p4_v2 = P[L->cells[cell_i_v2].vertIndices[4]];
           Particle p5_v2 = P[L->cells[cell_i_v2].vertIndices[5]];
           Particle p6_v2 = P[L->cells[cell_i_v2].vertIndices[6]];
           Particle p7_v2 = P[L->cells[cell_i_v2].vertIndices[7]];
           float u_v2 = meshVertices[v2].u;
           float v_v2 = meshVertices[v2].v;
           float w_v2 = meshVertices[v2].w;

           Vector3d v2prime;
           v2prime.set(((1 - u_v2) * (1 - v_v2) * w_v2 * p4_v2.position) + (u_v2 * (1 - v_v2) * w_v2 * p5_v2.position)
                        + ((1 - u_v2) * v_v2 * w_v2 * p6_v2.position) + (u_v2 * v_v2 * w_v2 * p7_v2.position)
                        + ((1 - u_v2) * (1 - v_v2) * (1 - w_v2) * p0_v2.position) + (u_v2 * (1 - v_v2) * (1 - w_v2) * p1_v2.position)
                        + ((1 - u_v2) * v_v2 * (1 - w_v2) * p2_v2.position) + (u_v2 * v_v2 * (1 - w_v2) * p3_v2.position));
           Vector3d v2Trans;
           v2Trans.set(v2prime.x - obj.VertexArray[v2].X, v2prime.y - obj.VertexArray[v2].Y, v2prime.z - obj.VertexArray[v2].Z);

           glNormal3f(obj.NormalArray[n0].X, obj.NormalArray[n0].Y, obj.NormalArray[n0].Z);
           glVertex3f(obj.VertexArray[v0].X + v0Trans.x, obj.VertexArray[v0].Y + v0Trans.y, obj.VertexArray[v0].Z + v0Trans.z);
           glNormal3f(obj.NormalArray[n1].X, obj.NormalArray[n1].Y, obj.NormalArray[n1].Z);
           glVertex3f(obj.VertexArray[v1].X + v1Trans.x, obj.VertexArray[v1].Y + v1Trans.y, obj.VertexArray[v1].Z + v1Trans.z);
           glNormal3f(obj.NormalArray[n2].X, obj.NormalArray[n2].Y, obj.NormalArray[n2].Z);
           glVertex3f(obj.VertexArray[v2].X + v2Trans.x, obj.VertexArray[v2].Y + v2Trans.y, obj.VertexArray[v2].Z + v2Trans.z);

     }
     glEnd();

     glDisable(GL_LIGHTING);

if (ShowLattice == true){
     glPointSize(4.0f);
     glBegin(GL_POINTS);
     glColor4f(0, 0.380, 0.352, 1.0);
     for (int i=0; i < np; i++){
        glVertex3f(S->states[i].x, S->states[i].y, S->states[i].z);
     }
     glEnd();

     glBegin(GL_LINES);
        for (int st=0; st < ns; st++){
           int p1 = ST[st].v_indices[0];
           int p2 = ST[st].v_indices[1];
           glVertex3f(P[p1].position.x, P[p1].position.y, P[p1].position.z);
           glVertex3f(P[p2].position.x, P[p2].position.y, P[p2].position.z);
        }
     glEnd();
}
          glEnable(GL_LIGHTING);
  }
}

//
// Redraw the display, including the box-ball model
//
void View::updateDisplay(){
  // clear the window to the background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // initialize modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // lights are positioned in camera space so they move with camera
  setLights();

  // position and aim the camera in modelview space
  camera->AimCamera();

  // draw the model
  drawModel();

  glutSwapBuffers();
}

//
// handle mouse button events to initiate and end camera motion via mouse movements
//
void View::handleButtons(int button, int state, int x, int y, bool shiftkey){
  camera->HandleMouseEvent(button, state, x, y, shiftkey);
}

//
// handle mouse motion events to move the camera
//
void View::handleMotion(int x, int y){
  camera->HandleMouseMotion(x, y);
}

//
// When window resized, keep viewport proportions the same as the camera's
// viewscreen proportions to avoid distortion of scene
//
void View::reshapeWindow(int w, int h){
  float camaspect = float(width) / float(height);   // camera's aspect ratio
  float newaspect = float(w) / float(h);            // current window aspect ratio
  float x0, y0;
  
  // tentatively set viewport dimensions to current window dimensions
  Width = w;
  Height = h;

  // correct Width or Height so that Width / Height will match camera's aspect ratio
  if(newaspect > camaspect)
    Width = int(h * camaspect);
  else
    Height = int(w / camaspect);

  // offset viewport to keep it centered in the window
  x0 = (w - Width) / 2;
  y0 = (h - Height) / 2;
  
  // update the viewport
  glViewport(x0, y0, Width, Height);
  
  // set up camera projection matrix
  camera->PerspectiveDisplay(Width, Height);
}
