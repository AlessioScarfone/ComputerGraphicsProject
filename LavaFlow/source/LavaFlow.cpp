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

#include "MyVertex.h"

using namespace std;

GLuint screenWidth = 1424;
GLuint screenHeight = 1000;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

GLfloat *readFile(char* path,bool checkNoValue);
GLfloat** createMatrix(GLfloat* input, int r, int c);
void createVertex(GLfloat** matrixVertex, GLfloat ** coordinates);
void sumLava(GLfloat** altitudeMatrix,GLfloat** lavaMatrix);
void createIndex(GLint* indices, vector<MyVertex>& verticesCoordinates);
glm::vec3 calculateNormal(GLfloat** matrix, int i, int j);
void computeNormal(vector<MyVertex>& verticesCoordinates,GLfloat** matrix);
void updateMaxAndMin(MyVertex v);
GLfloat* buildVBO(vector<MyVertex>& verticesCoordinates);
GLfloat normalizeNumber(GLfloat val);

void convertInMyVertex(vector<MyVertex>& verticesCoordinates,GLfloat** matrixVertex,GLfloat* temperatureCoordinates);

void destroyMatrix(GLfloat** matrix);

// _________Camera _________
Camera camera(glm::vec3(-0.5f, 1.0f, 2.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool enableMove =false;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float anglePitch = 0.0f;
float angleYao = 0.0f;

GLfloat maxVal = -100000;
GLfloat minVal = 100000;

GLfloat maxTemp = 0.0f;
GLfloat minTemp = 10000.0f;
// _________________________

// _________ Light _________
glm::vec3 lightpos(0.0f, 1.0f, 0.0f);
//__________________________

int rows = 0;
int cols = 0;
float cellSize = 0;
float novalue = 0;

int main(){

    glfwInit();
    //OPEN GL VERSION - MAX:3 and MIN:3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    //USE CORE PROFILE
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    //WINDOWS NOT RESIZABLE BY USERS
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"Lava Flow",nullptr,nullptr);
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
//    glfwSetScrollCallback(window, scroll_callback);

    // ----- END CONFIGURATION OPENGL AND WINDOW ----

//    GLfloat* altitudeCoordinates = readFile("../dataset/test/DEM_test.dat",true); //TEST
//    GLfloat* lavaCoordinates = readFile("../dataset/test/lava_test.dat",false);
//    GLfloat* temperatureCoordinates = readFile("../dataset/test/temperature_test.dat",false);

        GLfloat* altitudeCoordinates = readFile("../dataset/altitudes.dat",true);
        GLfloat* lavaCoordinates = readFile("../dataset/lava.dat",false);
        GLfloat* temperatureCoordinates = readFile("../dataset/temperature.dat",false);

    GLfloat** altitudeMatrix = createMatrix(altitudeCoordinates,rows,cols);
    GLfloat** lavaMatrix = createMatrix(lavaCoordinates,rows,cols);

    sumLava(altitudeMatrix,lavaMatrix);

     vector<MyVertex> verticesCoordinates;
     GLfloat** matrixVertex = new GLfloat*[rows+1];
     for (int i = 0; i < rows+1; ++i)
         matrixVertex[i] = new GLfloat[cols+1];

     createVertex(matrixVertex, altitudeMatrix);
     convertInMyVertex(verticesCoordinates,matrixVertex,temperatureCoordinates);

     cout<<"Vertex num:"<<verticesCoordinates.size()<<endl;
     cout<<"Max altitude:"<<maxVal<<endl;
     cout<<"Min altitude:"<<minVal<<endl;
     cout<<"Max Temp:"<<maxTemp<<endl;
     cout<<"Min Temp:"<<minTemp<<endl;

     int neededTriangles = (rows)*(cols)*2; //r*n square, each composed by 2 triangles
     cout<<"needed triangles:"<<neededTriangles<<endl;
     int indexNum = neededTriangles * 3; //r*c square,* 2 triangle , * 3 coord
     cout<<"index dim:"<<indexNum<<endl;
     GLint* indices = new GLint[indexNum];
     createIndex(indices,verticesCoordinates);

     computeNormal(verticesCoordinates,matrixVertex);

     Shader vulcan("../shaders/base.vs", "../shaders/base.frag");

     GLfloat* vertexData = buildVBO(verticesCoordinates);

     GLuint VAO,VBO,EBO;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * ((cols+1)*(rows+1)*11), vertexData, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*indexNum, indices, GL_STATIC_DRAW);
     // position
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);
     // normal
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
     glEnableVertexAttribArray(1);
     //color
     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
     glEnableVertexAttribArray(2);
     //texture
     glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
     glEnableVertexAttribArray(3);
     glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

     // Load textures
     GLuint diffuseMap, specularMap;
     glGenTextures(1, &diffuseMap);
     glGenTextures(1, &specularMap);
     int width, height;
     unsigned char* image;
     // Diffuse map
     image = SOIL_load_image("../textures/texture.png", &width, &height, 0, SOIL_LOAD_RGB);
     glBindTexture(GL_TEXTURE_2D, diffuseMap);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
     glGenerateMipmap(GL_TEXTURE_2D);
     //   SOIL_free_image_data(image);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
     // Specular map
     glBindTexture(GL_TEXTURE_2D, specularMap);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
     glGenerateMipmap(GL_TEXTURE_2D);
     SOIL_free_image_data(image);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
     glBindTexture(GL_TEXTURE_2D, 0);

     vulcan.Use();
     glUniform1i(glGetUniformLocation(vulcan.Program, "material.diffuse"),  0);
     glUniform1i(glGetUniformLocation(vulcan.Program, "material.specular"), 1);

     cout<<"..:: RENDER ::.."<<endl;

     while(!glfwWindowShouldClose(window)){
         // Set frame time
         GLfloat currentFrame = glfwGetTime();
         deltaTime = currentFrame - lastFrame;
         lastFrame = currentFrame;
         //checks if any event are triggered
         glfwPollEvents();
         Do_Movement();

         glClearColor(0.05f, 0.05f, 0.05f, 0.4f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
         glm::mat4 view = camera.GetViewMatrix();

         glUniform3f(glGetUniformLocation(vulcan.Program, "light.position"), camera.Front.x, camera.Front.y, -camera.Front.z);
         glUniform3f(glGetUniformLocation(vulcan.Program, "light.ambient"),  0.5f, 0.5f, 0.5f);
         glUniform3f(glGetUniformLocation(vulcan.Program, "light.diffuse"),  0.8f, 0.8f, 0.8f);

         glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
         glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
         glBindVertexArray(VAO);

         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, diffuseMap);
         glActiveTexture(GL_TEXTURE1);
         glBindTexture(GL_TEXTURE_2D, specularMap);

         glm::mat4 volcanModel;
         volcanModel = glm::rotate(volcanModel, glm::degrees(90.0f), glm::vec3(0.0, 1.0, 0.0));
         volcanModel = glm::rotate(volcanModel, glm::degrees(180.0f), glm::vec3(0.0, 0.0, 1.0));
         volcanModel = glm::rotate(volcanModel, anglePitch, glm::vec3(1.0f, 0.0f, 0.0f));
         volcanModel = glm::rotate(volcanModel, angleYao, glm::vec3(0.0f, 0.0f, 1.0f));
         volcanModel = glm::scale(volcanModel, glm::vec3(3.5f, 3.5f, 3.5f));
         glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "model"), 1, GL_FALSE, glm::value_ptr(volcanModel));
         glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
         glBindVertexArray(0);

         glfwSwapBuffers(window);
     }

    // Properly de-allocate all resources once they've outlived their purpose

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    destroyMatrix(altitudeMatrix);
    destroyMatrix(lavaMatrix);
    destroyMatrix(matrixVertex);
    delete vertexData;
    delete indices;
    delete altitudeCoordinates;
    delete temperatureCoordinates;
    delete lavaCoordinates;

    //EXIT LOOOP
    glfwTerminate();
    return 0;
}

void destroyMatrix(GLfloat** matrix){
    for(int i=0; i<rows;i++)
        delete matrix[i];
    delete matrix;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
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
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W]){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(keys[GLFW_KEY_S]){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(keys[GLFW_KEY_A]){
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(keys[GLFW_KEY_D]){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    // Camera rotation
    if(keys[GLFW_KEY_UP]){
      anglePitch += 0.01f;
    }
    if(keys[GLFW_KEY_DOWN]){
      anglePitch -= 0.01f;
    }
    if(keys[GLFW_KEY_RIGHT]){
      angleYao += 0.01f;
    }
    if(keys[GLFW_KEY_LEFT]){
      angleYao -= 0.01f;
    }
}

// ====================================================================

GLfloat** createMatrix(GLfloat* input, int r,int c){
    //create matrix structure
    GLfloat** matrix = new GLfloat*[r];
    for (int i = 0; i < r; ++i)
        matrix[i] = new GLfloat[c];
    //fill the matrix with data of input
    for(int i = 0; i<r ;i++){
        for(int j=0; j<c ;j++){
            matrix[i][j] = input[(i*c)+j];
        }
    }
    return matrix;
}

void sumLava(GLfloat** altitudeMatrix,GLfloat** lavaMatrix){
    for(int i = 0; i<rows ;i++){
        for(int j=0; j<cols ;j++){
            altitudeMatrix[i][j]+=lavaMatrix[i][j];
        }
    }
}

GLfloat normalizeNumber(GLfloat val){ //in range [0,1]
    GLfloat newVal = (val - minVal) / (maxVal - minVal);
    return newVal;
}

float checkNoValue(GLfloat** matrixVertex, int i, int j, int novalue){
    float height = 0;
    if(matrixVertex[i][j] != novalue){
        height = matrixVertex[i][j];
    } else {
        for (int z = i; z  < rows; z ++) {
            bool found = false;
            for (int k = j; k < cols; k ++) {
                if(matrixVertex[z][k] != novalue){
                    height = matrixVertex[z][k];
                    found = true;
                    break;
                }
            }
            if(found)
                break;
        }
    }
    return height;
}

void convertInMyVertex(vector<MyVertex>& verticesCoordinates,GLfloat** matrixVertex, GLfloat* temperatureCoordinates){
    int count=0;
    for(int i=0; i<rows+1; i++){
        for(int j=0; j<cols+1; j++){
            MyVertex v(i*cellSize,-matrixVertex[i][j],j*cellSize);
            v.setPos(count++);
            float temperature = temperatureCoordinates[(i*(cols)+j)];
            v.temperature=temperature;
            updateMaxAndMin(v);
            verticesCoordinates.push_back(v);

        }
    }

    for(int i = 0; i < verticesCoordinates.size(); i++){
//        verticesCoordinates[i].normalizeCoord(minVal,maxVal);
        verticesCoordinates[i].computeColor(maxTemp,minTemp);
    }
}

void createVertex(GLfloat** matrixVertex, GLfloat ** coordinates){
    for (int i = 0; i < rows+1; ++i){
            for (int j = 0; j < cols+1; j++){
                if (i == 0 && j == 0){
                        matrixVertex[i][j] = checkNoValue(coordinates, i ,j, novalue);
                } else if(i == 0) {
                    if(j<=(cols-1)){
                        float h1,h2;
                        h1 = checkNoValue(coordinates, i ,j-1, novalue);
                        h2 = checkNoValue(coordinates, i ,j, novalue);
                        matrixVertex[i][j] = (h1+h2)/2;
                    } else {
                        matrixVertex[i][j] = checkNoValue(coordinates, i ,j-1, novalue);
                    }
                } else if( (i == (rows))){
                    if(j == 0){
                        matrixVertex[i][j] = checkNoValue(coordinates, i-1 ,j, novalue);
                    } else if( j == cols){
                        matrixVertex[i][j] = checkNoValue(coordinates, i-1 ,j-1, novalue);
                    } else{
                        float h1,h2;
                        h1 = checkNoValue(coordinates, i-1 ,j-1, novalue);
                        h2 = checkNoValue(coordinates, i-1 ,j, novalue);
                        matrixVertex[i][j] = (h1+h2)/2;
                    }
                } else {
                    if(j == 0){
                        float h1,h2;
                        h1 = checkNoValue(coordinates, i-1 ,j, novalue);
                        h2 = checkNoValue(coordinates, i ,j, novalue);
                        matrixVertex[i][j] = (h1+h2)/2;
                    } else if( j == cols){
                        float h1,h2;
                        h1 = checkNoValue(coordinates, i-1 ,j-1, novalue);
                        h2 = checkNoValue(coordinates, i ,j-1, novalue);
                        matrixVertex[i][j] = (h1+h2)/2;

                    } else {
                        float h1,h2,h3,h4;
                        h1 = checkNoValue(coordinates, i ,j-1, novalue);
                        h2 = checkNoValue(coordinates, i ,j, novalue);
                        h3 = checkNoValue(coordinates, i-1 ,j, novalue);
                        h4 = checkNoValue(coordinates, i-1 ,j-1, novalue);
                        matrixVertex[i][j] = (h1+h2+h3+h4)/4;
                    }
                }
            }
          }
    }



void createIndex(GLint* indices,vector<MyVertex> &verticesCoordinates){
    int newcols=cols+1;
    int newrows=rows+1;
    int index = 0;
    int it = (newrows*newcols)-newcols;
    cout<<"iteration:"<<it<<endl;
    for(int i = 0; i < it; i++){
        if(i%newcols == cols){     //skip last col
//            cout<<i<<" skip"<<endl;
           continue;
        }
        indices[index] = i;
        indices[index+1] = i+1;
        indices[index+2] = i+newcols;

        indices[index+3] = indices[index+2];
        indices[index+4] = indices[index+2]+1;
        indices[index+5] = i+1;
        //        cout<<i<<":"<<indices[index]<<","<<indices[index+1]<<","<<indices[index+2]<<endl;
        //        cout<<i<<":"<<indices[index+3]<<","<<indices[index+4]<<","<<indices[index+5]<<endl;
        index= index + 6;
    }
//        std::cout<<"Indices num:"<<index<<endl;
}

glm::vec3 calculateNormal(GLfloat** matrix, int i, int j){
    glm::vec3 normals;
    glm::vec3 current;
    current.x = i; current.y = matrix[i][j]; current.z = j;
    if (i == 0 && j == 0){
        glm::vec3 norm1, norm2, norm3;
        norm1.x = i+1; norm1.y = matrix[i+1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j+1]; norm2.z = j+1;
        norm3.x = i+1; norm3.y = matrix[i+1][j+1]; norm3.z = j+1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3));

    } else if(i == 0 && j < (cols-1)) {
        glm::vec3 norm1,norm2, norm3, norm4;
        norm1.x = i+1; norm1.y = matrix[i+1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j+1]; norm2.z = j+1;
        norm3.x = i+1; norm3.y = matrix[i+1][j+1]; norm3.z = j+1;
        norm4.x = i; norm4.y = matrix[i][j-1]; norm4.z = j-1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3)) + (glm::cross(current, norm4));

    } else if(i == 0 && j == (cols-1)) {
        glm::vec3 norm1, norm2;
        norm1.x = i+1; norm1.y = matrix[i+1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j-1]; norm2.z = j-1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2));

    } else if(i == (rows-1) && j == 0){
        glm::vec3 norm1, norm2;
        norm1.x = i-1; norm1.y = matrix[i-1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j+1]; norm2.z = j+1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2));

    } else if(i == (rows-1) && j < (cols-1)) {
        glm::vec3 norm1, norm2, norm3, norm4;
        norm1.x = i-1; norm1.y = matrix[i-1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j+1]; norm2.z = j+1;
        norm3.x = i; norm3.y = matrix[i][j-1]; norm3.z = j-1;
        norm4.x = i-1; norm4.y = matrix[i-1][j-1]; norm4.z = j-1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3)) + (glm::cross(current, norm4));

    } else if(i == (rows-1) && j == (cols-1)) {
        glm::vec3 norm1, norm2, norm3;
        norm1.x = i-1; norm1.y = matrix[i-1][j]; norm1.z = j;
        norm2.x = i-1; norm2.y = matrix[i-1][j-1]; norm2.z = j-1;
        norm3.x = i; norm3.y = matrix[i][j-1]; norm3.z = j-1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3));

    } else if(i < (rows-1) && j == 0) {
        glm::vec3 norm1, norm2, norm3, norm4;
        norm1.x = i+1; norm1.y = matrix[i+1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j+1]; norm2.z = j+1;
        norm3.x = i+1; norm3.y = matrix[i+1][j+1]; norm3.z = j+1;
        norm4.x = i-1; norm4.y = matrix[i-1][j]; norm4.z = j;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3)) + + (glm::cross(current, norm4));

    } else if(i < (rows-1) && j == (cols-1)) {
        glm::vec3 norm1, norm2, norm3, norm4;
        norm1.x = i+1; norm1.y = matrix[i+1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j-1]; norm2.z = j-1;
        norm3.x = i-1; norm3.y = matrix[i-1][j-1]; norm3.z = j-1;
        norm4.x = i-1; norm4.y = matrix[i-1][j]; norm4.z = j;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3)) + + (glm::cross(current, norm4));

    } else {
        glm::vec3 norm1, norm2, norm3, norm4, norm5, norm6;
        norm1.x = i+1; norm1.y = matrix[i+1][j]; norm1.z = j;
        norm2.x = i; norm2.y = matrix[i][j-1]; norm2.z = j-1;
        norm3.x = i-1; norm3.y = matrix[i-1][j-1]; norm3.z = j-1;
        norm4.x = i-1; norm4.y = matrix[i-1][j]; norm4.z = j;
        norm5.x = i-1; norm5.y = matrix[i-1][j-1]; norm5.z = j-1;
        norm6.x = i+1; norm6.y = matrix[i+1][j+1]; norm6.z = j+1;
        normals = (glm::cross(current, norm1)) + (glm::cross(current, norm2)) + (glm::cross(current, norm3)) + (glm::cross(current, norm4)) + (glm::cross(current, norm5)) + (glm::cross(current, norm6));
    }
    return glm::normalize(normals);
}

void computeNormal(vector<MyVertex>& verticesCoordinates,GLfloat** matrix){
    cols++;
    rows++;
    for(int i=0;i<verticesCoordinates.size();i++){
        verticesCoordinates[i].normal = calculateNormal(matrix, verticesCoordinates[i].x()/cellSize , verticesCoordinates[i].z()/cellSize);
        verticesCoordinates[i].setTexCoord((verticesCoordinates[i].x()/cellSize)/(rows),(verticesCoordinates[i].z()/cellSize)/(cols));
        verticesCoordinates[i].normalizeCoord(minVal,maxVal);
    }
    cols--;
    rows--;
}


void updateMaxAndMin(MyVertex v){
    GLfloat* coord = v.getCoord();

    for(int i=0;i<3;i++){
        if( coord[i] > maxVal) {
            maxVal=coord[i];
        }
        if( coord[i] < minVal) {
            minVal=coord[i];
        }
    }
    if(v.temperature!=0) {
        if( v.temperature > maxTemp) {
            maxTemp=v.temperature;
        }
        if( v.temperature < minTemp) {
            minTemp=v.temperature;
        }
    }
}

GLfloat* buildVBO(vector<MyVertex>& verticesCoordinates){
    GLfloat* vertexData = new GLfloat[(rows+1)*(cols+1)*11]; //3 coord,3 norm, 3 color, 2 texture
    int index=0;
    for(int i=0;i<verticesCoordinates.size();i++){
         vector<GLfloat> vbol = verticesCoordinates[i].createVBOline();
         for(int i=0;i<vbol.size();i++){
             vertexData[index+i] = vbol[i];
         }
         index+=11;
    }
    return vertexData;
}

GLfloat *readFile(char* path,bool checkNoValue){
    GLfloat* coordinates = 0;
    ifstream file(path);
    string line;
    int index = 1;
    int pos = 0;
    char *line_copy;
    while(getline(file, line)){
//        std::cout<<line<<endl;
        line_copy = new char[line.size() + 1];
        std::copy(line.begin(), line.end(), line_copy);
        line_copy[line.size()] = '\0';
        char* tk;
        tk = strtok (line_copy," \n\r\t");
//        std::cout<<tk<<endl;
        while (tk != NULL)
        {
            if(index == 2){
                //atof: convert string to double
                cols = atof(tk);
            } else if(index == 4){
                rows = atof(tk);
            } else if(index == 10){
                cellSize = atof(tk);
            } else if(index == 12){
                coordinates = new GLfloat[rows*cols];
                float val = atof(tk);
                if(checkNoValue)
                    novalue = atof(tk);
            } else if(index > 12){
                coordinates[pos] = atof(tk);
                pos++;
            }
            index ++;
            tk = strtok (NULL, " \n\r\t");
        }
        delete tk;
        }
    file.close();
    return coordinates;
}
