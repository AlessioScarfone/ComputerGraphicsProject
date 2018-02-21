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
bool detectCollision(glm::vec3 object, float rag);

// _________Camera _________
Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
// _________________________

// _________ Light _________
glm::vec3 lightPos(-1.0f, 3.5f, 2.0f);
glm::vec3 lightDir(-2.0f, -10.0f, -3.0f);
//__________________________

int cubeNumber=3;
int sphereNumber=3;
glm::vec3 objectPosition[]={
    glm::vec3(5.0f, 0.5f, -0.5f),
    glm::vec3(-2.0f, 0.5f, 0.0f),
    glm::vec3(1.0f, 0.5f, 1.0f),
    //sphere
    glm::vec3(-1.0f, 0.0f, -3.0f),
    glm::vec3(0.0f, 2.0f, -0.5f),
    glm::vec3(-5.0f, 0.7f, -0.5f)
};

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


    Shader textureShader("../shaders/texture.vs", "../shaders/texture.frag");
    Shader sphereShader("../shaders/sphere.vs", "../shaders/sphere.frag");
    Shader lampShader("../shaders/lamp.vs", "../shaders/lamp.frag");

    GLfloat floor_vertices[] = {
        //  VERTEX         , Texture Coord, NORMAL
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f,   0.0,1.0,0.0,
        -1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0,1.0,0.0,
         1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   0.0,1.0,0.0,

         1.0f, 0.0f, 1.0f,  1.0f, 0.0f,    0.0,1.0,0.0,
         1.0f, 0.0f, -1.0f, 1.0f, 1.0f,    0.0,1.0,0.0,
        -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,    0.0,1.0,0.0,
    };

      GLfloat cube_vertices[] = {
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
      };


    // ----------- Set up vertex data (and buffer(s)) and attribute pointers ---> FLOOR
    GLuint floorVBO, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);
     // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    // ---------- Set up vertex data (and buffer(s)) and attribute pointers ---> Cube
    GLuint cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    // ---------------- LOAD TEXTURE ---------------
    // Load and create a floor texture --> floor
    GLuint textureFloor;
    glGenTextures(1, &textureFloor);
    glBindTexture(GL_TEXTURE_2D, textureFloor); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("../textures/floor_texture.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

    //Model Load
    char pathSphere[] = "../model/sphere.obj";
    Model sphereModel(pathSphere);

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
        // Bind Texture
        glBindTexture(GL_TEXTURE_2D, textureFloor);

        textureShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(textureShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(textureShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(textureShader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(textureShader.Program, "light.position"),lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(textureShader.Program, "light.direction"), lightDir.x, lightDir.y, lightDir.z);
        glUniform1f(glGetUniformLocation(textureShader.Program, "light.cutOff"), glm::cos(glm::radians(45.5f)));
        glUniform1f(glGetUniformLocation(textureShader.Program, "light.outerCutOff"), glm::cos(glm::radians(50.5f)));
        glUniform3f(glGetUniformLocation(textureShader.Program, "light.ambient"),   0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(textureShader.Program, "light.diffuse"),   0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(textureShader.Program, "light.specular"),  1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(textureShader.Program, "light.constant"),  1.0f);
        glUniform1f(glGetUniformLocation(textureShader.Program, "light.linear"),    0.09);
        glUniform1f(glGetUniformLocation(textureShader.Program, "light.quadratic"), 0.032);
        glUniform1f(glGetUniformLocation(textureShader.Program, "material.shininess"), 32.0f);

        glm::mat4 modelFloor;
        GLfloat dim = 10;
        modelFloor = glm::scale(modelFloor, glm::vec3(dim, dim, dim));
        glUniformMatrix4fv(glGetUniformLocation(textureShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelFloor));
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        //_________Draw Cube _____________
        // Bind Texture
        //NOTA: use same texture of the floor

        glUniformMatrix4fv(glGetUniformLocation(textureShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(textureShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        for(int i=0;i<cubeNumber;i++){
            glm::mat4 modelCube;
            //TODO: generate random position on the floor
            modelCube =  glm::translate(modelCube, objectPosition[i]);
            glUniformMatrix4fv(glGetUniformLocation(textureShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelCube));
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        //---------Draw Sphere -------------
        sphereShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(sphereShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(sphereShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(sphereShader.Program, "viewPos"),camera.Position.x, camera.Position.y, camera.Position.z);
        //sphere color
        glUniform3f(glGetUniformLocation(sphereShader.Program, "sphereColor"), 1.0f, 0.0f, 0.0f);
        //setting light of the sphere
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.direction"), lightDir.x, lightDir.y, lightDir.z);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.cutOff"),glm::cos(glm::radians(40.0f)));
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.outerCutOff"), glm::cos(glm::radians(45.0f)));
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.ambient"),   0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.diffuse"),   0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.specular"),  1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.constant"),  1.0f);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.linear"),    0.09);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.quadratic"), 0.032);

        glUniform3f(glGetUniformLocation(sphereShader.Program, "material.ambient"),   0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "material.diffuse"),   0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "material.specular"),  1.0f, 1.0f, 1.0f); // Specular doesn't have full effect on this object's material
        glUniform1f(glGetUniformLocation(sphereShader.Program, "material.shininess"), 32.0f);
        for(int i=cubeNumber;i<cubeNumber+sphereNumber;i++){
            glm::mat4 modelSphere;
            modelSphere = glm::translate(modelSphere, objectPosition[i]);
            GLfloat scaleSphere = 0.7;
            modelSphere = glm::scale(modelSphere, glm::vec3(scaleSphere, scaleSphere, scaleSphere));
            glUniformMatrix4fv(glGetUniformLocation(sphereShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));
            sphereModel.Draw(sphereShader);
        }

        // DRAW LAMP
        lampShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glm::mat4 modelLamp;
        modelLamp = glm::translate(modelLamp, lightPos);
        GLfloat scaleLamp = 0.1;
        modelLamp = glm::scale(modelLamp, glm::vec3(scaleLamp, scaleLamp, scaleLamp));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelLamp));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

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

void checkCollision(Camera_Movement back_move,GLfloat deltaTime){
    bool collision = false;
    for(int i = 0; i < cubeNumber; i++){
        if(detectCollision(objectPosition[i],0.9f)){
            collision = true;
            break;
        }
    }
    if(!collision){
        for(int i = cubeNumber; i < cubeNumber+sphereNumber; i++){
            if(detectCollision(objectPosition[i],1.2f)){
                collision = true;
                break;
            }
        }
    }
    if(collision)
        camera.ProcessKeyboard(back_move, deltaTime);
}

void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W]){
        camera.ProcessKeyboard(FORWARD, deltaTime);
        checkCollision(BACKWARD, deltaTime);
    }
    if(keys[GLFW_KEY_S]){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        checkCollision(FORWARD, deltaTime);
    }
    if(keys[GLFW_KEY_A]){
        camera.ProcessKeyboard(LEFT, deltaTime);
        checkCollision(RIGHT, deltaTime);
    }
    if(keys[GLFW_KEY_D]){
        camera.ProcessKeyboard(RIGHT, deltaTime);
        checkCollision(LEFT, deltaTime);
    }
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


bool detectCollision(glm::vec3 object, float rag){
    //distance between camera and the object
    float distance = sqrt(pow(camera.Position.x - object.x, 2) + pow(camera.Position.y - object.y, 2) + pow(camera.Position.z - object.z, 2));
    if(distance <= rag)
        return true;
    return false;
}

