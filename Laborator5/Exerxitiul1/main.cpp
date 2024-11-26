//
// =================================================
// | Grafica pe calculator                          |
// =================================================
// | Laborator VIII - 08_01_stive_matrice.cpp       |
// =================================================
// 
//	Program ce deseneaza doua cuburi ce se deplaseaza.
//  Ambele cuburi se rotesc in jurul axei proprii, unul dintre cuburi se roteste in jurul celuilat
//	ELEMENTE DE NOUTATE:
//   - transformarile de modelare si cea de vizualizare sunt inglobate intr-o singura matrice;
//	 - folosirea stivelor de matrice;
//   - utilizarea timpului scurs de la initializare;
//   - in 08_01_Shader.frag: stabilirea culorii obiectului in functie de pozitia fragmentului
//
// 
//	Biblioteci
#include <vector>
#include <cmath>
#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Defineste prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include<stack>


//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId,
VboId,
EboId,
ProgramId,
viewModelLocation,
projLocation,
codColLocation;

//	Valaorea lui pi;
float PI = 3.141592;

//	Dimensiunile ferestrei de afisare;
GLfloat
winWidth = 1400, winHeight = 600;

//	Variabila ce determina schimbarea culorii pixelilor in shader;
int codCol;

// Variabila pentru timpul scurs
float timeElapsed;

//	Elemente pentru matricea de vizualizare;
float obsX = 0.0, obsY = 0.0, obsZ = 300.f,
refX = 0.0f, refY = 0.0f, refZ = -100.f,
vX = 0.0;
//	Elemente pentru matricea de proiectie;

float xMin = -700.f, xMax = 700.f, yMin = -300.f, yMax = 300.f,
zNear = 100.f, zFar = 500.f,
width = 1400.f, height = 600.f, fov = 90.f * PI / 180;

//	Vectori pentru matricea de vizualizare;
glm::vec3
obs, pctRef, vert;

//	Variabile catre matricile de transformare;
glm::mat4
view, projection,
translateSystem,
rotateSun,
scalePlanet, rotatePlanetAxis, rotatePlanet, translatePlanet, rotateSatellit, scaleSatellit, translateSatellit;

// Stiva de matrice - inglobeaza matricea de modelare si cea de vizualizare
std::stack<glm::mat4> mvStack;

void ProcessNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':			//	Apasarea tastelor l si r modifica pozitia verticalei in planul de vizualizare;
		vX += 0.1;
		break;
	case 'r':
		vX -= 0.1;
		break;
	case '+':			//	Apasarea tastelor + si - schimba pozitia observatorului (se departeaza / aproprie);
		obsZ += 10;
		break;
	case '-':
		obsZ -= 10;
		break;
	}
	if (key == 27)
		exit(0);
}

void ProcessSpecialKeys(int key, int xx, int yy)
{
	switch (key)				//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN';
	{							//	duce la deplasarea observatorului pe axele Ox si Oy;
	case GLUT_KEY_LEFT:
		obsX -= 20;
		break;
	case GLUT_KEY_RIGHT:
		obsX += 20;
		break;
	case GLUT_KEY_UP:
		obsY += 20;
		break;
	case GLUT_KEY_DOWN:
		obsY -= 20;
		break;
	}
}

//  Crearea si compilarea obiectelor de tip shader;
void CreateShaders(void)
{
	ProgramId = LoadShaders("08_01_Shader.vert", "08_01_Shader.frag");
	glUseProgram(ProgramId);
}
std::vector<GLfloat> Vertices;
std::vector<GLubyte> Indices;

//  Se initializeaza un vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{

	const int numSlices = 18;  // Numar de sectiuni longitudinale
	const int numStacks = 9;  // Numar de sectiuni latitudinale
	const float radius = 50.0f;



	// Generarea vertecsilor
	for (int stack = 0; stack <= numStacks; ++stack) {
		float stackAngle = -PI / 2 - stack * PI / numStacks; // De la +PI/2 la -PI/2
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int slice = 0; slice <= numSlices; ++slice) {
			float sliceAngle = slice * 2 * PI / numSlices;

			// Coordonate pentru fiecare punct
			float x = xy * cosf(sliceAngle);
			float y = xy * sinf(sliceAngle);

			// Adaugare coordonate si culori (aleatorii pentru diversitate)
			Vertices.push_back(x);
			Vertices.push_back(y);
			Vertices.push_back(z);
			Vertices.push_back(1.0f);  // W coordonata omogena
			Vertices.push_back((float)rand() / RAND_MAX); // R
			Vertices.push_back((float)rand() / RAND_MAX); // G
			Vertices.push_back((float)rand() / RAND_MAX); // B
		}
	}

	// Generarea indicilor
	for (int stack = 0; stack < numStacks; ++stack) {
		for (int slice = 0; slice < numSlices; ++slice) {
			int first = (stack * (numSlices + 1)) + slice;
			int second = first + numSlices + 1;

			Indices.push_back(first);
			Indices.push_back(second);
			Indices.push_back(first + 1);

			Indices.push_back(second);
			Indices.push_back(second + 1);
			Indices.push_back(first + 1);
		}
	}

	// Transmiterea datelor catre GPU
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLubyte), &Indices[0], GL_STATIC_DRAW);

	// Asocieri pentru shadere
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;

	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	viewModelLocation = glGetUniformLocation(ProgramId, "viewModel");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	//	Realizarea proiectiei - pot fi utilizate si alte variante;
	projection = glm::ortho(xMin, xMax, yMin, yMax, zNear, zFar);
	//	projection = glm::frustum(xMin, xMax, yMin, yMax, zNear, zFar);
	//	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), zNear, zFar);
	// projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), zNear); 
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);
}

//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	glDisable(GL_CULL_FACE);  // Dezactiveaza culling
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Curatare buffer de culoare si adancime
	glEnable(GL_DEPTH_TEST);                                // Activare test de adancime

	// Variabila care indica timpul scurs de la initializare
	timeElapsed = glutGet(GLUT_ELAPSED_TIME);

	// Matricea de vizualizare - actualizare
	obs = glm::vec3(obsX, obsY, obsZ);                      // Pozitia observatorului
	refX = obsX; refY = obsY;                               // Punctul de referinta
	pctRef = glm::vec3(refX, refY, refZ);
	vert = glm::vec3(vX, 1.0f, 0.0f);                       // Verticala din planul de vizualizare
	view = glm::lookAt(obs, pctRef, vert);                  // Matricea de vizualizare

	// Matrice pentru miscarea obiectelor din sistem
	translateSystem = glm::translate(glm::mat4(1.0f), glm::vec3(-600 + 0.01 * timeElapsed, 0.0, 0.0));
	rotateSun = glm::rotate(glm::mat4(1.0f), -(float)0.0001 * timeElapsed, glm::vec3(0.0, 1.0, 0.0));
	scalePlanet = glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.4, 0.4));
	rotatePlanetAxis = glm::rotate(glm::mat4(1.0f), (float)0.001 * timeElapsed, glm::vec3(0.0, 1.0, 0.0));
	rotatePlanet = glm::rotate(glm::mat4(1.0f), (float)0.0005 * timeElapsed, glm::vec3(-0.1, 1.0, 0.0));
	translatePlanet = glm::translate(glm::mat4(1.0f), glm::vec3(150.0, 0.0, 0.0));
	scaleSatellit = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	rotateSatellit = glm::rotate(glm::mat4(1.0f), (float)0.0006 * timeElapsed, glm::vec3(-0.2, 1.0, 0.0));
	translateSatellit = glm::translate(glm::mat4(1.0f), glm::vec3(100.0, 0.0, 0.0));

	// Desenarea primitivelor + manevrarea stivei de matrice
	mvStack.push(view);
	mvStack.push(view);  // in varful stivei: view 

	// 0) Pentru intregul sistem
	mvStack.top() *= translateSystem;    // in varful stivei: view * translateSystem 
	mvStack.push(mvStack.top());         // Pe pozitia 2 a stivei: view * translateSystem 

	// 1) Pentru Soare (astrul central)
	mvStack.top() *= rotateSun;          // Rotire in jurul propriei axe
	glUniformMatrix4fv(viewModelLocation, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.pop();                       // Revenire la view * translateSystem 

	codCol = 1;                          // Culoare pentru Soare
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_BYTE, (void*)(0)); // Sfera Soarelui

	// 2) Pentru planeta
	mvStack.top() *= rotatePlanet;       // Rotire in jurul Soarelui
	mvStack.top() *= translatePlanet;    // Translatie fata de centrul Soarelui
	mvStack.top() *= rotatePlanetAxis;   // Rotire in jurul propriei axe
	mvStack.top() *= scalePlanet;        // Scalare (redimensionare)

	glUniformMatrix4fv(viewModelLocation, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	codCol = 2;                          // Culoare pentru planeta
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_BYTE, (void*)(0)); // Sfera planetei

	// Eliminare matrice din stiva si actualizare frame
	//mvStack.pop();                       // Revenire la view 
	// 3) pentru satelit
	mvStack.push(mvStack.top());            // Salvm pozitia planetei pe stiva
	mvStack.top() *= rotateSatellit;
	mvStack.top() *= translateSatellit;
	mvStack.top() *= scaleSatellit;

	glUniformMatrix4fv(viewModelLocation, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	codCol = 3;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_BYTE, (void*)(0));

	// Eliminare matrice din stiva si actualizare frame
	mvStack.pop();

	glutSwapBuffers();                   // inlocuieste imaginea desenata cu cea randata
	glFlush();                           // Asigura rularea comenzilor OpenGL apelate anterior
}


//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);		//	Se folosesc 2 buffere pentru desen (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB + 1 buffer pentru adancime;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Miscare relativa. Utilizarea stivelor de matrice");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();							//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);		//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);			//	Asigura rularea continua a randarii;
	glutKeyboardFunc(ProcessNormalKeys);	//	Functii ce proceseaza inputul de la tastatura utilizatorului;
	glutSpecialFunc(ProcessSpecialKeys);
	glutCloseFunc(Cleanup);					//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}
