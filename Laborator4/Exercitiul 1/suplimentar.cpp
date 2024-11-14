#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint VaoId, VboId, EboId, ProgramId, myMatrixLocation;
GLfloat winWidth = 800, winHeight = 600;
glm::mat4 myMatrix, resizeMatrix;
float xMin = -80, xMax = 80.f, yMin = -60.f, yMax = 60.f;

void CreateShaders(void) {
    ProgramId = LoadShaders("04_02_Shader.vert", "04_02_Shader.frag");
    glUseProgram(ProgramId);
}

void CreateVBO(void) {
    const int n = 8;
    const float radius_outer = 30.0f;
    const float radius_inner = 15.0f;
    const float PI = 3.14159265359f;
    GLfloat Vertices[8 * 2 * 4];

    for (int i = 0; i < n; ++i) {
        float angle = 2 * PI * i / n;

        // Outer circle
        Vertices[i * 4] = radius_outer * cos(angle);
        Vertices[i * 4 + 1] = radius_outer * sin(angle);
        Vertices[i * 4 + 2] = 0.0f;
        Vertices[i * 4 + 3] = 1.0f;

        // Inner circle
        Vertices[(i + n) * 4] = radius_inner * cos(angle);
        Vertices[(i + n) * 4 + 1] = radius_inner * sin(angle);
        Vertices[(i + n) * 4 + 2] = 0.0f;
        Vertices[(i + n) * 4 + 3] = 1.0f;
    }

    GLuint Indices[12 * n];
    int index = 0;
    for (int i = 0; i < n; ++i) {
        int next = (i + 1) % n;

        // Triunghiul exterior
        Indices[index++] = i;
        Indices[index++] = next;
        Indices[index++] = i + n;

        // Triunghiul interior
        Indices[index++] = next;
        Indices[index++] = next + n;
        Indices[index++] = i + n;
    }

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
}


void DestroyShaders(void) {
    glDeleteProgram(ProgramId);
}

void DestroyVBO(void) {
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboId);
    glDeleteBuffers(1, &EboId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void Cleanup(void) {
    DestroyShaders();
    DestroyVBO();
}

void Initialize(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    CreateVBO();
    CreateShaders();
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);

    // Setarea pentru a desena doar conturul (nu umplere)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void RenderFunction(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(3.0f);

    myMatrix = resizeMatrix;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    glDrawElements(GL_TRIANGLES, 6 * 8, GL_UNSIGNED_INT, (void*)(0));

    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Indexarea varfurilor - OpenGL <<nou>>");
    glewInit();

    Initialize();
    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);
    glutMainLoop();

    return 0;
}
