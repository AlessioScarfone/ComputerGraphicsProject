// Std. Includes
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class MyVertex {
private:
    int pos;   //position in a vertex list
    GLfloat coord [3];
    GLfloat color[3];
    GLfloat texCoord[2];

public:
    glm::vec3 normal;
    GLfloat temperature=0.0f;

    MyVertex (GLfloat x, GLfloat y, GLfloat z) {
        coord[0] = x;
        coord[1] = y;
        coord[2] = z;
    }

    MyVertex (const MyVertex &v){
        for(int i=0;i<3;i++)
            coord[i] = v.coord[i];
        pos = v.pos;
        temperature = v.temperature;
        texCoord[0] = v.texCoord[0];
        texCoord[1] = v.texCoord[1];
    }

    tuple<GLfloat,GLfloat,GLfloat> createTuple(GLfloat x, GLfloat y ,GLfloat z){
        make_tuple(x,y,z);
    }

    GLfloat* getCoord(){
        return this->coord;
    }

    GLfloat x(){
        return coord[0];
    }
    GLfloat y(){
        return coord[1];
    }
    GLfloat z(){
        return coord[2];
    }

    void normalizeCoord(GLfloat min, GLfloat max){ //in range [0,1]
        GLfloat diff=(max - min);
        for(int i=0;i<3;i++){
            coord[i] = (coord[i]-min) / diff;
        }
    }

    void setPos(int pos){
        this->pos = pos;
    }

    void computeColor(GLfloat maxTemp,GLfloat minTemp){
        if(temperature > 0.0f){
             GLfloat redVal = (temperature - minTemp) / (maxTemp - minTemp);
             color[0] = 1.0f;
             color[1] = redVal;
             color[2] = 0.0f;
//             cout<<"RED VALUE:"<<color[0]<<" GREEN VALUE:"<<color[1]<<endl;
        }
        else{
            //set color from altitude
            color[0] = y();
            color[1] = y();
            color[2] = y();
        }
    }



    void print(){
        cout<<"v:"<<pos<<" ("<<x()<<","<<y()<<","<<z()<<")"<<endl;
    }

    void printComplete(){
        cout<<pos<<": ("<<x()<<","<<y()<<","<<z()
           <<") N: ("<<normal.x<<","<<normal.y<<","<<normal.z<<")"<<" - T:"<<this->temperature<<endl;
    }

    vector<GLfloat> createVBOline(){
        vector<GLfloat> line;
        for(int i = 0; i < 3; i++)
            line.push_back(coord[i]);
        line.push_back(normal.x);
        line.push_back(normal.y);
        line.push_back(normal.z);

        for(int i = 0; i < 3; i++){
            line.push_back(color[i]);
        }
        for(int i = 0; i < 2; i++){
            line.push_back(texCoord[i]);
        }
        return line;
    }

    void setTexCoord(GLfloat c1,GLfloat c2){
        texCoord[0]=c1;
        texCoord[1]=c2;
    }

};
