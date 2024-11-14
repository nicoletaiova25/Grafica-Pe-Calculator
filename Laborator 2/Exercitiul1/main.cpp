#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"

GLuint VaoId, VboId, ColorBufferId, ProgramId, codColLocation;
GLint winWidth = 600, winHeight = 400;
int codCol;

void CreateVBO(void)
{
    GLfloat Vertices[] = {
        -0.2f, -0.2f, 0.0f, 1.0f,
         0.0f,  0.2f, 0.0f, 1.0f,
         0.2f, -0.2f, 0.0f, 1.0f,
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.0f,  0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };

    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyVBO(void)
{
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void)
{
    ProgramId = LoadShaders("02_01_Shader.vert", "02_01_Shader.frag");
    glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
    glDeleteProgram(ProgramId);
}

void Initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    CreateVBO();
    CreateShaders();
    codColLocation = glGetUniformLocation(ProgramId, "codColShader");
}

void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(5.0);
    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glDrawArrays(GL_LINE_STRIP, 0, 6);

    glEnable(GL_POINT_SMOOTH);
    glPointSize(20.0);

    // Prima pereche de puncte (aceeași culoare)
    codCol = 1; // Culoare mov
    glUniform1i(codColLocation, codCol);
    glDrawArrays(GL_POINTS, 0, 2);  // Desenează punctele 0 și 1 cu aceeași culoare

    // A doua pereche de puncte (aceeași culoare)
    codCol = 2; // Culoare galbenă
    glUniform1i(codColLocation, codCol);
    glDrawArrays(GL_POINTS, 2, 2);  // Desenează punctele 2 și 3 cu aceeași culoare

    // A treia pereche de puncte (aceeași culoare)
    codCol = 3; // Culoare cyan
    glUniform1i(codColLocation, codCol);
    glDrawArrays(GL_POINTS, 4, 2);  // Desenează punctele 4 și 5 cu aceeași culoare

    glDisable(GL_POINT_SMOOTH);
    glFlush();
}

void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Primitive si culori - OpenGL <<nou>>");

    glewInit();
    Initialize();
    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);

    glutMainLoop();
    return 0;
}
