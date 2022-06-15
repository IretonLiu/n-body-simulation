#include "gl_utilities.h"

GLFWwindow* window;

float canvas[WINDOW_WIDTH * WINDOW_HEIGHT];
unsigned int shaderStorageBuffer;

int initGLProgram(const char* programName) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open the glfw window and create the OpenGL context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, programName, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW.\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    std::cout << "GLFW initialized" << std::endl;
    return 0;
}

std::vector<float> GenVerticesFromBodies(std::vector<Body*> bodies, int P, int M) {
    long int scalePos = std::pow(10, P);
    long int scaleMass = std::pow(10, M);

    std::vector<float> vertices(bodies.size() * 4);
    for (int i = 0; i < bodies.size(); i++) {
        vertices[i * 4] = bodies[i]->position.x / scalePos;
        vertices[i * 4 + 1] = bodies[i]->position.y / scalePos;
        vertices[i * 4 + 2] = bodies[i]->position.z / scalePos;
        vertices[i * 4 + 3] = bodies[i]->mass / scaleMass;
    }

    return vertices;
}

// std::vector<float> GetMassFromBodies(std::vector<Body*> bodies, int M) {
//     long int scale = std::pow(10, M);
//     std::vector<float> vertices(bodies.size());
//     for (int i = 0; i < bodies.size(); i++) {
//         vertices[i] = bodies[i]->mass / scale;
//     }

//     return vertices;
// }

// void updateSSBO(float* canvas, int bindingPoint) {
//     GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageBuffer));

//     GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT, canvas, GL_DYNAMIC_COPY));
//     GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, shaderStorageBuffer));

//     // unbind
//     GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
// }

// main render function
void render(std::vector<Body*> bodies, int P, int M, void (*callback)(std::vector<Body*>&)) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    PerspectiveCamera* camera = new PerspectiveCamera(70.0, 4.0 / 3.0, 0.1, 100.0);
    camera->setPosition(glm::vec3(0.5, 0.5, 2));
    camera->setTarget(glm::vec3(bodies[bodies.size() - 1]->position.x / std::pow(10, P), bodies[bodies.size() - 1]->position.y / std::pow(10, P), bodies[bodies.size() - 1]->position.z / std::pow(10, P)));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // changes to wireframe mode
    // white background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    Shader* shader = new Shader();
    shader->CreateVFShader("../src/OpenGL/shaders/VertexShader.vert", "../src/OpenGL/shaders/FragmentShader.frag");
    shader->Bind();

    std::vector<float> vertices = GenVerticesFromBodies(bodies, P, M);
    // std::vector<float> mass = GenVerticesFromBodies(bodies, M);

    // generate the vertex array object
    VertexArray* va = new VertexArray();

    // array buffer for vertex position
    VertexBuffer* vb = new VertexBuffer(&vertices[0], vertices.size() * sizeof(float));
    // VertexBuffer* vb2 = new VertexBuffer(&mass[0], mass.size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3, GL_FALSE);
    layout.Push(GL_FLOAT, 1, GL_FALSE);
    va->AddBuffer(*vb, layout);
    // va->AddBuffer(*vb2, layout);

    // array buffer for indices
    // IndexBuffer* ib = new IndexBuffer(&imagePlane->indices[0], imagePlane->indices.size());
    int frameNumber = 0;
    do {
        // Clear the screen
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        shader->SetUniformMatrix4fv("modelView", camera->getModelViewMat());
        shader->SetUniformMatrix4fv("projection", camera->getProjectionMatrix());

        va->Bind();

        GLCall(glDrawArrays(GL_POINTS, 0, vertices.size() / 3));

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (frameNumber % 1 == 0) {
            callback(bodies);
            vertices = GenVerticesFromBodies(bodies, P, M);
            vb->UpdateBuffer(&vertices[0], vertices.size() * sizeof(float));
        }
        frameNumber++;
        camera->setTarget(glm::vec3(bodies[bodies.size() - 1]->position.x / std::pow(10, P), bodies[bodies.size() - 1]->position.y / std::pow(10, P), bodies[bodies.size() - 1]->position.z / std::pow(10, P)));

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
    // Cleanup VBO and shader
    glDeleteProgram(shader->GetRendererID());
    delete va;
    delete vb;
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
