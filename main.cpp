//Yağmur Cansu Yıldırım
#include "Angel.h"

typedef vec4  color4;
typedef vec4  point4;

static int submenu1,submenu2,submenu3;
static int menu1;

//CUBE Code taken from class example spinCube

const int NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 pointsCube[NumVerticesCube];
color4 colorsCube[NumVerticesCube];

vec4 startColor(0.0, 1.0, 1.0, 0.0);
vec4 myColor = startColor;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.1, -0.1,  0.1, 1.0 ),
    point4( -0.1,  0.1,  0.1, 1.0 ),
    point4(  0.1,  0.1,  0.1, 1.0 ),
    point4(  0.1, -0.1,  0.1, 1.0 ),
    point4( -0.1, -0.1, -0.1, 1.0 ),
    point4( -0.1,  0.1, -0.1, 1.0 ),
    point4(  0.1,  0.1, -0.1, 1.0 ),
    point4(  0.1, -0.1, -0.1, 1.0 )
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
GLuint program;

vec3 location(-0.7,0.8,0.0);
vec3 velocityy(0.0,0.01,0.0);
vec3 velocityx(0.01,0.0,0.0);
vec3 gravity(0.0,0.0001,0.0);

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int IndexCube = 0;
void
quad( int a, int b, int c, int d )
{
    colorsCube[IndexCube] = myColor;
    pointsCube[IndexCube] = vertices[a];
    IndexCube++;
    colorsCube[IndexCube] = myColor;
    pointsCube[IndexCube] = vertices[b];
    IndexCube++;
    colorsCube[IndexCube] = myColor;
    pointsCube[IndexCube] = vertices[c];
    IndexCube++;
    colorsCube[IndexCube] = myColor;
    pointsCube[IndexCube] = vertices[a];
    IndexCube++;
    colorsCube[IndexCube] = myColor;
    pointsCube[IndexCube] = vertices[c];
    IndexCube++;
    colorsCube[IndexCube] = myColor;
    pointsCube[IndexCube] = vertices[d];
    IndexCube++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
drawCube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//SPHERE Code taken from "Interactive Computer Graphics - A Top-Down Approach with Shader Based Opengl 6th edition"
//"https://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/"
const int NumTimesToSubdivideSphere = 5;
const int NumTrianglesSphere        = 4096;
const int NumVerticesSphere         = 3 * NumTrianglesSphere;

point4 pointsSphere[NumVerticesSphere];
vec3   normalsSphere[NumVerticesSphere];
color4 colorsSphere[NumVerticesSphere];

int indexSphere = 0;

void triangleSphere(const point4& a, const point4& b, const point4& c) {
    vec3  normal = normalize( cross(b - a, c - b) );

    normalsSphere[indexSphere] = normal;
    pointsSphere[indexSphere] = a;
    indexSphere++;
    normalsSphere[indexSphere] = normal;
    pointsSphere[indexSphere] = b;
    indexSphere++;
    normalsSphere[indexSphere] = normal;
    pointsSphere[indexSphere] = c;
    indexSphere++;
}

point4 unit(const point4& p) {
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    
    point4 t;
    if ( len > DivideByZeroTolerance ) {
    t = p / sqrt(len);
    t.w = 1.0;
    }

    return t;
}

void divide_triangle( const point4& a, const point4& b, const point4& c, int count ) {
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle(  a, v1, v2, count - 1 );
        divide_triangle(  c, v2, v3, count - 1 );
        divide_triangle(  b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangleSphere( a, b, c );
    }
}

void drawSphere(int count) {
    point4 v[4] = {
    vec4( 0.0, 0.0, 1.0, 100.0 ),
    vec4( 0.0, 0.942809, -0.333333, 100.0 ),
    vec4( -0.816497, -0.471405, -0.333333, 100.0),
    vec4( 0.816497, -0.471405, -0.333333, 100.0 )
    };

    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}

//----------------------------------------------------------------------------

bool cube = true;

// Switching between cube and sphere

void switchCube(){
    cube = true;
    IndexCube = 0;
    drawCube();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsCube) + sizeof(colorsCube), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointsCube), pointsCube);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointsCube), sizeof(colorsCube), colorsCube);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsCube)));

}

void switchSphere () {
    cube = false;
    indexSphere = 0;
    drawSphere( NumTimesToSubdivideSphere );
    GLuint buffer;
    
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(normalsSphere),
          NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsSphere),
             sizeof(normalsSphere), normalsSphere );
    program = InitShader( "/Users/yagmur/Desktop/Comp410/Homework 1-2/Homework 1-2/vshader56.glsl", "/Users/yagmur/Desktop/Comp410/Homework 1-2/Homework 1-2/fshader56.glsl" );
    glUseProgram( program );
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(pointsSphere)) );

    // Initialize shader lighting parameters
    point4 light_position( 0.0, 0.0, 2.0, 0.0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 1.0, 0.0, 1.0, 1.0 );
    float  material_shininess = 5.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
          1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
          1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
          1, specular_product );
    
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
          1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"),
         material_shininess );
         
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    
    glEnable( GL_DEPTH_TEST );
    
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

}
//Color Code
void color(int n) {

    if (n == 1) { //mavi
        myColor = vec4(0.0, 0.0, 1.0, 1.0);
        if (cube) {
            switchCube();
        }
        else {
            switchSphere();
        }
    }
    else if (n == 2) { //kırmızı
        myColor = vec4(1.0, 0.0, 0.0, 1.0);
        if (cube) {
            switchCube();
        }
        else {
            switchSphere();
        }
    }
    else if (n == 3) { //sarı
        myColor =  vec4(0.0, 1.0, 0.0, 1.0);
        if (cube) {
            switchCube();
        }
        else {
            switchSphere();
        }
    }
    else if (n == 4) { //yeşil
        myColor = vec4(1.0, 1.0, 0.0, 1.0);
        if (cube) {
            switchCube();
        }
        else {
            switchSphere();
        }
    }
    else if (n == 5) { //beyaz
        myColor = vec4(1.0, 1.0, 1.0, 1.0);
        if (cube) {
            switchCube();
            glClearColor(0.0, 0.0, 0.0, 1.0);
        }
        else {
            switchSphere();
            glClearColor(0.0, 0.0, 0.0, 1.0);
        }

    }
    else if (n == 6) {
        myColor = vec4(0.0, 0.0, 0.0, 1.0);
        if(cube){
            switchCube();
            glClearColor(1.0, 1.0, 1.0, 1.0);
        }else{
            switchSphere();
            glClearColor(1.0, 1.0, 1.0, 1.0);
        }
    }
}

// OpenGL initialization
void
init()
{
    drawCube();

    // Create and initialize a buffer object
    GLuint buffer;
    
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pointsCube) + sizeof(colorsCube),
                 NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsCube), pointsCube );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsCube), sizeof(colorsCube), colorsCube );

    // Load shaders and use the resulting shader program
    program = InitShader( "/Users/yagmur/Desktop/Comp410/Homework 1-2/Homework 1-2/vshader.glsl", "/Users/yagmur/Desktop/Comp410/Homework 1-2/Homework 1-2/fshader.glsl" );
    glUseProgram( program );

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsCube)) );

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    
    // Set projection matrix
    mat4  projection;
    projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Ortho(): user-defined function in mat.h
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
    if (cube) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mat4  model_view = (  Translate( location ) * Scale(1.0, 1.0, 1.0) *
                            RotateX( Theta[Xaxis] ) *
                            RotateY( Theta[Yaxis] ) *
                            RotateZ( Theta[Zaxis] ) );
        
        glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
    } else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mat4  model_view = (  Translate( location ) * Scale(0.1, 0.1, 0.1) *
                            RotateX( Theta[Xaxis] ) *
                            RotateY( Theta[Yaxis] ) *
                            RotateZ( Theta[Zaxis] ) );
        
        glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);
    }
    
    glutSwapBuffers();
    
}
//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
        case 033:  // Escape key
        case 'q': case 'Q':
        exit( EXIT_SUCCESS );
        break;
        case 'h': case 'H':
        std::cout<< "i -> initialize the pose\nh -> help\nq -> quit";
        break;
        case 'i': case 'I':
        location = vec3(-0.7,0.8,0.0);
        break;
    }
}

//----------------------------------------------------------------------------

void
idle( void )
{
    glutPostRedisplay();
}

static void movement(int n) {

    //sağ duvara çarpınca
    if (location.x > 0.9) {
        velocityx = vec3 (-velocityx.x, 0.0,0.0);
    }

    //sol duvara çarpınca
    if (location.x < -0.9) {
        velocityx = vec3 (-velocityx.x, 0.0,0.0);
    }
    
    location.x += velocityx.x;
    
    //yere çarpınca
    if (location.y <= -0.85 ) {
        velocityy = vec3 (0.0,-velocityy.y + gravity.y,0.0);
    }
    
    //yukarı çarpınca
    if (location.y >= 0.85 ) {
        velocityy = vec3 (0.0, -velocityy.y,0.0);
    }
    
    //takılmasın diye
    if (location.y < -0.85) {
        location.y = -0.8;
    }

    
    velocityy.operator+=(gravity);

    gravity.y += 0.0000099999;

    location.operator-=(velocityy);

    glutTimerFunc(1, movement, 0);
    glutPostRedisplay();
}

void reshape( int width, int height ) {

    glViewport( 0, 0, width, height );
    GLfloat left = -1.0, right = 1.0;
    GLfloat top = 1.0, bottom = -1.0;
    GLfloat zNear = -40.0, zFar = 40.0;
    GLfloat aspect = GLfloat(width)/height;

    if (aspect > 1.0) {
        
        left *= aspect;
        right *= aspect;
        
    }
    else {
        top /= aspect;
        bottom /= aspect;
    }
    mat4 projection = Ortho(left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

void menu(int num) {
    
    if (num == 1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (num == 2) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (num == 3) {
        switchCube();
        glutPostRedisplay();
    }
    else if (num == 4) {
        switchSphere();
        glutPostRedisplay();
    }
    else if (num == 5) {
        color(1);
        glutPostRedisplay();
    }
    else if (num == 6) {
        color(2);
        glutPostRedisplay();
    }
    else if (num == 7) {
        color(3);
        glutPostRedisplay();
    }
    else if (num == 8) {
        color(4);
        glutPostRedisplay();
    }
    else if (num == 9) {
        color(5);
        glutPostRedisplay();
    }
    else if (num == 10) {
        color(6);
        glutPostRedisplay();
    }
}

void makeMenu(void) {
    
    submenu1 = glutCreateMenu(menu);
    glutAddMenuEntry("wireframe", 1);
    glutAddMenuEntry("solid", 2);
    submenu2 = glutCreateMenu(menu);
    glutAddMenuEntry("cube", 3);
    glutAddMenuEntry("sphere", 4);
    submenu3 = glutCreateMenu(menu);
    glutAddMenuEntry("blue", 5);
    glutAddMenuEntry("red", 6);
    glutAddMenuEntry("green", 7);
    glutAddMenuEntry("yellow", 8);
    glutAddMenuEntry("white", 9);
    glutAddMenuEntry("black", 10);
    menu1 = glutCreateMenu(menu);
    glutAddSubMenu("Drawing mode", submenu1);
    glutAddSubMenu("Object type", submenu2);
    glutAddSubMenu("Color", submenu3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Homework 1 Yağmur Cansu Yıldırım 69499" );

    glewExperimental = GL_TRUE;
    glewInit();

    init();
    makeMenu();
    movement(0);

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );
    glutReshapeFunc( reshape );

    glutMainLoop();

    return 0;
}
