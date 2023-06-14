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


Poly	grid;
Line	xAxis;
Line	yAxis;
Line	ruler;

vec2_d mouse;


int nRegular0 = 0;
int nRegular1 = 0;


int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
inline Camera camera(SCR_WIDTH, SCR_HEIGHT, vec2(0.0f, 0.0f));

// settings

static int code = 4;

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

int main();

void editScene(GLFWwindow *window, vec2_d mouse, int code);

void HelpMarker(const char* desc);

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
 
	bool drawscreen = true;
	bool show_grid = 1;
	bool snapToGrid = false;
	bool snapP0ToGrid = false;
	bool snapP1ToGrid = false;
	bool snapToRuler = false;
	bool clearPolygon0 = false;
	bool clearPolygon1 = false;
	bool centerPolygon0 = false;
	bool copyFirstOntoSecond = false;
	bool copySecondOntoFirst = false;
	bool editPoly = true;
	bool editRuler = false;
	bool editInit = false;
	bool showStyleEditor = false;
	bool startDynamicalSystem = true;

	// Style variable 
	double rawMouseX, rawMouseY;

	//---------------------------------------- Billiard controls ----------------------------------------------//
	int batch = 10;
	int nIter = 100000;


	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	float backgroundColor[4] = {0.07f, 0.13f, 0.17f, 1.0f};
	//---------------------------------------------------------------------------------------------------------//
	//------------------------------------ END control Variables ----------------------------------------------//
	//---------------------------------------------------------------------------------------------------------//

	while ((!glfwWindowShouldClose(window)) && (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE))
	{
		mouse = mousePosCoord(window, SCR_WIDTH, SCR_HEIGHT, camera);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Specify the color of the background
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
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

		billiard.iterateSymplecticBilliards(batch, nIter);
		
		//---------------------------------------- Drawing Scene ----------------------------------------------//
		
		if(snapToGrid) billiard.snapToGrid();
		if(snapP0ToGrid) billiard.snapToGrid();
		if(snapP1ToGrid) billiard.snapToGrid();
		if(snapToRuler) mouse = projectOntoLine(mouse, ruler);
		if(copyFirstOntoSecond) billiard.copyPolygon0OntoPolygon1();
		if(copySecondOntoFirst) billiard.copyPolygon1OntoPolygon0();
		if(clearPolygon0) {
			billiard.ClearPolygon0();
			clearPolygon0 = false;
		}
		if(clearPolygon1){
			billiard.ClearPolygon1();
			clearPolygon1 = false;
		}
		// if(centerPolygon0) billiard.polygon0.center();



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
			// Text that appears in the window
			// Checkbox that appears in the window
			ImGui::Checkbox("Show Grid", &show_grid);
			ImGui::Checkbox("Snap to grid", &snapToGrid);ImGui::SameLine();
			ImGui::Checkbox("Snap to ruler", &snapToRuler);
			ImGui::Checkbox("Clear polygon0", &clearPolygon0);ImGui::SameLine();
			ImGui::Checkbox("Clear polygon1", &clearPolygon1);
			// ImGui::Checkbox("Center polygon0", &centerPolygon0);
			
			ImGui::Checkbox("Copy first polygon onto second", &copyFirstOntoSecond);
			ImGui::Checkbox("Copy second polygon onto first", &copySecondOntoFirst);
			
			if (ImGui::Button("Start Symplectic Billiards")) {
				billiard.reset();
			}
			ImGui::InputInt("Edges of Polygon 0", &nRegular0, 1);
			if (ImGui::Button("Make polygon0 regular")) {
				billiard.polygon0.makeRegularNPoly(nRegular0);
			}

			ImGui::InputInt("Edges of Polygon 1", &nRegular1, 1);
			if (ImGui::Button("Make polygon1 regular")) {
				billiard.polygon1.makeRegularNPoly(nRegular1);
			}

			ImGui::InputInt("Spikes of Polygon 0", &nRegular0, 1);
			if (ImGui::Button("Make polygon0 starshaped")) {
				billiard.polygon0.makeRegularNStar(nRegular0);
			}

			ImGui::InputInt("Spikes of Polygon 1", &nRegular1, 1);
			if (ImGui::Button("Make polygon1 starshaped")) {
				billiard.polygon1.makeRegularNStar(nRegular1);
			}
			ImGui::Text("Drag and drop whole objects (right-mouse or  two-finger press for Mac):");
			ImGui::RadioButton("Translate polygons", &code, 0);ImGui::SameLine();
			// ImGui::RadioButton("Translate polygon1", &code, 1);
			ImGui::RadioButton("x0 and x1", &code, 2);

			ImGui::Text("Drag and drop vertices of objects (right-mouse or two-finger press for Mac):");
			ImGui::RadioButton("Polygons", &code, 4); ImGui::SameLine();
			ImGui::RadioButton("polygon0", &code, 5); ImGui::SameLine();
			ImGui::RadioButton("polygon1", &code, 6); 
			ImGui::RadioButton("Ruler", &code, 7);
			
			glfwGetCursorPos(window, &rawMouseX, &rawMouseY);
			ImGui::Text("Raw Mouse position: (%.5f, %.5f)", rawMouseX, rawMouseY);
			// Framerate info
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::Text("Mouse position: (%.5f, %.5f)", mouse.x, mouse.y);
			ImGui::Text("Camera position: (%.5f, %.5f)", camera.Position.x, camera.Position.y);
			
			ImGui::Text("Width, height: (%.0f, %.0f)", float(SCR_WIDTH), float(SCR_HEIGHT));

			// Slider that appears in the window
			// ImGui::InputFloat("x0", &tInit.x, 0.001f, polygon.directions.size(), "%.6f");
			// ImGui::InputFloat("x1", &tInit.y, 0.001f, polygon.directions.size(), "%.6f");
			ImGui::InputDouble("t0", &billiard.t0, 0.001f, billiard.polygon0.directions_d.size());
			// ImGui::SliderFloat("t1", &billiard.t1, 0.001f, billiard.polygon1.directions_d.size());
			ImGui::InputInt("Number of iterations", &nIter, 1);
			ImGui::InputInt("Batch", &batch, 1);
			for (int i = 0; i < billiard.polygon1.directions_d.size(); i++)
			{
				ImGui::InputFloat2(("Vertex #"+std::to_string(i)).c_str(), &billiard.polygon1.vertexData[i].x);
			}


			// End of imgui
			ImGui::End();

			////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////// Styling ////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////
		
			ImGui::Begin("Style");
			// Fancy color editor that appears in the window
			ImGui::ColorEdit4("Background", &backgroundColor[0]);
			ImGui::ColorEdit3("Grid", &grid.color.x);
			ImGui::ColorEdit3("x-Axis", &xAxis.color.x);
			ImGui::ColorEdit3("y-Axis", &yAxis.color.x);
			
			ImGui::ColorEdit3("Polygon0", &billiard.polygon0Color.x);
			ImGui::ColorEdit3("Polygon1", &billiard.polygon1Color.x);
			ImGui::SameLine(); HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

			ImGui::Checkbox("Show Style Editor", &showStyleEditor);

			if (showStyleEditor) {
				ImGui::ShowStyleEditor();
				
			}
			ImGui::End();
			// Renders the ImGUI elements
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}


		// ------------------------- Reset trajectories if scene changed ---------------------------------------
		// All the edits happen using either the left (ImGui) or the right (all dragdrop functions) or the ENTER-key. 
		// So, since this changes the scene, we simly reset every time this happens.


		grid.Draw(shaderProgram);
		ruler.Draw(shaderProgram);
		xAxis.Draw(shaderProgram);
		yAxis.Draw(shaderProgram);
		billiard.drawPolygons(shaderProgram);
		billiard.drawTrajectories(shaderProgram);
		billiard.drawInitialValues(shaderProgram);
		editScene(window, mouse, code);

		if(	glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS || 
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || 
			glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {billiard.reset();}


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
	float gridsize = 1.0f;
	float nGridEdge = 100.0f;

	for (float i = -nGridEdge+1; i < nGridEdge; i = i + 2) {
		grid.vertexData.push_back(vec2(i * gridsize, nGridEdge));
		grid.vertexData.push_back(vec2(i * gridsize, -nGridEdge));
		grid.vertexData.push_back(vec2((i + 1) * gridsize, -nGridEdge));
		grid.vertexData.push_back(vec2((i + 1) * gridsize, nGridEdge));
	}
	for (float i = nGridEdge -1 ; i > -nGridEdge-1; i = i - 2) {
		grid.vertexData.push_back(vec2(nGridEdge, i * gridsize));
		grid.vertexData.push_back(vec2(-nGridEdge, i * gridsize));
		grid.vertexData.push_back(vec2(-nGridEdge, (i + 1) * gridsize));
		grid.vertexData.push_back(vec2(nGridEdge, (i + 1) * gridsize));
	}
	grid.color = vec3(0.3f, 0.3f, 0.3f);
	grid.Create();

	xAxis.set(vec2(-100.0, 0.0), vec2(100.0, 0.0));
	xAxis.color		= vec3(1.0, 1.0, 1.0);

	yAxis.set(vec2(0.0, -100.0), vec2(0.0, 100.0));
	yAxis.color		= vec3(1.0, 1.0, 1.0);

	ruler.set(vec2(10, 10), vec2(10, 10) + vec2(-2, 2));
	ruler.lineWidth = 30;
	ruler.color = vec3(0.7, 0.7, 0.7);

	billiard.polygon0Color = vec3(216.0/255.0,27.0/255.0,96.0/255.0);
	billiard.polygon1Color = vec3(30.0/255.0,136.0/255.0,229.0/255.0);
	
	billiard.x0.color = vec3(0.8,0.,0.);
	billiard.x1.color = vec3(0.0,0.8,0.);

	// Adding all the lines of the scene
	ruler.Create();
	xAxis.Create();
	yAxis.Create();
	billiard.Create();


	printf("\nUsage: \n");
	printf("Key 'w':     Camera pan +y \n");
	printf("Key 'a':     Camera pan -x\n");
	printf("Key 's':     Camera pan -y\n");
	printf("Key 'd':     Camera pan +x\n");
	printf("Scroll up:   Camera zoom in\n");
	printf("Scroll down: Camera zoom out\n");

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

void editScene(GLFWwindow* window, vec2_d mouse, int code){
// Note: it is important that all the edits of the polygons or the initial values have
// to happen using the billiard class since only there we update the scene_has_changed variable
		switch (code) 
		{
		case 0:
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				billiard.centerPolygonsAt(mouse);
			}
			break;

		// case 1:
		// 	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) 
		// 	{
		// 		billiard.translatepolygon1(mouseDelta);
		// 	}
		// 	break;
		case 2:
			billiard.x0.size = 20.0;
			billiard.x1.size = 20.0;
			ruler.lineWidth = 5.0;
			billiard.polygon0.lineWidth = 1.0;
			billiard.polygon1.lineWidth = 1.0;
			billiard.editVertexPositionX0X1(window, mouse);
			break;
		// case 3:
		// 	billiard.x0.size = 10.0;
		// 	billiard.x1.size = 20.0;
		// 	ruler.lineWidth = 5.0;
		// 	billiard.polygon0.lineWidth = 1.0;
		// 	billiard.polygon1.lineWidth = 1.0;
		// 	billiard.editVertexPositionX1(window, mouse);
		// 	break;
		case 4:
			billiard.x0.size = 10.0;
			billiard.x1.size = 10.0;
			ruler.lineWidth = 1.0;
			billiard.polygon0.lineWidth = 5.0;
			billiard.polygon1.lineWidth = 1.0;
			billiard.editVertexPositionPolygons(window, mouse);
			break;
		case 5:
			billiard.x0.size = 10.0;
			billiard.x1.size = 10.0;
			ruler.lineWidth = 1.0;
			billiard.polygon0.lineWidth = 5.0;
			billiard.polygon1.lineWidth = 1.0;
			billiard.editVertexPositionpolygon0(window, mouse);			
			break;
		case 6:
			billiard.x0.size = 10.0;
			billiard.x1.size = 10.0;
			ruler.lineWidth = 1.0;
			billiard.polygon0.lineWidth = 1.0;
			billiard.polygon1.lineWidth = 5.0;
			billiard.editVertexPositionpolygon1(window, mouse);			
			break;
		case 7:
			billiard.x0.size = 10.0;
			billiard.x1.size = 20.0;
			ruler.lineWidth = 5.0;
			billiard.polygon0.lineWidth = 1.0;
			billiard.polygon1.lineWidth = 1.0;
			ruler.dragDropTo(window, mouse);
			break;
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

void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}