#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "stb_image.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <thread>
#include <vector>


const std::string PROJ_DIR = "/home/lenty/scripts/cpp/opengl/";
const std::string SUB_DIR = "ch9.8_2";

const std::string VERTEX_SRC = PROJ_DIR + "src/" + SUB_DIR + "/vertex.sd";

const std::string FRAGMENT_SRC = PROJ_DIR + "src/" + SUB_DIR + "/fragment.sd";

const std::string TEXTURE_PATH_FLOOR = PROJ_DIR + "assets/floor.jpg";
const std::string TEXTURE_PATH_WALL = PROJ_DIR + "assets/wall.jpg";

class Shader {
public:
  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath);
  // use/activate the shader
  void use();
  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  unsigned int getProgramId();
  ~Shader();

private:
  // the program ID
  unsigned int mId{std::numeric_limits<unsigned int>::max()};

  void read_shader_file(const char *vertexPath, const char *fragmentPath,
                        std::string *vertexCode, std::string *fragCode);
  unsigned int compile_shader(GLenum shaderType, const char *shaderCode,
                              std::string const &shader_name);
  unsigned int create_program(unsigned int vertexShader,
                              unsigned int fragementShader);
};

std::vector<unsigned int> vbos{};
std::vector<unsigned int> vaos{};
std::vector<unsigned int> ebos{};

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void init_glfw() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLFWwindow *create_window() {
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);
  return window;
}

void load_glad() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    exit(-1);
  }
}

unsigned int create_vao() {
  float vertices[] = {
      // positions        // texture st
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //
      0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, //
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //
      -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
      0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, //
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, //
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f  //
  };

  unsigned int VBO, VAO;

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // layout num, type,xxx  ,stride, offset

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  vaos.push_back(VAO);
  vbos.push_back(VBO);
  return VAO;
}

unsigned int create_ebo(int vao) {
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 2, // first Triangle
      2, 3, 0  // second Triangle
  };

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(vao);
  // remember: do NOT unbind the EBO while a VAO is active as the bound element
  // buffer object IS stored in the VAO; keep the EBO bound.
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary.

  return EBO;
}

void clean_buffer() {
  while (!vaos.empty()) {
    glDeleteVertexArrays(1, &vaos.back());
    vaos.pop_back();
  }

  while (!vbos.empty()) {
    glDeleteBuffers(1, &vbos.back());
    vbos.pop_back();
  }
  while (!ebos.empty()) {
    glDeleteBuffers(1, &ebos.back());
    ebos.pop_back();
  }
}

unsigned int create_texture(const char *texture_path) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on currently bound texture)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int image_w, image_h, image_nCh;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *image_data =
      stbi_load(texture_path, &image_w, &image_h, &image_nCh, 0);

  if (image_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image_data);
  return texture;
}

// ===============================
Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::string vertexCode;
  std::string fragmentCode;

  read_shader_file(vertexPath, fragmentPath, &vertexCode, &fragmentCode);
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  auto vertexShader = compile_shader(GL_VERTEX_SHADER, vShaderCode, "vertex");
  auto fragmentShader =
      compile_shader(GL_FRAGMENT_SHADER, fShaderCode, "fragement");

  mId = create_program(vertexShader, fragmentShader);
}

void Shader::read_shader_file(const char *vertexPath, const char *fragmentPath,
                              std::string *vertexCode, std::string *fragCode) {
  try {
    std::ifstream vShaderFile{vertexPath};
    std::ifstream fShaderFile{fragmentPath};

    std::stringstream vShaderStream, fShaderStream;
    // read file’s buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    *vertexCode = vShaderStream.str();
    *fragCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR: SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
}

unsigned int Shader::compile_shader(GLenum shaderType, const char *shaderCode,
                                    std::string const &shader_name) {
  int success;
  char infoLog[512];
  // vertex Shader
  auto shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);
  // print compile errors if any
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR Failed to compile SHADER('" << shader_name << "',"
              << shaderType << ")\n"
              << infoLog << std::endl;
  };
  return shader;
}

unsigned int Shader::create_program(unsigned int vertex,
                                    unsigned int fragment) {
  int success;
  char infoLog[512];
  unsigned int id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);
  // print linking errors if any
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  // delete shaders; they’re linked into our program and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return id;
}

void Shader::use() { glUseProgram(mId); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(mId, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(mId, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(mId, name.c_str()), value);
}

Shader::~Shader() {
  // if (mId != std::numeric_limits<unsigned int>::max()) {
  //   glDeleteProgram(mId);
  // }
}

unsigned int Shader::getProgramId() { return mId; }
