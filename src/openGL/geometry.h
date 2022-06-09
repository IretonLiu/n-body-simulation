#pragma once

// openGL libraries
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>

// base class of the shapes
class Geometry {
   public:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};

// geometry for constructing a box
class BoxGeometry : public Geometry {
   public:
    BoxGeometry(GLfloat width, GLfloat height, GLfloat length);
};

// geometry for constructing a plane
class PlaneGeometry : public Geometry {
   public:
    GLfloat width;
    GLfloat height;
    GLuint widthSegments;
    GLuint heightSegments;

    PlaneGeometry(GLfloat _width, GLfloat _height, GLuint _widthSegments, GLuint _heightSegments);

   private:
    void generateVertices();  // generate the vertices from the size and segment info
    void generateIndices();   // generate the indices from the size and segment info
};
