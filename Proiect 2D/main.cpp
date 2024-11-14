#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>

// Variabile globale pentru pozitiile si vitezele masinilor
float car1PosX = -0.8f;
float car2PosX = -1.1f;
float car2PosY = -0.2f;
float initialSpeed = 0.001f;
float maxSpeed = 0.003f;
float acceleratedSpeed = 0.005f;
float car2Speed = initialSpeed;
float car3PosX = 1.0f; // Pozitia initiala a masinii care vine din sens opus
float car3PosY = 0.15f;
float car3Speed = -0.002f; // Viteza negativa pentru deplasarea in directia opusa

// Variabile globale pentru rotatia masinii 3
float car3Rotation = 0.0f;
bool isColliding = false; // variabila pentru a semnala coliziunea
bool hit = false;

float car4PosX = 1.2f;  // Pozitia initiala a noii masini
float car4PosY = 0.15f;
float car4Speed = -0.002f;
bool car4Visible = false; // variabila pentru vizibilitatea masinii car4
float car4StopPosX = 0.0f;




bool overtaking = false;
bool overtakingInProgress = false;
bool overtook = false;
bool isAccelerating = false;

float accelerationRate = 0.00005f;

GLuint shaderProgram;
GLuint VAO, VBO;
GLuint roadVAO, roadVBO;
GLuint policeCarTexture;

// Existing global variables
GLuint backgroundVAO, backgroundVBO; // VAO and VBO for the background
GLuint backgroundTexture; // Texture for the background


float car2Rotation = 0.0f;

// Shadere
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 transform;
out vec2 TexCoord;
void main() {
    gl_Position = transform * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform sampler2D texture1;
in vec2 TexCoord;
void main() {
    FragColor = texture(texture1, TexCoord);
}
)";

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    // Load texture with transparency (RGBA)
    unsigned char* data = SOIL_load_image(path, &width, &height, &nrChannels, SOIL_LOAD_RGBA);
    if (data) {
        // Use GL_RGBA for textures with transparency
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    SOIL_free_image_data(data);
    return textureID;
}

void initializeShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Initialize VAO for the background
void initializeBackgroundVAO() {
    // Vertices for a full-screen quad
    float backgroundVertices[] = {
        // Position       // Texture Coords
        -1.0f, -1.0f,     0.0f, 0.0f,
         1.0f, -1.0f,     1.0f, 0.0f,
         1.0f,  1.0f,     1.0f, 1.0f,
        -1.0f,  1.0f,     0.0f, 1.0f
    };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);

    glBindVertexArray(backgroundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Draw the background function
void drawBackground() {
    glUseProgram(shaderProgram);

    glm::mat4 transform = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glBindVertexArray(backgroundVAO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

void initializeVAO() {
    float carVertices[] = {
        -0.1f, -0.05f, 0.0f, 0.0f,
         0.0f, -0.05f, 1.0f, 0.0f,
         0.0f,  0.05f, 1.0f, 1.0f,
        -0.1f,  0.05f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(carVertices), carVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void initializeRoadVAO() {
    float roadVertices[] = {
        // PosX, PosY, TexCoordX, TexCoordY
       -1.0f, -0.4f, 0.0f, 0.0f,
        1.0f, -0.4f, 1.0f, 0.0f,
        1.0f,  0.4f, 1.0f, 1.0f,
       -1.0f,  0.4f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &roadVAO);
    glGenBuffers(1, &roadVBO);

    glBindVertexArray(roadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, roadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roadVertices), roadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawCar(float posX, float posY, GLuint textureID, float rotation = 0.0f) {
    glUseProgram(shaderProgram);

    // Matrice de transformare pentru pozitionarea si rotirea masinii
    glm::mat4 transform = glm::mat4(1.0f);

    // Translatare la pozitia initiala a masinii
    transform = glm::translate(transform, glm::vec3(posX, posY, 0.0f));

    // Roteste in jurul centrului masinii
    transform = glm::translate(transform, glm::vec3(0.025f, 0.025f, 0.0f)); // Mutam in centru
    transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotim in jurul centrului
    transform = glm::translate(transform, glm::vec3(-0.025f, -0.025f, 0.0f)); // Mutam inapoi

    // Aplicam transformarea
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Desenam masina cu textura
    glBindTexture(GL_TEXTURE_2D, textureID); // Aplicam textura masinii
    glBindVertexArray(VAO);                  // Activam VAO-ul masinii
    glDrawArrays(GL_QUADS, 0, 4);            // Desenam masina
    glBindVertexArray(0);                    // Dezactivam VAO-ul
}




GLuint carTexture;
GLuint roadTexture;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // fundalul
    drawBackground();

    // drumul
    glUseProgram(shaderProgram);
    glm::mat4 roadTransform = glm::mat4(1.0f);
    roadTransform = glm::rotate(roadTransform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(roadTransform));
    glBindTexture(GL_TEXTURE_2D, roadTexture);
    glBindVertexArray(roadVAO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);

    // masinile
    drawCar(car1PosX, -0.2f, carTexture);
    drawCar(car2PosX, car2PosY, carTexture, car2Rotation);
    drawCar(car3PosX, car3PosY, carTexture, car3Rotation);

    // masina de politie
    if (car4Visible) {
        drawCar(car4PosX, car4PosY, policeCarTexture);
    }

    glutSwapBuffers();
}



void checkCollisionAndAvoidance() {
    // Distanta minima intre masini
    float minSafeDistance = 0.10f;

    // Daca masina de pe contrasens (car3) este prea aproape de masina care depaseste (car2)
    if (fabs(car2PosX - car3PosX) < minSafeDistance && car2PosY >= 0.08f) {
        car3PosY += 0.05f;
        isColliding = true;
        hit = true;
        car4Visible = true;  // Activeaza vizibilitatea masinii suplimentare
        car4StopPosX = car3PosX;
    }
    else {
        isColliding = false;
    }
}


void update(int value) {
    car1PosX += initialSpeed;
    car2PosX += car2Speed;

    if (!hit) {
        car3PosX += car3Speed;
    }

    checkCollisionAndAvoidance();

    if (car4Visible) {
        if (car4PosX > car4StopPosX) {
            car4PosX += car4Speed;  // Misca masina 4 pana ajunge la pozitia de oprire
        }
        else {
            car4PosX = car4StopPosX;  // Opreste matina 4 la pozitia dorita

        }
    }

    if (isColliding) {
        car3Rotation += 10.0f;
    }

    if (isAccelerating && car2Speed < maxSpeed) {
        car2Speed += accelerationRate;
    }

    if (isAccelerating && !overtakingInProgress) {
        overtakingInProgress = true;
    }

    if (overtakingInProgress) {
        if (!overtook) {
            car2Rotation = 8.0f;
            if (car2PosY <= 0.1f) car2PosY += 0.002f;
            if (car2PosX >= car1PosX - 0.05f) {
                overtook = true;
            }
        }
        else {
            car2Rotation = 0.0f;
            if (car2PosY > -0.2f) car2PosY -= 0.001f;
            else {
                overtakingInProgress = false;
                isAccelerating = false;
                car2Speed = initialSpeed;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}



void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isAccelerating = true;
        car2Speed = initialSpeed;
    }
}

void initialize() {
    glewInit();

    initializeShaders();
    initializeVAO();
    initializeRoadVAO();
    initializeBackgroundVAO(); // Initialize background VAO

    carTexture = loadTexture("masina1.png");
    roadTexture = loadTexture("road.jpg");
    policeCarTexture = loadTexture("policecar1.jpg");
    backgroundTexture = loadTexture("iarba.jpg");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1500, 600);
    glutCreateWindow("OpenGL Overtaking Simulation");

    glewInit();

    initialize();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}