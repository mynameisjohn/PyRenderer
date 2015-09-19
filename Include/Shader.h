#pragma once

#include "GL_Includes.h"

// You should make copying / moving safe
#include <string>
#include <map>
#include <memory>

// Useful for looking up shader variables and handles
using HandleMap = std::map < std::string, GLint >;

class Shader
{
	// Private initializer
	int CompileAndLink();
public:
    // Constructors (default, source, disk), destructor (move!)
	Shader();
	Shader(std::string v, std::string f);
	Shader(std::string v, std::string f, std::string d);
	
	~Shader();

	void PrintHandles();
    
    // Bound status
	bool Bind();
	bool Unbind();
	bool IsBound() const;
    
    // Logging functions
	int PrintLog_V() const;
	int PrintLog_F() const;
	int PrintSrc_V() const;
	int PrintSrc_F() const;
	int PrintLog_P() const;
    
    // Public Accessors
	GLint GetHandle(const std::string& idx);
	GLint operator[](const std::string& idx);
    
    // Why not?
	inline HandleMap getHandleMap() { return m_Handles; }
private:
    // Bound status, program/shaders, source, handles
	bool m_bIsBound;
	GLuint m_Program;
	GLuint m_hVertShader;
	GLuint m_hFragShader;
	std::string m_VertShaderSrc, m_FragShaderSrc;
	HandleMap m_Handles;
    
    // Public scoped bind class
    // binds shader for as long as it lives
public:
    class ScopedBind {
        friend class Shader;
    protected:
        Shader& m_Shader;
        ScopedBind(Shader& s) : m_Shader(s) { m_Shader.Bind(); }
    public:
        ~ScopedBind(){ m_Shader.Unbind(); }
    };
    inline ScopedBind ScopeBind(){ return ScopedBind(*this); }
};

