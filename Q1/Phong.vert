#version 330 core
layout (location = 0) in vec3 aPos;   //
layout (location = 1) in vec3 aNormal; // 

out vec3 v_WorldPos;    // 
out vec3 v_WorldNormal; // ���� ���� ���

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix; // transpose(inverse(mat3(modelMatrix)))

void main()
{
    // ���� ���� ��ġ ���
    v_WorldPos = vec3(modelMatrix * vec4(aPos, 1.0));
    
    // ����ȭ
    v_WorldNormal = normalize(normalMatrix * aNormal);
    
    // ����  ���
    gl_Position = projectionMatrix * viewMatrix * vec4(v_WorldPos, 1.0);
}