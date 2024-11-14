#version 330 core

in vec4 exColor;                            // Culoarea primita de la vertex shader
uniform int codCol;                         // Variabila uniforma pentru a alege culoarea in functie de codCol

out vec4 fragColor;                         // Culoarea finala a fragmentului (pixelului)

void main()
{
    if (codCol == 0)
        fragColor = exColor;               // Foloseste culoarea varfului pentru triunghiul din fata
    else if (codCol == 1)
        fragColor = vec4(0.3, 0.3, 0.3, 1.0); // Seteaza o culoare de umbra pentru triunghiul din spate
    else
        fragColor = exColor;               // Fallback pentru alte valori
}
