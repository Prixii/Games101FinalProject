#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Model.h"

// 窗口大小回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// 处理输入：按下 ESC 关闭窗口
void processInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main()
{
  Model model;
  model.InitFromObj("assets/tetrahedron.obj");
  // 初始化 GLFW
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  // 配置 OpenGL 版本 (3.3 Core Profile)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建窗口
  GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW + GLAD Demo", nullptr, nullptr);
  if (window == nullptr)
  {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // 初始化 GLAD (在设置 OpenGL context 后)
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  // 设置视口和回调
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // 主循环
  while (!glfwWindowShouldClose(window))
  {
    // 输入
    processInput(window);

    // 清屏 (背景色: 深蓝)
    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 交换缓冲区并处理事件
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
