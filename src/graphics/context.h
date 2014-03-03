#ifndef _CONTEXT_H_
#define _CONTEXT_H_

/* Contains some global variables representing the game context */
SDL_Texture  **tiles;
SDL_Window   *window;
SDL_Renderer *renderer;

/* Initialize the context, return 0 on error */
int init_context(void);

/* Close and free the memory */
void close_context(void);

#endif /* _CONTEXT_H_ */
