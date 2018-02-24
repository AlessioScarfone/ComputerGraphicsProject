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


class Vertex {
private:
    GLfloat* coord = new GLfloat[3];
    glm::vec3 normal;

    vector<tuple<GLfloat,GLfloat,GLfloat>> vert_normals;

public:
    Altitude (GLfloat x, GLfloat y, GLfloat z) {
        coord[0] = x;
        coord[1] = y;
        coord[2] = z;
    }

    glm::vec3 calculateNormal(GLfloat p1[3], GLfloat p2[3]){
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
        return curNormal;
    }

    void addNormal(tuple<GLfloat,GLfloat,GLfloat> t){
        vert_normals.push_back(t);
    }

    tuple<GLfloat,GLfloat,GLfloat> createTuple(GLfloat x, GLfloat y ,GLfloat z){
        make_tuple(x,y,z);
    }

    void calculateNormal(){
        normal = glm::vec3(0.0f,0.0f,0.0f);
        for (int i = 0; i < vert_normals.size(); ++i) {
            normal = normal+vert_normals[i];
        }
        // normal.x *= -1;
        // normal.y *= -1;
        // normal.z *= -1;
        return glm::normalize(normal);
    }

};
