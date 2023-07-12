#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include"glad/glad.h"
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
//#include<cerrno>
#include"myVectors.h"

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

	int getLocation(const std::string& name);

	void setUniform(int i, const std::string& name);

	void setUniform(float f, const std::string& name);

	void setUniform(const vec2& v, const std::string& name);

	void setUniform(const vec3& v, const std::string& name);

	void setUniform(const vec4& v, const std::string& name);

	void setUniform(const mat4& mat, const std::string& name);
    
	// void setUniform(const std::vector<vec2> &array, const std::string &name);
	
	void setUniform(const vec2* array, const std::string &name, int N);

private:
    // Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);

	

	
};


#endif