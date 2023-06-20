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






Poly	grid;
Line	xAxis;
Line	yAxis;
Line	ruler;

vec2_d mouse;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void updateMousePosCoord(GLFWwindow* window, vec2& mouse);

void onInitialization();

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main();

void editScene(GLFWwindow* window, vec2_d mouse, int code, FourthBilliard& billiard);

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
	glViewport(0, 0, SCR_WIDTH * 2, SCR_HEIGHT * 2);
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

	int nRegular = 3; // variable to define how many edges the regular Polygon should have
	bool drawscreen = true;
	bool show_grid = 1;
	bool snapToGrid = false;
	bool snapToRuler = false;
	bool clearPolygon = false;
	bool centerPolygon = false;
	bool showStyleEditor = false;
	bool startDynamicalSystem = true;

	// Style variable 
	//double rawMouseX, rawMouseY;

	//---------------------------------------- Billiard controls ----------------------------------------------//
	int batch = 100;
	int nIter = 100000;
	FourthBilliard billiard;
	vec2 newInit = vec2();


	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	float backgroundColor[4] = { 0.07f, 0.13f, 0.17f, 1.0f };
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
		camera.Inputs(window, 400 * deltaTime);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		shaderProgram.setUniform(camera.Zoom, "zoom");
		shaderProgram.setUniform(camera.Position, "cameraPos");
		shaderProgram.setUniform(float(SCR_HEIGHT) / float(SCR_WIDTH), "x_scale");

		//---------------------------------------- Billiard ----------------------------------------------------//

		billiard.iterate(batch, nIter);

		//---------------------------------------- Drawing Scene ----------------------------------------------//

		if (snapToGrid) billiard.snapToGrid();
		if (snapToRuler) mouse = projectOntoLine(mouse, ruler);
		//if (clearPolygon) {
		//	billiard.clearPolygon();	// TODO, right now there is some Bug here that stops computation of trajectory. Need to track close Polygon in order to stop calculation and resume it
		//	clearPolygon = false;
		//}
		// Sadly for visibility this had to move up here, before the GUI
		grid.Draw(shaderProgram);
		ruler.Draw(shaderProgram);
		xAxis.Draw(shaderProgram);
		yAxis.Draw(shaderProgram);
		billiard.drawPolygon(shaderProgram);
		billiard.drawTrajectories(shaderProgram);

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
			ImGui::Checkbox("Snap to grid", &snapToGrid); ImGui::SameLine();
			ImGui::Checkbox("Snap to ruler", &snapToRuler);
			//ImGui::Checkbox("Clear polygon0", &clearPolygon);  // TODO DONE, consider making this a button instead of a checkbox
			if (ImGui::Button("Clear polygon")) {
				billiard.clearPolygon();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close polygon")) {
				billiard.polygon.ClosePolygon();
			}

			ImGui::PushItemWidth(80);
			ImGui::InputInt("Edges of Polygon", &nRegular, 1);
			ImGui::SameLine();
			if (ImGui::Button("Make polygon regular")) {
				billiard.makeRegularNPoly(nRegular);											/// make regular in billiards //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(140);
			ImGui::InputInt("Number of iterations", &nIter, 1);
			ImGui::InputInt("Batch", &batch, 1);
			ImGui::PopItemWidth();
			ImGui::Text("Drag and drop whole objects (right-mouse or  two-finger press for Mac):");
			ImGui::RadioButton("Ruler", &billiard.mode, -1);
			ImGui::RadioButton("Polygon", &billiard.mode, 0);									/////// mode inside billiards ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			for (int i = 0; i < billiard.trajectories.size(); i++)
			{
				// TODO maybe move these Buttons to the initial values of the trajectories, Otherwise if there are many trajectories, the Buttons "leave" the ImGUI Window
				
			}
			ImGui::Text("Drag and drop vertices of objects (right-mouse or two-finger press for Mac):");

			// Framerate info
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::Text("Mouse position: (%.5f, %.5f)", mouse.x, mouse.y);
			//ImGui::Text("Camera position: (%.5f, %.5f)", camera.Position.x, camera.Position.y);

			ImGui::Text("Width, height: (%.0f, %.0f)", float(SCR_WIDTH), float(SCR_HEIGHT));


			ImGui::Text("New Trajectory at:");
			ImGui::PushItemWidth(140);
			ImGui::SameLine();
			ImGui::InputScalarN(" ## starting point", ImGuiDataType_Float, &newInit.x, 2, NULL, NULL, "%.6f");
			ImGui::PopItemWidth();
			//////////// DONE //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// very big bug! FIXED!!! Problem was non unique names of the buttons
			// all InputScalarN change get the same number when one is changed! I don't understand why? even changing to InputFloat2 does not change it
			// also they are very hard to click. Consider rendering this last FIXED
			ImGui::SameLine();
			if (ImGui::Button("Add trajectory")) {
				billiard.addTrajectory(newInit, vec3(1., 0., 1.)); // TODO consider moving new trajectory part to top of trajectories to make it easier to click many times
			}
			
			// Slider that appears in the window
			for (size_t i = 0; i < billiard.trajectories.size(); i++)
			{
				ImGui::PushItemWidth(140); // TODO consider using this for all point fields to make it look better
				ImGui::Text("Starting point:");
				ImGui::SameLine();
				ImGui::InputScalarN((" ##" + std::to_string(i)).c_str(), ImGuiDataType_Float, &billiard.trajectories[i].vertexData[0].x, 2, NULL, NULL, "%.6f");
				ImGui::PopItemWidth();
				//  DONE consider using button here "apply changes" like this. That way, no tracking of changes of first iteration neccessary and now jumping of trajectory while getting input
				ImGui::SameLine();
				if ( ImGui::Button(("Apply changes##" + std::to_string(i)).c_str()) )  { // Buttons need unique names! but anything after ## wont be shown
					billiard.resetTrajectory(i);
				}
				
				ImGui::SameLine();
				if (ImGui::Button(("Remove trajectory##" + std::to_string(i)).c_str())) { // Buttons need unique names! but anything after ## wont be shown
					billiard.removeTrajectory(i);
				}
				ImGui::SameLine();
				ImGui::RadioButton(("change #" + std::to_string(i)).c_str(), &billiard.mode, i + 1);
			}
			
			ImGui::Text("Polygon:");	// TODO needs "apply changes" like behaviour as well
			for (int i = 0; i < billiard.polygon.directions_d.size(); i++)
			{
				ImGui::PushItemWidth(140);
				ImGui::InputFloat2(("Vertex #" + std::to_string(i)).c_str(), &billiard.polygon.vertexData[i].x);
				ImGui::PopItemWidth();
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

			ImGui::ColorEdit3("Polygon", &billiard.polygon.color.x);
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



		editScene(window, mouse, billiard.mode, billiard); // TODO consider moving this inside the big if And also just write the code there. No need for extra function

		// All the edits happen using either the left (ImGui) or the right (all dragdrop functions) or the ENTER-key. 
		// So, since this changes the scene, we simly reset every time this happens.
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			// billiard.reset(); TODO
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
	float gridsize = 1.0f;
	float nGridEdge = 100.0f;

	for (float i = -nGridEdge + 1; i < nGridEdge; i = i + 2) {
		grid.vertexData.push_back(vec2(i * gridsize, nGridEdge));
		grid.vertexData.push_back(vec2(i * gridsize, -nGridEdge));
		grid.vertexData.push_back(vec2((i + 1) * gridsize, -nGridEdge));
		grid.vertexData.push_back(vec2((i + 1) * gridsize, nGridEdge));
	}
	for (float i = nGridEdge - 1; i > -nGridEdge - 1; i = i - 2) {
		grid.vertexData.push_back(vec2(nGridEdge, i * gridsize));
		grid.vertexData.push_back(vec2(-nGridEdge, i * gridsize));
		grid.vertexData.push_back(vec2(-nGridEdge, (i + 1) * gridsize));
		grid.vertexData.push_back(vec2(nGridEdge, (i + 1) * gridsize));
	}
	grid.color = vec3(0.3f, 0.3f, 0.3f);
	grid.Create();

	xAxis.set(vec2(-100.0, 0.0), vec2(100.0, 0.0));
	xAxis.color = vec3(1.0, 1.0, 1.0);

	yAxis.set(vec2(0.0, -100.0), vec2(0.0, 100.0));
	yAxis.color = vec3(1.0, 1.0, 1.0);

	ruler.set(vec2(10, 10), vec2(10, 10) + vec2(-2, 2));
	ruler.lineWidth = 30;
	ruler.color = vec3(0.7, 0.7, 0.7);


	// Adding all the lines of the scene
	ruler.Create();
	xAxis.Create();
	yAxis.Create();
	// billiard object creates polygon and trajectories in the default constructor and in AddTrajectory respectively


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


void editScene(GLFWwindow* window, vec2_d mouse, int code, FourthBilliard& billiard) {
	// Note: it is important that all the edits of the polygons or the initial values have
	// to happen using the billiard class since only there we update the scene_has_changed variable
	
	// TODO add "thickness" variable to trajectories.

	if (code == -1) {
		ruler.lineWidth = 5.0;
		ruler.dragDropTo(window, mouse);
	}
	else {
		billiard.updateCoords(mouse, window);
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Apple somehow halves the resolution
#ifdef __APPLE__
	SCR_WIDTH = width / 2;
	SCR_HEIGHT = height / 2;
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