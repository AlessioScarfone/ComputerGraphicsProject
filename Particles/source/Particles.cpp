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


GLuint screenWidth = 1400;
GLuint screenHeight = 900;

int sphere_number = 2000;
float range = 0.29;
GLfloat maxDimension = 0.95f;
bool moveON = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
float getRandomInRange(float min,float max);
void checkposition(glm::vec3 &spherePos, GLfloat move);

// _________Camera _________
Camera camera(glm::vec3(0.0f, 0.0f, 3.2f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
// _________________________

// _________ Light _________
glm::vec3 lightPos(0.0f, 0.0f, 1.5f);
glm::vec3 target(0.0f, 0.0f, -10.0f);
glm::vec3 lightDir =  target - lightPos   ;

bool rotationON=false;
GLfloat rotationY=0.0f;
GLfloat rotationX=0.0f;
GLfloat lamp_self_rotate=0.0f;
//__________________________


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

    //create starting position of spheres
    glm::vec3 spherePos[sphere_number];
    for(int i=0; i < sphere_number;i++){
        float randX= getRandomInRange(-range,range);
        float randY= getRandomInRange(-range,range);
        float randZ= getRandomInRange(-range,range);
//        std::cout<<i<<": "<<randX<<","<<randY<<","<<randZ<<endl;
        spherePos[i] = glm::vec3(randX,randY,randZ);
    }


    Shader smallCubeShader("../shaders/smallCube.vs", "../shaders/smallCube.frag");
    Shader bigCubeShader("../shaders/bigCube.vs", "../shaders/bigCube.frag");
    Shader sphereShader("../shaders/sphere.vs", "../shaders/sphere.frag");
    Shader spotlightShader("../shaders/spotlight.vs", "../shaders/spotlight.frag");

    // Set up vertex data
    //vertex for small cube
    GLfloat vertices[] = {
        // CUBE VERTEX     , NORMAL
        //BOTTOM FACE
        -0.3f, -0.3f, -0.3f, 0, -0.3f,0, //0
        -0.3f, -0.3f, 0.3f,  0, -0.3f,0,//1
         0.3f, -0.3f, 0.3f,  0, -0.3f,0,//2

        0.3f, -0.3f, 0.3f,   0, -0.3f,0,//2
        0.3f, -0.3f, -0.3f,  0, -0.3f,0,//3
        -0.3f, -0.3f, -0.3f, 0, -0.3f,0,//0

        //LEFT FACE
        -0.3f, -0.3f, 0.3f,  -0.3,0,0,//1
        -0.3f, -0.3f, -0.3f, -0.3,0,0,//0
        -0.3f, 0.3f, -0.3f,  -0.3,0,0,//4

        -0.3f, 0.3f, -0.3f,  -0.3,0,0,//4
        -0.3f, 0.3f, 0.3f,   -0.3,0,0,//5
        -0.3f, -0.3f, 0.3f,  -0.3,0,0,//1

        //RIGHT FACE
        0.3f, -0.3f, 0.3f,   0.3,0,0,//2
        0.3f, 0.3f, -0.3f,   0.3,0,0,//6
        0.3f, -0.3f, -0.3f,  0.3,0,0,//3

        0.3f, 0.3f, -0.3f,   0.3,0,0,//6
        0.3f, 0.3f, 0.3f,    0.3,0,0,//7
        0.3f, -0.3f, 0.3f,   0.3,0,0,//2

        //TOP FACE
        0.3f, 0.3f, -0.3f,   0,0.3,0,//6
        -0.3f, 0.3f, -0.3f,  0,0.3,0,//4
        -0.3f, 0.3f, 0.3f,   0,0.3,0,//5

        -0.3f, 0.3f, 0.3f,   0,0.3,0,//5
        0.3f, 0.3f, 0.3f,    0,0.3,0,//7
        0.3f, 0.3f, -0.3f,   0,0.3,0,//6

        //FRONT FACE
        -0.3f, -0.3f, 0.3f,  0,0,0.3,//1
        -0.3f, 0.3f, 0.3f,   0,0,0.3,//5
        0.3f, 0.3f, 0.3f,    0,0,0.3,//7

        0.3f, 0.3f, 0.3f,    0,0,0.3,//7
        0.3f, -0.3f, 0.3f,   0,0,0.3,//2
        -0.3f, -0.3f, 0.3f,  0,0,0.3,//1

        //FAR FACE
        -0.3f, -0.3f, -0.3f, 0,0,-0.3,//0
        -0.3f, 0.3f, -0.3f,  0,0,-0.3,//4
        0.3f, 0.3f, -0.3f,   0,0,-0.3,//6

        0.3f, 0.3f, -0.3f,   0,0,-0.3,//6
        0.3f, -0.3f, -0.3f,  0,0,-0.3,//3
        -0.3f, -0.3f, -0.3f, 0,0,-0.3,//0
    };
    //vertex for big cube
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

    //Model Load
    char pathSphere[] = "../model/sphere.obj";
    Model sphereModel(pathSphere);

    char pathSpotlight[] = "../model/spotlight.obj";
    Model spotlightModel(pathSpotlight);

    //set buffers and attribute pointers
    //SMALL CUBE
    GLuint smallCubeVBO, smallCubeVAO;
    glGenVertexArrays(1, &smallCubeVAO);
    glGenBuffers(1, &smallCubeVBO);

    glBindVertexArray(smallCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, smallCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
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

        // ----- Draw smallCube ---------------------------------------
        smallCubeShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(smallCubeShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(smallCubeShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 modelSmallCube;
        modelSmallCube = glm::translate(modelSmallCube, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(smallCubeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSmallCube));

        glBindVertexArray(smallCubeVAO);
        //active trasparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDisable(GL_BLEND);
        glDepthMask(1);
        glBindVertexArray(0);

        // ------- Draw big cube (use lines) ---------------------------
        bigCubeShader.Use();

        glUniformMatrix4fv(glGetUniformLocation(bigCubeShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(bigCubeShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 modelBigCube;
        float dim = 1;
        modelBigCube = glm::scale(modelBigCube, glm::vec3(dim, dim, dim));
        glUniformMatrix4fv(glGetUniformLocation(bigCubeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelBigCube));

        glBindVertexArray(bigCubeVAO);
        glDrawArrays(GL_LINES, 0, 36);
        glBindVertexArray(0);

        // ------- Draw SpotLight -------------------------------------
        spotlightShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(spotlightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(spotlightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glm::mat4 modelSpotlight;
        dim = 0.05f;
        modelSpotlight = glm::translate(modelSpotlight, glm::vec3(lightPos.x,lightPos.y,lightPos.z));
        if(rotationON){
            modelSpotlight = glm::rotate(modelSpotlight, rotationY, glm::vec3(0.0, 1.0, 0.0));
//            modelSpotlight = glm::rotate(modelSpotlight, rotationX, glm::vec3(1.0, 0.0, 0.0));
            lightPos = glm::vec4(lightPos,1.0f) * modelSpotlight;
            target = glm::vec4(target,1.0f) * modelSpotlight;
            rotationY=0;
            rotationX=0;
            rotationON=false;
        }
        modelSpotlight = glm::rotate(modelSpotlight, lamp_self_rotate, glm::vec3(0.0, 1.0, 0.0));
        modelSpotlight =glm::scale(modelSpotlight, glm::vec3(dim, dim, dim));
        glUniformMatrix4fv(glGetUniformLocation(spotlightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSpotlight));

        spotlightModel.Draw(sphereShader);

        // ------- Draw Sphere ---------------------------------------
        sphereShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(sphereShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(sphereShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(sphereShader.Program, "viewPos"),camera.Position.x, camera.Position.y, camera.Position.z);
        //sphere color
        glUniform3f(glGetUniformLocation(sphereShader.Program, "sphereColor"), 0.4f, 0.8f, 0.9f);
        //setting light of the sphere
        lightDir = target - lightPos ;
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.direction"), lightDir.x, lightDir.y, lightDir.z);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.cutOff"),glm::cos(glm::radians(25.0f)));
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.outerCutOff"), glm::cos(glm::radians(30.0f)));
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.ambient"),   0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.diffuse"),   0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "light.specular"),  1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.constant"),  1.0f);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.linear"),    0.09);
        glUniform1f(glGetUniformLocation(sphereShader.Program, "light.quadratic"), 0.032);

        glUniform3f(glGetUniformLocation(sphereShader.Program, "material.ambient"),   0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "material.diffuse"),   0.7f, 0.7f, 0.7f);
        glUniform3f(glGetUniformLocation(sphereShader.Program, "material.specular"),  1.0f, 1.0f, 1.0f); // Specular doesn't have full effect on this object's material
        glUniform1f(glGetUniformLocation(sphereShader.Program, "material.shininess"), 32.0f);

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        for(int i=0; i < sphere_number;i++){
            glm::mat4 modelSphere;
            dim = 0.01;
            modelSphere = glm::translate(modelSphere, spherePos[i]);
            modelSphere =glm::scale(modelSphere, glm::vec3(dim, dim, dim));
            glUniformMatrix4fv(glGetUniformLocation(sphereShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelSphere));
            sphereModel.Draw(sphereShader);

            //move spheres
            if(moveON){
                GLfloat particle_speed=0.01f;
                glm::vec3 move(getRandomInRange(-particle_speed, particle_speed), getRandomInRange(-particle_speed, particle_speed), getRandomInRange(-particle_speed, particle_speed));
                spherePos[i] += move;
                checkposition(spherePos[i],particle_speed);
            }
        }

        // Swap the screen buffer
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &smallCubeVAO);
    glDeleteBuffers(1, &smallCubeVBO);
    glDeleteVertexArrays(1, &bigCubeVAO);
    glDeleteBuffers(1, &bigCubeVBO);

    //EXIT LOOOP
    glfwTerminate();
    return 0;
}

void checkposition(glm::vec3 &spherePos, GLfloat move){
    move=move*2;
    if(spherePos.x >= maxDimension)
        spherePos.x -= move;
    else if(spherePos.x <= -maxDimension)
        spherePos.x += move;

    if(spherePos.y >= maxDimension)
        spherePos.y -= move;
    else if(spherePos.y <= -maxDimension)
        spherePos.y += move;

    if(spherePos.z >= maxDimension)
        spherePos.z -= move;
    else if(spherePos.z <= -maxDimension)
        spherePos.z += move;

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

//void debugLightPrint(){
//            std::cout<<"LIGHT DIR:"<<"("<<lightDir.x<<","<<lightDir.y<<","<<lightDir.z<<")"<<endl;
//            std::cout<<"LIGHT POS:"<<"("<<lightPos.x<<","<<lightPos.y<<","<<lightPos.z<<")"<<endl;
//            std::cout<<"TARGET:"<<"("<<target.x<<","<<target.y<<","<<target.z<<")"<<endl;
//            std::cout<<"_________________________________________________________________________"<<endl;
//}

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
    //---------Active Particles--------
    if(keys[GLFW_KEY_X])
            moveON=true;
    if(keys[GLFW_KEY_Z])
            moveON=false;
    //------Light controll ------------
    //rotation up and down
//    if(keys[GLFW_KEY_UP]){
//        rotationON = true;
//        rotationX+=deltaTime;
//    }
//    if(keys[GLFW_KEY_DOWN]){
//        rotationON = true;
//        rotationX-=deltaTime;
//    }

    if(keys[GLFW_KEY_UP]){
        lightPos.y += 0.01f;
        target.y += 0.01f;
    }
    if(keys[GLFW_KEY_DOWN]){
        lightPos.y -= 0.01f;
        target.y -= 0.01f;
    }
    //rotation around the cube
    if(keys[GLFW_KEY_LEFT]){
        rotationON = true;
        rotationY+=deltaTime;
        lamp_self_rotate-=deltaTime;

    }
    if(keys[GLFW_KEY_RIGHT]){
        rotationON = true;
        rotationY-=deltaTime;
        lamp_self_rotate+=deltaTime;
    }

//    if(keys[GLFW_KEY_LEFT_SHIFT]){
//        lightPos.z += 0.01f;
//        target.z += 0.01f;
//    }
//    if(keys[GLFW_KEY_LEFT_CONTROL]){
//        lightPos.z -= 0.01f;
//        target.z -= 0.01f;
//    }
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

float getRandomInRange(float min,float max){
        float random = ((float) rand()/ (float) RAND_MAX);
        float diff = max - min;
        return (random * diff) + (min);
}

