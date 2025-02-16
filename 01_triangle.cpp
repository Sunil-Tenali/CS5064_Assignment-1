#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

GLuint shaderProgram;
GLuint gridVbo, gridVao, cubeVbo, cubeVao, filledCubeVbo, filledCubeVao, cubeEbo;
glm::mat4 model_matrix = glm::mat4(1.0f);
float rotationX = 0.0f, rotationY = 0.0f;
std::vector<std::pair<glm::vec3, glm::vec3>> gridFilledCells; // Stores position and color of filled cells
glm::vec3 cubePosition(0.0f, 0.0f, 0.0f); // Position of the solid cube
glm::vec3 cubeColor(1.0f, 0.0f, 0.0f); // Color of the solid cube (Red by default)

// Define grid parameters
const int gridSize = 5; // 5×5×5 grid
const float gridSpacing = 1.0f;
const float gridStart = -gridSize / 2.0f;
const float gridEnd = gridSize / 2.0f;

// Define vertices for the grid lines
std::vector<float> gridVertices;

// Define cube vertices with positions and colors
float cubeVertices[] = {
    // Positions        // Colors (Red by default)
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f
};

// Indices for cube faces
unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0, // Back
    4, 5, 6, 6, 7, 4, // Front
    0, 4, 7, 7, 3, 0, // Left
    1, 5, 6, 6, 2, 1, // Right
    3, 2, 6, 6, 7, 3, // Top
    0, 1, 5, 5, 4, 0  // Bottom
};

// Compile and link shaders
GLuint LoadShaderGL(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint CreateProgramGL(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}

void initShadersGL() {
    // Simple vertex shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        uniform mat4 model_matrix;
        uniform mat4 view_matrix;
        uniform mat4 projection_matrix;
        uniform vec3 object_color;
        
        out vec3 ourColor;
        
        void main() {
            gl_Position = projection_matrix * view_matrix * model_matrix * vec4(aPos, 1.0);
            ourColor = object_color;
        }
    )";

    // Simple fragment shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 ourColor;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(ourColor, 1.0);
        }
    )";

    GLuint vertexShader = LoadShaderGL(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = LoadShaderGL(GL_FRAGMENT_SHADER, fragmentShaderSource);
    shaderProgram = CreateProgramGL(vertexShader, fragmentShader);
    
    // Cleanup shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void initGrid() {
    // Lines parallel to X-axis
    for (int y = 0; y <= gridSize; ++y) {
        for (int z = 0; z <= gridSize; ++z) {
            float yPos = gridStart + y * gridSpacing;
            float zPos = gridStart + z * gridSpacing;
            
            gridVertices.push_back(gridStart); gridVertices.push_back(yPos); gridVertices.push_back(zPos);
            gridVertices.push_back(0.5f); gridVertices.push_back(0.5f); gridVertices.push_back(0.5f);
            
            gridVertices.push_back(gridEnd); gridVertices.push_back(yPos); gridVertices.push_back(zPos);
            gridVertices.push_back(0.5f); gridVertices.push_back(0.5f); gridVertices.push_back(0.5f);
        }
    }

    // Lines parallel to Y-axis
    for (int x = 0; x <= gridSize; ++x) {
        for (int z = 0; z <= gridSize; ++z) {
            float xPos = gridStart + x * gridSpacing;
            float zPos = gridStart + z * gridSpacing;
            
            gridVertices.push_back(xPos); gridVertices.push_back(gridStart); gridVertices.push_back(zPos);
            gridVertices.push_back(0.5f); gridVertices.push_back(0.5f); gridVertices.push_back(0.5f);
            
            gridVertices.push_back(xPos); gridVertices.push_back(gridEnd); gridVertices.push_back(zPos);
            gridVertices.push_back(0.5f); gridVertices.push_back(0.5f); gridVertices.push_back(0.5f);
        }
    }

    // Lines parallel to Z-axis
    for (int x = 0; x <= gridSize; ++x) {
        for (int y = 0; y <= gridSize; ++y) {
            float xPos = gridStart + x * gridSpacing;
            float yPos = gridStart + y * gridSpacing;
            
            gridVertices.push_back(xPos); gridVertices.push_back(yPos); gridVertices.push_back(gridStart);
            gridVertices.push_back(0.5f); gridVertices.push_back(0.5f); gridVertices.push_back(0.5f);
            
            gridVertices.push_back(xPos); gridVertices.push_back(yPos); gridVertices.push_back(gridEnd);
            gridVertices.push_back(0.5f); gridVertices.push_back(0.5f); gridVertices.push_back(0.5f);
        }
    }

    // Generate and bind VAO & VBO
    glGenVertexArrays(1, &gridVao);
    glGenBuffers(1, &gridVbo);

    glBindVertexArray(gridVao);
    glBindBuffer(GL_ARRAY_BUFFER, gridVbo);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void initCube() {
    glGenVertexArrays(1, &cubeVao);
    glGenBuffers(1, &cubeVbo);
    glGenBuffers(1, &cubeEbo);

    glBindVertexArray(cubeVao);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void initFilledCube() {
    glGenVertexArrays(1, &filledCubeVao);
    glGenBuffers(1, &filledCubeVbo);
    GLuint filledCubeEbo;
    glGenBuffers(1, &filledCubeEbo);

    glBindVertexArray(filledCubeVao);

    glBindBuffer(GL_ARRAY_BUFFER, filledCubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, filledCubeEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void renderGrid() {
    glBindVertexArray(gridVao);
    glDrawArrays(GL_LINES, 0, gridVertices.size() / 6);
}

void renderCube() {
    glBindVertexArray(cubeVao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void updateCubeColors() {
    // Update the cube's vertex colors in the VBO
    float updatedCubeVertices[48]; // 8 vertices * 6 attributes (position + color)
    for (int i = 0; i < 8; ++i) {
        // Copy the position data
        updatedCubeVertices[i * 6 + 0] = cubeVertices[i * 6 + 0];
        updatedCubeVertices[i * 6 + 1] = cubeVertices[i * 6 + 1];
        updatedCubeVertices[i * 6 + 2] = cubeVertices[i * 6 + 2];

        // Update the color data
        updatedCubeVertices[i * 6 + 3] = cubeColor.r;
        updatedCubeVertices[i * 6 + 4] = cubeColor.g;
        updatedCubeVertices[i * 6 + 5] = cubeColor.b;
    }

    // Update the VBO with the new colors
    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatedCubeVertices), updatedCubeVertices);
}

void renderFilledCells() {
    glBindVertexArray(filledCubeVao);
    for (const auto& cell : gridFilledCells) {
        glm::vec3 position = cell.first;
        glm::vec3 color = cell.second;

        // Create a model matrix for the filled cell
        glm::mat4 cellModel = model_matrix;
        cellModel = glm::translate(cellModel, position);

        // Set the color for the filled cell
        GLuint colorLoc = glGetUniformLocation(shaderProgram, "object_color");
        glUniform3f(colorLoc, color.r, color.g, color.b);

        // Render the filled cell
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model_matrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cellModel));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void clampCubePosition() {
    float minPos = gridStart;
    float maxPos = gridEnd - 1.0f;

    // Ensure the cube stays within grid boundaries
    cubePosition.x = std::max(minPos, std::min(maxPos, cubePosition.x));
    cubePosition.y = std::max(minPos, std::min(maxPos, cubePosition.y));
    cubePosition.z = std::max(minPos, std::min(maxPos, cubePosition.z));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_LEFT: cubePosition.x -= 1.0f; break;
            case GLFW_KEY_RIGHT: cubePosition.x += 1.0f; break;
            case GLFW_KEY_UP: cubePosition.y += 1.0f; break;
            case GLFW_KEY_DOWN: cubePosition.y -= 1.0f; break;
            case GLFW_KEY_U: cubePosition.z += 1.0f; break;
            case GLFW_KEY_B: cubePosition.z -= 1.0f; break;
            case GLFW_KEY_C: {
                std::cout << "Enter RGB values (0.0 to 1.0): ";
                float r, g, b;
                std::cin >> r >> g >> b;

                // Clamp the input values to the range [0.0, 1.0]
                r = std::max(0.0f, std::min(1.0f, r));
                g = std::max(0.0f, std::min(1.0f, g));
                b = std::max(0.0f, std::min(1.0f, b));

                // Update the cube's color
                cubeColor = glm::vec3(r, g, b);
                updateCubeColors();
                break;
            }
            case GLFW_KEY_F: {
                // Check if the cell is already filled
                auto it = std::find_if(gridFilledCells.begin(), gridFilledCells.end(),
                    [&](const std::pair<glm::vec3, glm::vec3>& cell) {
                        return cell.first == cubePosition;
                    });
                if (it == gridFilledCells.end()) {
                    // Fill the cell with the cube's color
                    gridFilledCells.push_back(std::make_pair(cubePosition, cubeColor));
                }
                break;
            }
            case GLFW_KEY_W: {
                // Clear the cell at the cube's position
                auto it = std::find_if(gridFilledCells.begin(), gridFilledCells.end(),
                    [&](const std::pair<glm::vec3, glm::vec3>& cell) {
                        return cell.first == cubePosition;
                    });
                if (it != gridFilledCells.end()) {
                    gridFilledCells.erase(it);
                }
                break;
            }
            case GLFW_KEY_L: rotationY -= 5.0f; break;
            case GLFW_KEY_R: rotationY += 5.0f; break;
            case GLFW_KEY_T: rotationX -= 5.0f; break;
            case GLFW_KEY_D: rotationX += 5.0f; break;
        }

        // Clamp the cube position within the grid boundaries
        clampCubePosition();
    }
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Grid with Cube", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glfwSetKeyCallback(window, key_callback);
    glEnable(GL_DEPTH_TEST);
    
    // Disable face culling to see all faces
    glDisable(GL_CULL_FACE);

    initShadersGL();
    initGrid();
    initCube();
    initFilledCube();
    
    // Initially position the cube at the center of the grid
    // Using grid coordinates that align with cell boundaries
    cubePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    clampCubePosition();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glUseProgram(shaderProgram);

        // Set up view and projection matrices
        glm::mat4 view_matrix = glm::lookAt(
            glm::vec3(7.0f, 7.0f, 7.0f),   // Camera position
            glm::vec3(0.0f, 0.0f, 0.0f),   // Look at point
            glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector
        );
        
        glm::mat4 projection_matrix = glm::perspective(
            glm::radians(45.0f),           // Field of view
            800.0f / 600.0f,               // Aspect ratio
            0.1f,                          // Near plane
            100.0f                         // Far plane
        );

        // Set up rotation matrix for the entire scene
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::rotate(model_matrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        model_matrix = glm::rotate(model_matrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

        // Set uniform values
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model_matrix");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view_matrix");
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
        GLuint colorLoc = glGetUniformLocation(shaderProgram, "object_color");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Render the grid
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f); // Gray color for grid
        renderGrid();

        // Render the filled grid cells
        renderFilledCells();

        // Render the cube at its current position
        glm::mat4 cubeModel = model_matrix;
        cubeModel = glm::translate(cubeModel, cubePosition);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform3f(colorLoc, cubeColor.r, cubeColor.g, cubeColor.b);
        renderCube();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &gridVao);
    glDeleteBuffers(1, &gridVbo);
    glDeleteVertexArrays(1, &cubeVao);
    glDeleteBuffers(1, &cubeVbo);
    glDeleteBuffers(1, &cubeEbo);
    glDeleteVertexArrays(1, &filledCubeVao);
    glDeleteBuffers(1, &filledCubeVbo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
