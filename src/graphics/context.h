#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 720

/* Contains some global variables representing the game context */
extern SDL_Texture  **sprites;
extern SDL_Window   *window;
extern SDL_Renderer *renderer;
extern SDL_Rect camera;
extern float scale;
extern unsigned scroll_dir;
extern int quit;

/* Initialize the context, return 0 on error */
int init_context(void);

/* Close and free the memory */
void close_context(void);

#endif /* _CONTEXT_H_ */
