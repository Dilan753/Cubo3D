#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shaders.h"
#include "Buffers.h"
#include "CarasCubo.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "filesystem"

const char* vertexShaderSource = getVertexShader();
const char* fragmentShaderSource = getFragmentShader();

unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string tipo = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cout << "Error compilando shader (" << tipo << "):\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc)
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Error linkeando shader program:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main()
{
    // ---------------- Ventana y contexto ----------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(900, 800, "Cuadrado Base", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error creando ventana\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error inicializando GLAD\n";
        return -1;
    }
    glViewport(0, 0, 900, 800);

    // Depth test: necesario para que al rotar en 3D las caras se dibujen
    // respetando cual esta mas cerca o mas lejos de la camara, en vez de
    // simplemente en el orden en que las mandas a dibujar.
    glEnable(GL_DEPTH_TEST);

    // ---------------- Shaders ----------------
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // ---------------- Geometria del cuadrado ----------------
    float vertices[] = {
        // posiciones          // colores           // texCoords
         0.3f,  0.3f, 0.0f,    1.0f, 0.0f, 0.0f,     1.0f, 1.0f, // arriba derecha
         0.3f, -0.3f, 0.0f,    0.0f, 1.0f, 0.0f,     1.0f, 0.0f, // abajo derecha
        -0.3f, -0.3f, 0.0f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f, // abajo izquierda
        -0.3f,  0.3f, 0.0f,    1.0f, 1.0f, 0.0f,     0.0f, 1.0f  // arriba izquierda
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    float verticesFondo[] = {
        // posiciones          // colores (no importan aqui) // texCoords
         1.0f,  1.0f, 0.0f,    1,1,1,                          1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,    1,1,1,                          1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,    1,1,1,                          0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,    1,1,1,                          0.0f, 1.0f
    };
    unsigned int indicesFondo[] = { 0, 1, 3, 1, 2, 3 };

    Mesh fondo(verticesFondo, sizeof(verticesFondo), indicesFondo, 6);

    // Toda la creacion/configuracion de VAO, VBO, EBO queda encapsulada aqui,
    // fuera del while, en una sola linea.
    Mesh cuadrado(vertices, sizeof(vertices), indices, 6);

    // Ubicacion del uniform "model" dentro del shader (se busca una sola vez,
    // fuera del loop, para no repetir esta busqueda en cada frame)
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Como se comporta la textura fuera del rango 0-1 y al escalarla
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    // stb_image por defecto invierte el eje Y de la imagen para que
    // coincida con como OpenGL espera las coordenadas de textura
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("stephanie_Vaquer_Profile.png", &width, &height, &nrChannels, 0);
    std::cout << "Buscando imagen en: " << std::filesystem::current_path() << std::endl;
    if (data)
    {
        GLenum formato = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, formato, width, height, 0, formato, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Error cargando la textura\n";
    }
    stbi_image_free(data); // ya se subio a la GPU, se libera la copia en RAM

    unsigned int texturaFondo;
    glGenTextures(1, &texturaFondo);
    glBindTexture(GL_TEXTURE_2D, texturaFondo);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int widthFondo, heightFondo, nrChannelsFondo;
    unsigned char* dataFondo = stbi_load("texturaFondo.jpg", &widthFondo, &heightFondo, &nrChannelsFondo, 0);

    if (dataFondo)
    {
        GLenum formatoFondo = (nrChannelsFondo == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, formatoFondo, widthFondo, heightFondo, 0, formatoFondo, GL_UNSIGNED_BYTE, dataFondo);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Error cargando la textura de fondo\n";
    }
    stbi_image_free(dataFondo);


    // ---------------- Loop principal (bien limpio) ----------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        
        glDepthMask(GL_FALSE); // el fondo no debe "tapar" al cubo despues

        glUseProgram(shaderProgram);
        glm::mat4 modelFondo = glm::mat4(1.0f); // identidad, sin mover nada
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelFondo));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaFondo);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

        fondo.draw();

        glDepthMask(GL_TRUE); // reactivar para que el cubo se dibuje con profundidad normal

        // Rotacion automatica basada en el tiempo transcurrido, para poder
        // ver todas las caras del "cubo" sin mover la camara. Esta matriz
        // se combina con la de cada cara (traslacion + rotacion propia).
        float tiempo = (float)glfwGetTime();
        glm::mat4 rotacionGlobal = glm::mat4(1.0f);
        rotacionGlobal = glm::rotate(rotacionGlobal, tiempo, glm::vec3(0.5f, 1.0f, 0.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);



        for (unsigned int i = 0; i < CANTIDAD_CARAS_CUBO; i++)
        {
            glm::mat4 model = rotacionGlobal;
            model = glm::translate(model, carasCubo[i].traslacion);
            if (carasCubo[i].angulo != 0.0f)
                model = glm::rotate(model, glm::radians(carasCubo[i].angulo), carasCubo[i].eje);

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            cuadrado.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ---------------- Limpieza ----------------
    cuadrado.destroy();
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}