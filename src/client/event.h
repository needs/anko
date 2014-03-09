#ifndef _EVENT_H_
#define _EVENT_H_

#include <client/ui/ui_frame.h>
#include <GLFW/glfw3.h>

void init_events(GLFWwindow *window);
void events_link_frame(ui_frame_t **frame);

#endif /* _EVENT_H_ */
