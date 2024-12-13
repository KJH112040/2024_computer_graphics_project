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
		shake = 1, y_radian = 180.0f, // shake = (발,다리)회전 각도, radian = 몸 y축 회전 각도
		color[3] = {},
		y{};
	BB bb{}; //왼쪽 상단, 오른쪽 하단
	int shake_dir{}, dir{};
	bool move = false; // 움직이고 있는지(대기 후 이동)
};
Robot player_robot, block_robot[9];

GLvoid drawScene();
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid SpecialKeyBoard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid TimerFunc(int x);

void InitBuffer();
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

GLint background_width, background_height;
float vertexPosition[] = {
	-1.0f, 1.0f, 1.0f, //앞면
	-1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,

	-1.0f, 1.0f,-1.0f, //윗면
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,

	-1.0f, 1.0f,-1.0f, //왼쪽옆
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,

	 1.0f, 1.0f,-1.0f, //뒷면
	 1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,

	-1.0f,-1.0f, 1.0f, //아랫면
	-1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,

	 1.0f, 1.0f, 1.0f, //오른쪽 옆
	 1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,//24

	 1.0f, 0.0f, 0.0f, //x,y,z 축
	-1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f,-1.0f, //30

	-1.0f, 0.0f, 1.0f, //바닥
	-1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f,-1.0f,
	 1.0f, 0.0f, 1.0f// 34
};//정육면체, 축,정사면체 벡터들
float vertexNormal[] = {
	 0.0f, 0.0f, 1.0f,//앞면
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 0.0f,//윗면
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,//왼면
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	 0.0f, 0.0f,-1.0f,//뒷면
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,

	 0.0f,-1.0f, 0.0f,//아래
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,

	 1.0f, 0.0f, 0.0f,//오른쪽
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,

	 //선
	   1.0f, 0.0f, 0.0f,
	  -1.0f, 0.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f,-1.0f, 0.0f,
	   0.0f, 0.0f, 1.0f,
	   0.0f, 0.0f,-1.0f,

	   0.0f, 1.0f, 0.0f,//아래
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f
};//정육면체, 축,정사면체 색깔들
float vertexTexture[] = {
	//앞면
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//윗면
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//왼면
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//뒷면
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//아래
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//오른쪽
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	//선
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,

   //아래
   1.0f, 1.0f,
   1.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 1.0f,
};//정육면체, 축,정사면체 색깔들

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

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
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragmentShaderSource.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
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
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		exit(-1);
	}
	glUseProgram(ShaderProgramID);

	return ShaderProgramID;
}
GLuint VAO, VBO[3];
void InitBuffer()
{
	glGenVertexArrays(1, &VAO); //--- VAO 를 지정하고 할당하기
	glGenBuffers(3, VBO); //--- 2개의 VBO를 지정하고 할당하기

	glBindVertexArray(VAO); //--- VAO를 바인드하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexNormal), vertexNormal, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vertexTexture), vertexTexture, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
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

	//그냥 맵===================================================================================================================================================================================
	{
		glViewport(0, 0, background_width, background_height);

		unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
		unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
		unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일

		//원근 투영
		glm::mat4 kTransform = glm::mat4(1.0f);
		kTransform = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 50.0f);
		kTransform = glm::translate(kTransform, glm::vec3(0.0f, 0.0f, -8.0f));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

		//뷰잉 변환
		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::vec3 cameraPos = glm::vec3(player_robot.x - 1.0f * sin(glm::radians(player_robot.y_radian)), 1.0f, player_robot.z - 1.0f * cos(glm::radians(player_robot.y_radian))); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(player_robot.x, 0.0f, player_robot.z); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

		vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		//vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

		//축
		glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
		axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//변환 행렬을 셰이더에 전달

		unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달
		unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달
		unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달

		//조명 위치 및 색
		glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

		//오브젝트 색 지정
		glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);

		/*여기에 로봇*/
		{
			glUniform3f(objColorLocation, player_robot.color[0], player_robot.color[1], player_robot.color[2]);
			glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
			shapeTransForm = glm::translate(shapeTransForm, glm::vec3(player_robot.x, player_robot.y, player_robot.z));      //robot위치
			shapeTransForm = glm::rotate(shapeTransForm, glm::radians(player_robot.y_radian), glm::vec3(0.0f, 1.0f, 0.0f));                 //보는 방향
			shapeTransForm = glm::scale(shapeTransForm, glm::vec3(2.0f, 2.0f, 2.0f));                                                                      //size
			/*오른다리*/ {
				glUniform3f(objColorLocation, 0.0f, 0.6f, 0.6f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.05f, 0.2f, 0.0f));                                                                //몸 위치에 따라 조정
				model = glm::rotate(model, glm::radians(player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));                                  //다리 흔들기
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //원점조정
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /*왼다리*/ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-0.05f, 0.2f, 0.0f));                                                               //몸 위치에 따라 조정
				model = glm::rotate(model, glm::radians(-player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));                                 //다리 흔들기
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //원점조정
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /* 몸통 */ {
				glUniform3f(objColorLocation, 0.6f, 0.0f, 0.6f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
				model = glm::scale(model, glm::vec3(0.1f, 0.15f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /*오른팔*/ {
				glUniform3f(objColorLocation, 0.6f, 0.6f, 0.0f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /*왼  팔*/ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /* 머리 */ {
				glUniform3f(objColorLocation, 0.6f, 0.0f, 0.6f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.55f, 0.0f));
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));                                                                       //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			}
		}
		/*이건 장애물 로봇*/
		for (int i = 0; i < 9; ++i) {
			glUniform3f(objColorLocation, block_robot[i].color[0], block_robot[i].color[1], block_robot[i].color[2]);
			glm::mat4 shapeTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
			shapeTransForm = glm::translate(shapeTransForm, glm::vec3(block_robot[i].x, 0.0f, block_robot[i].z));      //robot위치
			shapeTransForm = glm::rotate(shapeTransForm, glm::radians(block_robot[i].y_radian), glm::vec3(0.0f, 1.0f, 0.0f));                 //보는 방향
			shapeTransForm = glm::scale(shapeTransForm, glm::vec3(2.0f, 2.0f, 2.0f));                                                                      //size
			/*오른다리*/ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.05f, 0.2f, 0.0f));                                                                //몸 위치에 따라 조정
				model = glm::rotate(model, glm::radians(block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));                                  //다리 흔들기
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //원점조정
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /*왼다리*/ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-0.05f, 0.2f, 0.0f));                                                               //몸 위치에 따라 조정
				model = glm::rotate(model, glm::radians(-block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));                                 //다리 흔들기
				model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //원점조정
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /* 몸통 */ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
				model = glm::scale(model, glm::vec3(0.1f, 0.15f, 0.05f));                                                                      //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /*오른팔*/ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(-block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /*왼  팔*/ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-0.125f, 0.5f, 0.0f));
				model = glm::rotate(model, glm::radians(block_robot[i].shake), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
				model = glm::scale(model, glm::vec3(0.025f, 0.13f, 0.05f));                                                                    //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			} /* 머리 */ {
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.55f, 0.0f));
				model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));                                                                       //size
				model = axisTransForm * shapeTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24); //정육면체
			}
		}
		/*이건 일단 축*/
		{
			glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glUniform3f(objColorLocation, 0.2f, 0.2f, 0.2f);
			glDrawArrays(GL_LINES, 24, 6); //축
		}
		/*여기는 맵(바닥)*/
		{
			/*트랙1*/
			{
				glUniform3f(objColorLocation, 0.75f, 0.75f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(200.0f + 1.0f - 2.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f + 2.0f));
				model = glm::scale(model, glm::vec3(200.0f, 0.0f, 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-200.0f - 1.0f + 2.0f, 0.0f, 1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
			}

			/*트랙2*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.5f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(200.0f + 1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f));
				model = glm::scale(model, glm::vec3(200.0f + 1.0f, 0.0f, 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-200.0f - 1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
			}

			/*트랙3*/
			{
				glUniform3f(objColorLocation, 0.25f, 0.25f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(200.0f + 1.0f + 2.0f, 0.0f, -1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -150.0f - 2.0f));
				model = glm::scale(model, glm::vec3(200.0f + 1.0f + 1.0f, 0.0f, 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(-200.0f - 1.0f - 2.0f, 0.0f, -1.0f));
				model = glm::scale(model, glm::vec3(1.0f, 0.0f, 150.0f + 1.0f + 1.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
			}
		}
		/*골대*/
		{
			/*기둥1*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.25f, 0.25f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(203.9f, 2.0f, 150.9f));
				model = glm::scale(model, glm::vec3(0.1f, 2.0f, 0.1f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24);
			}

			/*기둥2*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.25f, 0.25f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(198.1f, 2.0f, 150.9f));
				model = glm::scale(model, glm::vec3(0.1f, 2.0f, 0.1f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 24);
			}

			/*깃발*/
			{
				glUniform3f(objColorLocation, 1.f, 1.f, 1.f);
				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::translate(model, glm::vec3(201.f, 3.2f, 149.9f));
				model = glm::scale(model, glm::vec3(3.0f, 0.8f, 1.f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_QUADS, 0, 4);
			}
		}

		/*Start 지점*/
		{
			glUniform3f(objColorLocation, 1.f, 1.f, 1.0f);

			glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
			model = glm::translate(model, glm::vec3(-201.0f, 0.0001f, 150.0f));
			model = glm::scale(model, glm::vec3(3.0f, 0.0f, 1.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
		}
	}
	//미니 맵===================================================================================================================================================================================
	if (1) {
		glViewport(3 * background_width / 4, 3 * background_height / 4, background_width / 4, background_height / 4); /*대충 오른쪽상단 어딘가에 배치 바라요*/

		unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//월드 변환 행렬값을 셰이더의 uniform mat4 modelTransform에게 넘겨줌
		unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//위와 동일
		unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//위와 동일

		unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos 값 전달
		unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor 값 전달
		unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color값 전달

		// 투영
		glm::mat4 kTransform = glm::mat4(1.0f);
		kTransform = glm::ortho(-7.5f, 7.5f, -8.0f, 8.0f, -5.0f, 5.0f);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

		//뷰잉 변환
		glm::mat4 vTransform = glm::mat4(1.0f);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

		//축
		glm::mat4 axisTransForm = glm::mat4(1.0f);//변환 행렬 생성 T
		axisTransForm = glm::rotate(axisTransForm, glm::radians(player_robot.y_radian - 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		axisTransForm = glm::translate(axisTransForm, glm::vec3(0.0f, 0.0f, 2.0f));
		axisTransForm = glm::rotate(axisTransForm, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//변환 행렬을 셰이더에 전달

		glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
		glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

		//오브젝트 색 지정
		glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);

		/*여기는 맵(바닥)*/
		{
			/*트랙1*/
			{
				glUniform3f(objColorLocation, 0.75f, 0.75f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(200.0f + 25.0f - 50.0f, 0.0f, -25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f - 50.0f));
				model = glm::scale(model, glm::vec3(200.0f, 0.0f, 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(-200.0f - 25.0f + 50.0f, 0.0f, -25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
			}

			/*트랙2*/
			{
				glUniform3f(objColorLocation, 0.5f, 0.5f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(200.0f + 25.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f));
				model = glm::scale(model, glm::vec3(200.0f + 25.0f, 0.0f, 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(-200.0f - 25.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
			}

			/*트랙3*/
			{
				glUniform3f(objColorLocation, 0.25f, 0.25f, 1.0f);

				glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(200.0f + 25.0f + 50.0f, 0.0f, 25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 150.0f + 50.0f));
				model = glm::scale(model, glm::vec3(200.0f + 25.0f + 25.0f, 0.0f, 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0

				model = glm::mat4(1.0f);//변환 행렬 생성 T
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
				model = glm::translate(model, glm::vec3(-200.0f - 25.0f - 50.0f, 0.0f, 25.0f));
				model = glm::scale(model, glm::vec3(25.0f, 0.0f, 150.0f + 25.0f + 25.0f));
				model = axisTransForm * model;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
			}
		}
		/*경계*/
		{
			glUniform3f(objColorLocation, 0.0f, 0.0f, 0.0f);

			glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_QUADS, 30, 4); //사각형 크기 1.0 x 0.0 x 1.0
		}
		/*플레이어*/
		{
			glUniform3f(objColorLocation, 0.6f, 0.0f, 0.6f);

			glm::mat4 model = glm::mat4(1.0f);//변환 행렬 생성 T
			model = glm::scale(model, glm::vec3(0.02f, 1.0f, 0.02f));
			model = glm::translate(model, glm::vec3(-player_robot.x, 1.0f, -player_robot.z));
			model = glm::scale(model, glm::vec3(50.0f, 0.0f, 50.0f));
			model = glm::rotate(model, glm::radians(player_robot.y_radian), glm::vec3(0.0f, 1.0f, 0.0f));
			model = axisTransForm * model;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_QUADS, 0, 24); //정육면체
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
	glm::mat4 Transform = glm::mat4(1.0f);//변환 행렬 생성 T
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