// Standard Includes
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>		//GLEW before GLFW include

//GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "../include/Shader.h"
#include "../include/Camera.h"
#include "../include/Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other
#include <SOIL/SOIL.h>


GLuint screenWidth = 1024;
GLuint screenHeight = 800;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// _________Camera _________
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
// _________________________

int main(){

    glfwInit();
    //OPEN GL VERSION - MAX:3 and MIN:3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    //USE CORE PROFILE
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //WINDOWS NOT RESIZABLE BY USERS
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"Walking",nullptr,nullptr);
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
    glViewport(0,0,screenWidth,screenHeight);

    glEnable(GL_DEPTH_TEST);

    //Register key callback
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);


    Shader cubeShader("../shaders/cube.vs", "../shaders/cube.frag");
    Shader floorShader("../shaders/floor.vs", "../shaders/floor.frag");

    GLfloat floor_vertices[] = {
        //  VERTEX         , NORMAL
        -1.0f, -0.3f, -1.0f, 0, 1.0f, 0,
        -1.0f, -0.3f, 1.0f,  0, 1.0f, 0,
         1.0f, -0.3f, 1.0f,  0, 1.0f, 0,

         1.0f, -0.3f, 1.0f,  0, 1.0f,0,
         1.0f, -0.3f, -1.0f, 0, 1.0f,0,
        -1.0f, -0.3f, -1.0f, 0, 1.0f,0,
    };
    GLuint floorVBO, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind VAO


    // checks at the start of each loop iteration if GLFW
    // has been instructed to close
    while(!glfwWindowShouldClose(window)){
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //checks if any event are triggered
        glfwPollEvents();
        Do_Movement();

        glClearColor(0.05f, 0.05f, 0.05f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        //------ Draw Floor -------
        floorShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(floorShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(floorShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 modelFloor;
        GLfloat dim = 5;
        modelFloor = glm::scale(modelFloor, glm::vec3(dim, dim, dim));
        glUniformMatrix4fv(glGetUniformLocation(floorShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelFloor));
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        //_______________________________________

        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    //EXIT LOOOP
    glfwTerminate();
    return 0;
}

//KEY Event on Escape
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true,
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if( key >= 0 && key < 1024){
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
//        lastY = ypos;
        firstMouse = false;
    }
    GLfloat xoffset = xpos - lastX;
//    GLfloat yoffset = lastY - ypos;
    GLfloat yoffset=0;

    lastX = xpos;
//    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


