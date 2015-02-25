// anim.cpp version 5.0 -- Template code for drawing an articulated figure.  CS 174A.
#include "../CS174a template/Utilities.h"
#include "../CS174a template/Shapes.h"
#include "terrainMap.h"
#include "vehicle.h"

std::stack<mat4> mvstack;

int g_width = 800, g_height = 800 ;
float zoom = 1 ;

int animate = 0, recording = 0, basis_to_display = -1;
double TIME = 0;
double AnimatedTime = 0;
double lastAnimatedTime(0);
bool animationJustStarted = false;

const unsigned X = 0, Y = 1, Z = 2;

vec4 eye( 10, 5, 0, 1), ref( 0, 0, 0, 1 ), up( 0, 1, 0, 0 );	// The eye point and look-at point.

mat4	orientation, model_view;
ShapeData cubeData, sphereData, coneData, cylData, customizedCubeData, pyramidData;				// Structs that hold the Vertex Array Object index and number of vertices of each shape.
GLuint	texture_cube, texture_earth, texture_customized_cube, texture_pyramid;
GLint   uModelView, uProjection, uView,
		uAmbient, uDiffuse, uSpecular, uLightPos, uShininess,
		uTex, uEnableTex;


TgaImage grassImage("texture_grass.tga");
TgaImage rockImage("texture_rock.tga");
TgaImage brickImage("texture_brick.tga");
TgaImage doorImage("texture_door.tga");
TgaImage newYearImage("newYear.tga");
TgaImage pyramidBricksImage("pyramid_bricks.tga");
TgaImage stoveImage("texture_stove.tga");
TgaImage mobHeadImage("texture.tga");
TgaImage mobBodyImage("texture_mob_body.tga");
TgaImage mobArmImage("texture_mob_arm.tga");
TgaImage mobLegImage("texture_mob_leg.tga");
TgaImage treeTrunkImage("texture_trunk.tga");
TgaImage treeLeavesImage("texture_tree_leaves.tga");
TgaImage magicDoorImage("texture_magic_door.tga");
TgaImage roadPaintImage("road_paint.tga");
TgaImage pavementImage("texture_pavement.tga");
TgaImage roadImage("texture_road.tga");
TgaImage buildingBricksImage("texture_building_bricks.tga");
TgaImage balconyImage("texture_balcony.tga");
TgaImage studioImage("texture_studio.tga");
TgaImage studioLogoImage("texture_minecraft_studio.tga");
TgaImage vehicleBodyImage("texture_vehicle_body.tga");
TgaImage vehicleWheelImage("texture_wheel.tga");
TgaImage woodImage("texture_wood.tga");
TgaImage trunkImage("texture_trunk_surface.tga");
TgaImage woodenDoorImage("texture_wooden_door.tga");
TgaImage motelLogoImage("texture_motel_logo.tga");
int currentCustomizedImageTexture = NONE;
int currentCubeImageTexture = NONE;

mat4 characterTransform; //transform done to character
mat4 characterPosition;  //position of character
bool charPosInit = false; //if character initial position is setup

enum charBehavior{NO_BEHAVIOR, FORWARD, BACKWARD, LEFT, RIGHT, TURN_LEFT, TURN_RIGHT, RUN};
enum scene{HOME, CITY, CAVE};
int currentScene = HOME;
int lastScene = HOME;
int currentBehavior = NO_BEHAVIOR;
int behaviorCount;
int lastBehavior = NO_BEHAVIOR;

int frameCount = 0;
double lastTime = 0;

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
    generatePyramid(program, &pyramidData);
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
    
    
    glGenTextures( 1, &texture_pyramid );
    glBindTexture( GL_TEXTURE_2D, texture_pyramid );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, pyramidBricksImage.width, pyramidBricksImage.height, 0,
                 (pyramidBricksImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, pyramidBricksImage.data );
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
	   orientation = RotateZ( 10 * (arcball_coords.y - anchor.y) ) * orientation;
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
    
    if (lastTime == 0) {
        lastTime = TIME;
    }
    if (TIME - lastTime >= 1) {
        printf("frame rate: %d\n", (int)(frameCount/(TIME-lastTime)));
        frameCount = 0;
        lastTime = TIME;
    }else{
        frameCount++;
    }
    
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

void drawPlainCube()		// draw a cube with dimensions 1,1,1 centered around the origin.
{
    //glBindTexture( GL_TEXTURE_2D, texture_cube );
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

void drawPyramid()
{
   	glBindTexture( GL_TEXTURE_2D, texture_pyramid );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( pyramidData.vao );
    glDrawArrays( GL_TRIANGLES, 0, pyramidData.numVertices );
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

/*********************************************************
 Deprecated!!!
 setTextureImage()
 DOES: set the image of texture
 args: @texture to be set
       @new image file
 *********************************************************/
void setTextureImage(GLuint &texture, char* imageFile)
{
    TgaImage newImg(imageFile);
    
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
 setTextureImage2()
 DOES: set the image of texture
 args: @texture to be changed
       @new instance of TgaImage
 This function will be faster than setTextureImage()
 since it does not need to load the new image file
 *********************************************************/
void setTextureImage2(GLuint &texture, TgaImage& newImg)
{
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
 generateTerrain()
 DOES: draw the given terrain
 *********************************************************/
void generateTerrain(bool customized, int width, int height, bool hollow = false)
{
    mvstack.push(model_view);
    for (int i=0; i<width; i++) {
        mvstack.push(model_view);
        for (int k=0; k<height; k++) {
            if (!hollow || i==0 || i==width-1){
                if (customized) {
                    drawCustomizedCube();
                }else{
                    drawCube();
                }
            } else{
                if (k==0 || k==height-1) {
                    if (customized) {
                        drawCustomizedCube();
                    }else{
                        drawCube();
                    }
                }
            }
            model_view *= Translate(0, 1, 0);
        }
        model_view = mvstack.top(); mvstack.pop();
        model_view *= Translate(1, 0, 0);
    }
    model_view = mvstack.top(); mvstack.pop();
}


/*********************************************************
 mapTerrain()
 DOES: draw the given terrain
 *********************************************************/
void mapObject(int layer[][OBJECT_WIDTH], int height);

void mapTerrain(int layer[][TERRAIN_SIZE], int mappingMethod=X_Y)
{
    mvstack.push(model_view);
    for (int i=0; i<TERRAIN_SIZE; i++) {
        mvstack.push(model_view);
        for (int k=0; k<TERRAIN_SIZE; k++) {
            if (layer[i][k] == NONE_ROW) {
                break;
            }
            if (layer[i][k] == GRASS) {
                if (currentCustomizedImageTexture != GRASS){
                    setTextureImage2(texture_customized_cube, grassImage);
                    currentCustomizedImageTexture = GRASS;
                }
                drawCustomizedCube();
            }
            if (layer[i][k] == ROCK) {
                if (currentCubeImageTexture != ROCK) {
                    setTextureImage2(texture_cube, rockImage);
                    currentCubeImageTexture = ROCK;
                }
                drawCube();
            }
            if (layer[i][k] == BRICK) {
                if (currentCustomizedImageTexture != BRICK) {
                    setTextureImage2(texture_customized_cube, brickImage);
                    currentCustomizedImageTexture = BRICK;
                }
                drawCustomizedCube();
            }
            if (layer[i][k] == BUILDING_BRICK) {
                if (currentCubeImageTexture != BUILDING_BRICK) {
                    setTextureImage2(texture_cube, buildingBricksImage);
                    currentCubeImageTexture = BUILDING_BRICK;
                }
                drawCube();
            }
            if (layer[i][k] == ROAD) {
                if (currentCubeImageTexture != ROAD) {
                    setTextureImage2(texture_cube, roadImage);
                    currentCubeImageTexture = ROAD;
                }
                drawCube();
            }
            if (layer[i][k] == BALCONY) {
                if (currentCustomizedImageTexture != BALCONY){
                    setTextureImage2(texture_customized_cube, balconyImage);
                    currentCustomizedImageTexture = BALCONY;
                }
                drawCustomizedCube();
            }
            if (layer[i][k] == CUBE) {
                drawCube();
            }
            if (layer[i][k] == TREE) {
                mvstack.push(model_view);
                mapObject(treeL1, 5);
                model_view *= Translate(0, 0, 1);
                mapObject(treeL1, 5);
                model_view *= Translate(0, 0, 1);
                mapObject(treeL1, 5);
                model_view *= Translate(0, 0, 1);
                mapObject(treeL2, 5);
                model_view *= Translate(0, 0, 1);
                mapObject(treeL2, 5);
                model_view *= Translate(0, 0, 1);
                mapObject(treeL3, 5);
                model_view *= Translate(0, 0, 1);
                mapObject(treeL4, 5);
                model_view = mvstack.top(); mvstack.pop();

            }
            model_view *= Translate(1, 0, 0);
        }
        model_view = mvstack.top(); mvstack.pop();
        if (mappingMethod == X_Y)
            model_view *= Translate(0, 1, 0);
        else if (mappingMethod == X_Z)
            model_view *= Translate(0, 0, 1);
    }
    model_view = mvstack.top(); mvstack.pop();
}

/*********************************************************
 mapTerrain()
 DOES: draw the given object in layer
 *********************************************************/

void mapObject(int layer[][OBJECT_WIDTH], int height)
{
    mvstack.push(model_view);
    for (int i=0; i<height; i++) {
        mvstack.push(model_view);
        for (int k=0; k<OBJECT_WIDTH; k++) {
            
            if (layer[i][k] == BRICK) {
                if (currentCustomizedImageTexture != BRICK) {
                    setTextureImage2(texture_customized_cube, brickImage);
                    currentCustomizedImageTexture = BRICK;
                }
                drawCustomizedCube();
            }
            else if (layer[i][k] == DOOR) {
                if (currentCustomizedImageTexture != DOOR) {
                    setTextureImage2(texture_customized_cube, doorImage);
                    currentCustomizedImageTexture = DOOR;
                }
                mvstack.push(model_view);
                model_view *= Translate(0, 0, 0.5);
                model_view *= Scale(0.1, 1, 2);
                drawCustomizedCube();
                model_view = mvstack.top(); mvstack.pop();
            }
            else if (layer[i][k] == STOVE) {
                if (currentCustomizedImageTexture != STOVE) {
                    setTextureImage2(texture_customized_cube, stoveImage);
                    currentCustomizedImageTexture = STOVE;
                }
                mvstack.push(model_view);
                model_view *= RotateZ(180);
                drawCustomizedCube();
                model_view = mvstack.top(); mvstack.pop();
            }
            else if (layer[i][k] == TREE_TRUNK) {
                if (currentCustomizedImageTexture != TREE_TRUNK) {
                    setTextureImage2(texture_customized_cube, treeTrunkImage);
                    currentCustomizedImageTexture = TREE_TRUNK;
                }
                drawCustomizedCube();
            }
            else if (layer[i][k] == LEAVES){
                if (currentCubeImageTexture != LEAVES) {
                    setTextureImage2(texture_cube, treeLeavesImage);
                    currentCubeImageTexture = LEAVES;
                }
                drawCube();
            }
            else if (layer[i][k] == CUBE) {
                drawCube();
            }
            model_view *= Translate(1, 0, 0);
        }
        model_view = mvstack.top(); mvstack.pop();
        model_view *= Translate(0, 1, 0);
    }
    model_view = mvstack.top(); mvstack.pop();
}

/*********************************************************
 sceneSetup() function
 *********************************************************/
double magicDoorPosition[2] = {2,17.5};
int magicDoorappearTime = 3;
void sceneSetup()
{
    
    mvstack.push(model_view);
    mapTerrain(layer1);
    model_view *= Translate(0, 0, 1);
    mapTerrain(layer2);
    model_view *= Translate(0, 0, 1);
    mapTerrain(layer3);
    model_view *= Translate(0, 0, 1);
    mapTerrain(layer4);
    model_view *= Translate(0, 0, 1);
    mapTerrain(layer5);
    model_view *= Translate(0, 0, 1);
    mapTerrain(layer5);
    model_view = mvstack.top(); mvstack.pop();
    
    mvstack.push(model_view);
    model_view *= Translate(7, 6, 1);
    mapObject(house_L1, HOUSE_HEIGHT);
    model_view *= Translate(0, 0, 1);
    mapObject(house_L2, HOUSE_HEIGHT);
    model_view *= Translate(0, 0, 1);
    mapObject(house_L3, HOUSE_HEIGHT);
    
    model_view *= Translate(4, 2, 0);
    mvstack.push(model_view);
    model_view *= Scale(5, 5, 5);
    drawPyramid();
    model_view = mvstack.top(); mvstack.pop();
    model_view = mvstack.top(); mvstack.pop();
    
    if (AnimatedTime > magicDoorappearTime){
        mvstack.push(model_view);
        model_view *= Translate(magicDoorPosition[0], magicDoorPosition[1], 1.5);
        model_view *= RotateZ(-90);
        model_view *= Scale(0.1, 1, 2);
        setTextureImage2(texture_customized_cube, magicDoorImage);
        drawCustomizedCube();
        model_view = mvstack.top(); mvstack.pop();
    }
    
}

/*********************************************************
 createCharacter() function
 *********************************************************/
double scaleFactor = 0.5;
const double headDimention = 1 * scaleFactor;
const double bodyWidth_X = 0.6 * scaleFactor;
const double bodyWidth_Y = 1 * scaleFactor;
const double bodyHeight = 1.4 * scaleFactor;
const double armWidth_X = bodyWidth_X;
const double armWidth_Y = 0.4 * scaleFactor;
const double armLength = 1.6 * scaleFactor;
const double legWidth_X = bodyWidth_X;
const double legWidth_Y = bodyWidth_Y/2;
const double legLength = 1.8 * scaleFactor;
const double totalHeight = headDimention + bodyHeight + legLength;
void createCharacter()
{
    mvstack.push(model_view);
    model_view = characterPosition*characterTransform;
    
    //draw head
    setTextureImage2(texture_customized_cube, mobHeadImage);
    mvstack.push(model_view);
    model_view *= Scale(headDimention, headDimention, headDimention);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    //drawBody
    model_view *= Translate(0, 0, -headDimention/2-bodyHeight/2);
    setTextureImage2(texture_customized_cube, mobBodyImage);
    mvstack.push(model_view);
    model_view *= Scale(bodyWidth_X, bodyWidth_Y, bodyHeight);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    //draw arms
    //left arm
    mvstack.push(model_view);
    model_view *= Translate(0, bodyWidth_Y/2+armWidth_Y/2, bodyHeight/2); //joint
    if (behaviorCount != 0 && lastBehavior != TURN_LEFT && lastBehavior != TURN_RIGHT)
        model_view *= RotateY(-10*sin(10*AnimatedTime)); // rotates arm
    model_view *= Translate(0, 0, -armLength/2);
    model_view *= Scale(armWidth_X, armWidth_Y, armLength);
    setTextureImage2(texture_customized_cube, mobArmImage);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    //right arm
    mvstack.push(model_view);
    model_view *= Translate(0, -(bodyWidth_Y/2+armWidth_Y/2), bodyHeight/2); //joint
    if (behaviorCount != 0 && lastBehavior != TURN_LEFT && lastBehavior != TURN_RIGHT)
        model_view *= RotateY(10*sin(10*AnimatedTime));   //rotates arm
    model_view *= Translate(0, 0, -armLength/2);
    model_view *= Scale(armWidth_X, armWidth_Y, armLength);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    //draw legs
    //left leg
    mvstack.push(model_view);
    model_view *= Translate(0, bodyWidth_Y/2-legWidth_Y/2, -bodyHeight/2); // joint
    if (behaviorCount != 0 && lastBehavior != TURN_LEFT && lastBehavior != TURN_RIGHT)
        model_view *= RotateY(10*sin(10*AnimatedTime));
    model_view *= Translate(0, 0, -legLength/2);
    setTextureImage2(texture_customized_cube, mobLegImage);
    model_view *= Scale(legWidth_X, legWidth_Y, legLength);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    //right leg
    mvstack.push(model_view);
    model_view *= Translate(0, -(bodyWidth_Y/2-legWidth_Y/2), -bodyHeight/2); // joint
    if(behaviorCount != 0 && lastBehavior != TURN_LEFT && lastBehavior != TURN_RIGHT)
        model_view *= RotateY(-10*sin(10*AnimatedTime));
    model_view *= Translate(0, 0, -legLength/2);
    setTextureImage2(texture_customized_cube, mobLegImage);
    model_view *= Scale(legWidth_X, legWidth_Y, legLength);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    
    model_view = mvstack.top(); mvstack.pop();
    currentCubeImageTexture = NONE;
    currentCustomizedImageTexture =NONE;
}

/*********************************************************
 behave() function
 *********************************************************/
void behave(int behavior)
{
    switch (behavior) {
        case FORWARD:
            characterTransform *= Translate(0.21, 0, 0);
            lastBehavior = FORWARD;
            break;
        case RUN:
            characterTransform *= Translate(0.5, 0, 0);
            lastBehavior = FORWARD;
            break;
        case BACKWARD:
            characterTransform *= Translate(-0.21, 0, 0);
            lastBehavior = BACKWARD;
            break;
        case LEFT:
            characterTransform *= Translate(0, 0.15, 0);
            lastBehavior = LEFT;
            break;
        case RIGHT:
            characterTransform *= Translate(0, -0.15, 0);
            lastBehavior = RIGHT;
            break;
        case TURN_LEFT:
            characterTransform *= RotateZ(3);
            lastBehavior = TURN_LEFT;
            break;
        case TURN_RIGHT:
            characterTransform *= RotateZ(-4);
            lastBehavior = TURN_RIGHT;
            break;
        default: ;
    }
}

/*********************************************************
 transformCharacter() function
 *********************************************************/
void transformCharacter()
{
    if (currentBehavior!=NONE) {
        behaviorCount = 5;
        behave(currentBehavior);
        currentBehavior = NONE;
    }
    else if (behaviorCount>0) {
        behaviorCount--;
        behave(lastBehavior);
    }
}


/*********************************************************
 newWorldSceneSetup() function
 *********************************************************/
const double buildingPos[2] = {60, 3};
const double stairWayPos[2] = {buildingPos[0]+0.5, buildingPos[1]+9.5};
const int cityLength = 80;
const int studioWidth = 40;
const int studioHeight = 14;
const int pavementWidth = 20;
const int pavement2Width = 10;
const int motelPos[2] = {3, pavementWidth+12};

void drawVehicle(int speed, int offset=0)
{
    //draw vehicle
    mvstack.push(model_view);
    model_view *= Translate(speed*TIME-(int)(speed*TIME+offset)/cityLength*cityLength, 0, 0);
    setTextureImage2(texture_cube, vehicleBodyImage);
    mapObject(vehicle_L1, VEHICLE_WIDTH);
    model_view *= Translate(0, 0, 1);
    mapObject(vehicle_L2, VEHICLE_WIDTH);
    model_view *= Translate(0, 0, 0.5);
    model_view *= Scale(1, 1, 0.1);
    mapObject(vehicle_L3, VEHICLE_WIDTH);
    model_view *= Scale(1, 1, 10);
    model_view *= Translate(0, 0, -1.5);
    setTextureImage2(texture_cube, vehicleWheelImage);
    mapObject(vehicle_L1_M2, VEHICLE_WIDTH);        drawAxes(basis_id++);
    model_view *= Translate(1, -0.5, 0);
    setTextureImage2(texture_cube, rockImage);
    
    mat4 tmp = model_view;
    model_view *= Scale(0.4, 0.3, 0.4);
    drawCube();
    model_view = tmp;
    model_view *= Translate(0, 3, 0);
    tmp = model_view;
    model_view *= Scale(0.4, 0.3, 0.4);
    drawCube();
    model_view = tmp;
    model_view *= Translate(3, 0, 0);
    tmp = model_view;
    model_view *= Scale(0.4, 0.3, 0.4);
    drawCube();
    model_view = tmp;
    model_view *= Translate(0, -3, 0);
    model_view *= Scale(0.4, 0.3, 0.4);
    drawCube();
    model_view = mvstack.top(); mvstack.pop();
}

void newWorldSceneSetup()
{
    //draw pavement
    setTextureImage2(texture_cube, pavementImage);
    generateTerrain(false, cityLength, pavementWidth);
    //draw road
    mvstack.push(model_view);
    model_view *= Translate(0, pavementWidth, 0);
    setTextureImage2(texture_cube, roadImage);
    currentCubeImageTexture = ROAD;
    generateTerrain(false, cityLength, 4);
    model_view *= Translate(0, 5, 0);
    generateTerrain(false, cityLength, 4);
    model_view *= Translate(0, -1, 0);
    mvstack.push(model_view);
    generateTerrain(false, 2, 1);
    setTextureImage2(texture_cube,roadPaintImage);
    for(int i=0; i<cityLength/10; i++)
    {
        model_view *= Translate(2, 0, 0);
        generateTerrain(false, 6, 1);
        model_view *= Translate(6, 0, 0);
        model_view *= Translate(2, 0, 0);
    }
    model_view = mvstack.top(); mvstack.pop();
    mvstack.push(model_view);
    setTextureImage2(texture_cube, roadImage);
    for (int i=0; i<cityLength/10; i++) {
        generateTerrain(false, 2, 1);
        model_view *= Translate(8, 0, 0);
        generateTerrain(false, 2, 1);
        model_view *= Translate(2, 0, 0);
    }
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 5, 0);
    setTextureImage2(texture_cube, pavementImage);
    generateTerrain(false, cityLength, pavement2Width);
    model_view = mvstack.top(); mvstack.pop();
    
    //draw Studio
    mvstack.push(model_view);
    model_view *= Translate(0, 4, 1);
    setTextureImage2(texture_cube, studioImage);
    for (int i=0; i<5; i++){
        generateTerrain(false, studioWidth, studioHeight, true);
        model_view *= Translate(0, 0, 1);
    }
    generateTerrain(false, studioWidth, studioHeight);
    model_view *= Translate(0, 0, 1);
    for(int i=studioWidth-4; i>studioWidth/2; i=i-4)
    {
        model_view *= Translate(2, 0, 0);
        generateTerrain(false, i, studioHeight);
        model_view *= Translate(0, 0, 1);
    }
    model_view = mvstack.top(); mvstack.pop();
    //draw studio logo
    mvstack.push(model_view);
    model_view *= Translate(studioWidth/2, studioHeight-0.5+4, 8);
    model_view *= RotateZ(90);
    model_view *= Scale(0.1, 8, 5.5);
    setTextureImage2(texture_customized_cube, studioLogoImage);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    //draw magic door
    mvstack.push(model_view);
    model_view *= Translate(magicDoorPosition[0], magicDoorPosition[1], 1.5);
    model_view *= RotateZ(-90);
    model_view *= Scale(0.1, 1, 2);
    setTextureImage2(texture_customized_cube, magicDoorImage);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    //draw vehicles
    mvstack.push(model_view);
    model_view *= Translate(0, pavementWidth, 1);
    drawVehicle(10);
    model_view *= Translate(50, 7, 0);
    model_view *= RotateZ(180);
    drawVehicle(15, cityLength-50);
    model_view = mvstack.top(); mvstack.pop();
    
    //draw motel
    mvstack.push(model_view);
    model_view *= Translate(motelPos[0], motelPos[1], 1);
    for (int k=0; k<2; k++){
        //draw doors
        mvstack.push(model_view);
        setTextureImage2(texture_customized_cube, woodenDoorImage);
        model_view *= Translate(0, 0, 0.5);
        for (int i=0; i<4; i++) {
            model_view *= Translate(1, 0, 0);
            mvstack.push(model_view);
            model_view *= Scale(1, 0.1, 2);
            drawCustomizedCube();
            model_view = mvstack.top(); mvstack.pop();
            model_view *= Translate(4, 0, 0);
        }
        model_view = mvstack.top(); mvstack.pop();
        
        setTextureImage2(texture_cube, trunkImage);
        mapTerrain(MOTEL_V_FRONT,X_Z);
        for (int i=0; i<3; i++) {
            mapTerrain(MOTEL_L1);
            model_view *= Translate(0, 0, 1);
        }
        mvstack.push(model_view);
        model_view *= Translate(-1, -2, 0);
        setTextureImage2(texture_cube, woodImage);
        generateTerrain(false, 22, 9);
        model_view = mvstack.top(); mvstack.pop();
        model_view *= Translate(0, 0, 1);
    }
    setTextureImage2(texture_cube, motelLogoImage);
    mapTerrain(MOTEL_V_LOGO, X_Z);
    model_view = mvstack.top(); mvstack.pop();
    
    //draw skyscraper
    mvstack.push(model_view);
    model_view *= Translate(0, 0, 1);
    model_view *= Translate(buildingPos[0], buildingPos[1], 0);
    mat4 tmp = model_view;
    for (int i=0; i<4; i++) {
        mapTerrain(building_L1);
        model_view *= Translate(0, 0, 1);
    }
    model_view = tmp;
    for (int i=0; i<4; i++) {
        mapTerrain(building_L1_M2);
        model_view *= Translate(0, 0, 1);
    }
    
    tmp = model_view;
    for (int k=0; k<10; k++) {
        mapTerrain(building_L2);
        //mapTerrain(building_L2_M2);
        model_view *= Translate(0, 0, 1);
        mapTerrain(building_L1);
        //mapTerrain(building_L2_M2);
        model_view *= Translate(0, 0, 1);
        mapTerrain(building_L1);
        model_view *= Translate(0, 0, 1);
    }
    model_view = tmp;
    for (int k=0; k<10; k++) {
        mapTerrain(building_L2_M2);
        model_view *= Translate(0, 0, 1);
        mapTerrain(building_L2_M2);
        model_view *= Translate(0, 0, 2);
    }
    mapTerrain(building_L2);
    model_view *= Translate(0, 0, 1);
    mapTerrain(building_L3);
    model_view *= Translate(0, 0, 1);
    mapTerrain(building_L4);
    model_view *= Translate(0, 0, 1);
    mapTerrain(building_L5);
    for (int i=0; i<5; i++) {
        model_view *= Translate(0, 0, 1);
        mapTerrain(building_L6);
    }
    model_view = mvstack.top(); mvstack.pop();
    
    
    //draw stairway
    mvstack.push(model_view);
    model_view *= Translate(stairWayPos[0], stairWayPos[1], 2.5);
    model_view *= Scale(0.1, 2, 4);
    setTextureImage2(texture_customized_cube, magicDoorImage);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
    mvstack.push(model_view);
    model_view *= Translate(stairWayPos[0]+2, stairWayPos[1]+0.5, 1.5+35);
    model_view *= Scale(0.1, 1, 2);
    drawCustomizedCube();
    model_view = mvstack.top(); mvstack.pop();
    
}

/*********************************************************
 triggerEvent() function
 *********************************************************/
bool trigger1 = false;
void triggerEvent()
{
    if (currentScene == HOME) {
        if (characterTransform[1][3] <=1+magicDoorPosition[1] && characterTransform[1][3] >= magicDoorPosition[1]
            &&  characterTransform[0][3] <= magicDoorPosition[0]+0.5 && characterTransform[0][3] >= magicDoorPosition[0]-0.5 &&     (AnimatedTime > magicDoorappearTime) ) {
            currentScene = CITY;
            orientation *= RotateY(180);
        }
    }
    if (currentScene == CITY) {
        if (characterTransform[0][3] >= stairWayPos[0] && characterTransform[0][3]<= stairWayPos[0]+1
            &&  characterTransform[1][3] >= stairWayPos[1]-1 && characterTransform[1][3] <= stairWayPos[1]+1
            && trigger1 == false) {
            characterTransform *= RotateZ(180)*Translate(0, 0, 35);
            orientation *= RotateY(180);
            trigger1 = true;
        }
        if (characterTransform[0][3] >= stairWayPos[0]+1 && characterTransform[0][3]<= stairWayPos[0]+3
            &&  characterTransform[1][3] >= stairWayPos[1] && characterTransform[1][3] <= stairWayPos[1]+1
            && trigger1 == true){
            characterTransform *= Translate(0, 0, -35);
            characterTransform *= RotateZ(180);
            characterTransform *= Translate(3, 0, 0);
            orientation *= RotateY(180);
            trigger1 = false;
        }
    }
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
	set_color( 1, 1, 1 );
	
	model_view = LookAt( eye, ref, up );

	model_view *= orientation;
    model_view *= Scale(zoom);
    
    float rotationBeginTime = 0;
    float timeToRotate = 5;
    float rotationSceneTime = AnimatedTime - rotationBeginTime;
    if (rotationSceneTime > 0 && rotationSceneTime < timeToRotate) {
        //eye = RotateY(360/timeToRotate*rotationSceneTime)*unrotatedPoint;
    }
    if (0<TIME && TIME < rotationBeginTime) {
        unrotatedPoint = eye;
    }
    model_view *= RotateX(-90);
    model_view *= RotateZ(180);    //initial setup for coordinate sys
    
    
    transformCharacter();                                       //define transform matrix for character
    model_view *= inverse(characterTransform);                  //camera tracking of character
    characterPosition = model_view;
    
    //initial setup of position for character
    if (!charPosInit) {
        charPosInit = true;
        characterTransform = Translate(5, 7, 0.2+totalHeight);
        characterTransform *= RotateZ(180);
        characterPosition *= Translate(5, 7, 0.2+totalHeight);
        characterPosition *= RotateZ(180);
    }
    
    
    createCharacter();
    //printf("x: %f y: %f\n",characterTransform[0][3], characterTransform[1][3]);
    
    switch (currentScene) {
        case HOME:
            sceneSetup();
            break;
        case CITY:
            //model_view *= Translate(0, 3, 0); //change coordinate position before creating new scene
            newWorldSceneSetup();
            break;
        default:
            break;
    }
    triggerEvent();
    
    lastScene = currentScene;
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
        case 'i':
            currentBehavior = FORWARD;
            break;
        case 'I':
            currentBehavior = RUN;
            break;
        case 'k':
            currentBehavior = BACKWARD;
            break;
        case 'j':
            currentBehavior = LEFT;
            break;
        case 'l':
            currentBehavior = RIGHT;
            break;
        case 'u':
            currentBehavior = TURN_LEFT;
            break;
        case 'o':
            currentBehavior = TURN_RIGHT;
            break;
        default:
            currentBehavior = NO_BEHAVIOR;
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