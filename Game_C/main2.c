#include "SDL.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#define TRUE 1
#define FALSE 0 
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60
#define FRAME_TARGET_TIME (1000/ FPS)

int game_is_running = FALSE;
int initialize_window(void);
void process_input();
void destroy_window();
void setup();
void update();
void render();
SDL_Window *win = NULL;
SDL_Renderer *renderer = NULL;

int last_frame_time = 0;

/*struct  ball {
	float x;
	float y;
	float w;
	float h;
} ball;*/

bool collision(float Ax, float Ay, float Aw, float Ah, float Bx, float By, float Bw, float Bh) {
	if(Ay + Ah < By) {
		return false;
	} else if(Ay > By + Bh) {
		return false;
	} else if(Ax + Aw < Bx) {
		return false;
	} else if(Ax > Bx + Bw) {
		return false;
	}

	return true;
}

struct game_object {
	float x;
	float y;
	float width;
	float height;
	float radius;
	float vel_x;
	float vel_y;
} ball, paddle;

int initialize_window(void) {
	
	//SDL_Texture *bitmapTex = NULL;
	//SDL_Surface *bitmapSurface = NULL

	//int posX = 100, posY = 100, width = 320, height = 240;

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");

		return FALSE; 
	}

	win = SDL_CreateWindow(
			"Hola Mundo", 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			WINDOW_WIDTH, 
			WINDOW_HEIGHT, 
			SDL_WINDOW_BORDERLESS
	);

	if(!win) {
		fprintf(stderr, "Error creating SDL Window.\n");

		return FALSE;
	}

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");

		return FALSE;
	}

	return TRUE;
}

void process_input() {
	SDL_Event ev;
	SDL_PollEvent(&ev);

	switch(ev.type) {
		case SDL_QUIT:
			game_is_running = FALSE;
			break;
		case SDL_KEYDOWN:
			if(ev.key.keysym.sym == SDLK_ESCAPE) {
				game_is_running = FALSE;
			}
			// Establece la velocidad de la paleta según las teclas de flecha izquierda / derecha
			switch(ev.key.keysym.sym) {
				case SDLK_LEFT:
					paddle.vel_x = -400;
					break;
				case SDLK_RIGHT:
					paddle.vel_x = +400;
					break;
			}

			break;
		case SDL_KEYUP:
			// Restablece la velocidad de la paleta según las teclas de flecha izquierda / derecha
			switch(ev.key.keysym.sym) {
				case SDLK_LEFT:
					paddle.vel_x = 0;
					break;
				case SDLK_RIGHT:
					paddle.vel_x = 0;
					break;
			}

			break;
	}

	/*while(1) {
        	SDL_Event ev;
                if(SDL_PollEvent(&ev)) {
                        if(ev.type == SDL_QUIT) {
                                break;
                        }
                }

                SDL_RenderClear(renderer);
                SDL_RenderPresent(renderer);
        }

i        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);

        SDL_Quit();*/
}

void setup(void) {
	/*ball.x = 20;
	ball.y = 20;
	ball.w = 15;
	ball.h = 15;*/

	// Inicializamos el objeto bola para que se mueva hacia abajo a una velocidad constante

	ball.width = 15;
	ball.height = 15;
	ball.radius = 15;
	ball.x = 20;
	ball.y = 20;
	ball.vel_x = 300;
	ball.vel_y = 300;

	// Inicializamos la posición de la paleta en la parte inferior de la pantalla
	paddle.width = 100;
	paddle.height = 20;
	paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
	paddle.y = WINDOW_HEIGHT - 40;
	paddle.vel_x = 0;
	paddle.vel_y = 0;
}

void update(void) {
	// Calcula cuánto tenemos que esperar hasta que alcancemos el marco de tiempo objetivo
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
	// Solo demora si somos demasiado rápidos para actualizar este marco
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);	
	}

	// Obtenga el factor delta_time convertido a segundos para usarlo para actualizar objetos
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	
	// Almacene los milisegundos del fotograma actual para usar en el siguiente
	last_frame_time = SDL_GetTicks();	

	ball.x += ball.vel_x * delta_time;
	ball.y += ball.vel_y * delta_time;

	// Actualizar la posición de la paleta en función de su velocidad
	paddle.x += paddle.vel_x * delta_time;
	paddle.y += paddle.vel_y * delta_time;
	// Evita que la paleta se mueva fuera de los límites de la ventana
	if( ((paddle.x + 100) > WINDOW_WIDTH) || (paddle.x < 0) ) {
		paddle.x -= paddle.vel_x;
	}

	// Compruebe si la bola colisiona con las paredes
	if( (ball.x + ball.radius) > WINDOW_WIDTH) {
		ball.vel_x = -ball.vel_x;
		ball.x = WINDOW_WIDTH - ball.radius;
	} else if((ball.x -ball.radius) < 0) {
		ball.vel_x = -ball.vel_x;
		ball.x = ball.radius;
	}

	if( (ball.y + ball.radius) > WINDOW_HEIGHT) {
		ball.vel_y = -ball.vel_y;
		ball.y = WINDOW_HEIGHT - ball.radius;
	} else if((ball.y -ball.radius) < 0) {
		ball.vel_y = -ball.vel_y;
		ball.y = ball.radius;
	} 
	// Verifique que el juego haya terminado cuando la pelota golpee la parte inferior de la pantalla
	else if( (ball.y + ball.radius) > (WINDOW_HEIGHT - ball.radius)) {
		game_is_running = FALSE;
	}

	// Verifique si la pelota colisiona con la paleta
	if(collision(ball.x, ball.y, ball.width, ball.height, paddle.x, paddle.y, paddle.width, paddle.height) == true) {
		ball.vel_y = -ball.y;
	}

	/*while(!SDL_TICKS_PASSED(SDL_GetTicks(),last_frame_time + FRAME_TARGET_TIME)); 
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	last_frame_time = SDL_GetTicks();	

	ball.x += 70 * delta_time;
	ball.y += 50 * delta_time;*/
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//dibujar rectángulo
	//SDL_Rect ball = {x, y, w, h};
	/*SDL_Rect pelota_rect = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.w,
		(int)ball.h
	};*/

	SDL_Rect pelota_rect = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.width,
		(int)ball.height
	};

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &pelota_rect);

	// Dibuja un rectángulo para el objeto paleta
	SDL_Rect paddle_rect = {
		(int)paddle.x,
		(int)paddle.y,
		(int)paddle.width,
		(int)paddle.height
	};

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle_rect);
	SDL_RenderPresent(renderer);
}

void destroy_window(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

int main(int argc, char *argv[]) {
	game_is_running = initialize_window();

	setup();

	while(game_is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}


