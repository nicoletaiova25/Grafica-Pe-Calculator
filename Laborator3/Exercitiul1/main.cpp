#include <iostream>
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

using namespace std;

// Identificatori OpenGL
GLuint
VaoId,
VboId,
ColorBufferId,
ProgramId,
myMatrixLocation,
codColLocation;

// Dimensiunile ferestrei
GLfloat winWidth = 800, winHeight = 600;

// Matricile de transformare
glm::mat4 myMatrix, resizeMatrix, matrTransl, matrRot;

// Variabile de control pentru deplasare si rotatie
float i = 0.0f, alpha = 0.0f, step = 0.3f, angle = 0.0f, beta = 0.002f;

// Functie pentru animarea patratului spre dreapta
void MoveRight(void) {
    i += step;
    if (i > 350.0f) step = -0.3f;  // Schimba directia la margine
    if (i < -350.0f) step = 0.3f;

    angle += beta;  // Incrementare unghi rotatie
    glutPostRedisplay();  // Reimprospatare ecran
}

void CreateVBO(void)
{
    //  Coordonatele varfurilor patratului si axelor;
    GLfloat Vertices[] = {
        // Varfuri pentru axe;
        -450.0f, 0.0f, 0.0f, 1.0f,  // linie orizontala
         450.0f, 0.0f, 0.0f, 1.0f,
         0.0f, -300.0f, 0.0f, 1.0f, // linie verticala
         0.0f, 300.0f, 0.0f, 1.0f,

         // Varfuri pentru patrat;
         -50.0f, -50.0f, 0.0f, 1.0f, // colt stanga-jos
         50.0f, -50.0f, 0.0f, 1.0f,  // colt dreapta-jos
         50.0f, 50.0f, 0.0f, 1.0f,   // colt dreapta-sus
         -50.0f, 50.0f, 0.0f, 1.0f   // colt stanga-sus
    };

    //  Culorile axelor si patratului;
    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,  // culoare rosu pentru axa Ox
        0.0f, 1.0f, 0.0f, 1.0f,  // verde pentru axa Oy
        0.0f, 0.0f, 1.0f, 1.0f,  // culoare pentru patrat
        0.0f, 0.0f, 1.0f, 1.0f,  // idem
        0.0f, 0.0f, 1.0f, 1.0f,  // idem
        0.0f, 0.0f, 1.0f, 1.0f   // idem
    };

    //  Creare VAO;
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    //  Creare buffer pentru varfuri;
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    //  Creare buffer pentru culori;
    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void CreateShaders(void)
{
    ProgramId = LoadShaders("03_03_Shader.vert", "03_03_Shader.frag");
    glUseProgram(ProgramId);

    // Identificarea variabilelor din shader
    codColLocation = glGetUniformLocation(ProgramId, "codCol");
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
}


// Functia de redare a graficii
void RenderFunction(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Matrice de redimensionare pentru proiectie ortogonala
    resizeMatrix = glm::ortho(-450.0f, 450.0f, -300.0f, 300.0f);

    // Matricea de translatie pentru deplasare pe Ox
    matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, 0.0f));

    // Matricea de rotatie
    matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

    // Compunerea matricei finale pentru deplasare si rotatie
    myMatrix = resizeMatrix * matrTransl * matrRot;

    // Transmiterea matricii transformate si a culorii
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
    glUniform1i(codColLocation, 1);  // Setam culoarea

    // Desenarea patratului
    glDrawArrays(GL_POLYGON, 4, 4);

    glutSwapBuffers();
    glFlush();
}

// Functie de initializare OpenGL
void Initialize(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    CreateVBO();
    CreateShaders();
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    codColLocation = glGetUniformLocation(ProgramId, "codCol");
}

// Functia main
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Patrat ");

    glewInit();
    Initialize();
    glutDisplayFunc(RenderFunction);
    glutIdleFunc(MoveRight);  // Pornim animatia in bucla
    glutMainLoop();

    return 0;
}
