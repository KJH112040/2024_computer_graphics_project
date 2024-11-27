#version 330
in vec3 FragPos; //--- ��ġ��
in vec3 Normal; //--- ���ؽ� ���̴����� ���� ��ְ�
out vec4 FragColor; //--- ���� ��ü�� �� ����

uniform vec3 objectColor;
uniform vec3 lightPos; //--- ������ ��ġ
uniform vec3 lightColor; //--- ���� ���α׷����� ������ ���� ����

void main(void) 
{	
	float ambientLight = 0.3; //--- �ֺ� ���� ���
	vec3 ambient = ambientLight * lightColor; //--- �ֺ� ���� ��
	
	vec3 normalVector = normalize (Normal);
	vec3 lightDir = normalize (lightPos - FragPos); //--- ǥ��� ������ ��ġ�� ������ ������ �����Ѵ�.
	float diffuseLight = max (dot (normalVector, lightDir), 0.0); //--- N�� L�� ���� ������ ���� ����: ���� ����
	vec3 diffuse = diffuseLight * lightColor; //--- ��� �ݻ� ����: ����ݻ簪 * �������
	
	vec3 result = (ambient + diffuse) * objectColor; //--- ���� ���� ������ �ȼ� ����: (�ֺ�+����ݻ�+�ſ�ݻ�����)*��ü ����

	FragColor = vec4 (result, 1.0);
}
