/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico/Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 01/03/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Protótipo da função de callback do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//// Protótipo da função de scrol do mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



// Protótipos das funções
int setupShader();
int setupGeometry();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de código aqui!
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

//Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX=false, rotateY=false, rotateZ=false;

bool rotate1 = false, rotate2 = false, rotate3 = false, rotate4 = false, rotate5 = false, rotate6 = false;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05;
float pitch = 0.0, yaw = -90.0;
float fov = 0.0;


// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	

	glfwSetCursorPos(window,WIDTH / 2, HEIGHT / 2);


	//Desabilita o desenho do cursor 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();


	glUseProgram(shaderID);

	// Matriz de model -- transformações no objeto
	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	//Matriz de view -- posição e orientação da câmera
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	GLint viewLoc = glGetUniformLocation(shaderID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(shaderID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();
		float angleCima = 70.7; 
		float angleBaixo = 29.85;
		float angleFrente = 85.0;
		float angleCosta = 40.82;
		float angleEsquerda = 39.3;
		float angleDireita = 80.1;

		model = glm::mat4(1); 

		// model = glm::translate(model, glm::vec3(0.0, 0.0, cos(angle)*10.0));

		//Rotação X Y Z
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		//Rotação Do 1 ao 5
		if (rotate1)
		{
			model = glm::rotate(model, angleCima, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		else if (rotate2)
		{
			model = glm::rotate(model, angleBaixo, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		else if (rotate3)
		{
			model = glm::rotate(model, angleFrente, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		else if (rotate4) 
		{
			model = glm::rotate(model, angleCosta, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotate5)
		{
			model = glm::rotate(model, angleEsquerda, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotate6)
		{
			model = glm::rotate(model, angleDireita, glm::vec3(0.0f, 1.0f, 0.0f));

		}

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		
		//Atualizando a posição e orientação da câmera
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 30);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		
		glDrawArrays(GL_POINTS, 0, 24);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

	// W S A D - Adicionais: Q e E
	float cameraSpeed = 0.05;

	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (key == GLFW_KEY_Q)
	{
		cameraPos += cameraUp * cameraSpeed;
	}
	if (key == GLFW_KEY_E)
	{
		cameraPos -= cameraUp * cameraSpeed;
	}

	// DO 1 ao 6
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)  // Ver topo
	{
		rotate1 = true;
		rotate2 = false;
		rotate3 = false;
		rotate4 = false;
		rotate5 = false;
		rotate6 = false;
	}

	if (key == GLFW_KEY_2) // Ver base
	{
		rotate1 = false;
		rotate2 = true;
		rotate3 = false;
		rotate3 = false;
		rotate4 = false;
		rotate5 = false;
		rotate6 = false;
	}
	if (key == GLFW_KEY_3) // Ver frente
	{
		rotate1 = false;
		rotate2 = false;
		rotate3 = true;
		rotate3 = false;
		rotate4 = false;
		rotate5 = false;
		rotate6 = false;
	}
	if (key == GLFW_KEY_4) // Ver costa
	{
		rotate1 = false;
		rotate2 = false;
		rotate3 = false;
		rotate3 = false;
		rotate4 = true;
		rotate5 = false;
		rotate6 = false;
	}
	if (key == GLFW_KEY_5) // Ver esquerda
	{
		rotate1 = false;
		rotate2 = false;
		rotate3 = false;
		rotate3 = false;
		rotate4 = false;
		rotate5 = true;
		rotate6 = false;
	}
	if (key == GLFW_KEY_6) // Ver direita
	{
		rotate1 = false;
		rotate2 = false;
		rotate3 = false;
		rotate3 = false;
		rotate4 = false;
		rotate5 = false;
		rotate6 = true;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//cout << xpos << " " << ypos << endl;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	offsetx *= sensitivity;
	offsety *= sensitivity;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

//Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
// shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
// fragmentShader source no iniçio deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {

		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b
	        -0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
	        -0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		 0.5, -0.5, 0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5,-0.5, 1.0, 0.0, 1.0,

		 // Topo - Cinza

		 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
		 0.5, 0.5,-0.5, 0.5, 0.5, 0.5,
	        -0.5, 0.5, 0.5, 0.5, 0.5, 0.5,

		 0.5, 0.5,-0.5, 0.5, 0.5, 0.5,
		-0.5, 0.5,-0.5, 0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5, 0.5, 0.5, 0.5,

		  // Frente - Verde

		-0.5, 0.5, 0.5, 0.5, 1.0, 0.5,
		 0.5, 0.5, 0.5, 0.5, 1.0, 0.5,
	        -0.5,-0.5, 0.5, 0.5, 1.0, 0.5,

		-0.5,-0.5, 0.5, 0.5, 1.0, 0.5,
		 0.5, 0.5, 0.5, 0.5, 1.0, 0.5,
		 0.5,-0.5, 0.5, 0.5, 1.0, 0.5,

		 // Costa - Rosa

		 0.5,-0.5,-0.5, 1.0, 0.5, 1.0,
		-0.5,-0.5,-0.5, 1.0, 0.5, 1.0,
		 0.5, 0.5,-0.5, 1.0, 0.5, 1.0,

		 0.5, 0.5,-0.5, 1.0, 0.5, 1.0,
		-0.5,-0.5,-0.5, 1.0, 0.5, 1.0,
		-0.5, 0.5,-0.5, 1.0, 0.5, 1.0,

		 //Chão
		//x    y    z    r    g    b
		-5.0, -0.5,-5.0, 1.0, 0.5, 0.5,
		-5.0, -0.5, 5.0, 1.5, 0.5, 0.5,
		 5.0, -0.5,-5.0, 1.5, 0.5, 0.5,

		-5.0, -0.5, 5.0, 1.5, 0.5, 0.5,
		 5.0, -0.5, 5.0, 1.5, 0.5, 0.5,
		 5.0, -0.5,-5.0, 1.5, 0.5, 0.5,


	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);



	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}
