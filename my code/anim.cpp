// anim.cpp version 5.0 -- Template code for drawing an articulated figure.  CS 174A.
#include "../CS174a template/Utilities.h"
#include "../CS174a template/Shapes.h"

std::stack<mat4> mvstack;

int g_width = 800, g_height = 800 ;
float zoom = 1 ;

int animate = 0, recording = 0, basis_to_display = -1;
double TIME = 0;
double AnimatedTime = 0;
double lastAnimatedTime(0);
bool animationJustStarted = false;

const unsigned X = 0, Y = 1, Z = 2;

vec4 eye( 5, 0, 0, 1), ref( 0, 0, 0, 1 ), up( 0, 1, 0, 0 );	// The eye point and look-at point.

mat4	orientation, model_view;
ShapeData cubeData, sphereData, coneData, cylData, customizedCubeData;				// Structs that hold the Vertex Array Object index and number of vertices of each shape.
GLuint	texture_cube, texture_earth, texture_customized_cube;
GLint   uModelView, uProjection, uView,
		uAmbient, uDiffuse, uSpecular, uLightPos, uShininess,
		uTex, uEnableTex;

void init()
{
#ifdef EMSCRIPTEN
    GLuint program = LoadShaders( "vshader.glsl", "fshader.glsl" );								// Load shaders and use the resulting shader program
    TgaImage coolImage ("challenge.tga");
    TgaImage customizedCubeImage ("texture.tga");
    TgaImage earthImage("earth.tga");

#else
	GLuint program = LoadShaders( "../my code/vshader.glsl", "../my code/fshader.glsl" );		// Load shaders and use the resulting shader program
    TgaImage coolImage ("../my code/challenge.tga");
    TgaImage customizedCubeImage ("../my code/texture.tga");
    TgaImage earthImage("../my code/earth.tga");
#endif
    glUseProgram(program);

	generateCube(program, &cubeData);		// Generate vertex arrays for geometric shapes
    generateCustomizedCube(program, &customizedCubeData);
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView		= glGetUniformLocation( program, "View"       );
    uAmbient	= glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse	= glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular	= glGetUniformLocation( program, "SpecularProduct" );
    uLightPos	= glGetUniformLocation( program, "LightPosition"   );
    uShininess	= glGetUniformLocation( program, "Shininess"       );
    uTex		= glGetUniformLocation( program, "Tex"             );
    uEnableTex	= glGetUniformLocation( program, "EnableTex"       );

    glUniform4f( uAmbient,    0.2,  0.2,  0.2, 1 );
    glUniform4f( uDiffuse,    0.6,  0.6,  0.6, 1 );
    glUniform4f( uSpecular,   0.2,  0.2,  0.2, 1 );
    glUniform4f( uLightPos,  15.0, 15.0, 30.0, 0 );
    glUniform1f( uShininess, 100);

    glEnable(GL_DEPTH_TEST);
    
    glGenTextures( 1, &texture_cube );
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
                 (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, coolImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_customized_cube );
    glBindTexture( GL_TEXTURE_2D, texture_customized_cube );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, customizedCubeImage.width, customizedCubeImage.height, 0,
                 (customizedCubeImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, customizedCubeImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_earth );
    glBindTexture( GL_TEXTURE_2D, texture_earth );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, earthImage.width, earthImage.height, 0,
                 (earthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, earthImage.data );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glUniform1i( uTex, 0);	// Set texture sampler variable to texture unit 0
	
	glEnable(GL_DEPTH_TEST);
}

struct color{ color( float r, float g, float b) : r(r), g(g), b(b) {} float r, g, b;};
std::stack<color> colors;
void set_color(float r, float g, float b)
{
	colors.push(color(r, g, b));

	float ambient  = 0.2, diffuse  = 0.6, specular = 0.2;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1 );
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1 );
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1 );
}

int mouseButton = -1, prevZoomCoord = 0 ;
vec2 anchor;
void myPassiveMotionCallBack(int x, int y) {	anchor = vec2( 2. * x / g_width - 1, -2. * y / g_height + 1 ); }

void myMouseCallBack(int button, int state, int x, int y)	// start or end mouse interaction
{
    mouseButton = button;
   
    if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
        mouseButton = -1 ;
    if( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
        prevZoomCoord = -2. * y / g_height + 1;

    glutPostRedisplay() ;
}

void myMotionCallBack(int x, int y)
{
	vec2 arcball_coords( 2. * x / g_width - 1, -2. * y / g_height + 1 );
	 
    if( mouseButton == GLUT_LEFT_BUTTON )
    {
	   orientation = RotateX( -10 * (arcball_coords.y - anchor.y) ) * orientation;
	   orientation = RotateY(  10 * (arcball_coords.x - anchor.x) ) * orientation;
    }
	
	if( mouseButton == GLUT_RIGHT_BUTTON )
		zoom *= 1 + .1 * (arcball_coords.y - anchor.y);
    glutPostRedisplay() ;
}

void idleCallBack(void)
{
    if( !animate )
        return;

	double prev_time = TIME;
    TIME = TM.GetElapsedTime() ;
    if( prev_time == 0 ) TM.Reset();
    
    if (animationJustStarted == true) {
        animationJustStarted = false;
        lastAnimatedTime = TIME;
    }
    AnimatedTime += TIME - (TIME >= lastAnimatedTime ? lastAnimatedTime : TIME);
    lastAnimatedTime = TIME;
    
    glutPostRedisplay() ;
}

void drawCylinder()	//render a solid cylinder oriented along the Z axis; bases are of radius 1, placed at Z = 0, and at Z = 1.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

void drawCone()	//render a solid cone oriented along the Z axis; bases are of radius 1, placed at Z = 0, and at Z = 1.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}

void drawCube()		// draw a cube with dimensions 1,1,1 centered around the origin.
{
	glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawCustomizedCube()
{
    glBindTexture( GL_TEXTURE_2D, texture_customized_cube );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( customizedCubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, customizedCubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawSphere()	// draw a sphere with radius 1 centered around the origin.
{
	glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

int basis_id = 0;
void drawOneAxis()
{
	mat4 origin = model_view;
	model_view *= Translate	( 0, 0, 4 );
	model_view *= Scale(.25) * Scale( 1, 1, -1 );
	drawCone();
	model_view = origin;
	model_view *= Translate	( 1,  1, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube();
	model_view = origin;
	model_view *= Translate	( 1, 0, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube();
	model_view = origin;
	model_view *= Translate	( 0,  1, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube();
	model_view = origin;
	model_view *= Translate	( 0,  0, 2 );
	model_view *= Scale(.1) * Scale(   1, 1, 20);
    drawCylinder();	
	model_view = origin;
}

void drawAxes(int selected)
{
	if( basis_to_display != selected ) 
		return;
    //GLuint tmp = texture_cube;
    //texture_cube = 0;
	mat4 given_basis = model_view;
	model_view *= Scale		(.25);
	drawSphere();
	model_view = given_basis;
	set_color( 0, 0, 1 );
	drawOneAxis();
	model_view *= RotateX	(-90);
	model_view *= Scale		(1, -1, 1);
	set_color( 1, 1, 1);
	drawOneAxis();
	model_view = given_basis;
	model_view *= RotateY	(90);
	model_view *= Scale		(-1, 1, 1);
	set_color( 1, 0, 0 );
	drawOneAxis();
	model_view = given_basis;
	
	colors.pop();
	colors.pop();
	colors.pop();
	set_color( colors.top().r, colors.top().g, colors.top().b );
    //texture_cube = tmp;
}

void drawGround(){
	mvstack.push(model_view);
    set_color( .0, .8, .0 );
    model_view *= Translate	(0, -10, 0);									drawAxes(basis_id++);
    model_view *= Scale		(100, 1, 100);									drawAxes(basis_id++);
    drawCube();
	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
}

void drawShapes()
{
	mvstack.push(model_view);

    model_view *= Translate	( 0, 3, 0 );									drawAxes(basis_id++);
    model_view *= Scale		( 3, 3, 3 );									drawAxes(basis_id++);
    set_color( .8, .0, .8 );
    drawCube();

    model_view *= Scale		( 1/3.0f, 1/3.0f, 1/3.0f );						drawAxes(basis_id++);
    model_view *= Translate	( 0, 3, 0 );									drawAxes(basis_id++);
    set_color( 0, 1, 0 );
    drawCone();

    model_view *= Translate	( 0, -9, 0 );									drawAxes(basis_id++);
    set_color( 1, 1, 0 );
    drawCylinder();

	model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
	
    model_view *= Scale		( 1/3.0f, 1/3.0f, 1/3.0f );						drawAxes(basis_id++);

	drawGround();
}

void drawPlanets()
{
    set_color( .8, .0, .0 );	//model sun
    mvstack.push(model_view);
    model_view *= Scale(3);													drawAxes(basis_id++);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
    
    set_color( .0, .0, .8 );	//model earth
    model_view *= RotateY	( 10*TIME );									drawAxes(basis_id++);
    model_view *= Translate	( 15, 5*sin( 30*DegreesToRadians*TIME ), 0 );	drawAxes(basis_id++);
    mvstack.push(model_view);
    model_view *= RotateY( 300*TIME );										drawAxes(basis_id++);
    drawCube();
    model_view = mvstack.top(); mvstack.pop();								drawAxes(basis_id++);
    
    set_color( .8, .0, .8 );	//model moon
    model_view *= RotateY	( 30*TIME );									drawAxes(basis_id++);
    model_view *= Translate	( 2, 0, 0);										drawAxes(basis_id++);
    model_view *= Scale(0.2);												drawAxes(basis_id++);
    drawCylinder();
	
}

/*********************************************************
 setTextureImage()
 DOES: set the image of texture
 *********************************************************/
void setTextureImage(GLuint &texture, char* imageFile)
{
    TgaImage newImg(imageFile);
    /*if (!newImg.loadTGA(imageFile))
    {
        printf("Error loading image file\n");
        exit(1);
    }*/
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, newImg.width, newImg.height, 0,
                 (newImg.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, newImg.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}
/*********************************************************
 display() function
 *********************************************************/
vec4 unrotatedPoint = eye;
void display(void)
{
	basis_id = 0;
    glClearColor( .1, .1, .2, 1 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	set_color( .6, .6, .6 );
	
	model_view = LookAt( eye, ref, up );

	model_view *= orientation;
    model_view *= Scale(zoom);							drawAxes(basis_id++);
    
    float rotationBeginTime = 0;
    float timeToRotate = 10;
    float rotationSceneTime = AnimatedTime - rotationBeginTime;
    if (rotationSceneTime > 0 && rotationSceneTime < timeToRotate) {
        eye = RotateY(360/timeToRotate*rotationSceneTime)*unrotatedPoint;
    }
    if (0<TIME && TIME < rotationBeginTime) {
        unrotatedPoint = eye;
    }
    
    model_view *= RotateX(-90);
    drawCustomizedCube();
    
    model_view *= Translate(0, 1.5, 0);
    GLuint tmp = texture_cube;
    setTextureImage(texture_cube, "newYear.tga");
    drawCube();
    texture_cube = tmp;
    
    model_view *= Translate(0, 1.5, 0);                    drawAxes(basis_id++);
    drawCustomizedCube();

    
    glutSwapBuffers();
}

void myReshape(int w, int h)	// Handles window sizing and resizing.
{    
    mat4 projection = Perspective( 50, (float)w/h, 1, 1000 );
    glUniformMatrix4fv( uProjection, 1, GL_FALSE, transpose(projection) );
	glViewport(0, 0, g_width = w, g_height = h);	
}		

void instructions() {	 std::cout <<	"Press:"									<< '\n' <<
										"  r to restore the original view."			<< '\n' <<
										"  0 to restore the original state."		<< '\n' <<
										"  a to toggle the animation."				<< '\n' <<
										"  b to show the next basis's axes."		<< '\n' <<
										"  B to show the previous basis's axes."	<< '\n' <<
										"  q to quit."								<< '\n';	}

void myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':   case 27:				// 27 = esc key
            exit(0); 
		case 'b':
			std::cout << "Basis: " << ++basis_to_display << '\n';
			break;
		case 'B':
			std::cout << "Basis: " << --basis_to_display << '\n';
			break;
        case 'a':							// toggle animation           		
            if(animate) std::cout << "Elapsed time " << TIME << '\n';
            animate = 1 - animate ;
            animationJustStarted = true;
            break ;
		case '0':							// Add code to reset your object here.
			TIME = 0;	TM.Reset() ;											
        case 'r':
			orientation = mat4();			
            break ;
    }
    glutPostRedisplay() ;
}

int main() 
{
	char title[] = "Title";
	int argcount = 1;	 char* title_ptr = title;
	glutInit(&argcount,		 &title_ptr);
	glutInitWindowPosition (230, 70);
	glutInitWindowSize     (g_width, g_height);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow(title);
	#if !defined(__APPLE__) && !defined(EMSCRIPTEN)
		glewExperimental = GL_TRUE;
		glewInit();
	#endif
    std::cout << "GL version " << glGetString(GL_VERSION) << '\n';
	instructions();
	init();

	glutDisplayFunc(display);
    glutIdleFunc(idleCallBack) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCallBack) ;
    glutMotionFunc(myMotionCallBack) ;
    glutPassiveMotionFunc(myPassiveMotionCallBack) ;

	glutMainLoop();
}