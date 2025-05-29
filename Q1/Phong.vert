#version 330 core
layout (location = 0) in vec3 aPos;   //
layout (location = 1) in vec3 aNormal; // 

out vec3 v_WorldPos;    // 
out vec3 v_WorldNormal; // 월드 공간 노멀

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix; // transpose(inverse(mat3(modelMatrix)))

void main()
{
    // 월드 공간 위치 계산
    v_WorldPos = vec3(modelMatrix * vec4(aPos, 1.0));
    
    // 정규화
    v_WorldNormal = normalize(normalMatrix * aNormal);
    
    // 최종  계산
    gl_Position = projectionMatrix * viewMatrix * vec4(v_WorldPos, 1.0);
}