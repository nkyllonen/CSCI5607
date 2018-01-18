//CSCI 5607 OpenGL Tutorial (HW 1/2)
//5 - Model Load

#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*=============================*/
// Global Variables
/*=============================*/
bool saveOutput = false;
float timePast = 0;

//Phong/Blinn-Phong vertex shader
const GLchar* BP_vertexSource =
	"#version 150 core\n"
	"in vec3 position;"
	//"in vec3 inColor;"
	"const vec3 inColor = vec3(0.f,0.7f,0.f);"
	"const vec3 inlightDir = normalize(vec3(1,0,0));"  //we should really pass this in
	"in vec3 inNormal;"
	"out vec3 Color;"
	"out vec3 normal;"
	"out vec3 pos;"
	"out vec3 lightDir;"
	"uniform mat4 model;"
	"uniform mat4 view;"
	"uniform mat4 proj;"
	"void main() {"
	"   Color = inColor;"
	"   gl_Position = proj * view * model * vec4(position,1.0);"
	"   vec4 pos4 = (view * model * vec4(position,1.0));"
	"   pos = pos4.xyz/pos4.w;"  //Convert vec4 pos to vec3 ... typically pos4.w and you don't need the divide
	"   vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal,0.0);"  //OR don't use the inverse-Transpose ... but you'll have to normalize the normal
	"   normal = norm4.xyz;"  //Convert the vec4 to a vec3 ... we don't need to normalize if the above line is correct
	"   lightDir = (view * vec4(inlightDir,0)).xyz;"  //Transform light into to view space
	"}";

//Blinn-Phong fragment shader
const GLchar* BP_fragmentSource =
	"#version 150 core\n"
	"in vec3 Color;"
	"in vec3 normal;"
	"in vec3 pos;"
	"in vec3 lightDir;"
	"out vec4 outColor;"
	//lighting constant(s)
	"const float ambient = .3;"
	"void main() {"
	"   vec3 h = normalize(lightDir - pos);"					//halfway vector
	"   vec3 diffuseC = Color*max(dot(lightDir,normal),0.0);"
	"   vec3 ambC = Color*ambient;"
	"   vec3 reflectDir = reflect(-lightDir,normal);"			//GLSL assumes first arg point towards base of second (therefore negate)
	"   vec3 viewDir = normalize(-pos);"						//We know the eye is at (0,0)!
	//"   float spec = max(dot(reflectDir,viewDir),0.0);"		//for Phong shading
	"   float spec = max(dot(h, normal),0.0);"
	"   if (dot(lightDir,normal) <= 0.0)spec = 0;"
	"   vec3 specC = vec3(1.0,1.0,1.0)*pow(spec,4);"
	"   outColor = vec4(diffuseC+ambC+specC, 1.0);"
	"}";

//Gouraud vertex shader
const GLchar* G_vertexSource =
	"#version 150 core\n"
	//"in vec3 inColor;"
	"const vec3 inColor = vec3(0.f,0.7f,0.f);"
	"const vec3 inlightDir = normalize(vec3(1,0,0));"  //we should really pass this in
	"in vec3 position;"
	"in vec3 inNormal;"
	"out vec4 Color;"
	"uniform mat4 model;"
	"uniform mat4 view;"
	"uniform mat4 proj;"
	//lighting constant(s)
	"const float ambient = .3;"
	"void main() {"
	//transform into correct space
	"   gl_Position = proj * view * model * vec4(position,1.0);"
	//calc pos and normal for shading
	"   vec4 pos4 = (view * model * vec4(position,1.0));"
	"   vec3 pos = pos4.xyz/pos4.w;"  //Convert vec4 pos to vec3 ... typically pos4.w and you don't need the divide
	//"   vec4 norm4 = transpose(inverse(model)) * vec4(inNormal,0.0);"  //Just model, than normalize normal
	"   vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal,0.0);"  //OR don't use the inverse-Transpose ... but you'll have to normalize the normal
	"   vec3 normal = norm4.xyz;"  //Convert the vec4 to a vec3 ... we don't need to normalize if the above line is correct
	"   vec3 lightDir = (view * vec4(inlightDir,0)).xyz;"  //Transform light into to view space
	//calc per vertex shading
	"   vec3 diffuseC = inColor*max(dot(lightDir,normal),0.0);"
	"   vec3 ambC = inColor*ambient;"
	"   vec3 reflectDir = reflect(-lightDir,normal);"
	"   vec3 viewDir = normalize(-pos);" //We know the eye is at (0,0)!
	"   float spec = max(dot(reflectDir,viewDir),0.0);"
	"   if (dot(lightDir,normal) <= 0.0)spec = 0;"
	"   vec3 specC = vec3(1.0,1.0,1.0)*pow(spec,4);"
	"   Color = vec4(diffuseC+ambC+specC, 1.0);"
	"}";

//Gouraud fragment shader
const GLchar* G_fragmentSource =
	"#version 150 core\n"
	"in vec4 Color;"
	"out vec4 outColor;"
	"void main() {"
	"   outColor = Color;"
	"}";

bool fullscreen = false;
int screen_width = 800;
int screen_height = 600;

/*=============================*/
// Helper Function Declarations
/*=============================*/
void Win2PPM(int width, int height);

/*==============================================================*/
// MAIN
/*==============================================================*/
int main(int argc, char *argv[]){
  //ask which shaders to use
  char shader_choice;
  printf("\nWhich shading technique would you like to see?");
  printf("\n(B - Blinn-Phong or G - Gouraud) ");
  cin >> shader_choice;

  //ask which model to see
  string model_name;
  printf("\nWhich model would you like to see? (sphere, teapot, etc) ");
  cin >> model_name;

  SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

  //Ask SDL to get a recent version of OpenGL (3.2 or greater)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  //Create a window (offsetx, offsety, width, height, flags)
  SDL_Window* window = SDL_CreateWindow("My ModelLoad_shaders Program", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
  float aspect = screen_width/(float)screen_height; //aspect ratio (needs to be updated if the window is resized)

  //The above window cannot be resized which makes some code slightly easier.
	//Below show how to make a full screen window or allow resizing
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen

  //Create a context to draw in
  SDL_GLContext context = SDL_GL_CreateContext(window);
	
  if (gladLoadGLLoader(SDL_GL_GetProcAddress))
  {
    printf("\nOpenGL loaded\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n\n", glGetString(GL_VERSION));
  }
  else {
    printf("ERROR: Failed to initialize OpenGL context.\n");
    return -1;
  }

  ifstream modelFile;
	modelFile.open("models/" + model_name + ".txt");

	if (modelFile.fail())
	{
		printf("ERROR. Failed to open model file.\n");
		exit(0);
	}

	int numLines = 0;
	modelFile >> numLines;
	float* modelData = new float[numLines];
	for (int i = 0; i < numLines; i++){
		modelFile >> modelData[i];
	}
	printf("Mode line count: %d\n\n",numLines);
	float numTris = numLines/8;
	
  //Load the vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	if (shader_choice == 'B') glShaderSource(vertexShader, 1, &BP_vertexSource, NULL);
	else glShaderSource(vertexShader, 1, &G_vertexSource, NULL);
	glCompileShader(vertexShader);
	
	//Let's double check the shader compiled 
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                         "Compilation Error",
                         "Failed to Compile: Check Consol Output.",
                         NULL);
		printf("Vertex Shader Compile Failed. Info:\n\n%s\n",buffer);
	}
	
	//Load the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (shader_choice == 'B') glShaderSource(fragmentShader, 1, &BP_fragmentSource, NULL);
	else glShaderSource(fragmentShader, 1, &G_fragmentSource, NULL);
	glCompileShader(fragmentShader);
	
	//Double check the shader compiled 
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                         "Compilation Error",
                         "Failed to Compile: Check Consol Output.",
                         NULL);
		printf("Fragment Shader Compile Failed. Info:\n\n%s\n",buffer);
	}
	
	//Join the vertex and fragment shaders together into one program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
	glLinkProgram(shaderProgram); //run the linker
	
	glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)


	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
	
	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, numLines*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used

    //Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, normalized?, stride, offset
	  //Binds to VBO current GL_ARRAY_BUFFER 
	glEnableVertexAttribArray(posAttrib);
	
	//GLint colAttrib = glGetAttribLocation(shaderProgram, "inColor");
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(colAttrib);
	
	GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

  glBindVertexArray(0); //Unbind the VAO

  //Maybe we need a second VAO, e.g., some of our models are stored in a second format
  //GLuint vao2;  
	//glGenVertexArrays(1, &vao2); //Create the VAO
  //glBindVertexArray(vao2); //Bind the above created VAO to the current context
  //  Creat VBOs ... 
  //  Set-up attributes ...
  //glBindVertexArray(0); //Unbind the VAO
	

	glEnable(GL_DEPTH_TEST);  
	
	/*===========================================================================================
	* EVENT LOOP (Loop forever processing each event as fast as possible)
	* List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
	* Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
	===========================================================================================*/
	SDL_Event windowEvent;
	bool quit = false;

	while (!quit)
	{
	  if (SDL_PollEvent(&windowEvent))
	  {
		  switch (windowEvent.type) //event type -- key up or down
		  {
		  case SDL_QUIT:
			  quit = true; //Exit event loop
			  break;
		  case SDL_KEYUP:
			  //check for escape or fullscreen before checking other commands
			  if (windowEvent.key.keysym.sym == SDLK_ESCAPE) quit = true; //Exit event loop
			  else if (windowEvent.key.keysym.sym == SDLK_f) fullscreen = !fullscreen;
			  break;
		  default:
			  break;
		  }//END polling switch

		  SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen
	  }//END polling if
      
      // Clear the screen to default color
      glClearColor(.2f, 0.4f, 0.8f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      if (!saveOutput) timePast = SDL_GetTicks()/1000.f; 
      if (saveOutput) timePast += .07; //Fix framerate at 14 FPS
      glm::mat4 model;
      model = glm::rotate(model,timePast * 3.14f/2,glm::vec3(0.0f, 1.0f, 1.0f));
      model = glm::rotate(model,timePast * 3.14f/4,glm::vec3(1.0f, 0.0f, 0.0f));
      
	  GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

      glm::mat4 view = glm::lookAt(
      	glm::vec3(3.f, 0.f, 0.f),  //Cam Position
      	glm::vec3(0.0f, 0.0f, 0.0f),  //Look at point
      	glm::vec3(0.0f, 0.0f, 1.0f)); //Up
      GLint uniView = glGetUniformLocation(shaderProgram, "view");
      glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
      
      glm::mat4 proj = glm::perspective(3.14f/4, aspect, 1.0f, 10.0f); //FOV, aspect, near, far
      GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
      glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
      
      glBindVertexArray(vao);  
      glDrawArrays(GL_TRIANGLES, 0, numTris); //(Primitives, Which VBO, Number of vertices)
      if (saveOutput) Win2PPM(screen_width,screen_height);
      

      SDL_GL_SwapWindow(window); //Double buffering
	}
	

  //Clean Up
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, vbo);
  glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}


//Write out PPM image from screen
void Win2PPM(int width, int height){
	char outdir[10] = "out/"; //Must be defined!
	int i,j;
	FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;
    
    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
      fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }
    
    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"ERROR: Failed to open file to write image\n");
    }
    
    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
    
    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
    }
    
    free(image);
    fclose(fptr);
    counter++;
}