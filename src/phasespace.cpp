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

double t_0 = 0.5;
double t_1 = 1.2478926389764982736;
SymplecticBilliardSystem billiard(t_0,  t_1);

Poly	quad;
Poly	grid;
Line	xAxis;
Line	yAxis;
Line	ruler;

vec2_d mouse;
double rawMouseX;
double rawMouseY;


int nRegular0 = 0;
int nRegular1 = 0;


int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
inline Camera camera(SCR_WIDTH, SCR_HEIGHT, vec2(0.0f, 0.0f));

// settings

static int code = 4;

// const char* vertexShaderSource = R"(
// #version 330 core
// // Positions/Coordinates
// layout (location = 0) in vec3 aPos;
// // Outputs the color for the Fragment Shader
// out vec3 color;
// // Imports the camera matrix from the main function
// uniform float zoom;
// uniform vec2 cameraPos;
// uniform vec3 vertexColor;
// uniform float x_scale;
// void main()
// {
// 	// Outputs the positions/coordinates of all vertices
// 	gl_Position = vec4(zoom*(vec3(x_scale * (aPos.x - cameraPos.x), aPos.y - cameraPos.y, 0.0) ), 1);
// 	// Assigns the colors from the Vertex Data to "color"
// 	color = vertexColor;
// })";

// const char* fragmentShaderSource = R"(
// #version 330 core
// // Outputs colors in RGBA
// out vec4 FragColor;
// // Inputs the color from the Vertex Shader
// in vec3 color;
// void main()
// {
// 	FragColor = vec4(color, 1.0);
// })";

const char* fileVertexShader = "../src/default.vert";
const char* fileFragmentShader = "../src/default.frag";
const std::string vertexShaderSource = get_file_contents(fileVertexShader);
const std::string fragmentShaderSource = get_file_contents(fileFragmentShader);


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void onInitialization();

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main()
{
	//---------------------------------------------- Window Creation (using GLFW) -----------------------------------------------------------------------//
	GLFWwindow* window;
	// glfw: initialize and configure
	glfwInit();
	// We use OpenGL 3.3 (i.e. Major = 3, Minor = 3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	onInitialization();

	float backgroundColor[4] = {0.07f, 0.13f, 0.17f, 1.0f};

    //imgui stuff
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows // NEEDED FOR MULTI VIEWPORT (DRAGGIND IMGUI WINDOWS OUTSIDE)
	ImGui::StyleColorsDark();
	// NEEDED FOR MULTI VIEWPORT (DRAGGIND IMGUI WINDOWS OUTSIDE)
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	while ((!glfwWindowShouldClose(window)) && (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE))
	{

		// Specify the color of the background
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		//---------------------------------------- ImGui ----------------------------------------------//
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

			// End of imgui
			ImGui::End();
		
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
						// NEEDED FOR MULTI VIEWPORT (DRAGGIND IMGUI WINDOWS OUTSIDE)
			// Update and Render additional Platform Windows
			// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
			//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
		}


		// ------------------------- Reset trajectories if scene changed ---------------------------------------
		// All the edits happen using either the left (ImGui) or the right (all dragdrop functions) or the ENTER-key. 
		// So, since this changes the scene, we simly reset every time this happens.



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

