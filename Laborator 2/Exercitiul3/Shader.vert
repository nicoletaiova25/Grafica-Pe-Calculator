#version 330 core

layout(location = 0) in vec4 inPosition;  // Coordonatele varfurilor
layout(location = 1) in vec4 inColor;     // Culoarea varfurilor

out vec4 exColor;                         // Culoarea ce va fi transmisa fragment shader-ului

void main()
{
    gl_Position = inPosition;             // Seteaza pozitia varfului in spatiul de ecran
    exColor = inColor;                    // Transmite culoarea mai departe catre fragment shader
}
