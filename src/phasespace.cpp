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
float zoom = 0.5;

Poly polygon;

int N = 4;
int n_iter = 10;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

const char* fileVertexShader = "../src/fourth.vert";
const char* fileFragmentShader = "../src/fourth.frag";
std::string vertexShaderSource = get_file_contents(fileVertexShader);
std::string fragmentShaderSource = get_file_contents(fileFragmentShader);

const char* fileVertexShaderShapes = "../src/default.vert";
const char* fileFragmentShaderShapes = "../src/default.frag";
std::string vertexShaderSourceShapes = get_file_contents(fileVertexShaderShapes);
std::string fragmentShaderSourceShapes = get_file_contents(fileFragmentShaderShapes);

const char* fileVertexShaderTex = "../src/loadTexture.vert";
const char* fileFragmentShaderTex = "../src/loadTexture.frag";
const std::string vertexShaderSourceTex = get_file_contents(fileVertexShaderTex);
const std::string fragmentShaderSourceTex = get_file_contents(fileFragmentShaderTex);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void onInitialization();

void refreshPhasespace(GLuint VAO, GLuint fbo, Shader shaderprogram, Shader shaderprogramTex, Rectangle rect, GLFWwindow* window);

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
	// Shader shaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	Shader shaderProgramTex(vertexShaderSourceTex.c_str(), fragmentShaderSourceTex.c_str());
	Shader shaderProgramShapes(vertexShaderSourceShapes.c_str(), fragmentShaderSourceShapes.c_str());

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
	rect.Create(SCR_WIDTH, SCR_HEIGHT);
 
	while ((!glfwWindowShouldClose(window)) && (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE))
	{
		std::string completeFragSource{R"(#version 410 core
		out vec4 FragColor;)"};
		completeFragSource.append("const int N = " + std::to_string(N) + ";");
		completeFragSource.append(fragmentShaderSource);
		Shader shaderProgramFourth(vertexShaderSource.c_str(), completeFragSource.c_str());

		polygon.vertexData[0] = polygon.vertexData[0] + vec2(0.0001, 0.0001);
		// Specify the color of the background
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use

        shaderProgramTex.Activate();
		shaderProgramTex.setUniform(zoom, "zoom");
		rect.Draw(shaderProgramTex, SCR_WIDTH, SCR_HEIGHT, polygon.vertexData);
		shaderProgramShapes.Activate();
		shaderProgramShapes.setUniform(zoom, "zoom");
		shaderProgramShapes.setUniform(float(SCR_HEIGHT) / float(SCR_WIDTH), "x_scale");
		polygon.Draw(shaderProgramShapes);
		// polygon.onMouse(shaderProgramShapes, mouse);
        
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
			ImGui::InputInt("Iterations", &n_iter, 1);
			ImGui::Text("Width, height: (%.0f, %.0f)", float(SCR_WIDTH), float(SCR_HEIGHT));
			ImGui::InputFloat("Zoom", &zoom);
			ImGui::InputInt("Number of vertices", &N);
            if(ImGui::Button("refresh phasespace")){
                refreshPhasespace(rect.VAO, rect.textureFBO, shaderProgramFourth,shaderProgramTex, rect, window);
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
		shaderProgramFourth.Delete();
	}
	shaderProgramTex.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

void onInitialization() {
	glLineWidth(1.0f); // width of lines in pixels
	polygon.Create();
	polygon.vertexData.push_back(vec2(cos(PI *  3./6.), sin(PI *  3./6.)));
	polygon.vertexData.push_back(vec2(cos(PI *  7./6.), sin(PI *  7./6.)));
	polygon.vertexData.push_back(vec2(cos(PI *  3./6.), -sin(PI *  3./6.)));
	polygon.vertexData.push_back(vec2(cos(PI * 11./6.), sin(PI * 11./6.)));
	polygon.color = vec3(0.0,1.0, 0.1);

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

void refreshPhasespace(GLuint VAO, GLuint fbo, Shader shaderprogram, Shader shaderprogramTex, Rectangle rect, GLFWwindow* window){
	int grid = 10;
	int blockWidth = 2*SCR_WIDTH/grid;
	int blockHeight = 2*SCR_HEIGHT/grid;
	// rect.create(SCR_WIDTH, SCR_HEIGHT);
	for (int i = 0; i < grid; i++)
	{
		for (int j = 0; j < grid; j++)
		{
			glBindVertexArray(VAO);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			// glClear(GL_COLOR_BUFFER_BIT);

			glViewport(i*blockWidth, j*blockHeight, blockWidth, blockHeight);

			shaderprogram.Activate();
			glUniform1f(glGetUniformLocation(shaderprogram.ID, "width"), 2*SCR_WIDTH);
			glUniform1f(glGetUniformLocation(shaderprogram.ID, "height"), 2*SCR_HEIGHT);
			shaderprogram.setUniform(polygon.vertexData, "VERTICES", N);
			shaderprogram.setUniform(n_iter, "ITERATIONS");
			shaderprogram.setUniform(N, "N");
			shaderprogram.setUniform(zoom,"zoom");

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER ,0);

			glViewport(0,0,2*SCR_WIDTH, 2*SCR_HEIGHT);
			// Specify the color of the background
			// glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
			// Clean the back buffer and assign the new color to it
			glClear(GL_COLOR_BUFFER_BIT);
			// Tell OpenGL which Shader Program we want to use

			shaderprogramTex.Activate();

			rect.Draw(shaderprogramTex, SCR_WIDTH, SCR_HEIGHT, polygon.vertexData);
					// Swap the back buffer with the front buffer
			glfwSwapBuffers(window);
			// Take care of all GLFW events
			glfwPollEvents();
		}
		
	}
	glViewport(0,0,2*SCR_WIDTH, 2*SCR_HEIGHT);	
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

