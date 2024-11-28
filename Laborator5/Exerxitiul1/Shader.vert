//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laborator VIII - 08_01_Shader.vert       |
// ===========================================
// 
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei; 
//

#version 330

layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 viewModel;
uniform mat4 projection;

out vec3 ex_Color;
out float yCoord; // Coordonata verticala transmisa la fragment shader

void main(void)
{
    gl_Position = projection * viewModel * in_Position;
    ex_Color = in_Color;
    yCoord = in_Position.y; // Salvam coordonata verticala a varfului
}

 
