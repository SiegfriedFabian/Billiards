// done: pause snap_to_grid whilst right_click event
// done: snap to ruler
// done: clear polygon
// done: translate whichever is closer
// make n-star
// list of known examples (second imgui?)
// show F 
// show C (sampled) (careful with non-convex vertices)
// read/write states from json(?)
// colinear test for consecutive vertices
// done: colorcoding of p0 and p1
// store relevant data as double

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"

#include<iostream>
#include<glad/glad.h>
#include"GLFW/glfw3.h"
#include<vector>
#include"shape.h"
#include"SymplecticBilliard.h"

// settings
const double PI = 3.14159265358979323846;

double rawMouseX;
double rawMouseY;



int nRegular0 = 0;
int nRegular1 = 0;


int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

const char* fileVertexShader = "../src/default.vert";
const char* fileFragmentShader = "../src/default.frag";
const std::string vertexShaderSource = get_file_contents(fileVertexShader);
const std::string fragmentShaderSource = get_file_contents(fileFragmentShader);

const char* fileVertexShaderTex = "../src/loadTexture.vert";
const char* fileFragmentShaderTex = "../src/loadTexture.frag";
const std::string vertexShaderSourceTex = get_file_contents(fileVertexShaderTex);
const std::string fragmentShaderSourceTex = get_file_contents(fileFragmentShaderTex);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void onInitialization();

void refreshPhasespace(GLuint VAO, GLuint fbo, Shader shaderprogram);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main()
{
	//---------------------------------------------- Window Creation (using GLFW) -----------------------------------------------------------------------//
	GLFWwindow* window;
	// glfw: initialize and configure
	glfwInit();
	// We use OpenGL 3.3 (i.e. Major = 3, Minor = 3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Symplectic Billiards", NULL, NULL);
	glfwSetWindowSize(window, SCR_WIDTH, SCR_HEIGHT);

	// GLFWwindow* window_phasespace = glfwCreateWindow(pwidth, pheight, "Symplectic Billiards Phasespace", NULL, NULL);
	// glfwHideWindow(window_phasespace);

	glfwSetWindowAspectRatio(window, SCR_WIDTH, SCR_HEIGHT);

	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window

#ifdef __APPLE__
	glViewport(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2);
#else
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
#endif
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	Shader shaderProgramTex(vertexShaderSourceTex.c_str(), fragmentShaderSourceTex.c_str());

	onInitialization();

	float backgroundColor[4] = {0.07f, 0.13f, 0.17f, 1.0f};

    //imgui stuff
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	// NEEDED FOR MULTI VIEWPORT (DRAGGIND IMGUI WINDOWS OUTSIDE)
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
	Rectangle rect;
	rect.create(SCR_WIDTH, SCR_HEIGHT);
    //     //////////////////Creation of the drawing Buffer and the texture to be drawn in///////////////////////////////
    // GLuint textureFBO;
    // GLuint texture;
    
    // glGenFramebuffers(1, &textureFBO);

    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_2D, texture);

    
    // glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB,  2*SCR_WIDTH,  2*SCR_HEIGHT, 0,GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // glBindFramebuffer(GL_FRAMEBUFFER, textureFBO);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    // glDrawBuffer(GL_FRONT_AND_BACK);
    // glReadBuffer(GL_FRONT_AND_BACK);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ///////////////////////////////////////
    // ///////////////////////////////////////
    // ///////////////////////////////////////
    // ///////////////////////////////////////
    //     GLfloat vertices[] = {
    //      1.0f,  0.0f,-1.0f,  // Top Right       
    //      1.0f,  0.0f, 1.0f,  // Bottom Right    
    //     -1.0f,  0.0f, 1.0f,  // Bottom Left     
    //     -1.0f,  0.0f,-1.0f,  // Top Left        
    // };

    // GLuint indices[] = {  
    //     0, 1, 2,  // First Triangle
    //     2, 3, 0,  // Second Triangle
    // };

    // GLuint VBO, VAO, EBO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);

    // glBindVertexArray(VAO); 
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);  

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);   

    // glBindBuffer(GL_ARRAY_BUFFER, 0); 

	vec2 vertices[3] = {
		vec2(cos(PI *  3./6.), sin(PI *  3./6.)),
		vec2(cos(PI *  7./6.), sin(PI *  7./6.)),
		vec2(cos(PI * 11./6.), sin(PI * 11./6.))
	};
	while ((!glfwWindowShouldClose(window)) && (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE))
	{


		// Specify the color of the background
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

        shaderProgramTex.Activate();

		rect.draw(shaderProgramTex, SCR_WIDTH, SCR_HEIGHT, vertices);
        
        {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////// Scene Editing ///////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////

			ImGui::NewFrame();
			// ImGUI window creation
			ImGui::Begin("Settings");
			glfwGetCursorPos(window, &rawMouseX, &rawMouseY);
			ImGui::Text("Raw Mouse position: (%.5f, %.5f)", rawMouseX, rawMouseY);
			// Framerate info
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::Text("Width, height: (%.0f, %.0f)", float(SCR_WIDTH), float(SCR_HEIGHT));
            if(ImGui::Button("refresh phasespace")){
                refreshPhasespace(rect.VAO, rect.textureFBO, shaderProgram);
            }

			// End of imgui
			ImGui::End();
		
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		}

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

void onInitialization() {
	// glLineWidth(1.0f); // width of lines in pixels
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_WIDTH);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}
void keyCallback(GLFWwindow* window){

}

void refreshPhasespace(GLuint VAO, GLuint fbo, Shader shaderprogram){
    glBindVertexArray(VAO);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderprogram.Activate();
    glUniform1f(glGetUniformLocation(shaderprogram.ID, "width"), 2*SCR_WIDTH);
    glUniform1f(glGetUniformLocation(shaderprogram.ID, "height"), 2*SCR_HEIGHT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER ,0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Apple somehow halves the resolution
	#ifdef __APPLE__
	SCR_WIDTH = width/2;
	SCR_HEIGHT = height/2;
	#else
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	#endif
    glViewport(0, 0, width, height);
}

