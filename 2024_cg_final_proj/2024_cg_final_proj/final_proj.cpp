#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
//#include <gl/glm/glm.hpp>
//#include <gl/glm/ext.hpp>
//#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <Windows.h>

typedef struct Bounding_Box {
	GLfloat x1, z1, x2, z2;
}BB;

struct Robot {
	GLfloat size{}, x{}, z{}, road[2][2]{},
		speed = 0.0f,
		shake = 1, y_radian = 180.0f, // shake = (��,�ٸ�)ȸ�� ����, radian = �� y�� ȸ�� ����
		color[3] = {},
		y{};
	BB bb{}; //���� ���, ������ �ϴ�
	int shake_dir{}, dir{};
	bool move = false; // �����̰� �ִ���(��� �� �̵�)
};
Robot player_robot, block_robot[9];

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid SpecialKeyBoard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);

void InitBuffer();
void InitTextures();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}
GLubyte* LoadDIBitmap(const char* filename, BITMAPINFO** info)
{
	FILE* fp;
	GLubyte* bits;
	int bitsize{}, infosize{};
	BITMAPFILEHEADER header;
	//--- ���̳ʸ� �б� ���� ������ ����
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;
	//--- ��Ʈ�� ���� ����� �д´�.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp); return NULL;
	}
	//--- ������ BMP �������� Ȯ���Ѵ�.
	if (header.bfType != 'MB') {
		fclose(fp); return NULL;
	}
	//--- BITMAPINFOHEADER ��ġ�� ����.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	//--- ��Ʈ�� �̹��� �����͸� ���� �޸� �Ҵ��� �Ѵ�.
	if ((*info = (BITMAPINFO*)malloc(infosize)) == NULL) {
		fclose(fp); return NULL;
	}
	//--- ��Ʈ�� ���� ����� �д´�.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp); return NULL;
	}
	//--- ��Ʈ���� ũ�� ����
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
			(*info)->bmiHeader.biBitCount + 7) / 8 *
		abs((*info)->bmiHeader.biHeight);
	//--- ��Ʈ���� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	if ((bits = (GLubyte*)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp); return NULL;
	}
	//--- ��Ʈ�� �����͸� bit(GLubyte Ÿ��)�� �����Ѵ�.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp); return NULL;
	}
	fclose(fp);
	return bits;
}

GLint background_width, background_height;
float vertexPosition[] = {
	-1.0f, 1.0f, 1.0f, //�ո�
	-1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,

	-1.0f, 1.0f,-1.0f, //����
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,

	-1.0f, 1.0f,-1.0f, //���ʿ�
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,

	 1.0f, 1.0f,-1.0f, //�޸�
	 1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,

	-1.0f,-1.0f, 1.0f, //�Ʒ���
	-1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,

	 1.0f, 1.0f, 1.0f, //������ ��
	 1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,//24

	 1.0f, 0.0f, 0.0f, //x,y,z ��
	-1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f,-1.0f, //30

	-1.0f, 0.0f, 1.0f, //�ٴ�
	-1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f, 1.0f// 34
};//������ü, ��,�����ü ���͵�
float vertexNormal[] = {
	 0.0f, 0.0f, 1.0f,//�ո�
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 0.0f,//����
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,//�޸�
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	 0.0f, 0.0f,-1.0f,//�޸�
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,

	 0.0f,-1.0f, 0.0f,//�Ʒ�
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,

	 1.0f, 0.0f, 0.0f,//������
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,

	 //��
	   1.0f, 0.0f, 0.0f,
	  -1.0f, 0.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f,-1.0f, 0.0f,
	   0.0f, 0.0f, 1.0f,
	   0.0f, 0.0f,-1.0f,

	   0.0f, 1.0f, 0.0f,//�Ʒ�
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f
};//������ü, ��,�����ü �����
float vertexTexture[] = {
	//�ո�
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//����
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//�޸�
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//�޸�
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//�Ʒ�
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//������
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//��
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,

   //�Ʒ�
   1.0f, 1.0f,
   1.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 1.0f,
};//������ü, ��,�����ü �����

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;
unsigned int texture_index[2];

int main(int argc, char** argv)
{
	background_width = 1200, background_height = 800;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(background_width, background_height);
	glutCreateWindow("2024_Computer_Graphics_Final_Project");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	make_vertexShaders();
	make_fragmentShaders();
	shaderID = make_shaderProgram();
	InitBuffer();
	glutKeyboardFunc(KeyBoard);
	glutSpecialFunc(SpecialKeyBoard);
	glutTimerFunc(10, TimerFunc, 1);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertexShaderSource.glsl");
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragmentShaderSource.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram()
{
	GLuint ShaderProgramID;

	ShaderProgramID = glCreateProgram();
	glAttachShader(ShaderProgramID, vertexShader);
	glAttachShader(ShaderProgramID, fragmentShader);
	glLinkProgram(ShaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];

	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		exit(-1);
	}
	glUseProgram(ShaderProgramID);

	return ShaderProgramID;
}
GLuint VAO, VBO[3];
void InitBuffer()
{
	glGenVertexArrays(1, &VAO); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glGenBuffers(3, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	glBindVertexArray(VAO); //--- VAO�� ���ε��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� �����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexNormal), vertexNormal, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� �����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vertexTexture), vertexTexture, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� �����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(2);

	player_robot.move = false;
	player_robot.y_radian = 180.0f, player_robot.shake_dir = 1;
	player_robot.x = -201, player_robot.z = 150, player_robot.y=0.f;
	{
		block_robot[0].road[0][0] = -203, block_robot[0].road[0][1] = 150;
		block_robot[0].road[1][0] = -203, block_robot[0].road[1][1] = -150;

		block_robot[1].road[0][0] = -199, block_robot[1].road[0][1] = 149;
		block_robot[1].road[1][0] = -199, block_robot[1].road[1][1] = -149;

		block_robot[2].road[0][0] = -203, block_robot[2].road[0][1] = -150;
		block_robot[2].road[1][0] = -203, block_robot[2].road[1][1] = 150;
	}
	for (int i = 0; i < 9; ++i) {
		block_robot[i].x = block_robot[i].road[0][0];
		block_robot[i].z = block_robot[i].road[0][1];
		block_robot[i].speed = 0.25f, block_robot[i].shake_dir = 1;
		if (block_robot[i].road[0][0] < block_robot[i].road[1][0])
			block_robot[i].y_radian = 90.0f;
		if (block_robot[i].road[0][0] > block_robot[i].road[1][0])
			block_robot[i].y_radian = -90.0f;
		if (block_robot[i].road[0][1] < block_robot[i].road[1][1])
			block_robot[i].y_radian = 0.0f;
		if (block_robot[i].road[0][1] > block_robot[i].road[1][1])
			block_robot[i].y_radian = 180.0f;
	}
}
void InitTextures() 
{
	BITMAPINFO* bmp;

	glGenTextures(2, texture_index);
	glUseProgram(shaderID);

	//--- texture[0]
	int tLocation1 = glGetUniformLocation(shaderID, "outTexture1"); //--- outTexture1 ������ ���÷��� ��ġ�� ������
	glUniform1i(tLocation1, 0); //--- ���÷��� 0�� �������� ����
	glBindTexture(GL_TEXTURE_2D, texture_index[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//unsigned char* data1 = LoadDIBitmap("front.bmp", &bmp);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 1000, 1000, 0, GL_BGR, GL_UNSIGNED_BYTE, data1);
}
GLfloat camera_move[3]{ 0.0f, 0.0f, 3.0f };
BB map_bb{ -204.0f,-153.f,-198.f,151.f }, map_bb2{ -204.f,-153.f,204.f,-147.f }, map_bb3{ 198.0f,-153.f,204.f,151.f };

BB get_bb(Robot robot);
bool collision(BB obj_a, BB obj_b);

GLvoid drawScene()
{
	glUseProgram(shaderID);
	glBindVertexArray(VAO);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	//�׳� ��===================================================================================================================================================================================
	{
		glViewport(0, 0, background_width, background_height);

		unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//���� ��ȯ ��İ��� ���̴��� uniform mat4 modelTransform���� �Ѱ���
		unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//���� ����
		unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//���� ����

		//���� ����
		glm::mat4 kTransform = glm::mat4(1.0f);
		kTransform = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 50.0f);
		kTransform = glm::translate(kTransform, glm::vec3(0.0f, 0.0f, -8.0f));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

		//���� ��ȯ
		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::vec3 cameraPos = glm::vec3(player_robot.x - 1.0f * sin(glm::radians(player_robot.y_radian)), 1.0f, player_robot.z - 1.0f * cos(glm::radians(player_robot.y_radian))); //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(player_robot.x, 0.0f, player_robot.z); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

		vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		//vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

		//��
		glm::mat4 axisTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
		axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//��ȯ ����� ���̴��� ����

		unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����
		unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����
		unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color�� ����

		//���� ��ġ �� ��
		glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

		//������Ʈ �� ����
		glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);

		/*���⿡ �κ�*/
		{
			glUniform3f(objColorLocation, player_robot.color[0], player_robot.color[1], player_robot.color[2]);
			glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
			shapeTransForm = glm::translate(shapeTransForm, glm::vec3(player_robot.x, player_robot.y, player_robot.z));      //robot��ġ
			shapeTransForm = glm::rotate(shapeTransForm, glm::radians(player_robot.y_radian), glm::vec3(0.0f, 1.0f, 0.0f));                 //���� ����
			shapeTransForm = glm::scale(shapeTransForm, glm::vec3(2.0f, 2.0f, 2.0f));                                                                      //size
			/*�����ٸ�*/ {
				glUniform3f(objColorLocation, 0.0f, 0.6f, 0.6f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.05f, 0.2f, 0.0f));                                                                //�� ��ġ�� ���� ����
				model = glm::rotate(model, glm::radians(player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));                                  //�ٸ� ����
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //��������
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /*�޴ٸ�*/ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-0.05f, 0.2f, 0.0f));                                                               //�� ��ġ�� ���� ����
				model = glm::rotate(model, glm::radians(-player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));                                 //�ٸ� ����
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //��������
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /* ���� */ {
				glUniform3f(objColorLocation, 0.6f, 0.0f, 0.6f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
				model = glm::scale(model, glm::vec3(0.1f, 0.15f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /*������*/ {
				glUniform3f(objColorLocation, 0.6f, 0.6f, 0.0f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /*��  ��*/ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /* �Ӹ� */ {
				glUniform3f(objColorLocation, 0.6f, 0.0f, 0.6f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.55f, 0.0f));
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));                                                                       //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			}
		}
		/*�̰� ��ֹ� �κ�*/
		for (int i = 0; i < 9; ++i) {
			glUniform3f(objColorLocation, block_robot[i].color[0], block_robot[i].color[1], block_robot[i].color[2]);
			glm::mat4 shapeTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
			shapeTransForm = glm::translate(shapeTransForm, glm::vec3(block_robot[i].x, 0.0f, block_robot[i].z));      //robot��ġ
			shapeTransForm = glm::rotate(shapeTransForm, glm::radians(block_robot[i].y_radian), glm::vec3(0.0f, 1.0f, 0.0f));                 //���� ����
			shapeTransForm = glm::scale(shapeTransForm, glm::vec3(2.0f, 2.0f, 2.0f));                                                                      //size
			/*�����ٸ�*/ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.05f, 0.2f, 0.0f));                                                                //�� ��ġ�� ���� ����
				model = glm::rotate(model, glm::radians(block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));                                  //�ٸ� ����
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //��������
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /*�޴ٸ�*/ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-0.05f, 0.2f, 0.0f));                                                               //�� ��ġ�� ���� ����
				model = glm::rotate(model, glm::radians(-block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));                                 //�ٸ� ����
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //��������
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /* ���� */ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
				model = glm::scale(model, glm::vec3(0.1f, 0.15f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /*������*/ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /*��  ��*/ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			} /* �Ӹ� */ {
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.55f, 0.0f));
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));                                                                       //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //������ü
			}
		}
		/*�̰� �ϴ� ��*/
		{
			glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glUniform3f(objColorLocation, 0.2f, 0.2f, 0.2f);
			glDrawArrays(GL_LINES, 24, 6); //��
		}
		/*����� ��(�ٴ�)*/
		{
			/*Ʈ��1*/
			{
				glUniform3f(objColorLocation, 0.75f, 0.75f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(200.0f + 1.0f - 2.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f + 2.0f));
				model = glm::scale(model, glm::vec3(200.0f, 0.0f, 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-200.0f - 1.0f + 2.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
			}

			/*Ʈ��2*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.5f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(200.0f + 1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f));
				model = glm::scale(model, glm::vec3(200.0f + 1.0f, 0.0f, 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-200.0f - 1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
			}

			/*Ʈ��3*/
			{
				glUniform3f(objColorLocation, 0.25f, 0.25f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(200.0f + 1.0f + 2.0f, 0.0f, -1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f - 2.0f));
				model = glm::scale(model, glm::vec3(200.0f + 1.0f + 1.0f, 0.0f, 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(-200.0f - 1.0f - 2.0f, 0.0f, -1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
			}
		}
		/*���*/
		{
			/*���1*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.25f, 0.25f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(203.9f, 2.0f, 150.9f));
				model = glm::scale(model, glm::vec3(0.1f, 2.0f, 0.1f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24);
			}

			/*���2*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.25f, 0.25f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(198.1f, 2.0f, 150.9f));
				model = glm::scale(model, glm::vec3(0.1f, 2.0f, 0.1f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24);
			}

			/*���*/
			{
				glUniform3f(objColorLocation, 1.f, 1.f, 1.f);
				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::translate(model, glm::vec3(201.f, 3.2f, 149.9f));
				model = glm::scale(model, glm::vec3(3.0f, 0.8f, 1.f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}

		/*Start ����*/
		{
			glUniform3f(objColorLocation, 1.f, 1.f, 1.0f);

			glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
			model = glm::translate(model, glm::vec3(-201.0f, 0.0001f, 150.0f));
			model = glm::scale(model, glm::vec3(3.0f, 0.0f, 1.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
		}
	}
	//�̴� ��===================================================================================================================================================================================
	if (1) {
		glViewport(3 * background_width / 4, 3 * background_height / 4, background_width / 4, background_height / 4); /*���� �����ʻ�� ��򰡿� ��ġ �ٶ��*/

		unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//���� ��ȯ ��İ��� ���̴��� uniform mat4 modelTransform���� �Ѱ���
		unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//���� ����
		unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//���� ����

		unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����
		unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����
		unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color�� ����

		// ����
		glm::mat4 kTransform = glm::mat4(1.0f);
		kTransform = glm::ortho(-7.5f, 7.5f, -8.0f, 8.0f, -5.0f, 5.0f);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

		//���� ��ȯ
		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f); //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

		//��
		glm::mat4 axisTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
		axisTransForm = glm::rotate(axisTransForm, glm::radians(player_robot.y_radian - 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		axisTransForm = glm::translate(axisTransForm, glm::vec3(0.0f, 0.0f, 2.0f));
		axisTransForm = glm::rotate(axisTransForm, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//��ȯ ����� ���̴��� ����

		glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

		//������Ʈ �� ����
		glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);

		/*����� ��(�ٴ�)*/
		{
			/*Ʈ��1*/
			{
				glUniform3f(objColorLocation, 0.75f, 0.75f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(200.0f + 25.0f - 50.0f, 0.0f, -25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f - 50.0f));
				model = glm::scale(model, glm::vec3(200.0f, 0.0f, 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(-200.0f - 25.0f + 50.0f, 0.0f, -25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
			}

			/*Ʈ��2*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.5f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(200.0f + 25.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f));
				model = glm::scale(model, glm::vec3(200.0f + 25.0f, 0.0f, 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(-200.0f - 25.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
			}

			/*Ʈ��3*/
			{
				glUniform3f(objColorLocation, 0.25f, 0.25f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(200.0f + 25.0f + 50.0f, 0.0f, 25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f + 50.0f));
				model = glm::scale(model, glm::vec3(200.0f + 25.0f + 25.0f, 0.0f, 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//��ȯ ��� ���� T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(-200.0f - 25.0f - 50.0f, 0.0f, 25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
			}
		}
		/*���*/
		{
			glUniform3f(objColorLocation, 0.0f, 0.0f, 0.0f);

			glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
		}
		/*�÷��̾�*/
		{
			glUniform3f(objColorLocation, 0.6f, 0.0f, 0.6f);

			glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
			model = glm::scale(model, glm::vec3(0.02f, 1.0f, 0.02f));
			model = glm::translate(model, glm::vec3(-player_robot.x, 1.0f, -player_robot.z));
			model = glm::scale(model, glm::vec3(50.0f, 0.0f, 50.0f));
			model = glm::rotate(model, glm::radians(player_robot.y_radian), glm::vec3(0.0f, 1.0f, 0.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_QUADS, 0, 24); //������ü
		}
	}

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'm':
		player_robot.move = true;
		player_robot.shake_dir = 1;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

GLvoid SpecialKeyBoard(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (player_robot.speed < 0.05f)
			player_robot.speed += 0.001f;
		break;
	case GLUT_KEY_DOWN:
		if (player_robot.speed > 0.0f)
			player_robot.speed -= 0.01f;
		break;
	case GLUT_KEY_LEFT:
		player_robot.y_radian += 45.0f;
		break;
	case GLUT_KEY_RIGHT:
		player_robot.y_radian -= 45.0f;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

GLvoid TimerFunc(int x)
{
	if (player_robot.move) {
		if(collision(map_bb,get_bb(player_robot)) || collision(map_bb2, get_bb(player_robot))|| collision(map_bb3, get_bb(player_robot))){
			player_robot.x += sin(glm::radians(player_robot.y_radian)) * player_robot.speed;
			player_robot.z += cos(glm::radians(player_robot.y_radian)) * player_robot.speed;
		}
		else {
			player_robot.y -= 0.1f;
			player_robot.speed = 0.0f;
			player_robot.move = false;
		}
		player_robot.shake += player_robot.shake_dir * 20 * player_robot.speed;
		if (player_robot.shake <= -60.0f || player_robot.shake >= 60.0f)
			player_robot.shake_dir *= -1;
		if (player_robot.speed < 0.25f)
			player_robot.speed += 0.001f;
	}
	if (player_robot.y < 0) {
		player_robot.y -= player_robot.speed;
		player_robot.speed += 0.01f;

		if (player_robot.y < -5.f) {
			player_robot.y_radian = 180.0f, player_robot.shake_dir = 0, player_robot.shake = false, player_robot.speed = 0.0f;;
			player_robot.x = -201, player_robot.z = 150, player_robot.y = 0.f;
		}
	}

	for (int i = 0; i < 9; ++i) {
		block_robot[i].x += sin(glm::radians(block_robot[i].y_radian)) * block_robot[i].speed;
		block_robot[i].z += cos(glm::radians(block_robot[i].y_radian)) * block_robot[i].speed;
		block_robot[i].shake += block_robot[i].shake_dir * 20 * block_robot[i].speed;
		if (block_robot[i].shake <= -60.0f || block_robot[i].shake >= 60.0f)
			block_robot[i].shake_dir *= -1;
		if ((block_robot[i].road[0][0] < block_robot[i].x and block_robot[i].x < block_robot[i].road[1][0]) ||
			(block_robot[i].road[0][0] > block_robot[i].x and block_robot[i].x > block_robot[i].road[1][0]) ||
			(block_robot[i].road[0][1] < block_robot[i].z and block_robot[i].z < block_robot[i].road[1][1]) ||
			(block_robot[i].road[0][1] > block_robot[i].z and block_robot[i].z > block_robot[i].road[1][1]));
		else
			block_robot[i].y_radian += 180.0f;
	}

	glutTimerFunc(10, TimerFunc, 1);
	glutPostRedisplay();
}

BB get_bb(Robot robot)
{
	glm::mat4 Transform = glm::mat4(1.0f);//��ȯ ��� ���� T
	Transform = glm::translate(Transform, glm::vec3(robot.x, 0.0f, robot.z));
	Transform = glm::rotate(Transform, glm::radians(robot.y_radian), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 spots[4]{ glm::vec3(-0.15f, 0.0f, 0.5f), glm::vec3(0.15f, 0.0f, 0.5f), glm::vec3(-0.15f, 0.0f, -0.5f), glm::vec3(0.15f, 0.0f, -0.5f) };
	for (int i = 0; i < 4; ++i)
		spots[i] = glm::vec3(Transform * glm::vec4(spots[i], 1.0f));

	BB bounding_box{ spots[0].x, spots[0].z, spots[0].x, spots[0].z };
	for (int i = 1; i < 3; ++i) {
		if (bounding_box.x1 > spots[i].x)
			bounding_box.x1 = spots[i].x;
		if (bounding_box.x2 < spots[i].x)
			bounding_box.x2 = spots[i].x;
		if (bounding_box.z1 > spots[i].z)
			bounding_box.z1 = spots[i].z;
		if (bounding_box.z2 < spots[i].z)
			bounding_box.z2 = spots[i].z;
	}

	return bounding_box;
}
bool collision(BB obj_a, BB obj_b) {
	if (obj_a.x1 > obj_b.x2) return false;
	if (obj_a.x2 < obj_b.x1) return false;
	if (obj_a.z1 > obj_b.z2) return false;
	if (obj_a.z2 < obj_b.z1)return false;

	return true;
}