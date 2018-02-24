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

#include "vertex.h"


GLuint screenWidth = 1024;
GLuint screenHeight = 800;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

GLfloat *readFile(char* path);
GLfloat** createMatrix(GLfloat* input, int r, int c);
void createVertex(GLfloat* vertex, GLfloat** altitudeMatrix, int rowsNum, int colsNum, int cellSize);
//void createNormal(GLfloat* normals,GLfloat** altitudeMatrix);
void createIndex(GLint* indices);
void updateMaxAndMin(GLfloat* vertex, GLint index);
glm::vec3 calculateNormal(GLfloat** matrix, int i, int j);
GLfloat normalizeNumber(GLfloat val);
void CalculateNormalForVertices(GLfloat* verticesCoordinates,GLfloat* normals,GLfloat* verticesNormalCoordinates);

// _________Camera _________
Camera camera(glm::vec3(0.0f, 0.5f, 2.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool enableMove =false;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float anglePitch = 0.0f;
float angleYao = 0.0f;

GLfloat maxVal=-10000;
GLfloat minVal=10000;
// _________________________

// _________ Light _________
glm::vec3 lightpos(0.0f, 0.0f, 50.0f);
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

//    GLfloat* altitudeCoordinates = readFile("../dataset/test/DEM_test.dat");
    GLfloat* altitudeCoordinates = readFile("../dataset/altitudes.dat");
    GLfloat** altitudeMatrix = createMatrix(altitudeCoordinates,rows,cols);

//    //print matrix
//    for(int i = 0; i<rows ;i++){
//        for(int j=0; j<cols ;j++){
//            std::cout<<altitudeMatrix[i][j]<<",";
//        }
//        std::cout<<endl;
//    }

     Vertex* verticesCoordinates = new Vertex[cols*rows];
     createVertex(verticesCoordinates, altitudeMatrix,rows,cols,cellSize);
     //print vertex list
//     for(int i = 0; i<300 ;i+=3){
//        std::cout<<i/3<<" - x:"<<verticesCoordinates[i]
//                   <<", y:"<<verticesCoordinates[i+1]
//                   <<", z:"<<verticesCoordinates[i+2]<<endl;
//     }

//     GLfloat* normals = new GLfloat[cols*rows*(3)]; //each vertex with 3 coord
//     createNormal(normals, altitudeMatrix);
//          for(int i = 0; i<cols*rows*(3) ;i+=3){
//              std::cout<<i/3<<" - "<<normals[i]
//                       <<" , "<<normals[i+1]
//                       <<" , "<<normals[i+2]<<endl;
//          }

     int neededTriangles = (rows-1)*(cols-1)* 2;
     int indexNum = neededTriangles * 3; //r-1*c-1 square,* 2 triangle , * 3 coord
//     std::cout<<"index dim:"<<indexNum<<endl;
     GLint* indices = new GLint[indexNum];
     createIndex(indices);
//     for(int i = 0; i<indexNum ;i+=3){
//         std::cout<<i/3<<" - "<<indices[i]
//                  <<" , "<<indices[i+1]
//                  <<" , "<<indices[i+2]<<endl;
//     }

     GLfloat* VNCoordinates = new GLfloat[cols*rows*(3+3)];//3 coord and 3 for normal
     CalculateNormalForVertices(verticesCoordinates,indices, VNCoordinates);
//     for(int i = 0; i<cols*rows*(3+3) ;i+=6){
//         std::cout<<i/3<<" - x:"<<VNCoordinates[i]
//                    <<", y:"<<VNCoordinates[i+1]
//                   <<", z:"<<VNCoordinates[i+2]
//                  << "  NORMAL ("<<VNCoordinates[i+3]<<","<<VNCoordinates[i+4]<<","<<VNCoordinates[i+5]<<")"<<endl;
//     }

     Shader vulcan("../shaders/base.vs", "../shaders/base.frag");
     Shader lamp("../shaders/light.vs", "../shaders/light.frag");

     char pathSphere[] = "../model/sphere.obj";
     Model modelSphere(pathSphere);


     GLuint VAO,VBO,EBO;;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (cols*rows*3), VNCoordinates, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*neededTriangles*2, indices, GL_STATIC_DRAW);
     // position
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
     // normal
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)3);
     glEnableVertexAttribArray(0);
     glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


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

        vulcan.Use();
        glUniform3f(glGetUniformLocation(vulcan.Program, "light.position"), camera.Front.x, camera.Front.y, -camera.Front.z);
        glUniform3f(glGetUniformLocation(vulcan.Program, "light.ambient"),  0.3f, 0.3f, 0.3f);

        glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);
        glm::mat4 volcanModel;
        glUniformMatrix4fv(glGetUniformLocation(vulcan.Program, "model"), 1, GL_FALSE, glm::value_ptr(volcanModel));
        glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //Draw Lamp
        lamp.Use();
        glUniformMatrix4fv(glGetUniformLocation(lamp.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lamp.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glm::mat4 spheres;
        spheres = glm::scale(spheres, glm::vec3(0.05f, 0.05f, 0.05f));
        spheres = glm::translate(spheres, lightpos);
        glUniformMatrix4fv(glGetUniformLocation(lamp.Program, "model"), 1, GL_FALSE, glm::value_ptr(spheres));
       // modelSphere.Draw(lamp);
        glBindVertexArray(0);


        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
//    glDeleteVertexArrays(1, &floorVAO);
//    glDeleteBuffers(1, &floorVBO);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    delete[] altitudeCoordinates;
    delete[] altitudeMatrix;
    delete[] verticesCoordinates;
    delete[] normals;
    delete[] indices;
    delete[] VNCoordinates;

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

GLfloat normalizeNumber(GLfloat val){ //in range [0,1]
    GLfloat newVal = (val - minVal) / (maxVal - minVal);
    return newVal;
}

void createVertex(Vertex* vertex,GLfloat ** altitudeMatrix, int rows, int cols, int cellSize){
    GLint index=0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            Vertex v = new Vertex(i*cellSize,altitudeMatrix[i][j],j*cellSize);
//            vertex[index] = i*cellSize;
//            vertex[index+1] = altitudeMatrix[i][j];
//            vertex[index+2] = j*cellSize;

            updateMaxAndMin(vertex,index);

            index = index+3;
        }
    }
    std::cout<<"MAX:"<<maxVal<<endl;
    std::cout<<"MIN:"<<maxVal<<endl;
    for(;index-1 >= 0; index--){
        vertex[index] = normalizeNumber(vertex[index]);
    }
}

//void createNormal(GLfloat* normals,GLfloat** altitudeMatrix){
//    GLint index=0;
//    for(int i = 0; i < rows; i++){
//        for(int j = 0; j < cols; j++){
//            glm::vec3 normal= calculateNormal(altitudeMatrix,i,j);
//            normals[index] = normal.x;
//            normals[index+1] = normal.y;
//            normals[index+2] = normal.z;

//            index = index+3;
//        }
//    }
//}

void createIndex(GLint* indices){
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

        indices[index+3] = indices[index+2];
        indices[index+4] = indices[index+2]+1;
        indices[index+5] = i+1;

        //        std::cout<<i<<":"<<indices[index]<<","<<indices[index+1]<<","<<indices[index+2]<<endl;
        //        std::cout<<i<<":"<<indices[index+3]<<","<<indices[index+4]<<","<<indices[index+5]<<endl;

        index= index + 6;
    }
    //    std::cout<<"END"<<index<<endl;
}

void CalculateNormalForVertices(GLfloat* verticesCoordinates,GLfloat* indices,GLfloat* VNCoordinates){
    int index=0;
    for (int i =0;i<cols*rows*(3);i+=3){
        //copy vertex
        for(int j=0;j<3;j++){
            VNCoordinates[index+j] = verticesCoordinates[i+j];
         }
        //copy normal

        for(int j=0;j<3;j++){
            VNCoordinates[index+j] = normals[i+j];
         }
    }
}

void updateMaxAndMin(GLfloat* vertex, GLint index){
    if( vertex[index] > maxVal) {
        maxVal=vertex[index];
    }
    if( vertex[index] > maxVal){
        maxVal=vertex[index+1];
    }
    if( vertex[index] > maxVal){
        maxVal=vertex[index+2];
    }

    if( vertex[index] < minVal) {
        minVal=vertex[index];
    }
    if( vertex[index] < minVal){
        minVal=vertex[index+1];
    }
    if( vertex[index] < minVal){
        minVal=vertex[index+2];
    }
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
