#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor;
GLuint programTexture;
GLuint proceduralTexture;
GLuint strippedTexture;

Core::Shader_Loader shaderLoader;

float appLoadingTime;

obj::Model shipModel;
obj::Model ruinsModel;
obj::Model fishModel;
obj::Model skyboxModel;



float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

GLuint textureRuins;
GLuint textureFish;
bool pause=false;
struct Fish {
	glm::vec3 position;
	glm::vec3 offset;
	float angle;
	float speed;
};
struct Bubble {
		glm::vec3 position;
		glm::vec3 offset;
		float speed;
		float size;
		float startingHeight;
		float resetHeight;
};
static const int NUM_FISHS = 20;
Fish fish[NUM_FISHS];
static const int NUM_MOVE_POINTS = 12;
glm::vec3 moveKeyPoints[NUM_MOVE_POINTS]{
	glm::vec3(-11.9,0,-3.6),
	glm::vec3(2.2  ,0,-21.6),
	glm::vec3(9.7  ,0,-1.8),
	glm::vec3(16.9 ,0,18.3),
	glm::vec3(36.4 ,0,28.2),
	glm::vec3(45.7 ,0,-14.1),
	glm::vec3(26.9 ,0,0),
	glm::vec3(32.2 ,0,-30.7),
	glm::vec3(45.4 ,-2.3,-17.7),
	glm::vec3(18.4 ,-2.5,-19.1),
	glm::vec3(13.7 ,-2.4,0.7),
	glm::vec3(5.6  ,-2,12.7),
};
float speedModifier=1.0f;
void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.3f*speedModifier;
	switch(key)
	{
	case 'q': cameraAngle -= angleSpeed; break;
	case 'e': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'r': cameraPos.y += 1* moveSpeed*3; break;
	case 'f': cameraPos.y -=  1* moveSpeed*3; break;
	case 'p': pause=!pause; break;
	case 't': speedModifier=speedModifier==1?2:1; break;
	case 'l': std::cout<<cameraPos.x<<" "<<cameraPos.y<<" "<<cameraPos.z<<std::endl; break;
	}
}

glm::mat4 createCameraMatrix()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;

	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);




	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectStripped(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 colorA,glm::vec3 colorB,float width)
{
	GLuint program = strippedTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "objectColorA"), colorA.x, colorA.y, colorA.z);
	glUniform3f(glGetUniformLocation(program, "objectColorB"), colorB.x, colorB.y, colorB.z);
	glUniform1f(glGetUniformLocation(program, "stripeWidth"), width);


	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawBackground(obj::Model * model, glm::mat4 modelMatrix)
{
	GLuint program = proceduralTexture;
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;

	glUseProgram(program);

	glUniform1f(glGetUniformLocation(program, "time"),time);
	glUniform1f(glGetUniformLocation(program, "scale"),30.0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;

	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0.6f, 0.75f, 1.0f);
	drawBackground(&skyboxModel, glm::translate(glm::vec3(0, 0, 0)+cameraPos)* glm::scale(glm::vec3(100.0f)));
	
	for (int i = 0; i < NUM_FISHS; i++)
	{
		if(!pause){
		int x = floorf(time/fish[i].speed);
		float s = glm::fract(time/fish[i].speed);
		fish[i].position = glm::catmullRom(
			moveKeyPoints[(x - 1+ i) % NUM_MOVE_POINTS],
			moveKeyPoints[(x + i) % NUM_MOVE_POINTS],
			moveKeyPoints[(x + 1 + i) % NUM_MOVE_POINTS],
			moveKeyPoints[(x + 2 + i) % NUM_MOVE_POINTS],
			s);

		glm::vec3 wektorStyczny = glm::normalize(
			glm::catmullRom(
				moveKeyPoints[(x - 1 + i)  % NUM_MOVE_POINTS],
				moveKeyPoints[(x + i) % NUM_MOVE_POINTS],
				moveKeyPoints[(x + 1 + i)  % NUM_MOVE_POINTS],
				moveKeyPoints[(x + 2 + i)  % NUM_MOVE_POINTS],
				s + 0.001)
			-
			glm::catmullRom(
				moveKeyPoints[(x - 1 + i)  % NUM_MOVE_POINTS],
				moveKeyPoints[(x + i) % NUM_MOVE_POINTS],
				moveKeyPoints[(x + 1 + i)  % NUM_MOVE_POINTS],
				moveKeyPoints[(x + 2 + i)  % NUM_MOVE_POINTS]
				, s - 0.001));
		fish[i].angle=  atan2f(wektorStyczny.z, wektorStyczny.x);
		}
		drawObjectStripped(&fishModel,
			glm::translate(fish[i].position + fish[i].offset)
			*glm::rotate(-fish[i].angle + glm::radians(90.0f), glm::vec3(0, 1, 0))
			* glm::scale(glm::vec3(0.9f)),
			glm::vec3(1.000,0.988,0.842),
			glm::vec3(1.000,0.492,0.218),
			2.0);
	}
	
	drawObjectTexture(&ruinsModel, glm::translate(glm::vec3(0, -20, 0))* glm::scale(glm::vec3(0.7f)),textureRuins);

	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	proceduralTexture = shaderLoader.CreateProgram("shaders/shader_background.vert", "shaders/shader_background.frag");
	strippedTexture = shaderLoader.CreateProgram("shaders/shader_stripped.vert", "shaders/shader_stripped.frag");
	fishModel = obj::loadModelFromFile("models/fish.obj");
	ruinsModel = obj::loadModelFromFile("models/ruins.obj");
	skyboxModel = obj::loadModelFromFile("models/sphere.obj");
	textureFish = Core::LoadTexture("textures/asteroid.png");
	textureRuins = Core::LoadTexture("textures/ruins.png");

	static const float astRadius = 16.0;
	for (int i = 0; i < NUM_FISHS; i++)
	{
		fish[i].offset = glm::vec3(glm::linearRand(-0.5f, 0.5f), glm::linearRand(-0.5f, 0.5f), glm::linearRand(-0.5f, 0.5f));
		fish[i].speed = glm::linearRand(2.0f, 5.0f);
	}

	static const float camRadius = 2.55;
	static const float camOffset = 0;



	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Duze akwarium");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
