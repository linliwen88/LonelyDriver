#version 410 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform int modelType; // 0: car, 1, tree
uniform int opacityNr;

uniform vec3 viewPos;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 lightDiffuse;
uniform vec3 lightAmbient;
uniform vec3 lightSpecular;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_opacity1;
uniform sampler2D texture_emissive1;

// // old model shader
//void main()
//{
////    FragColor = texture(texture_diffuse1, TexCoords);
//	
//	// ambient
//    vec4 ambient = vec4(lightAmbient, 1.0) * texture(texture_diffuse1, TexCoords).rgba;
//  	
//    // diffuse 
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(lightPosition - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec4 diffuse = vec4(lightDiffuse, 1.0) * diff * texture(texture_diffuse1, TexCoords).rgba;
//    
//    // specular
//    float materialShininess = 0.9;
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
//    vec4 specular = vec4(lightSpecular, 1.0) * spec * vec4(texture(texture_specular1, TexCoords).r);
//
//    vec4 result = ambient + diffuse + specular;
//    FragColor = result;
//}


void main()
{
    // ambient
    vec3 ambient = lightAmbient * texture(texture_diffuse1, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    // specular
    float materialShininess = 0.9;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = lightSpecular * spec * vec3(texture(texture_specular1, TexCoords).r);

    // opacity
    float opacity;
    if (modelType == 1 && opacityNr == 1) // tree mesh that hap opacity texture
    {
        opacity = texture(texture_opacity1, TexCoords).r;
    }
    else opacity = 1.0;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, opacity);
}