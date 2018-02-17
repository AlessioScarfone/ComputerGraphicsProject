#define GLEW_STATIC
#include <GL/glew.h>		//GLEW before GLFW include
#include <GLFW/glfw3.h>

#include <iostream>

//KEY Event on Escape
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true,
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(){

    glfwInit();
    //OPEN GL VERSION - MAX:3 and MIN:3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    //USE CORE PROFILE
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //WINDOWS NOT RESIZABLE BY USERS
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800,600,"Walking",nullptr,nullptr);
    if(window == nullptr){
        std::cout<<"FAIL TO CREATE GLFW WINDOW" <<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK){
        std::cout<<"FAIL TO INIZIALIZA GLEW"<<std::endl;
        return -1;
    }

    //Location: x,y  - Size: width, height
    glViewport(0,0,800,600);
    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    //glViewport(0, 0, width, height);

    //Register key callback
    glfwSetKeyCallback(window, key_callback);

    // checks at the start of each loop iteration if GLFW
    // has been instructed to close
    while(!glfwWindowShouldClose(window)){
        //checks if any event are triggered
        glfwPollEvents();
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        //SENZA QUESTO "LAMPEGGIA"
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    //EXIT LOOOP
    glfwTerminate();
    return 0;
}

