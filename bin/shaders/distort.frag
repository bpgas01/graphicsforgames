#version 410

in vec2 vTexCoord;

uniform sampler2D colorTarget;


out vec4 FragColor;

vec4 Default(vec2 texCoord){
    return texture(colorTarget, texCoord);
}


vec4 Distort(vec2 texCoord){
    vec2 mid = vec2(0.5f);
    float distanceFromCentre = distance(texCoord, mid);
    vec2 normalized = normalize(texCoord - mid);
    float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.05f;
    vec2 newCoord = mid + bias * normalized;
    return texture(colorTarget, newCoord);
}

vec4 BoxBlur(vec2 texCoord){

    vec2 texel = 1.0f / textureSize(colorTarget, 0);
    vec4 color = texture(colorTarget, texCoord);
    color += texture(colorTarget, texCoord + texel * vec2(-1,1));
    color += texture(colorTarget, texCoord + texel * vec2(-1,0));
    color += texture(colorTarget, texCoord + texel * vec2(-1,-1));
    color += texture(colorTarget, texCoord + texel * vec2(0,1));
    color += texture(colorTarget, texCoord + texel * vec2(0,-1));
    color += texture(colorTarget, texCoord + texel * vec2(1,1));
    color += texture(colorTarget, texCoord + texel * vec2(1,0));
    color += texture(colorTarget, texCoord + texel * vec2(1,-1));
   
    return color / 9;

}

void main(){

    // find texel size
    vec2 texSize = textureSize(colorTarget, 0);
    vec2 texelSize = 1.0f / texSize;

    vec2 scale = (texSize - texelSize) / texSize;
    vec2 texCoord = vTexCoord / scale + texelSize * 0.5f;

    FragColor = Distort(texCoord);


}