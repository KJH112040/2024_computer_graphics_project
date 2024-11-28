#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <time.h>

struct Robot {
    GLfloat bb[2][3], //���� ���, ������ �ϴ�
        size, x, y,
        shake, y_radian, // shake = (��,�ٸ�)ȸ�� ����, radian = �� y�� ȸ�� ����
        color;
    bool move; // �����̰� �ִ���(��� �� �̵�)
};

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

GLchar* vertexSource, * fragmentSource;
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

int main(int argc, char** argv)
{
    srand(time(NULL));
    background_width = 800, background_height = 800;
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
GLuint VAO, VBO[2];
void InitBuffer()
{
    glGenVertexArrays(1, &VAO); //--- VAO �� �����ϰ� �Ҵ��ϱ�
    glGenBuffers(2, VBO); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

    glBindVertexArray(VAO); //--- VAO�� ���ε��ϱ�
    //--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
    //--- triShape �迭�� ������: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);

    //--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute �ε��� 0���� ��밡���ϰ� ��
    glEnableVertexAttribArray(0);
    //--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //--- ���� colors���� ���ؽ� ������ �����Ѵ�.
    //--- colors �迭�� ������: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexNormal), vertexNormal, GL_STATIC_DRAW);
    //--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute �ε��� 1���� ��� �����ϰ� ��.
    glEnableVertexAttribArray(1);

}

GLfloat camera_move[3]{ 0.0f, 0.0f, 1.0f };

GLvoid drawScene()
{
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
        kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
        kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -8.0f));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //���� ��ȯ
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(camera_move[0], camera_move[1], camera_move[2]); //--- ī�޶� ��ġ
        glm::vec3 cameraDirection = glm::vec3(camera_move[0], camera_move[1], camera_move[2] - 1.0f); //--- ī�޶� �ٶ󺸴� ����
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        vTransform = glm::rotate(vTransform, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        //��
        glm::mat4 axisTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//��ȯ ����� ���̴��� ����

        unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����
        unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����
        unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color�� ����
        
        //���� ��ġ �� ��
        glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

        //������Ʈ �� ����
        glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

        /*���⿡ �κ�*/
        {
            glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
            model = axisTransForm * model;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
            glDrawArrays(GL_QUADS, 0, 24); //������ü
        } 

        /*�̰� ��ֹ� �κ�*/
        {
            glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
            model = axisTransForm * model;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
            glDrawArrays(GL_QUADS, 0, 24); //������ü
        }

        /*�̰� �ϴ� ��*/
        {
            glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
            model = axisTransForm * model;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glUniform3f(objColorLocation, 0.2, 0.2, 0.2);
            glDrawArrays(GL_LINES, 24, 6); //��
        }

        // ���� ���� �ϼ�, ���� ��(Ʈ��)�� ��ġ ���� �ʿ�, �̴ϸ� ���� �ʿ�

        /*����� ��(�ٴ�)*/
        {
            /*Ʈ��1*/
            {
                glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f-2.0f, 0.0f, 151.0f-0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f -197.0f, 0.0f, 151.0f-150.0f));
                model = glm::scale(model, glm::vec3(194.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f - 392.0f, 0.0f, 151.0f-0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
            }

            /*Ʈ��2*/
            {
                glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f+0.0f, 0.0f, 151.0f-0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f -197.0f, 0.0f, 151.0f-152.0f));
                model = glm::scale(model, glm::vec3(198.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f - 394.0f, 0.0f, 151.0f-0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
            }

            /*Ʈ��3*/
            {
                glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f+2.0f, 0.0f, 151.0f-1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 152.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f-197.0f, 0.0f, 151.0f - 154.0f));
                model = glm::scale(model, glm::vec3(200.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(394.0f - 396.0f, 0.0f, 151.0f - 1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 152.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
            }
        }
    }

    //�̴� ��===================================================================================================================================================================================
    {
        glViewport(3*background_width/4, 3*background_height/4, background_width/4, background_height/4); /*���� �����ʻ�� ��򰡿� ��ġ �ٶ��*/

        unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//���� ��ȯ ��İ��� ���̴��� uniform mat4 modelTransform���� �Ѱ���
        unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//���� ����
        unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//���� ����

        unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����
        unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����
        unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color�� ����

        //���� ����
        glm::mat4 kTransform = glm::mat4(1.0f);
        kTransform = glm::ortho(-250.0f, 250.f, -1.0f, 10.0f, -300.0f, 300.0f);
        //kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -8.0f));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //���� ��ȯ
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ��ġ
        glm::vec3 cameraDirection = glm::vec3(0.0f,0.0f,0.0f); //--- ī�޶� �ٶ󺸴� ����
        glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f); //--- ī�޶� ���� ����

        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        vTransform = glm::rotate(vTransform, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        //��
        glm::mat4 axisTransForm = glm::mat4(1.0f);//��ȯ ��� ���� T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//��ȯ ����� ���̴��� ����

        glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

        //������Ʈ �� ����
        glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

        /*����� ��(�ٴ�)*/
        {
            /*Ʈ��1*/
            {
                glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 2.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 197.0f, 0.0f,- 150.0f));
                model = glm::scale(model, glm::vec3(194.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 392.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
            }

            /*Ʈ��2*/
            {
                glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 197.0f, 0.0f,- 152.0f));
                model = glm::scale(model, glm::vec3(198.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 394.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
            }

            /*Ʈ��3*/
            {
                glm::mat4 model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(2.0f, 0.0f,- 1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 152.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 197.0f, 0.0f, - 154.0f));
                model = glm::scale(model, glm::vec3(200.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//��ȯ ��� ���� T
                model = glm::translate(model, glm::vec3(- 396.0f, 0.0f,- 1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 152.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //�簢�� ũ�� 1.0 x 0.0 x 1.0
            }
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
    default:
        break;
    }
    glutPostRedisplay();
}

GLvoid TimerFunc(int x) {
    switch (x)
    {
    case 1:
        break;
    }
    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}