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


GLuint screenWidth = 1024;
GLuint screenHeight = 800;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

GLfloat *readFile(char* path);
GLfloat** createMatrix(GLfloat* input, int r, int c);
void createVertex( vector<MyVertex>& vertexVector, GLfloat** altitudeMatrix,GLfloat** temperatureMatrix);
void sumLava(GLfloat** altitudeMatrix,GLfloat** lavaMatrix);
void createIndex(GLint* indices, vector<MyVertex>& verticesCoordinates);
void addConnectedVertex(vector<MyVertex>& verticesCoordinates,int index1,int index2,int index3);
void computeNormal(vector<MyVertex>& verticesCoordinates);
void updateMaxAndMin(MyVertex v);
GLfloat* buildVBO(vector<MyVertex>& verticesCoordinates);
GLfloat normalizeNumber(GLfloat val);

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

GLfloat maxVal=-100000;
GLfloat minVal=100000;

GLfloat maxTemp=0.0f;
GLfloat minTemp=0.0f;
// _________________________

// _________ Light _________
glm::vec3 lightpos(-4.0f, 25.0f, 7.0f);
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

//    GLfloat* altitudeCoordinates = readFile("../dataset/test/DEM_test.dat"); //TEST
    GLfloat* altitudeCoordinates = readFile("../dataset/altitudes.dat");
    GLfloat** altitudeMatrix = createMatrix(altitudeCoordinates,rows,cols);
    GLfloat* lavaCoordinates = readFile("../dataset/lava.dat");
    GLfloat** lavaMatrix = createMatrix(lavaCoordinates,rows,cols);
    GLfloat* temperatureCoordinates = readFile("../dataset/temperature.dat");
    GLfloat** temperatureMatrix = createMatrix(temperatureCoordinates,rows,cols);

    delete altitudeCoordinates;
    delete lavaCoordinates;

    sumLava(altitudeMatrix,lavaMatrix);

    //print matrix
//    for(int i = 0; i<rows ;i++){
//        for(int j=0; j<cols ;j++){
//            std::cout<<altitudeMatrix[i][j]<<",";
//        }
//        std::cout<<endl;
//    }

     vector<MyVertex> verticesCoordinates;
     createVertex(verticesCoordinates, altitudeMatrix,temperatureMatrix);
     cout<<"Vertex num:"<<verticesCoordinates.size()<<endl;
//     for (auto v:verticesCoordinates)
//         v.print();
     cout<<"Max altitude:"<<maxVal<<endl;
     cout<<"Min altitude:"<<minVal<<endl;
     cout<<"Max Temp:"<<maxTemp<<endl;
     cout<<"Min Temp:"<<minTemp<<endl;

     int neededTriangles = (rows-1)*(cols-1)* 2;
     int indexNum = neededTriangles * 3; //r-1*c-1 square,* 2 triangle , * 3 coord
//     std::cout<<"index dim:"<<indexNum<<endl;
     GLint* indices = new GLint[indexNum];
     createIndex(indices,verticesCoordinates);
//     for(int i = 0; i<indexNum ;i+=3){
//         std::cout<<i/3<<" - "<<indices[i]
//                  <<" , "<<indices[i+1]
//                  <<" , "<<indices[i+2]<<endl;
//     }

     computeNormal(verticesCoordinates);

//     for(int i =0;i<verticesCoordinates.size();i++){
//            if(verticesCoordinates[i].temperature!=0.0f)
//                verticesCoordinates[i].printComplete();
//     }

     Shader vulcan("../shaders/base.vs", "../shaders/base.frag");
     Shader lamp("../shaders/light.vs", "../shaders/light.frag");

     char pathSphere[] = "../model/sphere.obj";
     Model modelSphere(pathSphere);

     GLfloat* vertexData = buildVBO(verticesCoordinates);
//     for(int i=0;i<rows*cols*9;i+=9){
//         for(int j=0;j<9;j++){
//             if(j==0) cout<<"V:";
//             if(j==3) cout<<"|\t N:";
//             if(j==6) cout<<"|\t C:";
//            cout<<vertexData[i+j]<<"  ";
//         }
//         cout<<endl;
//     }


     GLuint VAO,VBO,EBO;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (cols*rows*9), vertexData, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*neededTriangles*2, indices, GL_STATIC_DRAW);
     // position
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);
     // normal
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
     glEnableVertexAttribArray(1);
     //color
     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
     glEnableVertexAttribArray(2);
     glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


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

         vulcan.Use();
         glUniform3f(glGetUniformLocation(vulcan.Program, "light.position"), camera.Front.x, camera.Front.y, -camera.Front.z);
         glUniform3f(glGetUniformLocation(vulcan.Program, "light.ambient"),  0.3f, 0.3f, 0.3f);
         glUniform3f(glGetUniformLocation(vulcan.Program, "light.diffuse"),  0.5f, 0.5f, 0.5f);

         glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
         glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
         glBindVertexArray(VAO);
         glm::mat4 volcanModel;
         volcanModel = glm::rotate(volcanModel, glm::degrees(90.0f), glm::vec3(0.0, 1.0, 0.0));
         glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "model"), 1, GL_FALSE, glm::value_ptr(volcanModel));
         glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
         glBindVertexArray(0);

         // ================= Draw Lamp ===============
         lamp.Use();
         glUniformMatrix4fv(glGetUniformLocation(lamp.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
         glUniformMatrix4fv(glGetUniformLocation(lamp.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
         glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
         glm::mat4 lampModel;
         lampModel = glm::scale(lampModel, glm::vec3(0.05f, 0.05f, 0.05f));
         lampModel = glm::translate(lampModel, lightpos);
         glUniformMatrix4fv(glGetUniformLocation(lamp.Program, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));
         modelSphere.Draw(lamp);
         glBindVertexArray(0);


         glfwSwapBuffers(window);
     }

    // Properly de-allocate all resources once they've outlived their purpose

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

//    delete[] altitudeCoordinates;
//    delete[] altitudeMatrix;
    delete vertexData;

    //EXIT LOOOP
    glfwTerminate();
    return 0;
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

void createVertex(vector<MyVertex>& vertexVector, GLfloat ** altitudeMatrix,GLfloat ** temperatureMatrix ){
    int index=0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(altitudeMatrix[i][j] == novalue)
                altitudeMatrix[i][j]=0.0f;
            MyVertex v (i*cellSize,altitudeMatrix[i][j],j*cellSize);
            v.temperature = temperatureMatrix[i][j];
            v.setPos(index);
            vertexVector.push_back(v);
            index++;
            updateMaxAndMin(v);
        }
    }

    for(int i = 0; i < vertexVector.size(); i++){
        vertexVector[i].normalizeCoord(minVal,maxVal);
        vertexVector[i].computeColor(maxTemp,minTemp);
    }
}

void addConnectedVertex(vector<MyVertex> &verticesCoordinates,int index1,int index2,int index3){
        verticesCoordinates[index1].addConnectedVertex(index2,index3);
        verticesCoordinates[index2].addConnectedVertex(index1,index3);
        verticesCoordinates[index3].addConnectedVertex(index1,index2);
}

void createIndex(GLint* indices,vector<MyVertex> &verticesCoordinates){
    int index=0;
    int it = (rows*cols)-cols;
    //    std::cout<<"iteration:"<<it<<endl;
    for(int i = 0; i < it; i++){
        if(i%cols == cols-1){      //skip last col
            //            std::cout<<i<<" skip"<<endl;
            continue;
        }
        indices[index] = i;
        indices[index+1] = i+1;
        indices[index+2] = i+cols;
        addConnectedVertex(verticesCoordinates,indices[index],indices[index+1],indices[index+2]);

        indices[index+3] = indices[index+2];
        indices[index+4] = indices[index+2]+1;
        indices[index+5] = i+1;
        addConnectedVertex(verticesCoordinates,indices[index+3],indices[index+4],indices[index+5]);

        //        std::cout<<i<<":"<<indices[index]<<","<<indices[index+1]<<","<<indices[index+2]<<endl;
        //        std::cout<<i<<":"<<indices[index+3]<<","<<indices[index+4]<<","<<indices[index+5]<<endl;

        index= index + 6;
    }
//        std::cout<<"Indices num:"<<index<<endl;
}

void computeNormal(vector<MyVertex>& verticesCoordinates){

    for(int i=0;i<verticesCoordinates.size();i++){
        vector<tuple<int,int>> connectedVertex = verticesCoordinates[i].getConnectedVertex();
        for(tuple<int,int> t: connectedVertex){
            int index1 = get<0>(t);
            int index2 = get<1>(t);
//            cout<<"index:"<<index1<<","<<index2<<endl;
            MyVertex v1= verticesCoordinates[index1];
            MyVertex v2= verticesCoordinates[index2];
            GLfloat p1[3] = {v1.getCoord()[0],v1.getCoord()[1],v1.getCoord()[2]};
            GLfloat p2[3] = {v2.getCoord()[0],v2.getCoord()[1],v2.getCoord()[2]};
            verticesCoordinates[i].calculateFaceNormal(p1,p2,true);
        }
        verticesCoordinates[i].calculateAverageNormal();
    }
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
    if( v.temperature > maxTemp) {
        maxTemp=v.temperature;
    }
    if( v.temperature < minTemp) {
        minTemp=v.temperature;
    }
}

GLfloat* buildVBO(vector<MyVertex>& verticesCoordinates){
    GLfloat* vertexData = new GLfloat[rows*cols*9]; //3 coord,3 norm, 3 color
    int index=0;
    for(int i=0;i<verticesCoordinates.size();i++){
         vector<GLfloat> vbol = verticesCoordinates[i].createVBOline();
         for(int i=0;i<vbol.size();i++){
             vertexData[index+i] = vbol[i];
         }
         index+=9;
    }
    return vertexData;
}

GLfloat *readFile(char* path){
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
                if(val > 0)
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
