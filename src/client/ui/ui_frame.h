#ifndef _UI_FRAME_H_
#define _UI_FRAME_H_

typedef struct ui_frame ui_frame_t;

/* Function Pointers Types */
typedef void(*update_func)(ui_frame_t*,float);
typedef void(*destroy_func)(ui_frame_t*);
typedef void(*draw_func)(ui_frame_t*);
typedef int(*mouse_move_func)(ui_frame_t*,double,double);
typedef int(*mouse_button_func)(ui_frame_t*,int,int,int);
typedef int(*mouse_scroll_func)(ui_frame_t*,double,double);
typedef int(*key_func)(ui_frame_t*,int,int,int,int);

struct ui_frame
{
	int movable;
	int hidden;
	float x;
	float y;
	float width;
	float height;

	destroy_func destroy;
	update_func update;
	draw_func draw;
	mouse_move_func on_mouse_move;
	mouse_button_func on_mouse_button;
	mouse_scroll_func on_mouse_scroll;
	key_func on_key;
	
	ui_frame_t *childs;
	void * data; // rlly ?
	
};

ui_frame_t *create_ui();

void destroy_ui(ui_frame_t *frame);
void draw_ui(ui_frame_t *frame);
void update_ui(ui_frame_t *frame,float deltatime);
void ui_on_mouse_move(ui_frame_t* frame, double x, double y);
void ui_on_mouse_button(ui_frame_t* frame, int button, int action, int mods);
void ui_on_mouse_scroll(ui_frame_t* frame, double sx, double sy);
void ui_on_key(ui_frame_t* frame, int key, int scancode, int action, int mods);

#endif /*  _UI_FRAME_H_ */
