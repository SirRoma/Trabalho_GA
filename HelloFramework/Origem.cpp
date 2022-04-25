#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <../glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Shader.h"

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void asteroides(glm::mat4 model, GLint modelLoc, GLuint VAO, GLuint asteroide, double pos_astx, double pos_asty);
void detecta_colisao();

int setupGeometry();
int loadTexture(string path);
GLuint createSprite();

//Variáveis para tela
const GLuint WIDTH = 800, HEIGHT = 600;
const int nPoints = 100 + 1 + 1;
const float pi = 3.14159;

//Variáveis universais para alterar imagens
//Algumas destas variáveis são para debug
double vel_nave[2] = { 0 }, pos_nave[2] = { WIDTH / 2,HEIGHT / 2 };
double vel_ast[12] = { 0 }, pos_ast[12] = { 0 };
double rodar = 0, vel_rodar = 0, angulo = 0;


int main()
{
	//Gera velocidades aleatórias para os asteróides
	//Também pode ser implementado para a posição inicial deles
	for (int i = 0; i < 12; i += 2) {
		vel_ast[i] = rand() % 100 + 10;
		vel_ast[i] = vel_ast[i] / 600;
		vel_ast[i+1] = rand() % 100 + 10;
		vel_ast[i+1] = vel_ast[i+1] / 600;
		//cout << vel_ast[i] << "," << vel_ast[i + 1] << "\n";
	}
	

	//--------------- Configs básicas-------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Nao sei oq tacontesin", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION); 
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	Shader* shader = new Shader("./shaders/sprite.vs", "./shaders/sprite.fs");
	//--------------- Configs básicas-------------



	// Carrega as texturas do jogo
	GLuint fundo = loadTexture("../img/fundo.png");		//Fundo totalmente preto
	GLuint nave = loadTexture("../img/nave.png");			//Autoexplicativo
	GLuint asteroide = loadTexture("../img/asteroide.png");	//Autoexplicativo
	GLuint fundo2 = loadTexture("../img/fundo2.png");	//Fundo com os planetas



	//--------------- Configs básicas-------------
	GLuint VAO = createSprite();

	shader->Use();

	GLint projLoc = glGetUniformLocation(shader->Program, "projection");
	assert(projLoc > -1);

	GLint modelLoc = glGetUniformLocation(shader->Program, "model");
	assert(modelLoc > -1);

	glUniform1i(glGetUniformLocation(shader->Program, "tex"), 0);

	glm::mat4 ortho = glm::mat4(1);
	glm::mat4 model = glm::mat4(1);

	shader->setMat4("projection", glm::value_ptr(ortho));
	//--------------- Configs básicas-------------


	//Variáveis para determinar tamanhos e deslocamento
	double xmin = 0.0, xmax = WIDTH, ymin = 0.0, ymax = HEIGHT;
	double paralax=WIDTH/2, paralay=HEIGHT/2;//Define a velocidade do paralaxe



	while (!glfwWindowShouldClose(window))
	{
		//Calcula o angulo rotacionado pela nave
		angulo = abs((rodar * 180) / pi);//Transforma radianos para graus (inútil)

		//Calcula rotação da nave entre -2pi e 2pi
		if (rodar >= 2 * pi) {
			rodar = 0;
		}
		if (rodar <= -2 * pi){
		rodar = 0;
		}

		//--------------- Configs básicas-------------
		glfwPollEvents();
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		ortho = glm::ortho(xmin, xmax, ymin, ymax, -1.0, 1.0);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f); //A cor de fundo está vermelha
		glClear(GL_COLOR_BUFFER_BIT);
		//--------------- Configs básicas-------------


		//--------------------------------------------------fundo-------------------------
		model = glm::mat4(1); //(Já inicializado como mat4(1)
		model = glm::translate(model, glm::vec3(width/2, height/2, 0.0f));
		model = glm::scale(model, glm::vec3(800, 600, 1.0));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fundo);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//--------------------------------------------------fundo-------------------------
		//--------------------------------------------------fundo2-------------------------
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(paralax, paralay, 0.0f));
		model = glm::scale(model, glm::vec3(800, 600, 1.0));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glActiveTexture(GL_TEXTURE0); //Redundante
		glBindTexture(GL_TEXTURE_2D, fundo2);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
		if (paralax > height*2) {
			paralax = -height;
		}

		paralax += 0.1;
		//paralay += 0.05;

		//--------------------------------------------------fundo-------------------------
		//--------------------------------------------------nave-------------------------
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(pos_nave[0], pos_nave[1], 0.0f));
		model = glm::rotate(model, (float)rodar, glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(25, 25, 1.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


		//Verifica laterais para a nave
		if (pos_nave[0] < 0) {
			pos_nave[0] = width;
		}
		else {
			pos_nave[0] += vel_nave[0];
		}

		if (pos_nave[0] > width) {
			pos_nave[0] = 0;
		}
		else {
			pos_nave[0] += vel_nave[0];
		}

		if (pos_nave[1] < 0) {
			pos_nave[1] = height;
		}
		else {
			pos_nave[1] += vel_nave[1];
		}

		if (pos_nave[1] > height) {
			pos_nave[1] = 0;
		}
		else {
			pos_nave[1] += vel_nave[1];
		}


		/*cout << "Posicao" << pos_nave[0] << "," << pos_nave[1] << "\n";
		cout << "Velocidade" << vel_nave[0] << "," << vel_nave[1] << "\n";
		cout << "Rotacao" << rodar << "\n";
		cout << "Angulo" << angulo << "\n";
		cout << "Cos - Angulo" << cos(angulo) << "\n";
		cout << "Sin - Angulo" << sin(angulo) << "\n\n";*/

		rodar += vel_rodar;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, nave);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//--------------------------------------------------nave-------------------------
		//--------------------------------------------------asteroide-------------------------
		model = glm::mat4(1);

		//Verifica laterais para os asteróides
		for (int i = 0; i < 12; i+=2) {
			if (pos_ast[i] < 0) {
				pos_ast[i] = width;
			}if (pos_ast[i] > width) {
				pos_ast[i] = 0;
			}
			else {
				pos_ast[i] += vel_ast[i];
			}
			
			if (pos_ast[i+1] < 0) {
				pos_ast[i+1] = height;
			}if (pos_ast[i+1] > height) {
				pos_ast[i + 1] = 0;
			}
			else {
				pos_ast[i + 1] += vel_ast[i + 1];
			}
		} 

		//Função para os asteróides, pq fazer na mão ia dar mais trabalho
		asteroides(model, modelLoc, VAO, asteroide, pos_ast[0], pos_ast[1]);
		asteroides(model, modelLoc, VAO, asteroide, pos_ast[2], pos_ast[3]);
		asteroides(model, modelLoc, VAO, asteroide, pos_ast[4], pos_ast[5]);
		asteroides(model, modelLoc, VAO, asteroide, pos_ast[6], pos_ast[7]);
		asteroides(model, modelLoc, VAO, asteroide, pos_ast[8], pos_ast[9]);
		asteroides(model, modelLoc, VAO, asteroide, pos_ast[10], pos_ast[11]);
		//--------------------------------------------------asteroide-------------------------

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(ortho));
		glfwSwapBuffers(window);
		detecta_colisao();
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void detecta_colisao() {
	
	//Variáveis para quadrado da nave
	//Acabei não utilizando elas, porém o código ta aí:
	double nv[4] = { pos_nave[0] + 25, pos_nave[0] - 25, pos_nave[1] + 25, pos_nave[1] - 25 }; // Sem utilidade
	//o 25 é referente à escala do objeto

	//vetor de vértices dos asteróides
	double as[24] = { 0 };
	int aux = 0;
	for (int i = 0; i < 12; i+=2) {
		as[aux] = pos_ast[i] + 25;
		as[aux+1] = pos_ast[i] - 25;
		as[aux+2] = pos_ast[i+1] + 25;
		as[aux+3] = pos_ast[i+1] - 25;
		aux += 4;
		//cout << as[i] << ","<< as[i+1] << "," << as[i+2] << "," << as[i+3] << "\n";//teste
	}
	cout << "\n";//teste


	//Colisão
	//Não tive tempo de implementar um final de jogo
	aux = 0;
	for (int i = 0; i < 12; i += 2) {
		if ((pos_nave[0] < as[aux]) && (pos_nave[0] > as[aux + 1])) {
			if ((pos_nave[1] < as[aux + 2]) && (pos_nave[1] > as[aux + 3])) {
				cout << "colisao\n";
			}
		}
		aux += 4;
	}
}

//verifica as letras pressionadas
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		vel_nave[0] += cos(rodar) * 0.05;
		vel_nave[1] += sin(rodar) * 0.05;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		vel_nave[0] -= cos(rodar) * 0.05;
		vel_nave[1] -= sin(rodar) * 0.05;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		if (vel_rodar <= 0.002) {
			vel_rodar += 0.006;
		}
		

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		if (vel_rodar >= -0.002) {
			vel_rodar += -0.006;
		}
}


//Gera a imagem dos asteróides
void asteroides(glm::mat4 model, GLint modelLoc, GLuint VAO, GLuint asteroide, double pos_astx, double pos_asty) {

	model = glm::translate(model, glm::vec3(pos_astx, pos_asty, 0.0f));
	//model = glm::rotate(model, (float)glfwGetTime()/*glm::radians(45.0f)*/, glm::vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, glm::vec3(25, 25, 1.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, asteroide);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE0);
}

//As demais funções são as padrões vistas em aula
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat* vertices;

	vertices = new GLfloat[nPoints * 3];

	float angle = 0.0;
	float deltaAngle = 2 * pi / (float)(nPoints - 2);
	float radius = 0.5;

	//Adicionar o centro
	vertices[0] = 0.0; // x
	vertices[1] = 0.0; // y
	vertices[2] = 0.0; // z sempre zero 

	for (int i = 3; i < nPoints * 3; i += 3)
	{
		vertices[i] = radius * cos(angle);
		vertices[i + 1] = radius * sin(angle);
		vertices[i + 2] = 0.0;

		angle += deltaAngle;
	}

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, (nPoints * 3) * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

GLuint createSprite()
{
	GLuint VAO;
	GLuint VBO, EBO;

	float vertices[] = {
		// positions          // colors          // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0  // top left 
	};
	unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return VAO;
}

