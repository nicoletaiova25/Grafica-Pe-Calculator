/*
Desenarea a doua obiecte folosind matrice/buffere diferite pentru varfuri/culori/indici.
*/

#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"
#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp" 
#include "glm/gtc/type_ptr.hpp"
using namespace std;

// identificatori 
GLuint
VaoId1, VaoId2, VaoId3,
VboId1, VboId2, VboId3,
EboId1, EboId2, EboId3,
ProgramId,
viewLocation,
projLocation,
codColLocation;

//GLuint VaoIdCone;
GLuint VaoIdCone, VboIdCone, EboIdCone;


float const PI = 3.141592f;
float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;
int const NR_PARR = 10, NR_MERID = 20;
float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;

// alte variabile
int codCol;
float radius = 50;
int index, index_aux;
int const coneSlices = 20; // Numarul de subdiviziuni pentru baza

// variabile pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = 0.0f, beta = 0.0f, dist = 300.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0f, Vy = 0.0f, Vz = -1.0f;
float incr_alpha1 = 0.01f, incr_alpha2 = 0.01f;

// variabile pentru matricea de proiectie
float width = 800, height = 600, znear = 1, fov = 30;

// pentru fereastra de vizualizare 
GLint winWidth = 1000, winHeight = 600;

// vectori
glm::vec3 Obs, PctRef, Vert;

// matrice utilizate
glm::mat4 view, projection;

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {
	case '-':
		dist -= 5.0;
		break;
	case '+':
		dist += 5.0;
		break;
	}
	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		beta -= 0.01f;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01f;
		break;
	case GLUT_KEY_UP:
		alpha += incr_alpha1;
		if (abs(alpha - PI / 2) < 0.05)
		{
			incr_alpha1 = 0.f;
		}
		else
		{
			incr_alpha1 = 0.01f;
		}
		break;
	case GLUT_KEY_DOWN:
		alpha -= incr_alpha2;
		if (abs(alpha + PI / 2) < 0.05)
		{
			incr_alpha2 = 0.f;
		}
		else
		{
			incr_alpha2 = 0.01f;
		}
		break;
	}
}
void CreateVAO1(void)
{
	// SFERA 1
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices1[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors1[(NR_PARR + 1) * NR_MERID];
	GLushort Indices1[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf = radius * sinf(u);

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices1[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors1[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices1[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices1[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices1[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices1[AUX + 4 * index + 1] = index2;
				Indices1[AUX + 4 * index + 2] = index3;
				Indices1[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId1);
	glBindVertexArray(VaoId1);
	glGenBuffers(1, &VboId1); // atribute
	glGenBuffers(1, &EboId1); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1) + sizeof(Colors1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices1), sizeof(Colors1), Colors1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices1), Indices1, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices1)));
}

void CreateVAO2(void)
{
	// SFERA 2
	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices2[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors2[(NR_PARR + 1) * NR_MERID];
	GLushort Indices2[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius / 1.5 * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius / 1.5 * cosf(u) * sinf(v);
			float z_vf = radius / 1.5 * sinf(u) + 80;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices2[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors2[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices2[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices2[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices2[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices2[AUX + 4 * index + 1] = index2;
				Indices2[AUX + 4 * index + 2] = index3;
				Indices2[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId2);
	glBindVertexArray(VaoId2);
	glGenBuffers(1, &VboId2); // atribute
	glGenBuffers(1, &EboId2); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2), sizeof(Colors2), Colors2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices2)));
}

void CreateVAO3(void)
{
	// SFERA 3


	// Matricele pentru varfuri, culori, indici
	glm::vec4 Vertices3[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors3[(NR_PARR + 1) * NR_MERID];
	GLushort Indices3[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius / 2.2 * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius / 2.2 * cosf(u) * sinf(v);
			float z_vf = radius / 2.2 * sinf(u) + 135;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices3[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors3[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices3[index] = index;

			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices3[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices3[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices3[AUX + 4 * index + 1] = index2;
				Indices3[AUX + 4 * index + 2] = index3;
				Indices3[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId3);
	glBindVertexArray(VaoId3);
	glGenBuffers(1, &VboId3); // atribute
	glGenBuffers(1, &EboId3); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices3) + sizeof(Colors3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices3), Vertices3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices3), sizeof(Colors3), Colors3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices3), Indices3, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices3)));
}

void CreateVAOCone(void) {
	// Parametrii conului
	
	float coneHeight = 50.0f;
	float coneRadius = 15.0f;
	float coneBaseZ = 135 + radius / 2.2; // Pozitioneaza baza conului deasupra sferei nr. 3

	// Matrice pentru varfuri si culori
	glm::vec4 coneVertices[coneSlices + 2]; // Adaugam un varf pentru centru si unul pentru varful conului
	glm::vec3 coneColors[coneSlices + 2];
	GLushort coneIndices[coneSlices * 3];

	// Varful conului
	coneVertices[0] = glm::vec4(0.0f, 0.0f, coneBaseZ + coneHeight, 1.0f);
	coneColors[0] = glm::vec3(1.0f, 0.0f, 0.0f); // Rosu

	// Centru baza
	coneVertices[1] = glm::vec4(0.0f, 0.0f, coneBaseZ, 1.0f);
	coneColors[1] = glm::vec3(0.5f, 0.5f, 0.5f); // Gri

	// Varfurile bazei
	for (int i = 0; i < coneSlices; i++) {
		float angle = 2.0f * PI * i / coneSlices;
		coneVertices[i + 2] = glm::vec4(
			coneRadius * cosf(angle),
			coneRadius * sinf(angle),
			coneBaseZ,
			1.0f
		);
		coneColors[i + 2] = glm::vec3(0.0f, 0.0f, 1.0f); // Albastru
	}

	// Indicii pentru triunghiuri
	for (int i = 0; i < coneSlices; i++) {
		coneIndices[3 * i] = 0; // Varful conului
		coneIndices[3 * i + 1] = i + 2; // Varful actual al bazei
		coneIndices[3 * i + 2] = (i + 1) % coneSlices + 2; // Urmatorul varf al bazei
	}

	// Generare VAO si buffere
	//GLuint VaoIdCone, VboIdCone, EboIdCone;
	glGenVertexArrays(1, &VaoIdCone);
	glBindVertexArray(VaoIdCone);
	glGenBuffers(1, &VboIdCone); // atribute
	glGenBuffers(1, &EboIdCone); // indici

	// Legare si incarcare buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboIdCone);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coneVertices) + sizeof(coneColors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(coneVertices), coneVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(coneVertices), sizeof(coneColors), coneColors);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboIdCone);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coneIndices), coneIndices, GL_STATIC_DRAW);

	// Atributele
	glEnableVertexAttribArray(0); // Atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1); // Atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(coneVertices)));
}


void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &EboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId2);
	glDeleteBuffers(1, &VboId3);
	glDeleteBuffers(1, &EboId3);
}
void CreateShaders(void)
{
	ProgramId = LoadShaders("08_01_Shader.vert", "08_01_Shader.frag");
	glUseProgram(ProgramId);
}
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	CreateVAO1();
	CreateVAO2();
	CreateVAO3();
	CreateVAOCone();
	CreateShaders();
	viewLocation = glGetUniformLocation(ProgramId, "viewShader");
	projLocation = glGetUniformLocation(ProgramId, "projectionShader");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
}
void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
	width = winWidth / 10, height = winHeight / 10;
}

void setMVP(void)
{
	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// reperul de vizualizare
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// matricea de proiectie 
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
}

void RenderFunction(void)
{


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA: negru cu transparenta completa
	//
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	setMVP();

	glPointSize(50.0f); // Dimensiunea punctelor negre (poti ajusta valoarea)

	// Desenam punctele negre
	glBegin(GL_POINTS);
	// Punctul 1
	glColor3f(0.0f, 0.0f, 0.0f); // Culoare neagra
	glVertex3f(5.0f, 4.0f, 0.0f); // Pozitia relativa (deasupra centrului sferei)

	// Punctul 2
	glColor3f(0.0f, 0.0f, 0.0f); // Culoare neagra
	glVertex3f(10.1f, 4.0f, 0.0f); // Pozitia relativa (usor la dreapta)
	glEnd();

	// SFERA 1
	glBindVertexArray(VaoId1);
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	// SFERA 2
	glBindVertexArray(VaoId2);
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	// SFERA 3
	glBindVertexArray(VaoId3);
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	// Desenare con
	glBindVertexArray(VaoIdCone);
	glDrawElements(GL_TRIANGLES, coneSlices * 3, GL_UNSIGNED_SHORT, 0);



	glutSwapBuffers();
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
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Doua obiecte");
	glewInit();
	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutReshapeFunc(reshapeFcn);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}
