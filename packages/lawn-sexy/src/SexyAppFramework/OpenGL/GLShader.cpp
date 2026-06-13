#if SEXY_USE_OPENGL

#ifdef PISTON_PATCH
#include "GLShader.h"
#else
#include "OpenGL/GLShader.h"
#include <fstream>
#include <filesystem>
#endif
#include <glm/gtc/type_ptr.hpp>

using namespace Sexy;

GLShader::~GLShader()
{
	if (mProgramID)
		glDeleteProgram(mProgramID);
}

bool GLShader::LoadFromSource(const std::string &vertexSrc, const std::string &fragmentSrc)
{
#ifdef PISTON_PATCH // smart destruction - avoid using smart pointers on callsites
	if (mProgramID) {
		glDeleteProgram(mProgramID);
        mProgramID = 0;
    }
    // a bunch of leaked resources if error happens, uhhhh
#endif
	GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
	GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

	if (!vertex_shader || !fragment_shader)
		return false;

	mProgramID = glCreateProgram();
	glAttachShader(mProgramID, vertex_shader);
	glAttachShader(mProgramID, fragment_shader);
	glLinkProgram(mProgramID);

	GLint success;
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
#ifdef PISTON_PATCH
		GLint logLen = 0;
		glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			std::string log(logLen, '\0');
			glGetProgramInfoLog(mProgramID, logLen, nullptr, log.data());
			printf("[GLRenderer] shader link error: %s\n", log.c_str());
		}
#endif
		return false;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return true;
}

void GLShader::Use() const
{
	glUseProgram(mProgramID);
}

GLuint GLShader::CompileShader(GLenum type, const std::string &source)
{
	GLuint shader = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
#ifdef PISTON_PATCH
	if (success != GL_TRUE)
	{
		GLint logLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			std::string log(logLen, '\0');
			glGetShaderInfoLog(shader, logLen, nullptr, log.data());
			printf("[GLRenderer] shader compile error: %s\n", log.c_str());
		}
		glDeleteShader(shader);
		return 0;
	}
#endif

	return shader;
}

GLuint GLShader::GetUniformLocation(const std::string &name) const
{
	GLuint pos = glGetUniformLocation(mProgramID, name.c_str());
	return pos;
}

void GLShader::SetUniform(const std::string &name, int value) const
{
	glUniform1i(GetUniformLocation(name), value);
}
void GLShader::SetUniform(const std::string &name, float value) const
{
	glUniform1f(GetUniformLocation(name), value);
}
void GLShader::SetUniform(const std::string &name, const glm::vec2 &value) const
{
	glUniform2f(GetUniformLocation(name), value.x, value.y);
}
void GLShader::SetUniform(const std::string &name, const glm::vec4 &value) const
{
	glUniform4f(GetUniformLocation(name), value.r, value.g, value.b, value.a);
}
void GLShader::SetUniform(const std::string &name, const glm::mat4 &value) const
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

#endif
