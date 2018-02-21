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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

GLfloat *readFile(char* path);
GLfloat** createMatrix(GLfloat* input, int r, int c);
void createVertexMatrix(GLfloat* vertex, GLfloat ** altitudeMatrix, int rowsNum, int colsNum, int cellSize);
void createIndex(GLint* indices, int rowsNum, int colsNum);

// _________Camera _________
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool enableMove =false;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float anglePitch = 0.0f;
float angleYao = 0.0f;
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

    GLfloat* altitudeCoordinates = readFile("../dataset/test/DEM_test.dat");
//    GLfloat* altitudeCoordinates = readFile("../dataset/altitudes.dat");
    GLfloat** altitudeMatrix = createMatrix(altitudeCoordinates,rows,cols);

//    //print matrix
//    for(int i = 0; i<rows ;i++){
//        for(int j=0; j<cols ;j++){
//            std::cout<<altitudeMatrix[i][j]<<",";
//        }
//        std::cout<<endl;
//    }

     GLfloat* vectorCoordinates = new GLfloat[cols*rows*3]; //each vertex with 3 coord
     createVertexMatrix(vectorCoordinates, altitudeMatrix,rows,cols,cellSize);
     //print vertex list
     for(int i = 0; i<300 ;i+=3){
        std::cout<<i/3<<" - x:"<<vectorCoordinates[i]
                   <<", y:"<<vectorCoordinates[i+1]
                   <<", z:"<<vectorCoordinates[i+2]<<endl;
     }
     int neededTriangles = (rows-1)*(cols-1)* 2;
     int indexNum = neededTriangles * 3; //r-1*c-1 square,* 2 triangle , * 3 coord
//     std::cout<<"index dim:"<<indexNum<<endl;
     GLint* indices = new GLint[indexNum];
     createIndex(indices, rows,cols);
//     for(int i = 0; i<indexNum ;i+=3){
//         std::cout<<i/3<<" - "<<indices[i]
//                  <<" , "<<indices[i+1]
//                  <<" , "<<indices[i+2]<<endl;
//     }

     Shader base("../shaders/base.vs", "../shaders/base.frag");


     GLuint VAO,VBO,EBO;;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (cols*rows*3), vectorCoordinates, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*neededTriangles*2, indices, GL_STATIC_DRAW);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
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

        base.Use();
        glUniformMatrix4fv(glGetUniformLocation(base.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(base.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);
        glm::mat4 volcanModel;
        glUniformMatrix4fv(glGetUniformLocation(base.Program, "model"), 1, GL_FALSE, glm::value_ptr(volcanModel));
        glDrawElements(GL_TRIANGLES, neededTriangles, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
//    glDeleteVertexArrays(1, &floorVAO);
//    glDeleteBuffers(1, &floorVBO);

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


void createVertexMatrix(GLfloat* vertex,GLfloat ** altitudeMatrix, int rows, int cols, int cellSize){
    int index=0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            glm::vec3 vec(i*cellSize, altitudeMatrix[i][j], j*cellSize);
            glm::vec3 normalizedVec=glm::normalize(vec);
            vertex[index] = normalizedVec.x;
            vertex[index+1] = normalizedVec.y;
            vertex[index+2] = normalizedVec.z;

            index = index+3;
        }
    }
}



void createIndex(GLint* indices, int rowsNum, int colsNum){
    int index=0;
    int it = (rowsNum*colsNum)-colsNum;
    //    std::cout<<"iteration:"<<it<<endl;
    for(int i = 0; i < it; i++){
        if(i%colsNum == colsNum-1){      //skip last col
            //            std::cout<<i<<" skip"<<endl;
            continue;
        }
        indices[index] = i;
        indices[index+1] = i+1;
        indices[index+2] = i+colsNum;

        indices[index+3] = indices[index+2];
        indices[index+4] = indices[index+2]+1;
        indices[index+5] = i+1;

        //        std::cout<<i<<":"<<indices[index]<<","<<indices[index+1]<<","<<indices[index+2]<<endl;
        //        std::cout<<i<<":"<<indices[index+3]<<","<<indices[index+4]<<","<<indices[index+5]<<endl;

        index= index + 6;
    }
    //    std::cout<<"END"<<index<<endl;
}
