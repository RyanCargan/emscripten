#include <stdio.h>
#include <assert.h>
#include <emscripten.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

GLFWwindow* g_window;

int inFullscreen = 0;
int wasFullscreen = 0;

void render();
void error_callback(int error, const char* description);

void render() {
  glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void error_callback(int error, const char* description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
  int isInFullscreen = EM_ASM_INT(return !!(document.fullscreenElement || document.mozFullScreenElement || document.webkitFullscreenElement || document.msFullscreenElement));
  if (isInFullscreen && !wasFullscreen) {
    printf("Successfully transitioned to fullscreen mode!\n");
    wasFullscreen = isInFullscreen;
  }
  
  if (wasFullscreen && !isInFullscreen) {
    printf("Exited fullscreen. Test succeeded.\n");
#ifdef REPORT_RESULT
    REPORT_RESULT(1);
#endif
    wasFullscreen = isInFullscreen;
    emscripten_cancel_main_loop();
    return;
  }  
}

int main() {
  // Setup g_window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    printf("Could not create window. Test failed.\n");      
#ifdef REPORT_RESULT
    REPORT_RESULT(0);
#endif      
    return -1;
  }
  glfwWindowHint(GLFW_RESIZABLE , 1);
  g_window = glfwCreateWindow(600, 450, "GLFW resizing test - windowed", NULL, NULL);
  if (!g_window)
  {
    printf("Could not create window. Test failed.\n");      
#ifdef REPORT_RESULT
    REPORT_RESULT(0);
#endif           
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(g_window);

  // Install callbacks
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);

  // Main loop
  printf("You should see a yellow canvas.\n");
  printf("Check the 'resize canvas' option on the upper right corner and then enter full screen mode.\n");
  printf("When in full screen, you should see the whole screen filled yellow, and after exiting, the yellow canvas should be restored in the window.\n");
  printf("REMARK: You MUST check the 'resize canvas' option!\n");
  emscripten_set_main_loop(render, 0, 1);

  glfwTerminate();

  return 0;
}
