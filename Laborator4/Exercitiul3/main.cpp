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
#include "SOIL.h"

GLuint VaoId, VboId, EboId, ProgramId, myMatrixLocation, texture;
GLfloat winWidth = 800, winHeight = 600;
glm::mat4 myMatrix, resizeMatrix;
float xMin = -80, xMax = 80.f, yMin = -60.f, yMax = 60.f;

void CreateShaders(void)
{
    ProgramId = LoadShaders("04_02_Shader.vert", "04_02_Shader.frag");
    glUseProgram(ProgramId);
}

void LoadTexture(const char* texturePath)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height;
    unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateVBO(void)
{
    static const GLfloat Vertices[] = {
        -15.0f, -15.0f,  0.0f,  1.0f,
         15.0f, -15.0f,  0.0f,  1.0f,
         15.0f,  15.0f,  0.0f,  1.0f,
        -15.0f,  15.0f,  0.0f,  1.0f
    };

    static const GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f
    };

    static const GLfloat TexCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    static const GLuint Indices[] = {
        0, 1, 2, 3, 0, 2
    };

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors) + sizeof(Vertices) + sizeof(TexCoords), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);             // Coordonate
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);  // Culori
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), sizeof(TexCoords), TexCoords); // Coordonate de textura

    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Asocierea atributelor (coordonate, culori, textura)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(Vertices));

    glEnableVertexAttribArray(2); // Coordonate de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(Vertices) + sizeof(Colors)));
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
    glDeleteBuffers(1, &VboId);
    glDeleteBuffers(1, &EboId);
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
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
    LoadTexture("text_smiley_face.png");
}

void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    myMatrix = resizeMatrix;
    myMatrix *= glm::mat4(0.25, 0, 0, 0,    // Scalare redusă pe axa x (din 2 la 1)
        0, 0.25, 0, 0,   // Scalare redusă pe axa y (din 0.5 la 0.25)
        0, 0, 1, 0,
        0, 0, 0, 1);
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    GLint codColLocation = glGetUniformLocation(ProgramId, "codCol");

    // Primul obiect (fara textura)
    int codColValue = 0;
    glUniform1i(codColLocation, codColValue);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

    // Al doilea obiect (cu textura, scalare si translatie)
    myMatrix *= glm::mat4(3.0, 0, 0, 0,    // Scalare redusă pe axa x (din 2 la 1)
        0, 0.8, 0, 0,   // Scalare redusă pe axa y (din 0.5 la 0.25)
        3, 0, 1, 0,
        0, 0, 0, 1);
    glm::vec3 translationVector = glm::vec3(20.0f, 50.0f, 0.0f);
    myMatrix = glm::translate(myMatrix, translationVector);
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    codColValue = 2;  // Aplicam textura pe al doilea obiect
    glUniform1i(codColLocation, codColValue);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

    // Al treilea obiect (fara textura)
    myMatrix = resizeMatrix;
       // Setăm coordonata de translatie mai jos (de exemplu, 30.0f)
        glm::vec3 newTranslationVector = glm::vec3(20.0f, 20.0f, 0.0f);  // Ajustăm coordonata y

    myMatrix = glm::translate(myMatrix, newTranslationVector);
    myMatrix *= glm::mat4(1.0, 0, 0, 0,     // Scalare redusă pe axa x
        0, 0.25, 0, 0,    // Scalare redusă pe axa y
        0, 0, 1, 0,
        0, 0, 0, 1);
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    codColValue = 1;  // Obiectul al treilea fara textura
    glUniform1i(codColLocation, codColValue);
    glBindTexture(GL_TEXTURE_2D, 0);  // Dezactiveaza textura pentru acest obiect
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

    glFlush();
}


int main(int argc, char* argv[])
{
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
