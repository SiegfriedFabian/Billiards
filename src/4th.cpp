// // TODO ---------------------------------------------------------------------------------------------------------------------------------------
// // How to arrange elements in IMGUI???
// // - fix resize
// // - big axes
// // - edit switch -> on "edit initial values" call billiard method "are_initial_values_updated" to match imgui input
// // - spawn new trajectory button like "number of iterations". Spawns trajectory at given "new starting point"
// // - multiple Trajectories with individual color-picker
// // - create integer field for a variable n and a button "create regular n-gon"  DONE
// //---------------------------------------------------------------------------------------------------------------------------------------

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"

#include<iostream>
#include<glad/glad.h>
#include"GLFW/glfw3.h"
#include<vector>
#include"shape.h"
#include"Trajectory.h"

#include "FourthBilliard.h"

// settings

Poly	polygon;
Poly	grid;
Line	xAxis;
Line	yAxis;
Line	ruler;
Points	traj;
Points	p1, p2, p3;

Trajectory traj2;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
inline Camera camera(SCR_WIDTH, SCR_HEIGHT, vec2(0.0f, 0.0f));

// settings

static int code = 0;

const char* vertexShaderSource = R"(
#version 330 core
// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Outputs the color for the Fragment Shader
out vec3 color;
// Imports the camera matrix from the main function
uniform float zoom;
uniform vec2 cameraPos;
uniform vec3 vertexColor;
uniform float x_scale;
void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(zoom*(vec3(x_scale * (aPos.x - cameraPos.x), aPos.y - cameraPos.y, 0.0) ), 1);
	// Assigns the colors from the Vertex Data to "color"
	color = vertexColor;
})";

const char* fragmentShaderSource = R"(
#version 330 core
// Outputs colors in RGBA
out vec4 FragColor;
// Inputs the color from the Vertex Shader
in vec3 color;
void main()
{
	FragColor = vec4(color, 1.0);
})";

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void updateMousePosCoord(GLFWwindow *window, vec2 &mouse);

void onInitialization();

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void editScene(GLFWwindow* window, vec2 mouse, int code, bool clearPolygon);

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
#elif
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
#endif

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram(vertexShaderSource, fragmentShaderSource);

	onInitialization();

	//imgui stuff
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//---------------------------------------------------------------------------------------------------------//
	//---------------------------------------- BEGIN Control Variables ----------------------------------------//
	//---------------------------------------------------------------------------------------------------------//

	//-------------------------------------------- Booleans --------------------------------------------------// 
	bool drawscreen = true;
	bool show_grid = 1;
	bool snapToGrid = false;
	bool snapToRuler = false;
	bool clearPolygon = false;
	bool editPoly = true;
	bool editRuler = false;
	bool showStyleEditor = false;
	bool startDynamicalSystem = true;

	//--------------------------------------------- Style -----------------------------------------------------// 
	float size = 5.0f;
	float color[3] = { 0.5f, 0.5f, 0.5f };
	double pX, pY;

	//---------------------------------------- Billiard controls ----------------------------------------------//
	int batch = 10;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	int count = 0;
	int nIter = 1000;
	int nRegular = 3; // How many edges for regular Polygon
	float init[2] = { 2.0, 2.0 };
	FourthBilliard billiard;
	//---------------------------------------------------------------------------------------------------------//
	//------------------------------------ END control Variables ----------------------------------------------//
	//---------------------------------------------------------------------------------------------------------//

	while ((!glfwWindowShouldClose(window)) && (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE))
	{
		vec2 mouse = mousePosCoord(window, SCR_WIDTH, SCR_HEIGHT, camera);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		

		// Camera
		camera.Inputs(window, 400*deltaTime);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		shaderProgram.setUniform(camera.Zoom, "zoom");
		shaderProgram.setUniform(camera.Position, "cameraPos");
		shaderProgram.setUniform(float(SCR_HEIGHT)/float(SCR_WIDTH), "x_scale");

		//---------------------------------------- Billiard ----------------------------------------------------//



		//---------------------------------------- Drawing Scene ----------------------------------------------//

		// if (clearPolygon) {
		// 	polygon.Clear();
		// 	polygon.AddVertex(vec2(0, 0));
		// 	polygon.AddVertex(vec2(1, 0));
		// 	clearPolygon = false;
		// 	polygon.vertexChange = true;
		// }

		// if (polygon.vertexChange || length(tInitold -tInit) > 10e-5 || x0x1.vertexChange) {
		// 	evenTraj.Clear();
		// 	oddTraj.Clear();
		// 	evenTraj.vertexData.push_back(polygon.ParamEdgeRatio(tInit.x));
		// 	oddTraj.vertexData.push_back(polygon.ParamEdgeRatio(tInit.y));
		// 	tCurrent.x = inverseParamPolygon(x0x1.vertexData[0], polygon);
		// 	//std::cout << tCurrent.x << std::endl;
		// 	tCurrent.y = inverseParamPolygon(x0x1.vertexData[1], polygon);
		// 	polygon.vertexChange = false;
		// 	x0x1.vertexChange = false;
		// 	count = 0;
		// }
		
		editScene(window, mouse, code, clearPolygon);

		grid.Draw(shaderProgram);
		// polygon.Draw(shaderProgram);
		ruler.Draw(shaderProgram);
		xAxis.Draw(shaderProgram);
		yAxis.Draw(shaderProgram);
		//---------------------------------------- ImGui ----------------------------------------------//
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			ImGui::NewFrame();
			// ImGUI window creation
			ImGui::Begin("Settings");
			// Text that appears in the window
			// Checkbox that appears in the window
			ImGui::Checkbox("Show Grid", &show_grid);
			ImGui::Checkbox("Snap to grid", &snapToGrid);
			ImGui::Checkbox("Snap to ruler", &snapToRuler);
			ImGui::Checkbox("Clear polygon", &clearPolygon);

			ImGui::Text("Drag and drop edit modes:");
			ImGui::RadioButton("Polygon", &code, 0); ImGui::SameLine();
			ImGui::RadioButton("Ruler", &code, 1); ImGui::SameLine();
			ImGui::RadioButton("x0 and x1", &code, 2);
	
			glfwGetCursorPos(window, &pX, &pY);
			ImGui::Text("Raw Mouse position: (%.5f, %.5f)", pX, pY);
			// Framerate info
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::Text("Mouse position: (%.5f, %.5f)", mouse.x, mouse.y);
			ImGui::Text("Camera position: (%.5f, %.5f)", camera.Position.x, camera.Position.y);
			
			ImGui::Text("Width, height: (%.0f, %.0f)", float(SCR_WIDTH), float(SCR_HEIGHT));


			// Slider that appears in the window
			// ImGui::InputFloat("p1.x", &p1.vertexData[0].x, 0.001f, 1000.0, "%.6f");
			// ImGui::InputFloat("p1.y", &p1.vertexData[0].y, 0.001f, 1000.0, "%.6f");
			ImGui::InputInt("Number of iterations", &nIter, 1);
			ImGui::InputInt("Batch", &batch, 1);
			for (int i = 0; i < polygon.directions.size(); i++)
			{
				// ImGui::InputFloat2(("Vertex #"+std::to_string(i)).c_str(), &polygon.vertexData[i].x);
			}

			// Fancy color editor that appears in the window
			ImGui::ColorEdit3("Grid color", &grid.color.x);


		billiard.updateCoords(mouse, window);

		if (billiard.polygonClosed()) {
			billiard.iterate(batch, nIter);
		}


		if (snapToGrid) billiard.snapToGrid();

		// Slider that appears in the window

		for (size_t i = 0; i < billiard.trajectories.size(); i++)
		{
			ImGui::InputScalarN("Starting point:", ImGuiDataType_Float, &billiard.trajectories[i].vertexData[0].x, 2, NULL, NULL, "%.6f");
			ImGui::InputScalarN("End point:", ImGuiDataType_Float, &billiard.trajectories[i].vertexData.back().x, 2, NULL, NULL, "%.6f");
		}
		
		ImGui::InputFloat2("new starting point:", init, "%.6f");

		ImGui::InputInt("Number of iterations", &nIter, 1);
		ImGui::InputInt("Batch", &batch, 1);
		for (int i = 0; i < billiard.polygon.directions.size(); i++)
		{
			// ImGui::InputFloat2(("Vertex #" + std::to_string(i)).c_str(), &billiard.polygon.vertexData[i].x);
		}

		// "make regular Polygon" button
		ImGui::InputInt("Edges", &nRegular, 1);
		if (ImGui::Button("Make regular")) {
			billiard.makeRegularNPoly(nRegular);
		}

		// Fancy color editor that appears in the window
		ImGui::ColorEdit3("Grid color", &grid.color.x);
		grid.Draw(shaderProgram);

		xAxis.Draw(shaderProgram);
		yAxis.Draw(shaderProgram);

		ImGui::ColorEdit3("Polygon color", &polygon.color.x);
		billiard.drawPolygon(shaderProgram);
		billiard.drawTrajectories(shaderProgram);

			ImGui::Checkbox("Show Style Editor", &showStyleEditor);
			if (showStyleEditor) {
				ImGui::ShowStyleEditor();
			}
			// End of imgui
			ImGui::End();

			// Renders the ImGUI elements
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_WIDTH);
	float gridsize = 1.0f;
	float nGridEdge = 100.0f;

	for (float i = -nGridEdge; i < nGridEdge; i = i + 2) {
		grid.vertexData.push_back(vec2(i * gridsize, nGridEdge));
		grid.vertexData.push_back(vec2(i * gridsize, -nGridEdge));
		grid.vertexData.push_back(vec2((i + 1) * gridsize, -nGridEdge));
		grid.vertexData.push_back(vec2((i + 1) * gridsize, nGridEdge));
	}
	grid.vertexData.push_back(vec2(nGridEdge, nGridEdge));
	grid.vertexData.push_back(vec2(nGridEdge, -nGridEdge));


	for (float i = -nGridEdge; i < nGridEdge; i = i + 2) {
		grid.vertexData.push_back(vec2(nGridEdge, i * gridsize));
		grid.vertexData.push_back(vec2(-nGridEdge, i * gridsize));
		grid.vertexData.push_back(vec2(-nGridEdge, (i + 1) * gridsize));
		grid.vertexData.push_back(vec2(nGridEdge, (i + 1) * gridsize));
	}
	grid.vertexData.push_back(vec2(nGridEdge, nGridEdge));
	grid.vertexData.push_back(vec2(-nGridEdge, nGridEdge));
	grid.color = vec3(0.3f, 0.3f, 0.3f);
	grid.Create();

	float axisLength = 10000.0;
	xAxis.set(vec2(-axisLength, 0.0), vec2(axisLength, 0.0));
	xAxis.color		= vec3(1.0, 1.0, 1.0);

	yAxis.set(vec2(0.0, -axisLength), vec2(0.0, axisLength));
	yAxis.color		= vec3(1.0, 1.0, 1.0);

	ruler.set(vec2(1, 1), vec2(1, 1) + vec2(-2, 2));
	//ruler.lineWidth = 3;
	ruler.color = vec3(1, 1, 0);


	traj.vertexData.push_back(vec2(2.2, 3.0));
	p1.vertexData.push_back(vec2());
	p2.vertexData.push_back(vec2());
	p3.vertexData.push_back(vec2());

	traj.color = vec3(0.0, 1.0, 0.0);
	p1.color = vec3(1.0, 0.0, 0.0);
	p2.color = vec3(0.0, 0.0, 1.0);
	p3.color = vec3(0.5, 0.5, 0.5);

	traj2 = Trajectory(vec2_d(5, 5), &polygon, vec3(1.0, 0.0, 0.0));

	traj.size = 3.0f;
	// Adding objects to the scene
	ruler.Create();
	xAxis.Create();
	yAxis.Create();
	polygon.Create();
	traj.Create();
	traj2.Create();
	p1.Create();
	p2.Create();
	p3.Create();

	printf("\nUsage: \n");
	printf("Key 'w' or 'up'		:     Camera pan +y \n");
	printf("Key 'a' or 'left'	:     Camera pan -x\n");
	printf("Key 's' or 'down'	:     Camera pan -y\n");
	printf("Key 'd' or 'right'	:     Camera pan +x\n");
	printf("Scroll up or 'space'			:   	Camera zoom in\n");
	printf("Scroll down or 'space + leftshift'	: 	Camera zoom out\n");

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera.Zoom *= 1.0f + yoffset * 0.1;
	}
	else {
		camera.Zoom *= 1.0f + yoffset * 0.02;
	}
}

void editScene(GLFWwindow* window, vec2 mouse, int code, bool clearPolygon){

		switch (code)
		{
		case 0:
			p1.size = 10.0;
			ruler.lineWidth = 1.0;
			polygon.lineWidth = 5.0;
			// polygon.color = vec3(1,1,1);
			polygon.onMouse(window, mouse);
			
			break;
		case 1:
			p1.size = 10.0;
			ruler.lineWidth = 5.0;
			polygon.lineWidth = 1.0;
			ruler.dragDropTo(window, mouse);
			ruler.updateDirections();
			std::cout << ruler.direction.x << "," << ruler.direction.y << std::endl;
			break;
		case 2:
			p1.size = 20.0;
			ruler.lineWidth = 1.0;
			polygon.lineWidth = 1.0;
			// p1.dragDropTo(window, projectOntoPolygon(mouse, polygon));
			p1.vertexChange = true;
			break;
		}

		if (clearPolygon) {
			polygon.Clear();
			polygon.AddVertex(vec2(0, 0));
			polygon.AddVertex(vec2(1, 0));
			clearPolygon = false;
			polygon.vertexChange = true;
		}
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


// // TODO ---------------------------------------------------------------------------------------------------------------------------------------
// // How to arrange elements in IMGUI???
// // - fix resize
// // - big axes
// // - edit switch -> on "edit initial values" call billiard method "are_initial_values_updated" to match imgui input
// // - spawn new trajectory button like "number of iterations". Spawns trajectory at given "new starting point"
// // - multiple Trajectories with individual color-picker
// // - create integer field for a variable n and a button "create regular n-gon"  DONE
// //---------------------------------------------------------------------------------------------------------------------------------------

// #include"imgui/imgui.h"
// #include"imgui/imgui_impl_glfw.h"
// #include"imgui/imgui_impl_opengl3.h"

// #include<iostream>
// #include<glad/glad.h>
// #include"GLFW/glfw3.h"
// #include<vector>
// #include"shape.h"
// #include"Trajectory.h"

// #include "FourthBilliard.h"

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
// int SCR_WIDTH = 1280;
// int SCR_HEIGHT = 720;

// Camera camera(SCR_WIDTH, SCR_HEIGHT, vec2(0.0,0.0));

// Poly	polygon;
// Poly	grid;
// Line	xAxis;
// Line	yAxis;
// Line	ruler;
// Points	traj;
// Points	p1, p2, p3;

// Trajectory traj2;

// void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// void processInput(GLFWwindow *window);

// void updateMousePosCoord(GLFWwindow *window, vec2 &mouse);

// void onInitialization();

// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// void editScene(GLFWwindow* window, vec2 mouse, int code);

// int main()
// {

// 	//---------------------------------------------- Window Creation (using GLFW) -----------------------------------------------------------------------//
// 	GLFWwindow* window;
// 	// glfw: initialize and configure
// 	glfwInit();
// 	// We use OpenGL 3.3 (i.e. Major = 3, Minor = 3)
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

// 	// Create a windowed mode window and its OpenGL context
// 	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Symplectic Billiards", NULL, NULL);


// 	// Error check if the window fails to create
// 	if (window == NULL)
// 	{
// 		std::cout << "Failed to create GLFW window" << std::endl;
// 		glfwTerminate();
// 		return -1;
// 	}
// 	// Introduce the window into the current context
// 	glfwMakeContextCurrent(window);

// 	//Load GLAD so it configures OpenGL
// 	gladLoadGL();
// 	// Specify the viewport of OpenGL in the Window
// 	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

// 	// Generates Shader object using shaders default.vert and default.frag
// 	Shader shaderProgram(vertexShaderSource, fragmentShaderSource);

// 	FourthBilliard billiard;

// 	onInitialization();


// 	//---------------------------------------------------------------------------------------------------------//
// 	//---------------------------------------- Control Variables ----------------------------------------------//
// 	//---------------------------------------------------------------------------------------------------------//

// 	//-------------------------------------------- Booleans --------------------------------------------------// 
// 	bool drawscreen = true;
// 	bool show_grid = 1;
// 	//bool show_phasespace = glfwGetWindowAttrib(window_phasespace, GLFW_VISIBLE);;
// 	bool snapToGrid = false;
// 	bool clearPolygon = false;
// 	bool editPoly = true;
// 	bool editRuler = false;
// 	bool editInit = false;
// 	bool showStyleEditor = false;

// 	//--------------------------------------------- Style -----------------------------------------------------// 
// 	float size = 5.0f;
// 	float color[3] = { 0.5f, 0.5f, 0.5f };

// 	//---------------------------------------- Billiard controls ----------------------------------------------//
// 	int batch = 1000;
// 	float deltaTime = 0.0f;	// Time between current frame and last frame
// 	float lastFrame = 0.0f; // Time of last frame
// 	int nIter = 100000;

// 	float init[2] = { 2.0, 2.0 };
// 	int nRegular = 3; // How many edges for regular Polygon
// 	//------------------------------------ END of control Variables ------------------------------------------//


// 	//imgui
// 	ImGui::CreateContext();
// 	ImGuiIO& io = ImGui::GetIO(); (void)io;
// 	ImGui::StyleColorsDark();
// 	ImGui_ImplGlfw_InitForOpenGL(window, true);
// 	ImGui_ImplOpenGL3_Init("#version 330");

// 	// Main while loop
// 	while (!glfwWindowShouldClose(window))
// 	{
// 		vec2 mouse = mousePosCoord(window, SCR_WIDTH, SCR_HEIGHT, camera);
// 		float currentFrame = glfwGetTime();
// 		deltaTime = currentFrame - lastFrame;
// 		lastFrame = currentFrame;
// 		// Specify the color of the background
// 		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
// 		// Clean the back buffer and assign the new color to it
// 		glClear(GL_COLOR_BUFFER_BIT);
// 		// Tell OpenGL which Shader Program we want to use
// 		shaderProgram.Activate();

// 		camera.Inputs(window, 400*deltaTime);

// 		glfwSetScrollCallback(window, scroll_callback);
// 		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

// 		shaderProgram.setUniform(camera.Zoom, "zoom");
// 		shaderProgram.setUniform(camera.Position, "cameraPos");
// 		shaderProgram.setUniform(float(SCR_HEIGHT)/float(SCR_WIDTH), "x_scale");

// 		// We set the initialvalues again since tInit might have changed

		

// 		ImGui_ImplOpenGL3_NewFrame();
// 		ImGui_ImplGlfw_NewFrame();

// 		ImGui::NewFrame();
// 		// ImGUI window creation
// 		ImGui::Begin("Settings");
// 		// Text that appears in the window
// 		ImGui::Text("Don't forget to also check the commandline!");
// 		// Checkbox that appears in the window
// 		ImGui::Checkbox("Show Grid", &show_grid);
// 		//ImGui::Checkbox("Show Phasespace", &show_phasespace);
// 		ImGui::Checkbox("Snap to Grid", &snapToGrid);
// 		ImGui::Checkbox("Clear Polygon", &clearPolygon);
// 		//ImGui::Checkbox("Edit Polygon", &editPoly);
// 		//ImGui::Checkbox("Edit Ruler", &editRuler);
// 		//ImGui::Checkbox("Edit x0 and x1", &editInit);
// 		ImGui::RadioButton("Polygon", &billiard.mode, 0); ImGui::SameLine();
// 		//ImGui::RadioButton("Ruler", &mode, 1); ImGui::SameLine();
// 		for (int i = 0; i < billiard.trajectories.size(); i++)
// 		{
// 			ImGui::RadioButton(("change #" + std::to_string(i)).c_str(), &billiard.mode, i + 1);
// 		}
		

// 		billiard.updateCoords(mouse, window);

// 		if (billiard.polygonClosed()) {
// 			billiard.iterate(batch, nIter);
// 		}


// 		if (snapToGrid) billiard.snapToGrid();

// 		// Slider that appears in the window

// 		for (size_t i = 0; i < billiard.trajectories.size(); i++)
// 		{
// 			ImGui::InputScalarN("Starting point:", ImGuiDataType_Float, &billiard.trajectories[i].vertexData[0].x, 2, NULL, NULL, "%.6f");
// 			ImGui::InputScalarN("End point:", ImGuiDataType_Float, &billiard.trajectories[i].vertexData.back().x, 2, NULL, NULL, "%.6f");
// 		}
		
// 		ImGui::InputFloat2("new starting point:", init, "%.6f");

// 		ImGui::InputInt("Number of iterations", &nIter, 1);
// 		ImGui::InputInt("Batch", &batch, 1);
// 		for (int i = 0; i < billiard.polygon.directions.size(); i++)
// 		{
// 			ImGui::InputFloat2(("Vertex #" + std::to_string(i)).c_str(), &billiard.polygon.vertexData[i].x);
// 		}

// 		// "make regular Polygon" button
// 		ImGui::InputInt("Edges", &nRegular, 1);
// 		if (ImGui::Button("Make regular")) {
// 			billiard.makeRegularNPoly(nRegular);
// 		}

// 		// Fancy color editor that appears in the window
// 		ImGui::ColorEdit3("Grid color", &grid.color.x);
// 		grid.Draw(shaderProgram);

// 		xAxis.Draw(shaderProgram);
// 		yAxis.Draw(shaderProgram);

// 		ImGui::ColorEdit3("Polygon color", &polygon.color.x);
// 		billiard.drawPolygon(shaderProgram);
// 		billiard.drawTrajectories(shaderProgram);


// 		// Framerate info
// 		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

// 		ImGui::Text("Mouse position: (%.5f, %.5f)", mouse.x, mouse.y);
// 		ImGui::Text("Width, height: (%.0f, %.0f)", float(SCR_WIDTH), float(SCR_HEIGHT));
// 		ImGui::Checkbox("Show Style Editor", &showStyleEditor);
// 		if (showStyleEditor) {
// 			ImGui::ShowStyleEditor();
// 		}

// 		// Ends of imgui
// 		ImGui::End();
  
// 		// Renders the ImGUI elements
// 		ImGui::Render();
// 		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

// 		if (clearPolygon) {
// 			billiard.clearPolygon();
// 			clearPolygon = false;
// 		}

// 		/*
// 		if (polygon.vertexChange || length(y - traj.vertexData[0]) < eps || traj.vertexChange) {
// 			traj.Clear();
// 			traj.vertexData.push_back(x);
// 			polygon.vertexChange = false;
// 			traj.vertexChange = false;
// 			count = 0;
// 		}*/

// 		// Swap the back buffer with the front buffer
// 		glfwSwapBuffers(window);
// 		// Take care of all GLFW events
// 		glfwPollEvents();
// 	}

// 	shaderProgram.Delete();
// 	// Delete window before ending the program
// 	glfwDestroyWindow(window);
// 	// Terminate GLFW before ending the program
// 	glfwTerminate();
// 	return 0;
// }

// void onInitialization() {
// 	glViewport(0, 0, SCR_WIDTH,  SCR_HEIGHT);
// 	glLineWidth(2.0f); // width of lines in pixels
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
// 	glEnable(GL_LINE_SMOOTH);
// 	glEnable(GL_POLYGON_SMOOTH);
// 	float gridsize = 1.0f;
// 	float nGridEdge = 100.0f;

// 	for (float i = -nGridEdge; i < nGridEdge; i = i + 2) {
// 		grid.vertexData.push_back(vec2(i * gridsize, nGridEdge));
// 		grid.vertexData.push_back(vec2(i * gridsize, -nGridEdge));
// 		grid.vertexData.push_back(vec2((i + 1) * gridsize, -nGridEdge));
// 		grid.vertexData.push_back(vec2((i + 1) * gridsize, nGridEdge));
// 	}
// 	for (float i = nGridEdge; i > -nGridEdge; i = i - 2) {
// 		grid.vertexData.push_back(vec2(nGridEdge, i * gridsize));
// 		grid.vertexData.push_back(vec2(-nGridEdge, i * gridsize));
// 		grid.vertexData.push_back(vec2(-nGridEdge, (i + 1) * gridsize));
// 		grid.vertexData.push_back(vec2(nGridEdge, (i + 1) * gridsize));
// 	}
// 	grid.color = vec3(0.3f, 0.3f, 0.3f);
// 	grid.Create();


// 	xAxis.set(vec2(-100.0, 0.0), vec2(100.0, 0.0));
// 	xAxis.color		= vec3(1.0, 1.0, 1.0);

// 	yAxis.set(vec2(0.0, -100.0), vec2(0.0, 100.0));
// 	yAxis.color		= vec3(1.0, 1.0, 1.0);

// 	ruler.set(vec2(1, 1), vec2(1, 1) + vec2(-2, 2));
// 	//ruler.lineWidth = 3;
// 	ruler.color = vec3(1, 1, 0);


// 	traj.vertexData.push_back(vec2(2.2, 3.0));
// 	p1.vertexData.push_back(vec2());
// 	p2.vertexData.push_back(vec2());
// 	p3.vertexData.push_back(vec2());

// 	polygon.color = vec3(0.0,1.0,1.0);
// 	polygon.lineWidth = 2;
// 	polygon.AddVertex(vec2(0.0, 0.0));
// 	polygon.AddVertex(vec2(5.0, 0.0));
// 	polygon.AddVertex(vec2(0.0, 5.0));

// 	traj.color = vec3(0.0, 1.0, 0.0);
// 	p1.color = vec3(1.0, 0.0, 0.0);
// 	p2.color = vec3(0.0, 0.0, 1.0);
// 	p3.color = vec3(0.5, 0.5, 0.5);

// 	traj2 = Trajectory(vec2_d(5, 5), &polygon, vec3(1.0, 0.0, 0.0));

// 	traj.size = 3.0f;
// 	// Adding objects to the scene
// 	ruler.Create();
// 	xAxis.Create();
// 	yAxis.Create();
// 	polygon.Create();
// 	traj.Create();
// 	traj2.Create();
// 	p1.Create();
// 	p2.Create();
// 	p3.Create();

// 	printf("\nUsage: \n");
// 	printf("Key 'w':     Camera pan +y \n");
// 	printf("Key 'a':     Camera pan -x\n");
// 	printf("Key 's':     Camera pan -y\n");
// 	printf("Key 'd':     Camera pan +x\n");
// 	printf("Scroll up:   Camera zoom in\n");
// 	printf("Scroll down: Camera zoom out\n");

// }

// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
// {
// 	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
// 		camera.Zoom *= 1.0f + yoffset * 0.1;
// 	}
// 	else {
// 		camera.Zoom *= 1.0f + yoffset * 0.02;
// 	}
// }

// void editScene(GLFWwindow* window, vec2 mouse, int code){

// 		switch (code)
// 		{
// 		case 0:
// 			polygon.onMouse(window, mouse);
			
// 			break;
// 		case 1:

// 			break;
// 		case 2:

// 			break;
// 		}
// }


// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
// 	// Apple somehow halves the resolution
// 	#ifdef __APPLE__
// 	SCR_WIDTH = width/2;
// 	SCR_HEIGHT = height/2;
// 	#else
// 	SCR_WIDTH = width;
// 	SCR_HEIGHT = height;
// 	#endif
//     glViewport(0, 0, width, height);
// }