#ifndef _EVENT_H_
#define _EVENT_H_

#include <GLFW/glfw3.h>

#define IS_KEY_PRESSED(w, k) glfwGetKey(w, k) == GLFW_PRESS

void init_events(GLFWwindow *window);
void process_events(float deltatime);

#endif /* _EVENT_H_ */
