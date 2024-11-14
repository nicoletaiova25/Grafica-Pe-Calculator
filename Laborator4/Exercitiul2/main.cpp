#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

// Identificatori pentru VAO, VBO, EBO si shader program
GLuint VaoHouse, VboHouse, EboHouse;
GLuint VaoTree, VboTree, EboTree;
GLuint shaderProgram;

// Vertex Shader (cod sursa)
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color; // Atribut pentru culoare
out vec4 vertexColor; // Culoarea ce va fi trimisa la fragment shader
void main() {
    gl_Position = position;
    vertexColor = color; // Transmite culoarea la fragment shader
}
)";

// Fragment Shader (cod sursa)
const char* fragmentShaderSource = R"(
#version 330 core
in vec4 vertexColor; // Culoarea primita de la vertex shader
out vec4 FragColor;
void main() {
    FragColor = vertexColor; // Foloseste culoarea primita
}
)";

// Functia de compilare a shaderului
GLuint CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

// Functia de creare a programului shader
void CreateShaderProgram() {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Functia pentru configurarea VAO-ului casei
void CreateHouseVAO() {
    GLfloat vertices[] = {
        // coordonate             // culori
        -0.5f, -0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // Stanga jos (alb)
         0.3f, -0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // Dreapta jos (alb)
         0.3f,  0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // Dreapta sus (alb)
        -0.5f,  0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // Stanga sus (alb)
        -0.6f,  0.3f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Acoperis stanga jos (maro)
         0.4f,  0.3f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Acoperis dreapta jos (maro)
         -0.1f,  0.6f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f  // Varful acoperisului (maro)
    };

    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,           // Corpul casei
        4, 5, 6                     // Acoperisul casei
    };

    glGenVertexArrays(1, &VaoHouse);
    glBindVertexArray(VaoHouse);

    glGenBuffers(1, &VboHouse);
    glBindBuffer(GL_ARRAY_BUFFER, VboHouse);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EboHouse);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboHouse);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
}

// Functia pentru configurarea VAO-ului copacului
void CreateTreeVAO() {
    GLfloat vertices[] = {
        // coordonate             // culori
        0.5f, -0.3f, 0.0f, 1.0f, 0.6f, 0.3f, 0.0f,  // Trunchi stanga jos (maro)
        0.6f, -0.3f, 0.0f, 1.0f, 0.6f, 0.3f, 0.0f,  // Trunchi dreapta jos (maro)
        0.6f,  0.0f, 0.0f, 1.0f, 0.6f, 0.3f, 0.0f,  // Trunchi dreapta sus (maro)
        0.5f,  0.0f, 0.0f, 1.0f, 0.6f, 0.3f, 0.0f,  // Trunchi stanga sus (maro)
        0.4f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // Prima coroana stanga jos (rosu)
        0.7f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // Prima coroana dreapta jos (rosu)
        0.55f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // Varful primei coroane (rosu)
        0.4f,  0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // A doua coroana stanga jos (rosu)
        0.7f,  0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // A doua coroana dreapta jos (rosu)
        0.55f, 0.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // Varful celei de-a doua coroane (rosu)
        0.4f,  0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // A treia coroana stanga jos (rosu)
        0.7f,  0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // A treia coroana dreapta jos (rosu)
        0.55f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f   // Varful celei de-a treia coroane (rosu)
    };

    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,           // Trunchiul copacului
        4, 5, 6,                    // Prima coroana
        7, 8, 9,                    // A doua coroana
        10, 11, 12                  // A treia coroana
    };

    glGenVertexArrays(1, &VaoTree);
    glBindVertexArray(VaoTree);

    glGenBuffers(1, &VboTree);
    glBindBuffer(GL_ARRAY_BUFFER, VboTree);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EboTree);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTree);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
}

// Functia de desenare
void RenderFunction() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    // Desenam casa
    glBindVertexArray(VaoHouse);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Corpul casei
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); // Acoperisul

    // Desenam copacul
    glBindVertexArray(VaoTree);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Trunchiul copacului
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); // Prima coroana
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint))); // A doua coroana
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint))); // A treia coroana

    glutSwapBuffers();
}

// Functia principala
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Casa si Copac OpenGL cu 2 VAO");

    glewInit();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    CreateShaderProgram();
    CreateHouseVAO();
    CreateTreeVAO();

    glutDisplayFunc(RenderFunction);

    glutMainLoop();

    // Eliberam memoria
    glDeleteVertexArrays(1, &VaoHouse);
    glDeleteBuffers(1, &VboHouse);
    glDeleteBuffers(1, &EboHouse);
    glDeleteVertexArrays(1, &VaoTree);
    glDeleteBuffers(1, &VboTree);
    glDeleteBuffers(1, &EboTree);
    glDeleteProgram(shaderProgram);

    return 0;
}
