#version 330 core
struct Light {
    vec3 position;

    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
};


in vec3 FragPos;
in vec3 Normal;
in vec3 colorFrag;

out vec4 color;

uniform vec3 viewPos;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient;

//    // Diffuse
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(light.position - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = light.diffuse * diff;

    vec3 result = ambient * colorFrag;
    color = vec4(result, 1.0f);
//    color = vec4(1.0);

//    if(colorFrag.x == 0.0f){
//        color = vec4(result, 1.0f);
//    } else if(colorFrag.y > colorFrag.x){
//         color = vec4(result, 1.0f);
//    } else{
//        color = vec4(colorFrag , 1.0f);
//    }
}
