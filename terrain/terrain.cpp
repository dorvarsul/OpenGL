
// main.cpp - OpenGL Terrain Visualization
// Visualizes procedural terrain using Perlin, Simplex, or Diamond-Square noise
// Dependencies: GLFW, GLEW, GLM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "noise.h"

// Window dimensions
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

// Terrain parameters
const int TERRAIN_WIDTH = 256;
const int TERRAIN_HEIGHT = 256;
const float TERRAIN_SCALE = 0.5f;
const float HEIGHT_SCALE = 20.0f;
const float NOISE_SCALE = 0.02f;

// Camera
glm::vec3 cameraPos = glm::vec3(TERRAIN_WIDTH * TERRAIN_SCALE / 2.0f, 30.0f,
                                TERRAIN_HEIGHT *TERRAIN_SCALE / 2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = -15.0f;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Noise type
enum NoiseType { PERLIN, SIMPLEX, DIAMOND_SQUARE };
NoiseType currentNoise = PERLIN;

// Vertex structure
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
std::vector<Vertex> generateTerrainVertices(NoiseType type);
std::vector<unsigned int> generateTerrainIndices();
glm::vec3 calculateNormal(const std::vector<Vertex> &vertices, int x, int z,
                          int width);
glm::vec3 getColorForHeight(float height);

// Vertex Shader
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Color = aColor;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Fragment Shader
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}
)";

int main() {
  // Initialize GLFW
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

  // Create window
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                        "Procedural Terrain", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // Capture mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Configure OpenGL
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Compile shaders
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
  }

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "Shader program linking failed: " << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Generate terrain
  std::cout << "Generating terrain with Perlin noise..." << std::endl;
  std::vector<Vertex> vertices = generateTerrainVertices(PERLIN);
  std::vector<unsigned int> indices = generateTerrainIndices();
  std::cout << "Generated " << vertices.size() << " vertices and "
            << indices.size() / 3 << " triangles" << std::endl;

  // Create VAO, VBO, EBO
  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  // Color attribute
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    // Timing
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Input
    processInput(window);

    // Render
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader
    glUseProgram(shaderProgram);

    // Set matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,
        1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1,
                       GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));

    // Set lighting
    glm::vec3 lightPos(TERRAIN_WIDTH * TERRAIN_SCALE / 2.0f, 100.0f,
                       TERRAIN_HEIGHT * TERRAIN_SCALE / 2.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1,
                 glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1,
                 glm::value_ptr(cameraPos));
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f,
                1.0f);

    // Draw terrain
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

std::vector<Vertex> generateTerrainVertices(NoiseType type) {
  std::vector<Vertex> vertices;
  vertices.reserve(TERRAIN_WIDTH * TERRAIN_HEIGHT);

  for (int z = 0; z < TERRAIN_HEIGHT; z++) {
    for (int x = 0; x < TERRAIN_WIDTH; x++) {
      Vertex vertex;

      // Calculate height based on noise type
      float height = 0.0f;

      switch (type) {
      case PERLIN:
        height = generatePerlinFBM(x * NOISE_SCALE, z * NOISE_SCALE, 6, 0.5,
                                   2.0, 12345);
        break;
      case SIMPLEX:
        height = generateSimplexFBM(x * NOISE_SCALE, z * NOISE_SCALE, 5, 0.5,
                                    2.0, 54321);
        break;
      case DIAMOND_SQUARE:
        // For diamond-square, we'd need to pre-generate the heightmap
        // Using Perlin as fallback for now
        height = generatePerlinFBM(x * NOISE_SCALE, z * NOISE_SCALE, 6, 0.5,
                                   2.0, 12345);
        break;
      }

      vertex.position = glm::vec3(x * TERRAIN_SCALE, height * HEIGHT_SCALE,
                                  z * TERRAIN_SCALE);
      vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Will be recalculated
      vertex.color = getColorForHeight(height);

      vertices.push_back(vertex);
    }
  }

  // Calculate normals
  for (int z = 0; z < TERRAIN_HEIGHT; z++) {
    for (int x = 0; x < TERRAIN_WIDTH; x++) {
      int index = z * TERRAIN_WIDTH + x;
      vertices[index].normal = calculateNormal(vertices, x, z, TERRAIN_WIDTH);
    }
  }

  return vertices;
}

std::vector<unsigned int> generateTerrainIndices() {
  std::vector<unsigned int> indices;
  indices.reserve((TERRAIN_WIDTH - 1) * (TERRAIN_HEIGHT - 1) * 6);

  for (int z = 0; z < TERRAIN_HEIGHT - 1; z++) {
    for (int x = 0; x < TERRAIN_WIDTH - 1; x++) {
      unsigned int topLeft = z * TERRAIN_WIDTH + x;
      unsigned int topRight = topLeft + 1;
      unsigned int bottomLeft = (z + 1) * TERRAIN_WIDTH + x;
      unsigned int bottomRight = bottomLeft + 1;

      // First triangle
      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);

      // Second triangle
      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }

  return indices;
}

glm::vec3 calculateNormal(const std::vector<Vertex> &vertices, int x, int z,
                          int width) {
  int index = z * width + x;

  float heightL =
      (x > 0) ? vertices[index - 1].position.y : vertices[index].position.y;
  float heightR = (x < width - 1) ? vertices[index + 1].position.y
                                  : vertices[index].position.y;
  float heightD =
      (z > 0) ? vertices[index - width].position.y : vertices[index].position.y;
  float heightU = (z < width - 1) ? vertices[index + width].position.y
                                  : vertices[index].position.y;

  glm::vec3 normal;
  normal.x = heightL - heightR;
  normal.y = 2.0f * TERRAIN_SCALE;
  normal.z = heightD - heightU;

  return glm::normalize(normal);
}

glm::vec3 getColorForHeight(float height) {
  // Color based on height
  if (height < 0.3f) {
    // Water/Low: Blue-green
    return glm::mix(glm::vec3(0.2f, 0.4f, 0.8f), glm::vec3(0.4f, 0.6f, 0.3f),
                    height / 0.3f);
  } else if (height < 0.5f) {
    // Grass: Green
    return glm::mix(glm::vec3(0.4f, 0.6f, 0.3f), glm::vec3(0.3f, 0.5f, 0.2f),
                    (height - 0.3f) / 0.2f);
  } else if (height < 0.7f) {
    // Hills: Brown-green
    return glm::mix(glm::vec3(0.3f, 0.5f, 0.2f), glm::vec3(0.5f, 0.4f, 0.3f),
                    (height - 0.5f) / 0.2f);
  } else if (height < 0.85f) {
    // Mountain: Gray-brown
    return glm::mix(glm::vec3(0.5f, 0.4f, 0.3f), glm::vec3(0.5f, 0.5f, 0.5f),
                    (height - 0.7f) / 0.15f);
  } else {
    // Peak: White (snow)
    return glm::mix(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.9f, 0.9f, 0.95f),
                    (height - 0.85f) / 0.15f);
  }
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = 25.0f * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPos.y += cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cameraPos.y -= cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  // Optional: implement zoom
}
