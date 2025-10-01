#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Protótipos das funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
int setupShader();
void renderTriangles(GLuint shaderID);

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Armazena os vértices para os triângulos
std::vector<GLfloat> vertices;
std::vector<GLuint> VAOs;

// Código fonte do Vertex Shader
const GLchar* vertexShaderSource = R"(
    #version 400
    layout (location = 0) in vec3 position;
    void main()
    {
        gl_Position = vec4(position, 1.0);
    }
)";

// Código fonte do Fragment Shader
const GLchar* fragmentShaderSource = R"(
    #version 400
    uniform vec4 inputColor;
    out vec4 color;
    void main()
    {
        color = inputColor;
    }
)";

// Função MAIN
int main() {
    // Inicialização da GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Olá Triângulo! -- Rossana", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Registro das funções de callback
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Configurações de viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();

    // Loop da aplicação
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderTriangles(shaderID);

        glfwSwapBuffers(window);
    }

    // Desalocando recursos
    for(auto vao : VAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    glfwTerminate();
    return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Função de callback de mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        GLfloat xNDC = xPos / WIDTH * 2.0f - 1.0f;
        GLfloat yNDC = 1.0f - yPos / HEIGHT * 2.0f;

        vertices.push_back(xNDC);
        vertices.push_back(yNDC);
        vertices.push_back(0.0f);

        if(vertices.size() / 3 == 3) {
            GLuint VAO, VBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            VAOs.push_back(VAO);

            vertices.clear();
        }
    }
}

// Função que configura o shader
int setupShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Função para renderizar triângulos
void renderTriangles(GLuint shaderID) {
    static const std::vector<glm::vec4> colors = {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Green
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), // Cyan
        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)  // Magenta
    };

    glUseProgram(shaderID);
    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

    for(size_t i = 0; i < VAOs.size(); ++i) {
        glBindVertexArray(VAOs[i]);
        glUniform4fv(colorLoc, 1, glm::value_ptr(colors[i % colors.size()]));
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glBindVertexArray(0);
}