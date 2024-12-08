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
#include <time.h>

struct Robot {
    GLfloat bb[2][3], //¿ÞÂÊ »ó´Ü, ¿À¸¥ÂÊ ÇÏ´Ü
        size, x, z,
        speed = 0.25f,
        shake, y_radian, // shake = (¹ß,´Ù¸®)È¸Àü °¢µµ, radian = ¸ö yÃà È¸Àü °¢µµ
        color[3];
    int shake_dir;
    bool move; // ¿òÁ÷ÀÌ°í ÀÖ´ÂÁö(´ë±â ÈÄ ÀÌµ¿)
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
    -1.0f, 1.0f, 1.0f, //¾Õ¸é
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,

    -1.0f, 1.0f,-1.0f, //À­¸é
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f, 1.0f,-1.0f, //¿ÞÂÊ¿·
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    
     1.0f, 1.0f,-1.0f, //µÞ¸é
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,

    -1.0f,-1.0f, 1.0f, //¾Æ·§¸é
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,

     1.0f, 1.0f, 1.0f, //¿À¸¥ÂÊ ¿·
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,//24

     1.0f, 0.0f, 0.0f, //x,y,z Ãà
    -1.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f,-1.0f, //30

    -1.0f, 0.0f, 1.0f, //¹Ù´Ú
    -1.0f, 0.0f,-1.0f,
     1.0f, 0.0f,-1.0f,
     1.0f, 0.0f, 1.0f// 34
};//Á¤À°¸éÃ¼, Ãà,Á¤»ç¸éÃ¼ º¤ÅÍµé
float vertexNormal[] = {
     0.0f, 0.0f, 1.0f,//¾Õ¸é
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,

     0.0f, 1.0f, 0.0f,//À­¸é
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,

    -1.0f, 0.0f, 0.0f,//¿Þ¸é
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

     0.0f, 0.0f,-1.0f,//µÞ¸é
     0.0f, 0.0f,-1.0f,
     0.0f, 0.0f,-1.0f,
     0.0f, 0.0f,-1.0f,

     0.0f,-1.0f, 0.0f,//¾Æ·¡
     0.0f,-1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
    
     1.0f, 0.0f, 0.0f,//¿À¸¥ÂÊ
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,

   //¼±
     1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f,-1.0f, 0.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f,-1.0f,

     0.0f, 1.0f, 0.0f,//¾Æ·¡
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f
};//Á¤À°¸éÃ¼, Ãà,Á¤»ç¸éÃ¼ »ö±òµé

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
    //--- ¹öÅØ½º ¼¼ÀÌ´õ °´Ã¼ ¸¸µé±â
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //--- ¼¼ÀÌ´õ ÄÚµå¸¦ ¼¼ÀÌ´õ °´Ã¼¿¡ ³Ö±â
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    //--- ¹öÅØ½º ¼¼ÀÌ´õ ÄÄÆÄÀÏÇÏ±â
    glCompileShader(vertexShader);
    //--- ÄÄÆÄÀÏÀÌ Á¦´ë·Î µÇÁö ¾ÊÀº °æ¿ì: ¿¡·¯ Ã¼Å©
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader ÄÄÆÄÀÏ ½ÇÆÐ\n" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragmentShaderSource.glsl");
    //--- ÇÁ·¡±×¸ÕÆ® ¼¼ÀÌ´õ °´Ã¼ ¸¸µé±â
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //--- ¼¼ÀÌ´õ ÄÚµå¸¦ ¼¼ÀÌ´õ °´Ã¼¿¡ ³Ö±â
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    //--- ÇÁ·¡±×¸ÕÆ® ¼¼ÀÌ´õ ÄÄÆÄÀÏ
    glCompileShader(fragmentShader);
    //--- ÄÄÆÄÀÏÀÌ Á¦´ë·Î µÇÁö ¾ÊÀº °æ¿ì: ÄÄÆÄÀÏ ¿¡·¯ Ã¼Å©
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader ÄÄÆÄÀÏ ½ÇÆÐ\n" << errorLog << std::endl;
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
        std::cerr << "ERROR: shader program ¿¬°á ½ÇÆÐ\n" << errorLog << std::endl;
        exit(-1);
    }
    glUseProgram(ShaderProgramID);

    return ShaderProgramID;
}
GLuint VAO, VBO[2];
void InitBuffer()
{
    glGenVertexArrays(1, &VAO); //--- VAO ¸¦ ÁöÁ¤ÇÏ°í ÇÒ´çÇÏ±â
    glGenBuffers(2, VBO); //--- 2°³ÀÇ VBO¸¦ ÁöÁ¤ÇÏ°í ÇÒ´çÇÏ±â

    glBindVertexArray(VAO); //--- VAO¸¦ ¹ÙÀÎµåÇÏ±â
    //--- 1¹øÂ° VBO¸¦ È°¼ºÈ­ÇÏ¿© ¹ÙÀÎµåÇÏ°í, ¹öÅØ½º ¼Ó¼º (ÁÂÇ¥°ª)À» ÀúÀå
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //--- º¯¼ö diamond ¿¡¼­ ¹öÅØ½º µ¥ÀÌÅÍ °ªÀ» ¹öÆÛ¿¡ º¹»çÇÑ´Ù.
    //--- triShape ¹è¿­ÀÇ »çÀÌÁî: 9 * float
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexPosition), vertexPosition, GL_STATIC_DRAW);

    //--- ÁÂÇ¥°ªÀ» attribute ÀÎµ¦½º 0¹ø¿¡ ¸í½ÃÇÑ´Ù: ¹öÅØ½º ´ç 3* float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute ÀÎµ¦½º 0¹øÀ» »ç¿ë°¡´ÉÇÏ°Ô ÇÔ
    glEnableVertexAttribArray(0);
    //--- 2¹øÂ° VBO¸¦ È°¼ºÈ­ ÇÏ¿© ¹ÙÀÎµå ÇÏ°í, ¹öÅØ½º ¼Ó¼º (»ö»ó)À» ÀúÀå
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //--- º¯¼ö colors¿¡¼­ ¹öÅØ½º »ö»óÀ» º¹»çÇÑ´Ù.
    //--- colors ¹è¿­ÀÇ »çÀÌÁî: 9 *float 
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vertexNormal), vertexNormal, GL_STATIC_DRAW);
    //--- »ö»ó°ªÀ» attribute ÀÎµ¦½º 1¹ø¿¡ ¸í½ÃÇÑ´Ù: ¹öÅØ½º ´ç 3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //--- attribute ÀÎµ¦½º 1¹øÀ» »ç¿ë °¡´ÉÇÏ°Ô ÇÔ.
    glEnableVertexAttribArray(1);
}

GLfloat camera_move[3]{ 0.0f, 0.0f, 3.0f };

GLvoid drawScene()
{
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    //±×³É ¸Ê===================================================================================================================================================================================
    {
        glViewport(0, 0, background_width, background_height);

        unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//¿ùµå º¯È¯ Çà·Ä°ªÀ» ¼ÎÀÌ´õÀÇ uniform mat4 modelTransform¿¡°Ô ³Ñ°ÜÁÜ
        unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//À§¿Í µ¿ÀÏ
        unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//À§¿Í µ¿ÀÏ

        //¿ø±Ù Åõ¿µ
        glm::mat4 kTransform = glm::mat4(1.0f);
        kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
        kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -8.0f));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //ºäÀ× º¯È¯
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(player_robot.x - 1.0f * sin(glm::radians(player_robot.y_radian)), 1.0f, player_robot.z - 1.0f * cos(glm::radians(player_robot.y_radian))); //--- Ä«¸Þ¶ó À§Ä¡
        glm::vec3 cameraDirection = glm::vec3(player_robot.x, 0.0f, player_robot.z); //--- Ä«¸Þ¶ó ¹Ù¶óº¸´Â ¹æÇâ
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- Ä«¸Þ¶ó À§ÂÊ ¹æÇâ

        vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        //vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        //Ãà
        glm::mat4 axisTransForm = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0));
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//º¯È¯ Çà·ÄÀ» ¼ÎÀÌ´õ¿¡ Àü´Þ

        unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos °ª Àü´Þ
        unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor °ª Àü´Þ
        unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color°ª Àü´Þ
        
        //Á¶¸í À§Ä¡ ¹× »ö
        glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

        //¿ÀºêÁ§Æ® »ö ÁöÁ¤
        glUniform3f(objColorLocation, 1.0, 1.0, 1.0);
        
        /*¿©±â¿¡ ·Îº¿*/
        {
            glUniform3f(objColorLocation, player_robot.color[0], player_robot.color[1], player_robot.color[2]);
            glm::mat4 shapeTransForm = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
            shapeTransForm = glm::translate(shapeTransForm, glm::vec3(player_robot.x, 0.0f, player_robot.z));      //robotÀ§Ä¡
            shapeTransForm = glm::rotate(shapeTransForm, glm::radians(player_robot.y_radian), glm::vec3(0.0f, 1.0f, 0.0f));                 //º¸´Â ¹æÇâ
            shapeTransForm = glm::scale(shapeTransForm, glm::vec3(2.0f, 2.0f, 2.0f));                                                                      //size
            /*¿À¸¥´Ù¸®*/ {
                glUniform3f(objColorLocation, 0.0, 0.6, 0.6);
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.05f, 0.2f, 0.0f));                                                                //¸ö À§Ä¡¿¡ µû¶ó Á¶Á¤
                model = glm::rotate(model, glm::radians(player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));                                  //´Ù¸® Èçµé±â
                model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //¿øÁ¡Á¶Á¤
                model = glm::scale(model, glm::vec3(0.05, 0.1, 0.05));                                                                      //size
                model = axisTransForm * shapeTransForm * model ;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
            } /*¿Þ´Ù¸®*/ {
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(-0.05f, 0.2f, 0.0f));                                                               //¸ö À§Ä¡¿¡ µû¶ó Á¶Á¤
                model = glm::rotate(model, glm::radians(-player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));                                 //´Ù¸® Èçµé±â
                model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));                                                                //¿øÁ¡Á¶Á¤
                model = glm::scale(model, glm::vec3(0.05, 0.1, 0.05));                                                                      //size
                model = axisTransForm * shapeTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
            } /* ¸öÅë */ {
                glUniform3f(objColorLocation, 0.6, 0.0, 0.6);
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.0f, 0.35f, 0.0f));
                model = glm::scale(model, glm::vec3(0.1f, 0.15f, 0.05f));                                                                      //size
                model = axisTransForm * shapeTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
            } /*¿À¸¥ÆÈ*/ {
                glUniform3f(objColorLocation, 0.6, 0.6, 0.0);
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.125f, 0.5f, 0.0f));
                model = glm::rotate(model, glm::radians(-player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
                model = glm::scale(model, glm::vec3(0.025, 0.13, 0.05));                                                                    //size
                model = axisTransForm * shapeTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
            } /*¿Þ  ÆÈ*/ {
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(-0.125f, 0.5f, 0.0f));
                model = glm::rotate(model, glm::radians(player_robot.shake), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::translate(model, glm::vec3(0.0f, -0.13f, 0.0f));
                model = glm::scale(model, glm::vec3(0.025, 0.13, 0.05));                                                                    //size
                model = axisTransForm * shapeTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
            } /* ¸Ó¸® */ {
                glUniform3f(objColorLocation, 0.6, 0.0, 0.6);
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.0f, 0.55f, 0.0f));
                model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));                                                                       //size
                model = axisTransForm * shapeTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
            }
        } 

        /*ÀÌ°Ç Àå¾Ö¹° ·Îº¿*/
        for (int i = 0; i < 9; ++i) {
            glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
            model = axisTransForm * model;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
            //glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
        }

        /*ÀÌ°Ç ÀÏ´Ü Ãà*/
        {
            glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
            model = axisTransForm * model;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

            glUniform3f(objColorLocation, 0.2, 0.2, 0.2);
            glDrawArrays(GL_LINES, 24, 6); //Ãà
        }

        // ¤§ÀÚ ¸ÊÀº ¿Ï¼º, ¤§ÀÚ ¸Ê(Æ®·¢)ÀÇ À§Ä¡ Á¶Á¤ ÇÊ¿ä, ¹Ì´Ï¸Ê ¼öÁ¤ ÇÊ¿ä

        /*¿©±â´Â ¸Ê(¹Ù´Ú)*/
        {
            /*Æ®·¢1*/
            {
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(195.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, -149.0f));
                model = glm::scale(model, glm::vec3(194.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(-195.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.75, 0.75, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
            }

            /*Æ®·¢2*/
            {
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(197.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, -151.0f));
                model = glm::scale(model, glm::vec3(198.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(-197.0f, 0.0f,1.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 151.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.5, 0.5, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
            }

            /*Æ®·¢3*/
            {
                glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(199.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 152.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, - 153.0f));
                model = glm::scale(model, glm::vec3(200.0f, 0.0f, 1.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                model = glm::translate(model, glm::vec3(-199.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.0f, 0.0f, 152.0f));
                model = axisTransForm * model;
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                glUniform3f(objColorLocation, 0.25, 0.25, 1.0);
                glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
            }
        }
    }

    //¹Ì´Ï ¸Ê===================================================================================================================================================================================
    {
        glViewport(3*background_width/4, 3*background_height/4, background_width/4, background_height/4); /*´ëÃæ ¿À¸¥ÂÊ»ó´Ü ¾îµò°¡¿¡ ¹èÄ¡ ¹Ù¶ó¿ä*/

        unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//¿ùµå º¯È¯ Çà·Ä°ªÀ» ¼ÎÀÌ´õÀÇ uniform mat4 modelTransform¿¡°Ô ³Ñ°ÜÁÜ
        unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//À§¿Í µ¿ÀÏ
        unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//À§¿Í µ¿ÀÏ

        unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos °ª Àü´Þ
        unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor °ª Àü´Þ
        unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color°ª Àü´Þ

        // Åõ¿µ
        glm::mat4 kTransform = glm::mat4(1.0f);
        //kTransform = glm::ortho(-250.0f, 250.f, -1.0f, 10.0f, -300.0f, 300.0f);
        kTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 200.0f);
        kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -8.0f));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

        //ºäÀ× º¯È¯
        glm::mat4 vTransform = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f, 100.f, 0.0f); //--- Ä«¸Þ¶ó À§Ä¡
        glm::vec3 cameraDirection = glm::vec3(0.0f,0.0f,0.0f); //--- Ä«¸Þ¶ó ¹Ù¶óº¸´Â ¹æÇâ
        glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f); //--- Ä«¸Þ¶ó À§ÂÊ ¹æÇâ

        vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
        //vTransform = glm::translate(vTransform, glm::vec3(0.0f, 0.0f, -200.f));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

        //Ãà
        glm::mat4 axisTransForm = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
        axisTransForm = glm::rotate(axisTransForm, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//º¯È¯ Çà·ÄÀ» ¼ÎÀÌ´õ¿¡ Àü´Þ

        glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

        //¿ÀºêÁ§Æ® »ö ÁöÁ¤
        glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

        //¹Ì´Ï ¸Ê===================================================================================================================================================================================
        {
            glViewport(3 * background_width / 4, 3 * background_height / 4, background_width / 4, background_height / 4); /*´ëÃæ ¿À¸¥ÂÊ»ó´Ü ¾îµò°¡¿¡ ¹èÄ¡ ¹Ù¶ó¿ä*/

            unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");//¿ùµå º¯È¯ Çà·Ä°ªÀ» ¼ÎÀÌ´õÀÇ uniform mat4 modelTransform¿¡°Ô ³Ñ°ÜÁÜ
            unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");//À§¿Í µ¿ÀÏ
            unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");//À§¿Í µ¿ÀÏ

            unsigned int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos °ª Àü´Þ
            unsigned int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor °ª Àü´Þ
            unsigned int objColorLocation = glGetUniformLocation(shaderID, "objectColor"); //--- object Color°ª Àü´Þ

            // Åõ¿µ
            glm::mat4 kTransform = glm::mat4(1.0f);
            kTransform = glm::ortho(-7.0f, 7.0f, -6.0f, 6.0f, -5.0f, 5.0f);
            //        kTransform = glm::perspective(glm::radians(75.0f), 0.75f, 9.1f, 11.0f);
            //        kTransform = glm::translate(kTransform, glm::vec3(0.0, 0.0, -9.0f));
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &kTransform[0][0]);

            //ºäÀ× º¯È¯
            glm::mat4 vTransform = glm::mat4(1.0f);
            glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f); //--- Ä«¸Þ¶ó À§Ä¡
            glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- Ä«¸Þ¶ó ¹Ù¶óº¸´Â ¹æÇâ
            glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- Ä«¸Þ¶ó À§ÂÊ ¹æÇâ

            vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &vTransform[0][0]);

            //Ãà
            glm::mat4 axisTransForm = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
            axisTransForm = glm::translate(axisTransForm, glm::vec3(0.0f, 0.0f, 2.0f));
            axisTransForm = glm::rotate(axisTransForm, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisTransForm));//º¯È¯ Çà·ÄÀ» ¼ÎÀÌ´õ¿¡ Àü´Þ

            glUniform3f(lightPosLocation, 0.0f, 3.0f, 0.0f);
            glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);

            //¿ÀºêÁ§Æ® »ö ÁöÁ¤
            glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

            /*¿©±â´Â ¸Ê(¹Ù´Ú)*/
            {
                /*Æ®·¢1*/
                {
                    glUniform3f(objColorLocation, 0.75, 0.75, 1.0);

                    glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(-3.0f + 0.5f, 0.0f, -0.5f));
                    model = glm::scale(model, glm::vec3(0.5f, 0.0f, 2.6f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                    model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.1f));
                    model = glm::scale(model, glm::vec3(3.0f, 0.0f, 0.5f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                    model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(3.0f - 0.5f, 0.0f, -0.5f));
                    model = glm::scale(model, glm::vec3(0.5f, 0.0f, 2.6f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
                }

                /*Æ®·¢2*/
                {
                    glUniform3f(objColorLocation, 0.5, 0.5, 1.0);

                    glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(-4.0f + 0.5f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(0.5f, 0.0f, 3.1f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                    model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.1f));
                    model = glm::scale(model, glm::vec3(4.0f, 0.0f, 0.5f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                    model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(4.0f - 0.5f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(0.5f, 0.0f, 3.1f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
                }

                /*Æ®·¢3*/
                {
                    glUniform3f(objColorLocation, 0.25, 0.25, 1.0);

                    glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(-5.0f + 0.5f, 0.0f, 0.5f));
                    model = glm::scale(model, glm::vec3(0.5f, 0.0f, 3.6f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                    model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.1f));
                    model = glm::scale(model, glm::vec3(5.0f, 0.0f, 0.5f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0

                    model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(5.0f - 0.5f, 0.0f, 0.5f));
                    model = glm::scale(model, glm::vec3(0.5f, 0.0f, 3.6f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
                }

                /*°æ°è*/
                {
                    glUniform3f(objColorLocation, 0.0, 0.0, 0.0);

                    glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 30, 4); //»ç°¢Çü Å©±â 1.0 x 0.0 x 1.0
                }

                /*ÇÃ·¹ÀÌ¾î*/
                {
                    glUniform3f(objColorLocation, 0.6, 0.0, 0.6);

                    glm::mat4 model = glm::mat4(1.0f);//º¯È¯ Çà·Ä »ý¼º T
                    model = glm::translate(model, glm::vec3(-player_robot.x / 100, 0.0f, -player_robot.z / 100));
                    model = axisTransForm * model;
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

                    glDrawArrays(GL_QUADS, 0, 24); //Á¤À°¸éÃ¼
                }
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
    case 'm':
        player_robot.y_radian = 180.0f;
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
        player_robot.y_radian = 180.0f;
        player_robot.shake_dir = 1;
        player_robot.move = true;
        break;
    case GLUT_KEY_DOWN:
        player_robot.y_radian = 0.0f;
        player_robot.shake_dir = 1;
        player_robot.move = true;
        break;
    case GLUT_KEY_LEFT:
        player_robot.y_radian = -90.0f;
        player_robot.shake_dir = 1;
        player_robot.move = true;
        break;
    case GLUT_KEY_RIGHT:
        player_robot.y_radian = 90.0f;
        player_robot.shake_dir = 1;
        player_robot.move = true;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

GLvoid TimerFunc(int x) 
{
    if (player_robot.move/*trueÀÏ¶§ ·Îº¿ ¿òÁ÷ÀÓ, È÷ÆR*/) {
        player_robot.x += sin(glm::radians(player_robot.y_radian)) * player_robot.speed;
        player_robot.z += cos(glm::radians(player_robot.y_radian)) * player_robot.speed;
        player_robot.shake += player_robot.shake_dir * 10 * player_robot.speed;
        if (player_robot.shake <= -60.0f || player_robot.shake >= 60.0f)
            player_robot.shake_dir *= -1;
        if (player_robot.speed < 0.45f)
            player_robot.speed += 0.01f;
    }
    glutTimerFunc(10, TimerFunc, 1);
    glutPostRedisplay();
}