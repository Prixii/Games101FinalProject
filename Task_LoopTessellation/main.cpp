#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Model.h"

// ���ڴ�С�ص�
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// �������룺���� ESC �رմ���
void processInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main()
{
  Model model;
  model.InitFromObj("assets/tetrahedron.obj");
  // ��ʼ�� GLFW
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  // ���� OpenGL �汾 (3.3 Core Profile)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // ��������
  GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW + GLAD Demo", nullptr, nullptr);
  if (window == nullptr)
  {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // ��ʼ�� GLAD (������ OpenGL context ��)
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  // �����ӿںͻص�
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // ��ѭ��
  while (!glfwWindowShouldClose(window))
  {
    // ����
    processInput(window);

    // ���� (����ɫ: ����)
    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // �����������������¼�
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
