#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint VaoId, VboId, ColorBufferId, ProgramId, myMatrixLocation;
GLfloat winWidth = 900, winHeight = 600;
glm::mat4 myMatrix, resizeMatrix;
float angle = 0.0f;  // Unghiul de rotație
float translateX = 0.0f;  // Translația pe axa X

void CreateShaders(void) {
    ProgramId = LoadShaders("03_03_Shader.vert", "03_03_Shader.frag");
    glUseProgram(ProgramId);
}

void CreateVBO(void) {
    GLfloat Vertices[] = {
        // Pătratul
        -25.0f, -25.0f, 0.0f, 1.0f,  // stânga jos
        25.0f, -25.0f, 0.0f, 1.0f,   // dreapta jos
        25.0f, 25.0f, 0.0f, 1.0f,    // dreapta sus
        -25.0f, 25.0f, 0.0f, 1.0f    // stânga sus
    };

    GLfloat Colors[] = {
        // Culoarea pătratului (albastru)
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
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

void DestroyShaders(void) {
    glDeleteProgram(ProgramId);
}

void DestroyVBO(void) {
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

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
    resizeMatrix = glm::ortho(0.0f, winWidth, 0.0f, winHeight);
}

void RenderFunction(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Aplica rotația și translația
    myMatrix = resizeMatrix;  // Resetăm matricea
    myMatrix = glm::translate(myMatrix, glm::vec3(translateX + winWidth / 2, winHeight / 3, 0.0f));  // Translatare
    myMatrix = glm::rotate(myMatrix, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotație
    myMatrix = glm::translate(myMatrix, glm::vec3(-25.0f, -25.0f, 0.0f));  // Centrare pătrat

    // Trimitem matricea la shader
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    // Desenăm pătratul
    glDrawArrays(GL_QUADS, 0, 4);

    glFlush();
}

// Funcția pentru control cu taste
void KeyboardFunction(unsigned char key, int x, int y) {
    if (key == 'a') {
        translateX -= 10.0f;  // Mișcare la stânga
        angle -= 10.0f;       // Rotație în sens invers acelor de ceasornic
    }
    if (key == 'd') {
        translateX += 10.0f;  // Mișcare la dreapta
        angle += 10.0f;       // Rotație în sensul acelor de ceasornic
    }

    glutPostRedisplay();  // Reînnoim fereastra
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pătrat care se rostogolește");

    glewInit();
    Initialize();
    glutDisplayFunc(RenderFunction);
    glutKeyboardFunc(KeyboardFunction);  // Controlul cu tastatura
    glutCloseFunc(Cleanup);
    glutMainLoop();

    return 0;
}
