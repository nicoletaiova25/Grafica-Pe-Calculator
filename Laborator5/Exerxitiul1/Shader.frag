//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laborator VIII - 08_01_Shader.frag       |
// ===========================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330

in vec3 ex_Color; 
in float yCoord; // Coordonata verticala primita de la vertex shader

out vec3 out_Color;

uniform int codCol;

void main(void)
{
    if (codCol == 1 || codCol == 2 || codCol == 3) { 
        // Normalizare yCoord intr-un interval [0, 1]
        float normalizedY = (yCoord + 50.0) / 100.0; // Presupunem ca sfera are raza 50
        normalizedY = clamp(normalizedY, 0.0, 1.0); // Asiguram că valorile sunt intre 0 si 1

        if (codCol == 1)
            out_Color = mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 1.0, 0.0), normalizedY); // Gradient portocaliu-galben
        else if (codCol == 2)
            out_Color = mix(vec3(0.0, 0.0, 0.5), vec3(0.0, 0.0, 1.0), normalizedY); // Gradient albastru
        else
            out_Color = mix(vec3(0.5, 0.0, 0.0), vec3(1.0, 0.0, 0.0), normalizedY); // Gradient rosu
    } else {
        out_Color = ex_Color; // Culoare implicita
    }
}

