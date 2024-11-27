#version 330

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 vNormal;

out vec3 FragPos; //--- ��ü�� ��ġ���� �����׸�Ʈ ���̴��� ������.
out vec3 Normal; //--- ��ְ��� �����׸�Ʈ ���̴��� ������.

uniform mat4 modelTransform;
uniform mat4 projectionTransform;
uniform mat4 viewTransform;

void main(void) 
{
	gl_Position =  projectionTransform * viewTransform * modelTransform * vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);

	FragPos = vec3(modelTransform * vec4(in_Position, 1.0));	//--- ��ü�� ���� ���� ����� �����׸�Ʈ ���̴����� �Ѵ�.
	//--- ���� ��������� �ִ� ���ؽ� ���� �����׸�Ʈ ���̴��� ������.
	Normal = vec3(modelTransform * vec4(vNormal, 1.0f));		//--- ��ְ��� �����׸�Ʈ ���̴��� ������.
}