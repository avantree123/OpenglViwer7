#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "sphere_scene.h" // �� ������ ���� ���

// --- �Լ� ���� ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string loadShaderSource(const std::string& filePath);
unsigned int compileShader(unsigned int type, const std::string& source);
unsigned int createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
void setUniforms(unsigned int shaderProgram);

// --- ���� ���� ---
const unsigned int SCR_WIDTH = 512;
const unsigned int SCR_HEIGHT = 512;

// ���� �� ���� �Ķ���� (HW6�� ����)
const glm::vec3 mat_ka = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 mat_kd = glm::vec3(0.0f, 0.5f, 0.0f);
const glm::vec3 mat_ks = glm::vec3(0.5f, 0.5f, 0.5f);
const float mat_p_shininess = 32.0f;
const float light_Ia_intensity = 0.2f;
const glm::vec3 light_pos_world = glm::vec3(-4.0f, 4.0f, -3.0f);
const glm::vec3 light_Il_intensity = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 eye_pos_world = glm::vec3(0.0f, 0.0f, 0.0f);
const float gamma_val = 2.2f;

// ���
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat3 normalMatrix;

// --- ���� �Լ� ---
int main() {
    // 1. GLFW �ʱ�ȭ �� â ����
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW7 - OpenGL Phong Shader", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 2. GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 3. �� ������ ����
    create_scene();
    if (!gVertexBuffer || !gIndexBuffer) {
        std::cerr << "Failed to create scene geometry" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 4. ���̴� �ε� �� ������
    std::string vertexShaderSource = loadShaderSource("Phong.vert");
    std::string fragmentShaderSource = loadShaderSource("Phong.frag");
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        delete_scene();
        glfwTerminate();
        return -1;
    }
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        delete_scene();
        glfwTerminate();
        return -1;
    }

    // 5. VBO, VAO, EBO ����
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ���� ��ġ�� ��� �����͸� ���ļ� VBO�� �ε� (��ġ�� ����� �����ϹǷ� gVertexBuffer �� �� ��� ����)
    // �Ǵ�, ���͸��� ������� (��ġ, ���, ��ġ, ���...) VBO�� ���� �� ������, ���⼭�� �����ϰ� ó��.
    // ���� ��� ��ġ=����̹Ƿ� gVertexBuffer�� ���.
    glBufferData(GL_ARRAY_BUFFER, gNumVertices * sizeof(glm::vec3), gVertexBuffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gNumTriangles * 3 * sizeof(int), gIndexBuffer, GL_STATIC_DRAW);

    // ���� ��ġ �Ӽ� ���� (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    // ���� ��� �Ӽ� ���� (location = 1) - ��ġ �����Ϳ� ������ VBO ���
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 6. ��� ��� (HW6�� ����)
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
    viewMatrix = glm::lookAt(eye_pos_world, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float nearVal = 0.1f;
    float farVal = 1000.0f;
    projectionMatrix = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, nearVal, farVal);
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    // 7. OpenGL ����
    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // ���� ����

    // 8. ������ ����
    while (!glfwWindowShouldClose(window)) {
        // �Է� ó��
        processInput(window);

        // ������
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ���̴� ���α׷� ���
        glUseProgram(shaderProgram);

        // ������ ���� ����
        setUniforms(shaderProgram);

        // VAO ���ε� �� �׸���
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, gNumTriangles * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // VAO ���ε� ����

        // ���� ���� �� �̺�Ʈ ����
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 9. �ڿ� ����
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    //delete_scene();
    glfwTerminate();

    return 0;
}

// ���̴� ���� �ε�
std::string loadShaderSource(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Error: Could not open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

// ���̴� ������
unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

// ���̴� ���α׷� ���� �� ��ũ
unsigned int createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (vs == 0 || fs == 0) return 0;

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cerr << "Failed to link shader program!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// ������ ���� ����
void setUniforms(unsigned int shaderProgram) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glUniform3fv(glGetUniformLocation(shaderProgram, "eyePosWorld"), 1, glm::value_ptr(eye_pos_world));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosWorld"), 1, glm::value_ptr(light_pos_world));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightIl"), 1, glm::value_ptr(light_Il_intensity));
    glUniform1f(glGetUniformLocation(shaderProgram, "lightIa"), light_Ia_intensity);

    glUniform3fv(glGetUniformLocation(shaderProgram, "matKa"), 1, glm::value_ptr(mat_ka));
    glUniform3fv(glGetUniformLocation(shaderProgram, "matKd"), 1, glm::value_ptr(mat_kd));
    glUniform3fv(glGetUniformLocation(shaderProgram, "matKs"), 1, glm::value_ptr(mat_ks));
    glUniform1f(glGetUniformLocation(shaderProgram, "matShininess"), mat_p_shininess);

    glUniform1f(glGetUniformLocation(shaderProgram, "gamma"), gamma_val);
}


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// â ũ�� ���� �ݹ�
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}