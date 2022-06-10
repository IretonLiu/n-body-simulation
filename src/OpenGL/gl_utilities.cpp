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

void ClearCanvas() {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            canvas[x + WINDOW_WIDTH * y] = 0.0f;
        }
    }
}

void UpdateCanvas(std::vector<Body*> bodies, int P) {
    long double maxPos = GetMaxPosition(P);
    for (Body* body : bodies) {
        int canvasX = std::floor(WINDOW_WIDTH * body->x / maxPos);
        int canvasY = std::floor(WINDOW_HEIGHT * body->y / maxPos);

        if (canvasX >= 0 && canvasX < WINDOW_WIDTH && canvasY >= 0 && canvasY < WINDOW_HEIGHT) {
            canvas[canvasX + WINDOW_WIDTH * canvasY] += 1.0f / bodies.size();
        }
    }
}

void updateSSBO(float* canvas, int bindingPoint) {
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageBuffer));

    GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT, canvas, GL_DYNAMIC_COPY));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, shaderStorageBuffer));

    // unbind
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

// main render function
void render() {
    Geometry* imagePlane = new Geometry();
    imagePlane->vertices = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f};

    imagePlane->indices = {
        0, 1, 3,
        3, 1, 2};

    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // changes to wireframe mode
    // white background
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    Shader* shader = new Shader();
    shader->CreateVFShader("../src/OpenGL/shaders/VertexShader.vert", "../src/OpenGL/shaders/FragmentShader.frag");
    shader->Bind();

    GLCall(glGenBuffers(1, &shaderStorageBuffer));

    // generate the vertex array object
    VertexArray* va = new VertexArray();
    // array buffer for vertex position
    VertexBuffer* vb = new VertexBuffer(&imagePlane->vertices[0], imagePlane->vertices.size() * sizeof(GLfloat));

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3, GL_FALSE);
    va->AddBuffer(*vb, layout);

    // array buffer for indices
    IndexBuffer* ib = new IndexBuffer(&imagePlane->indices[0], imagePlane->indices.size());
    int frameNumber = 0;
    ClearCanvas();
    do {
        // Clear the screen
        UpdateCanvas(bodies, MAGNITUDE);

        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        if (frameNumber % 1 == 0) {
            updateSSBO(canvas, 1);
            BruteForce(bodies);
        }
        va->Bind();
        ib->Bind();
        // glDrawArrays(GL_TRIANGLES, 0, 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles
        GLCall(glDrawElements(GL_TRIANGLES, imagePlane->indices.size(), GL_UNSIGNED_INT, nullptr));

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        ClearCanvas();
        frameNumber++;
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
