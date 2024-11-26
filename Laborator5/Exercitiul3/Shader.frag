#version 330

in vec3 ex_Color;  
out vec3 out_Color;

uniform int codCol;

void main(void)
{
    switch (codCol)
    {
        case 1: 
            out_Color = vec3(1.0, 1.0, 1.0);  
            break;
        default: 
            out_Color = ex_Color * 0.5 + vec3(1.0, 1.0, 1.0);  
    }
}
