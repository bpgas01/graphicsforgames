// Phong shader for simple hame lighting
#version 410
 
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBiTangent;
 
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
 
uniform vec3 Ka; // The ambient color of the model's material
uniform vec3 Kd; // The diffuse color of the model's material
uniform vec3 Ks; // The specular color of the model's material
uniform float Ns; // The speculat power of the model's material
 
uniform vec3 AmbientColor; // A<noemt color of the light
uniform vec3 LightColor; // Color of the light 
uniform vec3 LightDirection; 
 
uniform vec3 CameraPosition; // position of the viewportcamera for the specular calculations
 
out vec4 FragColor;
 
void main()
{
    // Make sure the normal and the light directions have been normalized 
    vec3 N = normalize(vNormal);
    vec3 L = normalize(LightDirection);
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBiTangent);
 
    mat3 TBN = mat3(T,B,N);
 
    vec3 textDiffuse = texture(diffuseTexture, vTexCoord).rgb;
    vec3 textSpecular = texture(specularTexture, vTexCoord).rgb;
    vec3 textNormal = texture(normalTexture, vTexCoord).rgb;
 
    //N = TBN * (textNormal * 2-1);
 
    // Now we can calculate the lambert term, negate the light direction
    float lambertTerm = max(0, min(1, dot(N, -L)));
 
    // Calculate  the view vector and the reflection vetor
    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R = reflect(L, N);
 
    // Determine the value of the specular term 
    float specularTerm = pow(max(0, dot(R, V)), 100);
 
    // Determine the value of the ambient 
    vec3 ambient = AmbientColor * Ka * textDiffuse;
    // Determine the value of the diffuse 
    vec3 diffuse = LightColor * Kd * textDiffuse * lambertTerm; 
    // Dteremine the value of the specular
    vec3 specular = LightColor * Ks * textSpecular * specularTerm;
 
    // Show the standatd normals
    //FragColor = vec4(N, 1);
 
    //FragColor = vec4(textNormal, 1);
    // Output of the final color
      FragColor = vec4(ambient + diffuse + specular, 1);
}