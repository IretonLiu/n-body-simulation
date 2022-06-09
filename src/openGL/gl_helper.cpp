#include "gl_helper.h"

GLFWwindow* window;

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
    window = glfwCreateWindow(1024, 768, programName, NULL, NULL);
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

// main render function
void render(PerspectiveCamera* camera, Geometry* geometry) {
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // changes to wireframe mode
    // white background
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    Shader* shader = new Shader();
    shader->CreateVFShader("../src/shaders/VertexShader.vert", "../src/shaders/FragmentShader.frag");
    shader->Bind();

    // generate the vertex array object
    VertexArray* va = new VertexArray();
    // array buffer for vertex position
    VertexBuffer* vb = new VertexBuffer(&geometry->vertices[0], geometry->vertices.size() * sizeof(GLfloat));

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3, GL_FALSE);
    va->AddBuffer(*vb, layout);

    // array buffer for indices
    IndexBuffer* ib = new IndexBuffer(&geometry->indices[0], geometry->indices.size());

    do {
        // Clear the screen
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        shader->SetUniformMatrix4fv("modelView", camera->getModelViewMat());
        shader->SetUniformMatrix4fv("projection", camera->getProjectionMatrix());

        va->Bind();
        ib->Bind();
        // glDrawArrays(GL_TRIANGLES, 0, 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles
        GLCall(glDrawElements(GL_TRIANGLES, geometry->indices.size(), GL_UNSIGNED_INT, nullptr));

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
    // Cleanup VBO and shader
    glDeleteProgram(shader->GetRendererID());
    delete va;
    delete vb;
    delete ib;
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
