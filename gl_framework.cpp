#include "gl_framework.hpp"

namespace glFramework
{
    void initGL(void)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black
        glClearDepth(1.0);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST); 
        glShadeModel(GL_SMOOTH); // Enable smooth shading
    }
  
    void error_callback(int error, const char* description)
    {
        std::cerr << description << std::endl;
    }
  
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height); // Resize viewport to fit window size
    }
  
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE); // Close the window on ESC
    }
};




