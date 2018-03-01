#include <stdio.h>
// yes, I know stdio.h is not good C++, but I like the *printf()
#include <stdlib.h>
#include <ctype.h>
//#include <afxwin.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include "Vector.cpp"

float vx_min_max[2];
float vy_min_max[2];
float mins, maxs, mind, maxd;
int d_sep = 20;
float d_test_percentage;
typedef struct contourPoint{
	float x, y, z, T;
	bool isIntersect = false;
	int index;
} contourPoint;
contourPoint contourPointArray[125000 * 3][4];
typedef struct Line{
	contourPoint p0;
	contourPoint p1;
} Line;
typedef struct pixel
{
	int i, j;
}pixel;

int arrowState, licState = 1, directMethodState = 0;
const int IMG_RES = 512;
unsigned char noise_tex[IMG_RES][IMG_RES][3];
unsigned char vec_img[IMG_RES][IMG_RES][3];
unsigned char LIC_tex[IMG_RES][IMG_RES][3];
const int kernel_size = 1000;
double xyarray[2] = { 0, 0 };
float vxvyarray[2] = { 0, 0 };

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif


// You need to adjust the location of these header files according to your configuration


#include <GL/include/gl.h>
#include <GL/include/glu.h>
#include <GL/include/glut.h>
#include <GL/include/glui.h>

#include "Skeleton.h"
#include <vector>
#include <queue>
//#include "glui.h"

//
//
//	This is a sample OpenGL / GLUT / GLUI program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with radio buttons
//
//	The left mouse button allows rotation
//	The middle mouse button allows scaling
//	The glui window allows:
//		1. The 3d object to be transformed
//		2. The projection to be changed
//		3. The color of the axes to be changed
//		4. The axes to be turned on and off
//		5. The transformations to be reset
//		6. The program to quit
//
//	Author: Joe Graphics
//


//
// constants:
//
// NOTE: There are a bunch of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch() statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch() statements.  Those are #defines.
//
//


// This source code has been modified by Guoning Chen since its release


// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT / GLUI Sample -- Edward Lie" };
const char *GLUITITLE = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE = { true };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 512 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// able to use the left mouse for either rotation or scaling,
// in case have only a 2-button mouse:

enum LeftButton
{
	ROTATE,
	SCALE
};


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const float BACKCOLOR[] = { 0., 0., 0., 0. };


// line width for the axes:

const GLfloat AXES_WIDTH = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};


// the object numbers:
// 
enum MODELS
{
	BNOISE,
	BRUNO3,
	CNOISE,
	DIPOLE,
	VNOISE,
};

enum SHAPES
{
	DIESELFIELD,
	DISTANCEFIELD1,
	DISTANCEFIELD2,
	ICELANDCURRENTFIELD,
	TORUSFIELD,
};

enum DISPLAYOPTION
{
	VECTORMAGNITUDE,
	VECTORANGLE,
	XCOMPONENT,
	YCOMPONENT,
};


GLUI_HSlider *TempSlider;
GLUI_StaticText *TempLabel;
GLUI_HSlider* XSlider;
GLUI_StaticText *XLabel;
GLUI_HSlider* YSlider;
GLUI_StaticText *YLabel;
GLUI_HSlider* ZSlider;
GLUI_StaticText *ZLabel;
GLUI_HSlider* GradSlider;
GLUI_StaticText *GradLabel;
GLUI_Spinner* num_Contour_Spinner;
GLUI_Spinner* scalar_Spinner;
GLUI_Spinner* xval;
GLUI_Spinner* yval;
GLUI_Spinner* zval;
GLUI_HSlider* DistSlider;
GLUI_StaticText *DistLabel;
GLUI_HSlider* BilinearSlider;
GLUI_StaticText *BilinearLabel;

// the color definitions:
// this order must match the radio button order

const GLfloat Colors[8][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE = { GL_LINEAR };
const GLfloat FOGDENSITY = { 0.30f };
const GLfloat FOGSTART = { 1.5 };
const GLfloat FOGEND = { 4. };



//
// non-constant global variables:
//

int	ActiveButton;		// current button that is down
GLuint	AxesList;		// list to hold the axes
int	AxesOn;			// != 0 means to draw the axes
int	DebugOn;			// != 0 means to print debugging info
int	DepthCueOn;		// != 0 means to use intensity depth cueing
GLUI *	Glui;			// instance of glui window
int	GluiWindow;		// the glut id for the glui window
int	LeftButton;		// either ROTATE or SCALE
GLuint	BoxList;		// object display list
int	MainWindow;		// window id for main graphics window
GLfloat	RotMatrix[4][4];	// set by glui rotation widget
float	Scale, Scale2;		// scaling factors
int	WhichColor;		// index into Colors[]
int	WhichProjection;	// ORTHO or PERSP
int	Xmouse, Ymouse;		// mouse values
float	Xrot, Yrot;		// rotation angles in degrees
float	TransXYZ[3];		// set by glui translation widgets
int colorMap = 0;
int discreteState = 0;
int contourState = 0;
#define X 0
#define Y 1
#define Z 2
std::vector<pixel> samplePoints;

struct sources
{
	double xc, yc, zc; // heat source location for this assignment zc=0
	double a; // temperature value of the source
};

struct node
{
	float x, y, z; // location
	float T; // temperature
	float r, g, b; // the assigned color
	float rad; // radius
	float dTdx, dTdy, dTdz; // can store these if you want, or not
	float grad; // total gradient
};

//
// function prototypes:
//

void	Animate(void);
void	Buttons(int);
void	Display(void);
void	DoRasterString(float, float, float, char *);
void	DoStrokeString(float, float, float, float, char *);
float	ElapsedSeconds(void);
void	InitGlui(void);
void	InitGraphics(void);
void	InitLists(void);
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset(void);
void	Resize(int, int);
void	Visibility(int);

void	Arrow(float[3], float[3]);
void	Cross(float[3], float[3], float[3]);
float	Dot(float[3], float[3]);
float	Unit(float[3], float[3]);
void	Axes(float);
void	HsvRgb(float[3], float[3]);

void    Display_Model(void);
void    set_view(GLenum mode, Polyhedron *poly);
void    set_scene(GLenum mode, Polyhedron *poly);
void    display_shape(GLenum mode, Polyhedron *this_poly);
void    Choose_Object();

void	Rainbow(float rgb[3], float, float, float);
void	BlueWhiteRed(float rgb[3], float, float, float);
void	BlackRed(float rgb[3], float, float, float);
void	Dynamic(float rgb[3], float, float, float);
void	Discrete(float rgb[3], float min, float max, float value);

void	CreateGrid();
void	CalcGradient();
double Temperature(double x, double y, double z);

void Buttons(int);
void Sliders(int);

void scalarSpinner();
void numContourSpinner();

int contourCount = 0;
void DrawQuad();
//std::vector<Line> ProcessQuad(struct node p0, struct node p1, struct node p2, struct node p3);
void ProcessTriangle(Triangle * tri);

double radius_factor = 0.5;
int display_mode = 0;

int ObjectId = 0;
char object_name[128] = "bnoise";

int scalarValue = 5;
int numOfContours = 20;

void compute_streamline(pixel);

void draw_arrow_head(float arrowlength, double head[2], float direct[2]);
void draw_arrow_helper(Polyhedron* poly);
void gen_noise_tex();
void render_vec_img(Polyhedron *);
void compute_lic_image(Polyhedron *);

float* calcminmax(int numberverts, float* svalues);
void directcall();

Polyhedron *poly = NULL;
//extern PlyFile *in_ply;
//extern FILE *this_file;

//
// main program:
//

int main(int argc, char *argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// Load the model and data here
	FILE *this_file = fopen("../models/bnoise.ply", "r");
	poly = new Polyhedron(this_file);
	fclose(this_file);
	//mat_ident( rotmat );	

	poly->initialize(); // initialize everything

	poly->calc_bounding_sphere();
	poly->calc_face_normals_and_area();
	poly->average_normals();
	gen_noise_tex();
	render_vec_img(poly);
	////intiialize lic texture
	//for (int x = 0; x < IMG_RES; x++)
	//	for (int y = 0; y < IMG_RES; y++)
	//	{
	//		LIC_tex[x][y][0] =
	//			LIC_tex[x][y][1] =
	//			LIC_tex[x][y][2] = 0;
	//	}
	compute_lic_image(poly);

	// setup all the graphics stuff:

	InitGraphics();


	// create the display structures that will not change:

	InitLists();


	// init all the global variables used by Display():
	// this will also post a redisplay
	// it is important to call this before InitGlui()
	// so that the variables that glui will control are correct
	// when each glui widget is created

	Reset();


	// setup all the user interface stuff:

	InitGlui();


	// draw the scene once and wait for some interaction:
	// (will never return)

	glutMainLoop();

	// finalize the object if loaded

	if (poly != NULL)
		poly->finalize();

	// this is here to make the compiler happy:

	return 0;
}



//
// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display() from here -- let glutMainLoop() do it
//

void
Animate(void)
{
	// put animation stuff in here -- change some global variables
	// for Display() to find:



	// force a call to Display() next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}




//
// glui buttons callback:
//

void
Buttons(int id)
{
	char str[256];
	switch (id)
	{
	case RESET:
		Reset();
		Glui->sync_live();
		glutSetWindow(MainWindow);
		glutPostRedisplay();
		/*sprintf(str, TEMPFORMAT, TempLowHigh[0], TempLowHigh[1]);*/
		TempLabel->set_text(str);
		break;

	case QUIT:
		// gracefully close the glui window:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:

		Glui->close();
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Button ID %d\n", id);
	}

}



//
// draw the complete scene:
//

void
Display(void)
{
	switch (colorMap)
	{
	case VECTORMAGNITUDE:
		poly->calc_min_max_vector_magnitude();
		break;
	case VECTORANGLE:
		poly->calc_min_max_vector_angle();
		break;
	case XCOMPONENT:
		poly->calc_min_max_x_component();
		break;
	case YCOMPONENT:
		poly->calc_min_max_y_component();
		break;
	}

	glViewport(0, 0, (GLsizei)512, (GLsizei)512);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	//glutSetWindow(MainWindow);

	//glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (licState == 0 && directMethodState == 1)
	{
		//directcall();
		display_shape(GL_RENDER, poly);

		//return;
	}

	gen_noise_tex();
	render_vec_img(poly);
	//intiialize lic texture
	for (int x = 0; x < IMG_RES; x++)
		for (int y = 0; y < IMG_RES; y++)
		{
			LIC_tex[x][y][0] =
				LIC_tex[x][y][1] =
				LIC_tex[x][y][2] = 0;
		}
	compute_lic_image(poly);
	GLsizei vx, vy, v;		// viewport dimensions
	GLint xl, yb;		// lower-left corner of viewport
	GLfloat scale2;		// real glui scale factor

	//if (DebugOn != 0)
	//{
	//	fprintf(stderr, "Display\n");
	//}
	if (licState == 1){
		//glDisable(GL_COLOR_MATERIAL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		/*gen_noise_tex();*/
		/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMG_RES, IMG_RES, 0,
		GL_RGB, GL_UNSIGNED_BYTE, noise_tex);*/
		/*render_vec_img(poly);*/
		/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMG_RES, IMG_RES, 0,
		GL_RGB, GL_UNSIGNED_BYTE, vec_img);*/
		/*if (licState == 1)*/
		//{
		/*compute_lic_image(poly);*/
		//if (licState == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMG_RES, IMG_RES, 0,
			GL_RGB, GL_UNSIGNED_BYTE, LIC_tex);

		/*	}*/
		glBegin(GL_QUAD_STRIP);
		glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0);
		glTexCoord2f(0.0, 1.0); glVertex2f(0.0, 1.0);
		glTexCoord2f(1.0, 0.0); glVertex2f(1.0, 0.0);
		glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	// Add you arrow plot here and use a checkbox to enable its visualization
	if (arrowState == 1)
	{
		draw_arrow_helper(poly);
	}
	glutSwapBuffers();
	glFlush();
}

void directcall()
{
	display_shape(GL_RENDER, poly);
}

//
// use glut to display a string of characters using a raster font:
//

void
DoRasterString(float x, float y, float z, char *s)
{
	char c;			// one character to print

	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}



//
// use glut to display a string of characters using a stroke font:
//

void
DoStrokeString(float x, float y, float z, float ht, char *s)
{
	char c;			// one character to print
	float sf;		// the scale factor

	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	sf = ht / (119.05 + 33.33);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}



//
// return the number of seconds since the start of the program:
//

float
ElapsedSeconds(void)
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.;
}



//
// initialize the glui window:
//

void
InitGlui(void)
{
	GLUI_Panel *panel;
	GLUI_RadioGroup *group;
	GLUI_Rotation *rot;
	GLUI_Translation *trans, *scale;

	char str[128];

	// setup the glui window:

	glutInitWindowPosition(INIT_WINDOW_SIZE + 50, 0);
	Glui = GLUI_Master.create_glui((char *)GLUITITLE);


	Glui->add_statictext((char *)GLUITITLE);
	Glui->add_separator();

	//Glui->add_checkbox("Axes", &AxesOn);

	//Glui->add_checkbox("Perspective", &WhichProjection);

	//Glui->add_checkbox("Intensity Depth Cue", &DepthCueOn);

	// Add a rollout for the axes color
	/*GLUI_Rollout *rollout = Glui->add_rollout(" Axes Color ", 0);*/

	//panel = Glui->add_panel(  "Axes Color" );
	//GLUI_Rollout *rollout = Glui->add_rollout_to_panel(panel,  "Axes Color", 1 );
	/*group = Glui->add_radiogroup_to_panel( panel, &WhichColor );*/

	//group = Glui->add_radiogroup_to_panel(rollout, &WhichColor);
	//Glui->add_radiobutton_to_group(group, "Red");
	//Glui->add_radiobutton_to_group(group, "Yellow");
	//Glui->add_radiobutton_to_group(group, "Green");
	//Glui->add_radiobutton_to_group(group, "Cyan");
	//Glui->add_radiobutton_to_group(group, "Blue");
	//Glui->add_radiobutton_to_group(group, "Magenta");
	//Glui->add_radiobutton_to_group(group, "White");
	//Glui->add_radiobutton_to_group(group, "Black");

	//Add a list for the different models
	GLUI_Rollout *rollout = Glui->add_rollout(" Models ", 0);
	panel = Glui->add_panel("Choose object to open ");
	GLUI_Listbox *obj_list = Glui->add_listbox_to_panel(panel, "Objects", &ObjectId, -1, (GLUI_Update_CB)Choose_Object);
	obj_list->add_item(0, "bnoise");
	obj_list->add_item(1, "bruno3");
	obj_list->add_item(2, "cnoise");
	obj_list->add_item(3, "dipole");
	obj_list->add_item(4, "vnoise");
	//obj_list->add_item(5, "torus");

	Glui->add_checkbox("Show Vectors", &arrowState);
	Glui->add_checkbox("LIC", &licState);
	//Glui->add_checkbox("Direct Method", &directMethodState);

	//Glui->add_separator();
	//rollout = Glui->add_rollout(" Display Option ", 0);
	//group = Glui->add_radiogroup_to_panel(rollout, &colorMap);
	//Glui->add_radiobutton_to_group(group, "Original");
	//Glui->add_radiobutton_to_group(group, "Rainbow");
	//Glui->add_radiobutton_to_group(group, "BlueWhiteRed");
	//Glui->add_radiobutton_to_group(group, "BlackRed");
	//Glui->add_radiobutton_to_group(group, "Vector Magnitude");
	//Glui->add_radiobutton_to_group(group, "Vector Angle");
	//Glui->add_radiobutton_to_group(group, "X Component");
	//Glui->add_radiobutton_to_group(group, "Y Component");
	/*Glui->add_radiobutton_to_group(group, "Dynamic");*/

	//panel = Glui->add_panel("");
	//GLUI_Listbox *obj_list3 = Glui->add_listbox_to_panel(panel, "Mode", &display_mode, -1, (GLUI_Update_CB)display_shape);
	//obj_list3->add_item(0, "models");
	//obj_list3->add_item(1, "point cloud");
	//obj_list3->add_item(2, "models");

	panel = Glui->add_panel("Separation");
	scalar_Spinner = Glui->add_spinner_to_panel(panel, "Choose", GLUI_SPINNER_INT, &d_sep, -1, (GLUI_Update_CB)scalarSpinner);
	scalar_Spinner->set_int_limits(1., 100.0f);

	//panel = Glui->add_panel("Dtest");
	//scalar_Spinner = Glui->add_spinner_to_panel(panel, "Choose", GLUI_SPINNER_INT, &d_test_percentage, -1, (GLUI_Update_CB)scalarSpinner);
	//scalar_Spinner->set_int_limits(1., 100.0f);

	//panel = Glui->add_panel("Object Transformation");

	//rot = Glui->add_rotation_to_panel(panel, "Rotation", (float *)RotMatrix);

	//// allow the object to be spun via the glui rotation widget:

	//rot->set_spin(1.0);

	//Glui->add_column_to_panel(panel, GLUIFALSE);
	//scale = Glui->add_translation_to_panel(panel, "Scale", GLUI_TRANSLATION_Y, &Scale2);
	//scale->set_speed(0.005f);

	//Glui->add_column_to_panel(panel, GLUIFALSE);
	//trans = Glui->add_translation_to_panel(panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0]);
	//trans->set_speed(0.05f);

	//Glui->add_column_to_panel(panel, GLUIFALSE);
	//trans = Glui->add_translation_to_panel(panel, "Trans Z", GLUI_TRANSLATION_Z, &TransXYZ[2]);
	//trans->set_speed(0.05f);

	Glui->add_checkbox("Debug", &DebugOn);

	panel = Glui->add_panel("", GLUIFALSE);

	Glui->add_button_to_panel(panel, "Reset", RESET, (GLUI_Update_CB)Buttons);

	Glui->add_column_to_panel(panel, GLUIFALSE);

	Glui->add_button_to_panel(panel, "Quit", QUIT, (GLUI_Update_CB)Buttons);


	// tell glui what graphics window it needs to post a redisplay to:

	Glui->set_main_gfx_window(MainWindow);


	// set the graphics window's idle function:

	GLUI_Master.set_glutIdleFunc(NULL);
}



//
// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
//

void
InitGraphics(void)
{
	// setup the display mode:
	// ( *must* be done before call to glutCreateWindow() )
	// ask for color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);


	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);


	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);


	// setup the clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);


	// setup the callback routines:


	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	//glutDisplayFunc( Display_Model );
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(NULL);
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(0, NULL, 0);

	// DO NOT SET THE GLUT IDLE FUNCTION HERE !!
	// glutIdleFunc( NULL );
	// let glui take care of it in InitGlui()
}




//
// initialize the display lists that will not change:
//

void
InitLists(void)
{
	float dx = BOXSIZE / 2.;
	float dy = BOXSIZE / 2.;
	float dz = BOXSIZE / 2.;

	// create the object:

	BoxList = glGenLists(1);
	glNewList(BoxList, GL_COMPILE);

	glBegin(GL_QUADS);

	glColor3f(0., 0., 1.);
	glNormal3f(0., 0., 1.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	glNormal3f(0., 0., -1.);
	glTexCoord2f(0., 0.);
	glVertex3f(-dx, -dy, -dz);
	glTexCoord2f(0., 1.);
	glVertex3f(-dx, dy, -dz);
	glTexCoord2f(1., 1.);
	glVertex3f(dx, dy, -dz);
	glTexCoord2f(1., 0.);
	glVertex3f(dx, -dy, -dz);

	glColor3f(1., 0., 0.);
	glNormal3f(1., 0., 0.);
	glVertex3f(dx, -dy, dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(dx, dy, dz);

	glNormal3f(-1., 0., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy, -dz);
	glVertex3f(-dx, -dy, -dz);

	glColor3f(0., 1., 0.);
	glNormal3f(0., 1., 0.);
	glVertex3f(-dx, dy, dz);
	glVertex3f(dx, dy, dz);
	glVertex3f(dx, dy, -dz);
	glVertex3f(-dx, dy, -dz);

	glNormal3f(0., -1., 0.);
	glVertex3f(-dx, -dy, dz);
	glVertex3f(-dx, -dy, -dz);
	glVertex3f(dx, -dy, -dz);
	glVertex3f(dx, -dy, dz);

	glEnd();

	glEndList();


	// create the axes:

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
}



//
// the keyboard callback:
//

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		Buttons(QUIT);	// will not return here
		break;			// happy compiler

	case 'r':
	case 'R':
		LeftButton = ROTATE;
		break;

	case 's':
	case 'S':
		LeftButton = SCALE;
		break;

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}


	// synchronize the GLUI display with the variables:

	Glui->sync_live();


	// force a call to Display():

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

//called when the mouse button transitions down or up:


void
MouseButton(int button, int state, int x, int y)
{
	int b;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}


	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}



//
// called when the mouse moves while a button is down:
//

void
MouseMotion(int x, int y)
{
	int dx, dy;		// change in mouse coordinates

	if (DebugOn != 0)
		fprintf(stderr, "MouseMotion: %d, %d\n", x, y);


	dx = x - Xmouse;		// change in mouse coords
	dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		switch (LeftButton)
		{
		case ROTATE:
			Xrot += (ANGFACT*dy);
			Yrot += (ANGFACT*dx);
			break;

		case SCALE:
			Scale += SCLFACT * (float)(dx - dy);
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;
		}
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}



//
// reset the transformations and the colors:
//
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
//

void
Reset(void)
{
	ActiveButton = 0;
	AxesOn = GLUITRUE;
	DebugOn = GLUIFALSE;
	DepthCueOn = GLUIFALSE;
	LeftButton = ROTATE;
	Scale = 1.0;
	Scale2 = 0.0;		// because we add 1. to it in Display()
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;

	RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0] = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1] = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3] = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;
}



//
// called when user resizes the window:
//

void
Resize(int width, int height)
{
	if (DebugOn != 0)
		fprintf(stderr, "ReSize: %d, %d\n", width, height);

	// don't really need to do anything since window size is
	// checked each time in Display():

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


//
// handle a change to the window's visibility:
//

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}




//////////////////////////////////////////  EXTRA HANDY UTILITIES:  /////////////////////////////

// size of wings as fraction of length:

#define WINGS	0.10


// axes:

#define X	1
#define Y	2
#define Z	3


// x, y, z, axes:

static float axx[3] = { 1., 0., 0. };
static float ayy[3] = { 0., 1., 0. };
static float azz[3] = { 0., 0., 1. };


void
Arrow(float tail[3], float head[3])
{
	float u[3], v[3], w[3];		// arrow coordinate system
	float d;			// wing distance
	float x, y, z;			// point to plot
	float mag;			// magnitude of major direction
	float f;			// fabs of magnitude
	int axis;			// which axis is the major


	// set w direction in u-v-w coordinate system:

	w[0] = head[0] - tail[0];
	w[1] = head[1] - tail[1];
	w[2] = head[2] - tail[2];


	// determine major direction:

	axis = X;
	mag = fabs(w[0]);
	if ((f = fabs(w[1])) > mag)
	{
		axis = Y;
		mag = f;
	}
	if ((f = fabs(w[2])) > mag)
	{
		axis = Z;
		mag = f;
	}


	// set size of wings and turn w into a Unit vector:

	d = WINGS * Unit(w, w);


	// draw the shaft of the arrow:

	glBegin(GL_LINE_STRIP);
	glVertex3fv(tail);
	glVertex3fv(head);
	glEnd();

	// draw two sets of wings in the non-major directions:

	if (axis != X)
	{
		Cross(w, axx, v);
		(void)Unit(v, v);
		Cross(v, w, u);
		x = head[0] + d * (u[0] - w[0]);
		y = head[1] + d * (u[1] - w[1]);
		z = head[2] + d * (u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
		x = head[0] + d * (-u[0] - w[0]);
		y = head[1] + d * (-u[1] - w[1]);
		z = head[2] + d * (-u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
	}


	if (axis != Y)
	{
		Cross(w, ayy, v);
		(void)Unit(v, v);
		Cross(v, w, u);
		x = head[0] + d * (u[0] - w[0]);
		y = head[1] + d * (u[1] - w[1]);
		z = head[2] + d * (u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
		x = head[0] + d * (-u[0] - w[0]);
		y = head[1] + d * (-u[1] - w[1]);
		z = head[2] + d * (-u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
	}



	if (axis != Z)
	{
		Cross(w, azz, v);
		(void)Unit(v, v);
		Cross(v, w, u);
		x = head[0] + d * (u[0] - w[0]);
		y = head[1] + d * (u[1] - w[1]);
		z = head[2] + d * (u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
		x = head[0] + d * (-u[0] - w[0]);
		y = head[1] + d * (-u[1] - w[1]);
		z = head[2] + d * (-u[2] - w[2]);
		glBegin(GL_LINE_STRIP);
		glVertex3fv(head);
		glVertex3f(x, y, z);
		glEnd();
	}
}



float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}



void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];

	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];

	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}



float
Unit(float vin[3], float vout[3])
{
	float dist, f;

	dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];

	if (dist > 0.0)
	{
		dist = sqrt(dist);
		f = 1. / dist;
		vout[0] = f * vin[0];
		vout[1] = f * vin[1];
		vout[2] = f * vin[2];
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}

	return dist;
}



// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
	0.f, 1.f, 0.f, 1.f
};

static float xy[] = {
	-.5f, .5f, .5f, -.5f
};

static int xorder[] = {
	1, 2, -3, 4
};


static float yx[] = {
	0.f, 0.f, -.5f, .5f
};

static float yy[] = {
	0.f, .6f, 1.f, 1.f
};

static int yorder[] = {
	1, 2, 3, -2, 4
};


static float zx[] = {
	1.f, 0.f, 1.f, 0.f, .25f, .75f
};

static float zy[] = {
	.5f, .5f, -.5f, -.5f, 0.f, 0.f
};

static int zorder[] = {
	1, 2, 3, 4, -5, 6
};


// fraction of the length to use as height of the characters:

const float LENFRAC = 0.10f;


// fraction of length to use as start location of the characters:

const float BASEFRAC = 1.10f;


//
//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)
//

void
Axes(float length)
{
	int i, j;			// counters
	float fact;			// character scale factor
	float base;			// character start location


	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	fact = LENFRAC * length;
	base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < 4; i++)
	{
		j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact*xx[j], fact*xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < 5; i++)
	{
		j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact*yx[j], base + fact*yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < 6; i++)
	{
		j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact*zy[j], base + fact*zx[j]);
	}
	glEnd();

}




//
// routine to convert HSV to RGB
//
// Reference:  Foley, van Dam, Feiner, Hughes,
//		"Computer Graphics Principles and Practices,"
//		Additon-Wesley, 1990, pp592-593.


void
HsvRgb(float hsv[3], float rgb[3])
{
	float h, s, v;			// hue, sat, value
	float r, g, b;			// red, green, blue
	float i, f, p, q, t;		// interim values


	// guarantee valid input:

	h = hsv[0] / 60.;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;


	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}


	// get an rgb from the hue itself:

	i = floor(h);
	f = h - i;
	p = v * (1. - s);
	q = v * (1. - s*f);
	t = v * (1. - (s * (1. - f)));

	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


void set_view(GLenum mode, Polyhedron *poly)
{
	icVector3 up, ray, view;
	GLfloat light_ambient0[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_diffuse0[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_specular0[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_ambient1[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse1[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular1[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_ambient2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);


	glMatrixMode(GL_PROJECTION);
	if (mode == GL_RENDER)
		glLoadIdentity();

	if (WhichProjection == ORTHO)
		glOrtho(-radius_factor, radius_factor, -radius_factor, radius_factor, 0.0, 40.0);
	else
		gluPerspective(45.0, 1.0, 0.1, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	light_position[0] = 5.5;
	light_position[1] = 0.0;
	light_position[2] = 0.0;
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	light_position[0] = -0.1;
	light_position[1] = 0.0;
	light_position[2] = 0.0;
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);
}

void set_scene(GLenum mode, Polyhedron *poly)
{
	glTranslatef(0.0, 0.0, -3.0);

	glScalef(1.0 / poly->radius, 1.0 / poly->radius, 1.0 / poly->radius);
	glTranslatef(-poly->center.entry[0], -poly->center.entry[1], -poly->center.entry[2]);
}

void display_shape(GLenum mode, Polyhedron *this_poly)
{
	//unsigned int i, j;
	GLfloat mat_diffuse[4];
	float hsv[3], rgb[3];

	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1., 1.);

	//glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	/*glEnable(GL_LIGHTING);*/
	/*glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);*/
	glEnable(GL_COLOR_MATERIAL);

	/*if (arrowState == 1)
	{
	draw_arrow_helper(this_poly);
	}*/

	float dMax = 0.0, dMin = 0.0;

	for (int i = 0; i < this_poly->ntris; i++)
	{

		//if (mode == GL_SELECT)
		//	glLoadName(i + 1);

		Triangle *temp_t = this_poly->tlist[i];

		switch (display_mode) {
		case 0:
			/*if (i == this_poly->seed) {
			mat_diffuse[0] = 0.0;
			mat_diffuse[1] = 0.0;
			mat_diffuse[2] = 1.0;
			mat_diffuse[3] = 1.0;
			}
			else {
			mat_diffuse[0] = 0.6;
			mat_diffuse[1] = 0.8;
			mat_diffuse[2] = 0.7;
			mat_diffuse[3] = 1.0;
			}
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);*/
			glBegin(GL_POLYGON);
			for (int j = 0; j < 3; j++)
			{

				Vertex *temp_v = temp_t->verts[j];
				//glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
				if (i == this_poly->seed)
					glColor3f(0.0, 0.0, 1.0);
				else
					glColor3f(1.0, 1.0, 0.0);
				if (colorMap == VECTORMAGNITUDE)
				{
					float mag = sqrt(pow((temp_v->vx), 2) + pow((temp_v->vy), 2) + pow((temp_v->vz), 2));
					hsv[0] = 240.0 - 240.0*(mag - mins) / (maxs - mins);
					hsv[1] = 1.0;
					hsv[2] = 1.0;
					HsvRgb(hsv, rgb);
					glColor3f(rgb[0], rgb[1], rgb[2]);
				}
				else if (colorMap == VECTORANGLE)
				{
					float ang = atan2(temp_v->vy, temp_v->vx);
					hsv[0] = 240.0 - 240.0*(ang - mins) / (maxs - mins);
					hsv[1] = 1.0;
					hsv[2] = 1.0;
					HsvRgb(hsv, rgb);
					glColor3f(rgb[0], rgb[1], rgb[2]);
				}
				else if (colorMap == XCOMPONENT)
				{
					hsv[0] = 240.0 - 240.0*(temp_v->vx - mins) / (maxs - mins);
					hsv[1] = 1.0;
					hsv[2] = 1.0;
					HsvRgb(hsv, rgb);
					glColor3f(rgb[0], rgb[1], rgb[2]);
				}
				else if (colorMap == YCOMPONENT)
				{
					hsv[0] = 240.0 - 240.0*(temp_v->vy - mins) / (maxs - mins);
					hsv[1] = 1.0;
					hsv[2] = 1.0;
					HsvRgb(hsv, rgb);
					glColor3f(rgb[0], rgb[1], rgb[2]);
				}
				//glVertex3d(temp_v->x, temp_v->y, temp_v->z);
				glVertex2d(temp_v->x, temp_v->y);
			}
			glEnd();
			//	//case 6:
			//	//	glBegin(GL_POLYGON);
			//	//	for (j = 0; j < 3; j++) {
			//	//		Vertex *temp_v = temp_t->verts[j];
			//	//		glNormal3d(temp_t->normal.entry[0], temp_t->normal.entry[1], temp_t->normal.entry[2]);
			//	//		glVertex3d(temp_v->x, temp_v->y, temp_v->z);
			//	//	}
			//	//	glEnd();
			//	//	break;

			//	//case 10:
			//	//	glBegin(GL_POLYGON);
			//	//	for (j = 0; j < 3; j++) {
			//	//		mat_diffuse[0] = 1.0;
			//	//		mat_diffuse[1] = 0.0;
			//	//		mat_diffuse[2] = 0.0;
			//	//		mat_diffuse[3] = 1.0;

			//	//		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

			//	//		Vertex *temp_v = temp_t->verts[j];
			//	//		glNormal3d(temp_t->normal.entry[0], temp_t->normal.entry[1], temp_t->normal.entry[2]);

			//	//		glColor3f(1.0, 0.0, 0.0);
			//	//		glVertex3d(temp_v->x, temp_v->y, temp_v->z);
			//	//	}
			//	//	glEnd();
			//	//	break;
			//}
		}
	}
}

void draw_arrow_helper(Polyhedron* poly)
{
	float arrowlength = 1.5 - ((poly->nverts - 1500) / (5000 - 1500));
	for (int i = 0; i < poly->nverts; i++)
	{
		xyarray[0] = poly->vlist[i]->x;
		xyarray[1] = poly->vlist[i]->y;
		vxvyarray[0] = poly->vlist[i]->vx;
		vxvyarray[1] = poly->vlist[i]->vy;
		draw_arrow_head(arrowlength, xyarray, vxvyarray);
	}
}



void Display_Model(void)
{
	GLint viewport[4];
	int jitter;

	glClearColor(1.0, 1.0, 1.0, 1.0);  // background for rendering color coding and lighting
	glGetIntegerv(GL_VIEWPORT, viewport);

	set_view(GL_RENDER, poly);
	set_scene(GL_RENDER, poly);

	display_shape(GL_RENDER, poly);
	glFlush();
	glutSwapBuffers();
	glFinish();
}


void Choose_Object()
{
	int w, h;
	switch (ObjectId){
	case BNOISE:
		strcpy(object_name, "bnoise");
		break;

	case BRUNO3:
		strcpy(object_name, "bruno3");
		break;

	case CNOISE:
		strcpy(object_name, "cnoise");
		break;

	case DIPOLE:
		strcpy(object_name, "dipole");
		break;

	case VNOISE:
		strcpy(object_name, "vnoise");
		break;
	}

	poly->finalize();

	Reset();

	char tmp_str[512];

	sprintf(tmp_str, "../models/%s.ply", object_name);

	FILE *this_file = fopen(tmp_str, "r");
	poly = new Polyhedron(this_file);
	fclose(this_file);

	////Following codes build the edge information
	clock_t start, finish; //Used to show the time assumed
	start = clock(); //Get the first time

	poly->initialize(); // initialize everything
	poly->calc_bounding_sphere();
	poly->calc_face_normals_and_area();
	poly->average_normals();

	gen_noise_tex();
	render_vec_img(poly);
	//intiialize lic texture
	for (int x = 0; x < IMG_RES; x++)
		for (int y = 0; y < IMG_RES; y++)
		{
			LIC_tex[x][y][0] =
				LIC_tex[x][y][1] =
				LIC_tex[x][y][2] = 0;
		}
	compute_lic_image(poly);

	finish = clock(); //Get the current time after finished
	double t = (double)(finish - start) / CLOCKS_PER_SEC;

	printf("\n");
	printf("The number of the edges of the object %s is %d \n", object_name, poly->nedges);
	printf("The Euler Characteristics of the object %s is %d \n", object_name, (poly->nverts - poly->nedges + poly->ntris));

	printf("Time to building the edge link is %f seconds\n", t);

	glutSetWindow(MainWindow);
	glutPostRedisplay();
	Glui->sync_live();
}

void Polyhedron::calc_min_max()
{
	maxs = vlist[0]->s;
	mins = vlist[0]->s;
	for (int i = 1; i < nverts; i++)
	{
		if (vlist[i]->s > maxs)
			maxs = vlist[i]->s;
		if (vlist[i]->s < mins)
			mins = vlist[i]->s;
	}
}

float* calcminmax(int numberverts, float* svalues)
{
	float min = 9999999;
	float max = -9999999;
	float* minmax = new float[2];
	for (int i = 0; i < numberverts; i++)
	{
		if (svalues[i] <= min)
			min = svalues[i];
	}
	for (int i = 0; i < numberverts; i++)
	{
		if (svalues[i] >= max)
			max = svalues[i];
	}
	minmax[0] = min, minmax[1] = max;
	return minmax;
}

void Polyhedron::calc_min_max_vector_magnitude()
{
	maxs = sqrt(pow((vlist[0]->vx), 2) + pow((vlist[0]->vy), 2) + pow((vlist[0]->vz), 2));
	mins = sqrt(pow((vlist[0]->vx), 2) + pow((vlist[0]->vy), 2) + pow((vlist[0]->vz), 2));
	for (int i = 1; i < nverts; i++)
	{
		if (sqrt(pow((vlist[i]->vx), 2) + pow((vlist[i]->vy), 2) + pow((vlist[i]->vz), 2)) > maxs)
			maxs = sqrt(pow((vlist[i]->vx), 2) + pow((vlist[i]->vy), 2) + pow((vlist[i]->vz), 2));
		if (sqrt(pow((vlist[i]->vx), 2) + pow((vlist[i]->vy), 2) + pow((vlist[i]->vz), 2)) < mins)
			mins = sqrt(pow((vlist[i]->vx), 2) + pow((vlist[i]->vy), 2) + pow((vlist[i]->vz), 2));
	}
}

void Polyhedron::calc_min_max_vector_angle()
{
	maxs = atan2(vlist[0]->vy, vlist[0]->vx);
	mins = atan2(vlist[0]->vy, vlist[0]->vx);
	for (int i = 1; i < nverts; i++)
	{
		if (atan2(vlist[i]->vy, vlist[i]->vx) > maxs)
			maxs = atan2(vlist[i]->vy, vlist[i]->vx);
		if (atan2(vlist[i]->vy, vlist[i]->vx) < mins)
			mins = atan2(vlist[i]->vy, vlist[i]->vx);
	}
}

void Polyhedron::calc_min_max_x_component()
{
	maxs = vlist[0]->vx;
	mins = vlist[0]->vx;
	for (int i = 1; i < nverts; i++)
	{
		if (vlist[i]->vx > maxs)
			maxs = vlist[i]->vx;
		if (vlist[i]->vx < mins)
			mins = vlist[i]->vx;
	}
}

void Polyhedron::calc_min_max_y_component()
{
	maxs = vlist[0]->vy;
	mins = vlist[0]->vy;
	for (int i = 1; i < nverts; i++)
	{
		if (vlist[i]->vy > maxs)
			maxs = vlist[i]->vy;
		if (vlist[i]->vy < mins)
			mins = vlist[i]->vy;
	}
}

void Rainbow(float rgb[3], float smin, float smax, float s)
{
	float hsv[3];
	hsv[0] = 240.0 - 240.0*(s - smin) / (smax - smin);
	hsv[1] = 1.0;
	hsv[2] = 1.0;
	HsvRgb(hsv, rgb);
}

void BlueWhiteRed(float rgb[3], float smin, float smax, float s)
{
	float hsv[3];

	//finding midpoint
	const float& mid = (smin + smax) / 2.0;

	if (s <= mid)//first half - blue to white
	{
		hsv[0] = 240;
		hsv[1] = (mid - s) / (mid - smin);  //low s -> high sat (blue). higher s -> low sat (white) //calculating percentage toward middle
	}
	else //second half - white to red
	{
		hsv[0] = 0;
		hsv[1] = (s - mid) / (smax - mid); //low -> low sat (white), higher s -> high sat (red) // calculating percentage toward middle
	}
	hsv[2] = 1.0;
	HsvRgb(hsv, rgb);
}

void BlackRed(float rgb[3], float smin, float smax, float s)
{
	float hsv[3];
	hsv[0] = 0;
	hsv[1] = 1.0;
	hsv[2] = (s - smin) / (smax - smin); //low s -> 0, high s -> 1;
	HsvRgb(hsv, rgb);
}



void draw_arrow_head(float arrowlength, double head[2], float direct[2])
{
	glPushMatrix();
	glTranslatef(head[0], head[1], 0);
	glRotatef(atan2(direct[1], direct[0]) * 360 / (2 * M_PI), 0, 0, 1);
	glScalef(0.03, 0.03, 1);
	glColor3f(1.0, 1.0, 0.0);

	//draw tail

	glBegin(GL_LINES);
	glVertex2f(-(arrowlength), 0);
	glVertex2f(0, 0);
	glEnd();

	//draw head
	glBegin(GL_TRIANGLES);
	if (arrowlength < 1)
	{
		glVertex2f(0, 0);
		glVertex2f(-0.2*arrowlength, 0.1*arrowlength);
		glVertex2f(-0.2*arrowlength, -0.1*arrowlength);
	}
	else
	{
		glVertex2f(0, 0);
		glVertex2f(-0.2*arrowlength, 0.1*arrowlength);
		glVertex2f(-0.2*arrowlength, -0.1*arrowlength);
	}
	glEnd();
	glPopMatrix();
}

void gen_noise_tex()
{
	for (int x = 0; x < IMG_RES; x++)
		for (int y = 0; y < IMG_RES; y++)
		{
			noise_tex[x][y][0] =
				noise_tex[x][y][1] =
				noise_tex[x][y][2] = 255 /*(unsigned char)255 * (rand() % 32768) / 32768.0*/;
		}
}

void render_vec_img(Polyhedron *this_poly)
{
	glViewport(0, 0, (GLsizei)512, (GLsizei)512);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawBuffer(GL_BACK);
	int i, j;
	// first search the max_vx, min_vx, max_vy, min_vy through the entire field
	float *vx = new float[this_poly->nverts], *vy = new float[this_poly->nverts];
	for (int k = 0; k < this_poly->nverts; k++)
	{
		vx[k] = this_poly->vlist[k]->vx;
		vy[k] = this_poly->vlist[k]->vy;
	}

	vx_min_max[0] = vx_min_max[1] = vy_min_max[0] = vy_min_max[1] = 0;
	float min = vx[0], max = vx[0];
	for (int i = 0; i < this_poly->nverts; i++)
	{
		if (vx[i] <= min)
			min = vx[i];
		if (vx[i] >= max)
			max = vx[i];
	}
	vx_min_max[0] = min;
	vx_min_max[1] = max;
	min = max = vy[0];
	for (int i = 0; i < this_poly->nverts; i++)
	{
		if (vy[i] <= min)
			min = vy[i];
		if (vy[i] >= max)
			max = vy[i];
	}
	vy_min_max[0] = min;
	vy_min_max[1] = max;
	/*vy_min_max = calcminmax(this_poly->nverts, vy);*/

	// render the mesh
	for (i = 0; i < this_poly->ntris; i++)
	{
		Triangle *temp_t = this_poly->tlist[i];
		float rgb[3];
		glBegin(GL_TRIANGLES);
		for (j = 0; j < 3; j++)
		{
			Vertex *v = temp_t->verts[j];
			//determine the color for this vertex based on its vector value
			rgb[0] = (v->vx - vx_min_max[0]) / (vx_min_max[1] - vx_min_max[0]);
			rgb[1] = (v->vy - vy_min_max[0]) / (vy_min_max[1] - vy_min_max[0]);
			rgb[2] = 0.5;
			glColor3f(rgb[0], rgb[1], rgb[2]);
			glVertex2f(v->x, v->y);
		}
		glEnd();
	}
	// save the rendered image into the vec_img
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, vec_img);//vec_img saved here
}

void numContourSpinner(){};
void scalarSpinner(){};

typedef struct streamline
{
	std::vector<pixel> streamlinePoints;
	std::vector<pixel> samplePoints;
}streamline;
std::vector<streamline> streamlineArray;

void compute_lic_image(Polyhedron *this_poly)
{
	float threshold = pow(10., -5);
	
	pixel beg_pix; 
	if (ObjectId == 3)
	{
		beg_pix.i = 256; beg_pix.j = 256;
	}
	else
	{
		beg_pix.i = 256; beg_pix.j = 256;
	}
	samplePoints.clear(); //reinitialize sampling points
	streamlineArray.clear();
	compute_streamline(beg_pix);
	
	pixel cur_pix;

	int pix_dist = 0;
	int counter = 0, kernel_count = 0, loop_counter = 0;
	int loop_10 = 1000;
	bool isValid = true;
	float vx = 0, vy = 0;
	/*std::vector<pixel> pix_vector;*/
	float y = beg_pix.i + .5;
	float x = beg_pix.j + .5;
	int next_i = beg_pix.i;
	int next_j = beg_pix.j;
	int original_i = beg_pix.i;
	int original_j = beg_pix.j;
	float length = 0;
	float vx_cur = 0, vy_cur = 0;
	do //forward euler integration
	{
		float x_old = 0, y_old = 0;
		x_old = x;
		y_old = y;
		if (isValid) //get vx and vy of valid pixel.
		{
			vx = vx_min_max[0] + (vx_min_max[1] - vx_min_max[0]) * vec_img[next_i][next_j][0] / 255.0; //find where arrow points to
			vy = vy_min_max[0] + (vy_min_max[1] - vy_min_max[0]) * vec_img[next_i][next_j][1] / 255.0;
			length = sqrt(pow(vx, 2) + pow(vy, 2));
			//normalize vector here
			vx = vx / length;
			vy = vy / length;
			vx_cur = vx;
			vy_cur = vy;
			x = next_i + .5; //starting x
			y = next_j + .5; //starting y
			x_old = x;
			y_old = y;
		}

		if (length < threshold)
			break;

		x = x_old + -vy_cur; //move perpendicularly. either this x keeps accumulating or vy_cur
		y = y_old + vx_cur;
		next_i = int(y); 
		next_j = int(x); 
		x_old = int(x_old);
		y_old = int(y_old);
		if (next_j != x_old || next_i != y_old)
		{
			pix_dist++;
			y = next_i + .5; 
			x = next_j + .5; 
		}
		if (pix_dist >= d_sep)
		{
			pixel pix;
			next_i = int(y); //determine which pixel to jump to
			next_j = int(x);
			if (next_i < 0 || next_j < 0) //clamping minimum to 0 and max to 512
			{
				next_i = 0;
				next_j = 0;
			}
			if (next_i > IMG_RES || next_j > IMG_RES)
			{
				next_i = IMG_RES;
				next_j = IMG_RES;
			}
			beg_pix.i = next_i;
			beg_pix.j = next_j;
			//pix_array[kernel_count] = beg_pix;
			counter++; //this is for forward while loop
			kernel_count++;
			pix.i = beg_pix.i;
			pix.j = beg_pix.j;
			compute_streamline(pix); //found adjacent streamline to start tracing from

			isValid = true;
			pix_dist = 0;
		}
		else
		{
			isValid = false;
		}
		loop_counter++;
	} while (next_i >= 0 && next_j >= 0 && next_i <= IMG_RES && next_j <= IMG_RES && loop_counter < loop_10);

	//reset variables
	pix_dist = 0, vx = 0, vy = 0, x = beg_pix.j + .5, y = beg_pix.i + .5, counter = 0, kernel_count = 0, loop_counter = 0, loop_10 = 1000;
	next_i = beg_pix.i;
	next_j = beg_pix.j;
	original_i = beg_pix.i;
	original_j = beg_pix.j;
	isValid = true;
	length = 0;
	vx_cur = 0, vy_cur = 0;
	//compute_streamline(beg_pix);

	do //backward euler integration
	{
		float x_old = 0, y_old = 0; 
		x_old = x;
		y_old = y;
		if (isValid) //get vx and vy of valid pixel.
		{
			vx = vx_min_max[0] + (vx_min_max[1] - vx_min_max[0]) * vec_img[next_i][next_j][0] / 255.0; //find where arrow points to
			vy = vy_min_max[0] + (vy_min_max[1] - vy_min_max[0]) * vec_img[next_i][next_j][1] / 255.0;
			length = sqrt(pow(vx, 2) + pow(vy, 2));
			//normalize vector here
			vx = vx / length;
			vy = vy / length;
			vx_cur = vx;
			vy_cur = vy;
			x = next_i + .5; //starting x
			y = next_j + .5; //starting y
			x_old = x;
			y_old = y;
		}

		if (length < threshold)
			break;

		x = x_old - (-vy_cur); //move perpendicularly. either this x keeps accumulating or vy_cur
		y = y_old - vx_cur;
		next_i = int(y);
		next_j = int(x);
		x_old = int(x_old);
		y_old = int(y_old);
		if (next_j != x_old || next_i != y_old)
		{
			pix_dist++;//
			y = next_i + .5; //
			x = next_j + .5; //new x and y location of new pixel
		}
		if (pix_dist >= d_sep)
		{
			pixel pix;
			next_i = int(y); //determine which pixel to jump to
			next_j = int(x);
			if (next_i < 0 || next_j < 0) //clamping minimum to 0 and max to 512
			{
				next_i = 0;
				next_j = 0;
			}
			if (next_i > IMG_RES || next_j > IMG_RES)
			{
				next_i = IMG_RES;
				next_j = IMG_RES;
			}
			beg_pix.i = next_i;
			beg_pix.j = next_j;
			//pix_array[kernel_count] = beg_pix;
			counter++; //this is for forward while loop
			kernel_count++;
			pix.i = beg_pix.i;
			pix.j = beg_pix.j;
			compute_streamline(pix);

			isValid = true;
			pix_dist = 0;
		}
		else
		{
			isValid = false;
		}
		loop_counter++;
	} while (next_i >= 0 && next_j >= 0 && next_i <= IMG_RES && next_j <= IMG_RES && loop_counter < loop_10);

	for (int i = 0; i < IMG_RES; i += 32)
	{
		for (int j = 0; j < IMG_RES; j += 32)
		{
			cur_pix.i = i;
			cur_pix.j = j;
			compute_streamline(cur_pix);
		}
	}
	for (int i = 0; i < streamlineArray.size(); i++)
	{
		for (int j = 0; j < streamlineArray[i].streamlinePoints.size(); j++)
		{
			LIC_tex[streamlineArray[i].streamlinePoints[j].i][streamlineArray[i].streamlinePoints[j].j][0] = 100;
			LIC_tex[streamlineArray[i].streamlinePoints[j].i][streamlineArray[i].streamlinePoints[j].j][1] = 100;
			LIC_tex[streamlineArray[i].streamlinePoints[j].i][streamlineArray[i].streamlinePoints[j].j][2] = 100;
		}
	}
}

float GetSeedDist(pixel p1, pixel p2)
{
	return sqrt(pow((double)p2.i - p1.i, 2) + pow((double)p2.j - p1.j, 2));
}
bool IsGreaterDsep(pixel pix) //function loops thru samplePoints and checks if pix is >= dsep with each sample point. 
{
	bool isGreaterDsep = true;
	for (int i = 0; i < samplePoints.size(); i++)
	{
		float dist = GetSeedDist(samplePoints[i], pix);
		if (dist < d_sep)
		{
			isGreaterDsep = false;
			break;
		}
	}
	return isGreaterDsep;
}

void compute_streamline(pixel pix) 
{
	float threshold = pow(10., -3);
	float vx = 1., vy = 1.;
	int counter = 0, kernel_count = 0;
	//pixel pix_array[1005];
	std::vector<pixel> pix_vector;
	float y = pix.i + .5;
	float x = pix.j + .5;
	int next_i = pix.i;
	int next_j = pix.j;
	int original_i = pix.i;
	int original_j = pix.j;
	bool isValid = true;
	float vx_cur = 0, vy_cur = 0;
	int pixcounter = 0; //for evenly spaced sampling points along streamline
	std::vector<pixel> samplePointsTemp;
	int dsample = 10;

	if (pix.i == 256 && pix.j == 256 && samplePoints.empty()) //this is the beginning pixel. use as first sample point
	{
		samplePointsTemp.push_back(pix);
		//pix_array[kernel_count] = pix;
		pix_vector.push_back(pix);
		kernel_count++;
	}
	else if (!samplePoints.empty())
	{
		//check if pix is valid by check distance to samplePoints. pix dist to sample points >= dsep
		if (IsGreaterDsep(pix)) //valid pixel, continue streamline computation
		{
			samplePointsTemp.push_back(pix);
			//pix_array[kernel_count] = pix;
			pix_vector.push_back(pix);
			kernel_count++;
		}
		else //invalid pixel, do NOT start streamline computation
			return;
	}

	//another big while loop here to find perpendicular streamline seed point at dist = d_sep.
	do //forward euler integration
	{
		vx = vx_min_max[0] + (vx_min_max[1] - vx_min_max[0]) * vec_img[next_i][next_j][0] / 255.0; //find where arrow points to
		vy = vy_min_max[0] + (vy_min_max[1] - vy_min_max[0]) * vec_img[next_i][next_j][1] / 255.0;
		float length = sqrt(pow(vx, 2) + pow(vy, 2));
		if (length < threshold)
			break;
		vx = vx / length;
		vy = vy / length;
		x = x + vx; //move coordinate
		y = y + vy;
		next_i = int(y); //determine which pixel to jump to
		next_j = int(x);
		if (next_i < 0 || next_j < 0) //clamping minimum to 0 and max to 512
		{
			next_i = 0;
			next_j = 0;
		}
		if (next_i > IMG_RES || next_j > IMG_RES)
		{
			next_i = IMG_RES;
			next_j = IMG_RES;
		}

		if (pix.i != next_i || pix.j != next_j)//used to create sample points along streamline. when pixcounter >= dsample, add a sample point
			pixcounter++;

		pix.i = next_i;
		pix.j = next_j;

		if (IsGreaterDsep(pix))
		{
			//pix_array[kernel_count] = pix;
			pix_vector.push_back(pix);
			kernel_count++; //counter for pixel array index
		}
		else
			break;
		//check when to put this pix into sampling point vector
		if (pixcounter >= dsample /*&& GetSeedDist(samplePointsTemp.back(), pix) < d_sep*/)
		{
			samplePointsTemp.push_back(pix);
			pixcounter = 0;
		}

		counter++; //this is for forward while loop
		//kernel_count++; //counter for pixel array index
	} while (next_i >= 0 && next_j >= 0 && next_i <= IMG_RES && next_j <= IMG_RES && counter < kernel_size / 2);

	counter = 0;
	pix.i = original_i;
	pix.j = original_j;
	y = pix.i + .5;
	x = pix.j + .5;
	next_i = pix.i;
	next_j = pix.j;
	original_i = pix.i;
	original_j = pix.j;
	isValid = true;
	vx_cur = 0, vy_cur = 0;
	pixcounter = 0; //for evenly spaced sampling points along streamline

	do//backward euler integration
	{
		vx = vx_min_max[0] + (vx_min_max[1] - vx_min_max[0]) * vec_img[next_i][next_j][0] / 255.0; //find where arrow points to
		vy = vy_min_max[0] + (vy_min_max[1] - vy_min_max[0]) * vec_img[next_i][next_j][1] / 255.0;
		float length = sqrt(pow(vx, 2) + pow(vy, 2));
		if (length < threshold)
			break;
		vx = vx / length;
		vy = vy / length; //make unit vector
		x = x - vx; //move coordinate
		y = y - vy;
		next_i = int(y); //determine which pixel to jump to
		next_j = int(x);
		if (next_i < 0 || next_j < 0)
		{
			next_i = 0;
			next_j = 0;
		}
		if (next_i > IMG_RES || next_j > IMG_RES)
		{
			next_i = IMG_RES;
			next_j = IMG_RES;
		}

		if (pix.i != next_i || pix.j != next_j)//used to create sample points along streamline. when pixcounter >= dsample, add a sample point
			pixcounter++;

		pix.i = next_i;
		pix.j = next_j;

		if (IsGreaterDsep(pix))
		{
			//pix_array[kernel_count] = pix;
			pix_vector.push_back(pix);
			kernel_count++; //counter for pixel array index
		}
		else
			break;
		//check when to put this pix into sampling point vector
		if (pixcounter >= dsample /*&& GetSeedDist(samplePointsTemp.back(), pix) < d_sep*/)
		{
			samplePointsTemp.push_back(pix);
			pixcounter = 0;
		}

		/*	pix.i = next_i;
			pix.j = next_j;
			pix_array[kernel_count] = pix;*/
		counter++; //this is for backward while loop
		//kernel_count++; //counter for pixel array index
	} while (next_i >= 0 && next_j >= 0 && next_i <= IMG_RES && next_j <= IMG_RES && counter < kernel_size / 2);

	//reset pix to original values
	pix.i = original_i;
	pix.j = original_j;

	//draw pixels. color gray scale
	/*for (int i = 0; i < kernel_count - 1; i++)
	{
		LIC_tex[pix_array[i].i][pix_array[i].j][0] = 100;
		LIC_tex[pix_array[i].i][pix_array[i].j][1] = 100;
		LIC_tex[pix_array[i].i][pix_array[i].j][2] = 100;		
	}*/

	streamline newStreamline; //create streamline object. store in main streamline array "streamlineArray"
	newStreamline.streamlinePoints = pix_vector;
	newStreamline.samplePoints = samplePointsTemp;
	//if (streamlineArray.size() < 2)
	//	return;

	streamlineArray.push_back(newStreamline);

	//insert samplePointsTemp into global samplePoints
	for (int i = 0; i < samplePointsTemp.size(); i++)
	{
		samplePoints.push_back(samplePointsTemp[i]);
	}

}
