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


GLuint screenWidth = 800;
GLuint screenHeight = 600;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
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
    srand(time(NULL));
    glfwInit();
    //OPEN GL VERSION - MAX:3 and MIN:3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    //USE CORE PROFILE
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //WINDOWS NOT RESIZABLE BY USERS
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"Particles",nullptr,nullptr);
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


    Shader smallCube("../shaders/smallCube.vs", "../shaders/smallCube.frag");
    Shader bigCube("../shaders/bigCube.vs", "../shaders/bigCube.frag");


    // Set up vertex data (and buffer(s)) and attribute pointers
      GLfloat vertices[] = {
           //BOTTOM FACE
          -0.5f, -0.5f, -0.5f,  //0
          -0.5f, -0.5f, 0.5f,   //1
           0.5f, -0.5f, 0.5f,   //2

          0.5f, -0.5f, 0.5f,   //2
          0.5f, -0.5f, -0.5f,  //3
          -0.5f, -0.5f, -0.5f,  //0

          //LEFT FACE
          -0.5f, -0.5f, 0.5f,  //1
          -0.5f, -0.5f, -0.5f, //0
          -0.5f, 0.5f, -0.5f,  //4

          -0.5f, 0.5f, -0.5f,  //4
          -0.5f, 0.5f, 0.5f,   //5
          -0.5f, -0.5f, 0.5f,  //1

          //RIGHT FACE
          0.5f, -0.5f, 0.5f,    //2
          0.5f, 0.5f, -0.5f,    //6
          0.5f, -0.5f, -0.5f,   //3

          0.5f, 0.5f, -0.5f,    //6
          0.5f, 0.5f, 0.5f,     //7
          0.5f, -0.5f, 0.5f,    //2

          //TOP FACE
          0.5f, 0.5f, -0.5f,    //6
          -0.5f, 0.5f, -0.5f,  //4
          -0.5f, 0.5f, 0.5f,   //5

          -0.5f, 0.5f, 0.5f,   //5
          0.5f, 0.5f, 0.5f,     //7
          0.5f, 0.5f, -0.5f,    //6

          //FRONT FACE
          -0.5f, -0.5f, 0.5f,   //1
          -0.5f, 0.5f, 0.5f,   //5
          0.5f, 0.5f, 0.5f,   //7

          0.5f, 0.5f, 0.5f,   //7
          0.5f, -0.5f, 0.5f,   //2
          -0.5f, -0.5f, 0.5f,   //1

          //FAR FACE
          -0.5f, -0.5f, -0.5f,   //0
          -0.5f, 0.5f, -0.5f,   //4
          0.5f, 0.5f, -0.5f,   //6

          0.5f, 0.5f, -0.5f,   //6
          0.5f, -0.5f, -0.5f,   //3
          -0.5f, -0.5f, -0.5f,   //0
      };

      GLfloat lines[] = {
          -1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f,  1.0f,
          -1.0f,  1.0f,  1.0f,
          -1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f, -1.0f,
          -1.0f, -1.0f, -1.0f,
          -1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f,  1.0f,
          -1.0f,  1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,
          1.0f,  1.0f,  1.0f,
          -1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f,  1.0f, -1.0f

      };

      //SMALL CUBE
      GLuint smallCubeVBO, smallCubeVAO;
      glGenVertexArrays(1, &smallCubeVAO);
      glGenBuffers(1, &smallCubeVBO);

      glBindVertexArray(smallCubeVAO);

      glBindBuffer(GL_ARRAY_BUFFER, smallCubeVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);
      glBindVertexArray(0); // Unbind VAO

      //BIG CUBE
      GLuint bigCubeVBO, bigCubeVAO;
      glGenVertexArrays(1, &bigCubeVAO);
      glGenBuffers(1, &bigCubeVBO);

      glBindVertexArray(bigCubeVAO);

      glBindBuffer(GL_ARRAY_BUFFER, bigCubeVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);
      glBindVertexArray(0); // Unbind VAO



    // GAME LOOP
    while(!glfwWindowShouldClose(window)){

        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //checks if any event are triggered
        glfwPollEvents();
        Do_Movement();

        //Render
        //Clear the colorbuffer
        glClearColor(0.05f, 0.05f, 0.05f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        //Draw smallCube
        smallCube.Use();
        glUniformMatrix4fv(glGetUniformLocation(smallCube.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(smallCube.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 modelSmallCube;
        float dim = 0.6;
        modelSmallCube =glm::scale(modelSmallCube, glm::vec3(dim, dim, dim));
        glUniformMatrix4fv(glGetUniformLocation(smallCube.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSmallCube));

        glBindVertexArray(smallCubeVAO);
        //active trasparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDisable(GL_BLEND);
        glDepthMask(1);
        glBindVertexArray(0);

        //Draw big cube (use lines)
        bigCube.Use();

        glUniformMatrix4fv(glGetUniformLocation(bigCube.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(bigCube.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 modelBigCube;
        modelBigCube = glm::translate(modelBigCube, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(bigCube.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelBigCube));

        glBindVertexArray(bigCubeVAO);
        glDrawArrays(GL_LINES, 0, 36);
        glBindVertexArray(0);


        // Swap the screen buffer
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &smallCubeVAO);
    glDeleteBuffers(1, &smallCubeVBO);

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
        lastY = ypos;
        firstMouse = false;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

