#include "geometry.h"

BoxGeometry::BoxGeometry(GLfloat width, GLfloat height, GLfloat length) {
    vertices = std::vector<GLfloat>{
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f};

    indices = std::vector<GLuint>{
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        2, 3, 4,
        4, 3, 5,
        // back
        4, 5, 7,
        6, 4, 7,
        // left
        6, 7, 1,
        0, 6, 1,
        // top
        0, 2, 6,
        6, 2, 4,
        // bottom
        1, 7, 3,
        3, 7, 5};
}

PlaneGeometry::PlaneGeometry(GLfloat _width, GLfloat _height, GLuint _widthSegments, GLuint _heightSegments) {
    width = _width;
    height = _height;
    widthSegments = _widthSegments;
    heightSegments = _heightSegments;
    generateVertices();
    generateIndices();
}

void PlaneGeometry::generateVertices() {
    vertices = std::vector<GLfloat>();
    float segmentWidth = width / widthSegments;
    float segmentHeight = height / heightSegments;
    for (GLuint z = 0; z <= heightSegments; z++) {
        for (GLuint x = 0; x <= widthSegments; x++) {
            vertices.push_back(x * segmentWidth - (width / 2));
            vertices.push_back(0.0f);
            vertices.push_back(z * segmentHeight - (height / 2));
        }
    }
}

void PlaneGeometry::generateIndices() {
    indices = std::vector<GLuint>();
    // loop to go through every rectangle in the mesh
    for (GLuint z = 0; z < heightSegments; z++) {
        for (GLuint x = 0; x < widthSegments; x++) {
            GLuint index = x + heightSegments * z + z;

            // first triangle in the rectangle
            indices.push_back(index);
            indices.push_back(index + widthSegments + 1);
            indices.push_back(index + 1);

            // second triangle
            indices.push_back(index + 1);
            indices.push_back(index + widthSegments + 1);
            indices.push_back(index + widthSegments + 2);
        }
    }
}