// Phong shader for simple game lighting
#version 410

in vec4 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;

uniform vec3 Ka; // Ambient color of the models material
uniform vec3 Kd; // The Diffuse color of the models material
uniform vec3 Ks; // Specular Color
uniform float Ns; // Specular power


uniform vec3 AmbientColor; // Ambient Color of the light
uniform vec3 LightColor; // Color of the light=
uniform vec3 LightDirection;

const int MAX_LIGHTS = 4;
uniform int numLights;
uniform vec3 PointLightColor[MAX_LIGHTS];
uniform vec3 PointLightPosition[MAX_LIGHTS];

uniform vec3 CameraPosition; // Position Of The Camera for Specular calculations
out vec4 FragColor;

vec3 diffuse (vec3 direction, vec3 color, vec3 normal)
{
    return color * max(0, dot(normal, -direction));
}

vec3 specular(vec3 direction, vec3 colour, vec3 normal, vec3 view)
{
    vec3 R = reflect(direction, normal);
    float specTerm = pow(max( 0,dot(R,view)), Ns);
    return specTerm * colour;
}

void main(){
    // Make sure the normal and light directions have been normalized
    vec3 N = normalize(vNormal);
    vec3 L = normalize(LightDirection);
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBiTangent);

    vec3 diffuseTotal = diffuse(L, LightColor, N);
    // Now we calculate the lambert term
    float lambertTerm = max(0, min(1,dot(N,-L)));

    // Calculate the view vector and the reflection vector
    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R = reflect(L,N);

    vec3 specularTotal = specular(L, LightColor, N, V);

    for (int i = 0; i < numLights; i++)
    {
        vec3 direction = vPosition.xyz - PointLightPosition[i];
        float distance = length(direction);
        direction = direction / distance;

        vec3 color = PointLightColor[i] / (distance * distance);

        diffuseTotal += diffuse(direction, color, N);
        specularTotal += specular(direction, color, N, V);
    }


    // Calcilate the value of the ambient
    vec3 ambient = AmbientColor * Ka;

    // Determine the value of the diffuse
    vec3 diffuse = Kd * diffuseTotal;

    // Determine the value of the specular 
    vec3 specular = Ks * specularTotal;

    // Ouput the final color
    FragColor = vec4(ambient + diffuse + specular,1);

    
}