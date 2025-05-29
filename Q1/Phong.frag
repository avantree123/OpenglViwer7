#version 330 core
out vec4 FragColor;

in vec3 v_WorldPos;
in vec3 v_WorldNormal;

// C++���� ���޵� Uniform ������
uniform vec3 eyePosWorld;

uniform vec3 lightPosWorld;
uniform vec3 lightIl;       
uniform float lightIa;     

uniform vec3 matKa;         
uniform vec3 matKd;        
uniform vec3 matKs;         // Specular reflectivity (0.5, 0.5, 0.5)
uniform float matShininess; // 

uniform float gamma;        // Gamma value (2.2)

void main()
{
    // ����ȭ�� ��� ���� 
    vec3 N = normalize(v_WorldNormal);
    
    // 1. Ambient Term
    vec3 ambient = lightIa * matKa;

    // 2. Diffuse Term
    vec3 L = normalize(lightPosWorld - v_WorldPos);
    float diffFactor = max(dot(N, L), 0.0);
    vec3 diffuse = lightIl * matKd * diffFactor;

    // 3. Specular Term (Phong)
    vec3 V = normalize(eyePosWorld - v_WorldPos);
    vec3 R = reflect(-L, N);
    float specFactor = pow(max(dot(V, R), 0.0), matShininess);
    vec3 specular = lightIl * matKs * specFactor;

    // ���� ���� (���� ����)
    vec3 finalColorLinear = ambient + diffuse + specular;
    
    // ���� ����
    vec3 finalColorGammaCorrected = pow(finalColorLinear, vec3(1.0 / gamma));

    FragColor = vec4(finalColorGammaCorrected, 1.0);
}