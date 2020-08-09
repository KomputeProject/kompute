#include "gl.hpp"

#if defined(WIN32)
#include <mutex>

typedef PROC(APIENTRYP PFNWGLGETPROCADDRESS)(LPCSTR);
PFNWGLGETPROCADDRESS glad_wglGetProcAddress;
#define wglGetProcAddress glad_wglGetProcAddress

static void* getGlProcessAddress(const char* namez) {
    static HMODULE glModule = nullptr;
    if (!glModule) {
        glModule = LoadLibraryW(L"opengl32.dll");
        glad_wglGetProcAddress = (PFNWGLGETPROCADDRESS)GetProcAddress(glModule, "wglGetProcAddress");
    }

    auto result = wglGetProcAddress(namez);
    if (!result) {
        result = GetProcAddress(glModule, namez);
    }
    if (!result) {
        OutputDebugStringA(namez);
        OutputDebugStringA("\n");
    }
    return (void*)result;
}

void gl::init() {
    static std::once_flag once;
    std::call_once(once, [] { gladLoadGL(); });
}

GLuint gl::loadShader(const std::string& shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    int sizes = (int)shaderSource.size();
    const GLchar* strings = shaderSource.c_str();
    glShaderSource(shader, 1, &strings, &sizes);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        std::string strError;
        strError.insert(strError.end(), errorLog.begin(), errorLog.end());

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(shader);  // Don't leak the shader.
        throw std::runtime_error("Shader compiled failed");
    }
    return shader;
}

GLuint gl::buildProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    GLuint program = glCreateProgram();
    GLuint vs = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fs = loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    // fixme error checking
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void gl::report() {
    std::cout << glGetString(GL_VENDOR) << std::endl;
    std::cout << glGetString(GL_RENDERER) << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    GLint n;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    if (n > 0) {
        GLint i;
        for (i = 0; i < n; i++) {
            std::cout << "\t" << glGetStringi(GL_EXTENSIONS, i) << std::endl;
        }
    }
}

static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    if (GL_DEBUG_SEVERITY_NOTIFICATION == severity) {
        return;
    }
    // FIXME For high severity errors, force a sync to the log, since we might crash
    // before the log file was flushed otherwise.  Performance hit here
    std::cout << "OpenGL: " << message;
}

void gl::setupDebugLogging() {
    glDebugMessageCallback(debugMessageCallback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
#endif
