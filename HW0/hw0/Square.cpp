//HW 0 - Moving Square
//Starter code for the first homework assignment.
//This code assumes SDL2 and OpenGL are both properly installed on your system
//===========================================//
//Nikki Kyllonen - CSCI 5607 - 9/15/2017
//===========================================//

#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>
#include <math.h>

//===========================================//
//Global Variables
//===========================================//
float g_pos_x = 0.0f;
float g_pos_y = 0.0f;
float g_size = 0.25f;
float g_angle = M_PI / 4; //M_PI/4 = 45deg

float prev_x = NULL;
float prev_y = NULL;

float vertices[] = {  //These values should be updated to match the square's state when it changes
  0.25f,  0.25f,  // top right
  0.25f, -0.25f,  // bottom right
  -0.25f,  0.25f,  // top left 
  -0.25f, -0.25f,  // bottom left
}; 

int screen_width = 800;
int screen_height = 800;

float g_mouse_down = false;

bool g_bTranslate = false;
bool g_bRotate = false;
bool g_bScale = false;

//===========================================//
//Function Declarations
//===========================================//
void mouseClicked(float mx, float my);
void mouseDragged(float mx, float my);
void screenToObjectSpace(float s_x, float s_y, float &o_x, float &o_y);

//////////////////////////
///  Begin your code here
/////////////////////////

/*--------------------------------------------
	updateVertices
--------------------------------------------*/
void updateVertices(){ 
	vertices[0] = g_pos_x + (g_size*cos(g_angle) - g_size*sin(g_angle));  //Top right x (2)
	vertices[1] = g_pos_y + (g_size*sin(g_angle) + g_size*cos(g_angle));  //Top right y (2)
   
	vertices[2] = g_pos_x + (g_size*cos(g_angle) + g_size*sin(g_angle));  //Bottom right x (3)
	vertices[3] = g_pos_y + (g_size*sin(g_angle) - g_size*cos(g_angle));  //Bottom right y (3)
   
	vertices[4] = g_pos_x + (-g_size*cos(g_angle) - g_size*sin(g_angle));  //Top left x (1)
	vertices[5] = g_pos_y + (-g_size*sin(g_angle) + g_size*cos(g_angle));  //Top left y (1)
   
	vertices[6] = g_pos_x + (-g_size*cos(g_angle) + g_size*sin(g_angle));  //Bottom left x (4)
	vertices[7] = g_pos_y + (-g_size*sin(g_angle) - g_size*cos(g_angle));  //Bottom left y (4)
}
/*--------------------------------------------
	mouseClicked
--------------------------------------------*/
void mouseClicked(float m_x, float m_y){   
   printf("Clicked at %f, %f\n",m_x,m_y);
   
   g_bTranslate = false;
   g_bRotate = false;
   g_bScale = false;

   //---------------//
   //local variables
   //---------------//
   float sq_x = 0;
   float sq_y = 0;
   screenToObjectSpace(m_x, m_y, sq_x, sq_y);

   //---------------//
   //translation
   //---------------//
   //check if we clicked inside the square -> translate iff true
   float pad = 0.1;
   
   //1. completely out of bounds check
   if (sq_x > (1 + pad) || sq_x < (-1 - pad) || sq_y > (1 + pad) || sq_y < (-1 - pad))
   {
	   prev_x = NULL;
	   prev_y = NULL;
	   return;
   }
   //2. completely in bounds -> translation
   else if (sq_x < (1 - pad) && sq_x > (-1 + pad) && sq_y < (1 - pad) && sq_y > (-1 + pad))
   {
	   g_bTranslate = true;
   }
   //3. corners -> rotation
   else if ((sq_x > (1 - pad) || sq_x < (-1 + pad)) && (sq_y >(1 - pad) || sq_y < (-1 + pad)))
   {
	   g_bRotate = true;
	   prev_x = NULL;
	   prev_y = NULL;
   }
   //4. sides -> scaling
   else
   {
	   g_bScale = true;
	   prev_x = NULL;
	   prev_y = NULL;
   }
}
/*--------------------------------------------
	mouseDragged
--------------------------------------------*/
//TODO: My version of translate "jumps" when you click in the square, try to make it move smoothly
void mouseDragged(float m_x, float m_y){
   printf("Dragging through %f, %f\n",m_x,m_y);
   
   //---------------//
   //local variables
   //---------------//
   float sq_x = 0;
   float sq_y = 0;
   screenToObjectSpace(m_x, m_y, sq_x, sq_y);

   if (g_bTranslate)
   {
	   /*if (prev_x != NULL && prev_y != NULL) //if we have previous values
	   {
		   float ab_x = sq_x - prev_x;
		   float ab_y = sq_y - prev_y;

		   g_pos_x += ab_x;
		   g_pos_y += ab_y;

		   prev_x = m_x;
		   prev_y = m_y;
	   }
	   else
	   {
		   g_pos_x = m_x;
		   g_pos_y = m_y;

		   prev_x = m_x;
		   prev_y = m_y;
	   }*/

	   g_pos_x = m_x;
	   g_pos_y = m_y;
   }
   
   if (g_bScale)
   {
	   if (fabs(sq_x) > fabs(sq_y)) //scaling along x
	   {
		   g_size = m_x - g_pos_x;
	   }
	   else //scaling along y
	   {
		   g_size = m_y - g_pos_y;
	   }
   }

   if (g_bRotate)
   {

   }

   updateVertices();
}
/*--------------------------------------------
	screentoObjectSpace :
		converts (s_x, s_y) to (o_x, o_y)
--------------------------------------------*/
void screenToObjectSpace(float s_x, float s_y, float &o_x, float &o_y)
{
	//square space : centered on (0,0) and sides of length 1
	o_x = s_x;
	o_y = s_y;

	//subract global state values to undo translation
	o_x -= g_pos_x;
	o_y -= g_pos_y;

	//rotate by negative g_angle to undo rotation
	o_x = o_x*cos(-g_angle) - o_y*sin(-g_angle);
	o_y = o_x*sin(-g_angle) + o_y*cos(-g_angle);

	//divide global size value to undo scaling
	o_x /= g_size;
	o_y /= g_size;
}

/////////////////////////////////////////////////////
/// ... you can ignore any code below (for now)...///
/////////////////////////////////////////////////////

// Shader sources
const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "void main() {"
    "   gl_Position = vec4(position, 0.0, 1.0);"
    "}";
    
const GLchar* fragmentSource =
    "#version 150 core\n"
    "uniform vec3 triangleColor;"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(triangleColor, 1.0);"
    "}";
    
bool fullscreen = false;

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
	
	//The above window cannot be resized which makes some code slightly easier.
	//Below show how to make a full screen window or allow resizing
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen

    float aspect = screen_width/(float)screen_height; //aspect ratio (needs to be updated if the window is resized)
	
	updateVertices(); //set initial position of the square to match it's state
	
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//OpenGL functions using glad library
    if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
       printf("OpenGL loaded\n");
       printf("Vendor:   %s\n", glGetString(GL_VENDOR));
       printf("Renderer: %s\n", glGetString(GL_RENDERER));
       printf("Version:  %s\n", glGetString(GL_VERSION));
    }
    else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }
	
	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
	
	
	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo;
	glGenBuffers(1, &vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

	
	//Load the vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	
	//Let's double check the shader compiled 
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status){
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("Vertex Shader Compile Failed. Info:\n\n%s\n",buffer);
	}
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	
	//Double check the shader compiled 
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status){
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf("Fragment Shader Compile Failed. Info:\n\n%s\n",buffer);
	}
	
	//Join the vertex and fragment shaders together into one program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
	glLinkProgram(shaderProgram); //run the linker
	
	glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)
	
	
	//Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	  //Attribute, vals/attrib., type, normalized?, stride, offset
	  //Binds to VBO current GL_ARRAY_BUFFER 
	glEnableVertexAttribArray(posAttrib);
	
	GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
	  
	
	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	bool done = false;
	while (!done){
      while (SDL_PollEvent(&windowEvent)){  //Process input events (e.g., mouse & keyboard)
        if (windowEvent.type == SDL_QUIT) done = true;
        //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
        //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
        if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
          done = true; //Exit event loop
        if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
          fullscreen = !fullscreen;
          SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
      }
      
      
      int mx, my;
      if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) { //Is the mouse down?
         if (g_mouse_down == false){
			 mouseClicked(2 * mx / (float)screen_width - 1, 1 - 2 * my / (float)screen_height);
         } 
         else{
			 mouseDragged(2 * mx / (float)screen_width - 1, 1 - 2 * my / (float)screen_height);
         }
         g_mouse_down = true;
      } 
      else{
        g_mouse_down = false;
      }
      
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo

      
      // Clear the screen to black
      glClearColor(.2f, 0.4f, 0.8f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      
      glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);
        
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      

      SDL_GL_SwapWindow(window); //Double buffering
	}
	
	glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);


	//Clean Up
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}
