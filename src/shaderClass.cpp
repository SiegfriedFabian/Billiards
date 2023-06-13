#include"shaderClass.h"

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{

	// Convert the shader source strings into character arrays
	const char* vertexSource = vertexFile;
	const char* fragmentSource = fragmentFile;

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);
	// Checks if Shader compiled succesfully
	compileErrors(vertexShader, "VERTEX");

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);
	// Checks if Shader compiled succesfully
	compileErrors(fragmentShader, "FRAGMENT");

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);
	// Checks if Shaders linked succesfully
	compileErrors(ID, "PROGRAM");

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// Constructor that build the Shader Program from 2 different shaders
//Shader::Shader(const char* vertexFile, const char* fragmentFile)
//{
//	// Read vertexFile and fragmentFile and store the strings
//	std::string vertexCode = get_file_contents(vertexFile);
//	std::string fragmentCode = get_file_contents(fragmentFile);
//
//	// Convert the shader source strings into character arrays
//	const char* vertexSource = vertexCode.c_str();
//	const char* fragmentSource = fragmentCode.c_str();
//
//	// Create Vertex Shader Object and get its reference
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	// Attach Vertex Shader source to the Vertex Shader Object
//	glShaderSource(vertexShader, 1, &vertexSource, NULL);
//	// Compile the Vertex Shader into machine code
//	glCompileShader(vertexShader);
//	// Checks if Shader compiled succesfully
//	compileErrors(vertexShader, "VERTEX");
//
//	// Create Fragment Shader Object and get its reference
//	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	// Attach Fragment Shader source to the Fragment Shader Object
//	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
//	// Compile the Vertex Shader into machine code
//	glCompileShader(fragmentShader);
//	// Checks if Shader compiled succesfully
//	compileErrors(fragmentShader, "FRAGMENT");
//
//	// Create Shader Program Object and get its reference
//	ID = glCreateProgram();
//	// Attach the Vertex and Fragment Shaders to the Shader Program
//	glAttachShader(ID, vertexShader);
//	glAttachShader(ID, fragmentShader);
//	// Wrap-up/Link all the shaders together into the Shader Program
//	glLinkProgram(ID);
//	// Checks if Shaders linked succesfully
//	compileErrors(ID, "PROGRAM");
//
//	// Delete the now useless Vertex and Fragment Shader objects
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//}

// Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// Checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}

int Shader::getLocation(const std::string& name) {	// get the address of a GPU uniform variable
	GLuint location = glGetUniformLocation(ID, name.c_str());
	if (location < 0) {
		printf("uniform %s cannot be set\n", name.c_str());
		std::cout << location << std::endl;
	}
	return location;
}

void Shader::setUniform(int i, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1i(location, i);
}

void Shader::setUniform(float f, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform1f(location, f);
}

void Shader::setUniform(const vec2& v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform2fv(location, 1, &v.x);
}

void Shader::setUniform(const vec3& v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform3fv(location, 1, &v.x);
}

void Shader::setUniform(const vec4& v, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniform4fv(location, 1, &v.x);
}

void Shader::setUniform(const mat4& mat, const std::string& name) {
	int location = getLocation(name);
	if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, mat);
}