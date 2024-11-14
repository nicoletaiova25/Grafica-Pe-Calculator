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

GLuint VaoId, VboId, ColorBufferId, ProgramId, myMatrixLocation, codColLocation;
GLfloat winWidth = 900, winHeight = 600;
glm::mat4 myMatrix, resizeMatrix, matrTransl;
int codCol;
float xMin = 0, xMax = 800, yMin = 0, yMax = 600;
float deltax = xMax - xMin, deltay = yMax - yMin, xcenter = (xMin + xMax) * 0.5, ycenter = (yMin + yMax) * 0.5;

void CreateShaders(void)
{
    ProgramId = LoadShaders("03_03_Shader.vert", "03_03_Shader.frag");
    glUseProgram(ProgramId);
}

void CreateVBO(void)
{
    GLfloat Vertices[] = {
        // Poligon convex (P1)
        250.0f, 200.0f, 0.0f, 1.0f, // Punct 1
        275.0f, 200.0f, 0.0f, 1.0f, // Punct 2
        285.0f, 225.0f, 0.0f, 1.0f, // Punct 3
        265.0f, 235.0f, 0.0f, 1.0f, // Punct 4
        250.0f, 225.0f, 0.0f, 1.0f, // Punct 5

        // Poligon concav (P2)
        500.0f, 200.0f, 0.0f, 1.0f,  // Punct 1
        525.0f, 200.0f, 0.0f, 1.0f,  // Punct 2
        512.5f, 235.0f, 0.0f, 1.0f,  // Punct 3 (mijloc)
        525.0f, 270.0f, 0.0f, 1.0f,  // Punct 4
        500.0f, 270.0f, 0.0f, 1.0f,  // Punct 5

        // Puncte pentru axele de coordonate
        400.0f, 0.0f, 0.0f, 1.0f,   // Punctul de jos pentru axa Y
        400.0f, 600.0f, 0.0f, 1.0f, // Punctul de sus pentru axa Y
        0.0f, 300.0f, 0.0f, 1.0f,   // Punctul din stanga pentru axa X
        800.0f, 300.0f, 0.0f, 1.0f  // Punctul din dreapta pentru axa X
    };

    // Culori monocrome pentru fiecare poligon si axele de coordonate
    GLfloat Colors[] = {
        // Culoare uniforma pentru poligonul convex (P1) - rosu
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,

        // Culoare uniforma pentru poligonul concav (P2) - albastru
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        // Culoare pentru axele de coordonate - negru
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
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

void DestroyShaders(void)
{
    glDeleteProgram(ProgramId);
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

void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
}

void Initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    CreateVBO();
    CreateShaders();
    codColLocation = glGetUniformLocation(ProgramId, "codCol");
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
    matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    myMatrix = resizeMatrix;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    // Desenare poligon convex (P1) - micsorat, monocrom (rosu)
    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Deseneaza interiorul
    glDrawArrays(GL_POLYGON, 0, 5);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Deseneaza conturul
    glDrawArrays(GL_POLYGON, 0, 5);

    // Desenare poligon concav (P2) - micsorat, monocrom (albastru)
    codCol = 1;
    glUniform1i(codColLocation, codCol);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Deseneaza interiorul
    glDrawArrays(GL_TRIANGLE_FAN, 5, 5);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Deseneaza conturul
    glDrawArrays(GL_TRIANGLE_FAN, 5, 5);

    // Desenare axe de coordonate - monocrome (negru)
    codCol = 2;
    glUniform1i(codColLocation, codCol);
    glLineWidth(2.0f);  // Latimea liniei pentru axele de coordonate
    glDrawArrays(GL_LINES, 10, 2);  // Deseneaza axa Y
    glDrawArrays(GL_LINES, 12, 2);  // Deseneaza axa X

    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Poligoane Convexe si Concave cu Axe de Coordonate");

    glewInit();
    Initialize();
    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);
    glutMainLoop();

    return 0;
}
