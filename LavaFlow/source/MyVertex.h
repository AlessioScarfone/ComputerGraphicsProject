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
    vector<tuple<int,int>> connectedVertex;
    vector<tuple<GLfloat,GLfloat,GLfloat>> vert_normals;
    GLfloat color[3];

    void addNormal(tuple<GLfloat,GLfloat,GLfloat> t){
        vert_normals.push_back(t);
    }

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
    }

    glm::vec3 calculateFaceNormal(GLfloat p1[3], GLfloat p2[3],bool add){
        glm::vec3 curNormal;
        float vect[3];
        // calculates vectors from this Vertex to p1
        // A(x1,y1,z1), B(x2,y2,z2)
        // V(AB) = (x2-x1, y2-y1, z2-z1)
        for (int i=0;i<3;i++)
            vect[i] = p1[i]-this->coord[i];
        glm::vec3 u = glm::vec3(vect[0], vect[1], vect[2]);

        // calculates vectors from this Vertex to p2
        for (int i=0;i<3;i++)
            vect[i] = p2[i]-this->coord[i];
        glm::vec3 v = glm::vec3(vect[0], vect[1], vect[2]);

        // calculates vector at 90° to 2 vectors
        curNormal = glm::cross(v, u);

        // makes the vector length 1
        glm::normalize(curNormal);
        if(add)
            vert_normals.push_back(make_tuple(curNormal.x,curNormal.y,curNormal.z));
        return curNormal;
    }


    tuple<GLfloat,GLfloat,GLfloat> createTuple(GLfloat x, GLfloat y ,GLfloat z){
        make_tuple(x,y,z);
    }

    void calculateAverageNormal(){
        normal = glm::vec3(0.0f,0.0f,0.0f);
        for (int i = 0; i < vert_normals.size(); ++i) {
            normal = normal+glm::vec3(get<0>(vert_normals[i]),get<1>(vert_normals[i]),get<2>(vert_normals[i]));
        }
        // normal.x *= -1;
         normal.y *= -1;
        // normal.z *= -1;
        glm::normalize(normal);
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

    void normalizeCoord(GLfloat min,GLfloat max){ //in range [0,1]
        GLfloat diff=(max - min);
        for(int i=0;i<3;i++){
            coord[i] = (coord[i]-min) / diff;
        }
    }

    void setPos(int pos){
        this->pos = pos;
    }

    void addConnectedVertex(int v1Index,int v2Index){
        connectedVertex.push_back(make_tuple(v1Index,v2Index));
    }

    vector<tuple<int,int>> getConnectedVertex(){
        return connectedVertex;
    }

    void computeColor(GLfloat maxTemp,GLfloat minTemp){
        //TODO temperature...
        GLfloat redVal = y();
        if(temperature!=0.0f){
             redVal = (temperature - minTemp) / (maxTemp - minTemp);
             color[0] = redVal;
             color[1] = 1.0f-y();
             color[2] = 0.0f;


             cout<<"RED VALUE:"<<color[0]<<" GREEN VALUE:"<<color[1]<<endl;
        }
        else{
            //set color with altitude
            color[0] = y();
            color[1] = y();
            color[2] = y();
        }

    }


    void print(){
        cout<<"v: ("<<x()<<","<<y()<<","<<z()<<")"<<endl;
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
        return line;
    }

};
