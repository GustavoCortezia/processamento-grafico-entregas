#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cassert>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
GLuint setupShader();
GLuint setupGeometry(const std::vector<GLfloat>& vertices);

const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
void main()
{
    gl_Position = vec4(position, 1.0);
}
)";

const GLchar *fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main()
{
    color = inputColor;
}
)";

int main()
{
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Pessoa de Palitos -- Rossana", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    cout << "Renderer: " << reinterpret_cast<const char*>(renderer) << endl;
    cout << "OpenGL version supported: " << reinterpret_cast<const char*>(version) << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();
    glUseProgram(shaderID);

    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

    glLineWidth(6.0f);
    glPointSize(8.0f);

    float pi = 3.14159265f;

    std::vector<GLfloat> head;
    const int headSegments = 64;
    const float cx = 0.0f, cy = 0.50f, r = 0.15f;
    for (int i = 0; i < headSegments; ++i) {
        float a = (float)i * 2.0f * pi / (float)headSegments;
        head.push_back(cx + r * cosf(a));
        head.push_back(cy + r * sinf(a));
        head.push_back(0.0f);
    }
    GLuint VAO_head = setupGeometry(head);

    std::vector<GLfloat> body = {
        0.0f,  0.35f, 0.0f,
        0.0f, -0.20f, 0.0f
    };
    GLuint VAO_body = setupGeometry(body);

    std::vector<GLfloat> arms = {
        0.0f,  0.25f, 0.0f,   -0.30f, 0.00f, 0.0f,
        0.0f,  0.25f, 0.0f,    0.30f, 0.00f, 0.0f
    };
    GLuint VAO_arms = setupGeometry(arms);

    std::vector<GLfloat> legs = {
        0.0f, -0.20f, 0.0f,   -0.20f, -0.70f, 0.0f,
        0.0f, -0.20f, 0.0f,    0.20f, -0.70f, 0.0f
    };
    GLuint VAO_legs = setupGeometry(legs);



    double prev_s = glfwGetTime();
    double title_countdown_s = 0.1;

    while (!glfwWindowShouldClose(window)) {
        double curr_s = glfwGetTime();
        double elapsed_s = curr_s - prev_s;
        prev_s = curr_s;
        title_countdown_s -= elapsed_s;
        if (title_countdown_s <= 0.0 && elapsed_s > 0.0) {
            double fps = 1.0 / elapsed_s;
            char tmp[256];
            sprintf(tmp, "Pessoa de Palitos -- Rossana\tFPS %.2lf", fps);
            glfwSetWindowTitle(window, tmp);
            title_countdown_s = 0.1;
        }

        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);

        glBindVertexArray(VAO_head);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINE_LOOP, 0, headSegments);

        glBindVertexArray(VAO_body);
        glUniform4f(colorLoc, 0.7f, 0.7f, 1.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, 2);

        glBindVertexArray(VAO_arms);
        glUniform4f(colorLoc, 0.7f, 1.0f, 0.7f, 1.0f);
        glDrawArrays(GL_LINES, 0, 4);

        glBindVertexArray(VAO_legs);
        glUniform4f(colorLoc, 1.0f, 0.7f, 0.7f, 1.0f);
        glDrawArrays(GL_LINES, 0, 4);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO_head);
    glDeleteVertexArrays(1, &VAO_body);
    glDeleteVertexArrays(1, &VAO_arms);
    glDeleteVertexArrays(1, &VAO_legs);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint setupGeometry(const std::vector<GLfloat>& vertices)
{
    GLuint VBO = 0, VAO = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}