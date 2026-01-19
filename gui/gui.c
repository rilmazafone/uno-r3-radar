#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720
#define MAX_DISTANCE 40

int serial_fd = -1;
char rawData[256] = "";
float distanceInPixels = 0;
int angle = 0;
int distance = 0;
int showRedLine = 0;
float radarRadius;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(
        "Arduino Radar",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    radarRadius = (WINDOW_WIDTH - 50) / 2.0;
    
    const char* port = "/dev/cu.usbmodem11401";
    serial_fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    
    if (serial_fd == -1) {
        printf("Warning: Could not open serial port %s\n", port);
    } else {
        struct termios options;
        tcgetattr(serial_fd, &options);
        
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        
        tcsetattr(serial_fd, TCSANOW, &options);
    }
}

float map_value(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_arc(float cx, float cy, float radius, float start_angle, float end_angle) {
    int segments = 50;
    float angle_step = (end_angle - start_angle) / segments;
    
    for (int i = 0; i < segments; i++) {
        float angle1 = start_angle + i * angle_step;
        float angle2 = start_angle + (i + 1) * angle_step;
        
        int x1 = (int)(cx + radius * cos(angle1));
        int y1 = (int)(cy + radius * sin(angle1));
        int x2 = (int)(cx + radius * cos(angle2));
        int y2 = (int)(cy + radius * sin(angle2));
        
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void drawRadar() {
    float centerX = WINDOW_WIDTH / 2.0;
    float centerY = WINDOW_HEIGHT - 50;
    
    SDL_SetRenderDrawColor(renderer, 100, 250, 25, 255);
    
    int numberOfCircles = 4;
    for (int i = 1; i <= numberOfCircles; i++) {
        float r = radarRadius * i / numberOfCircles;
        draw_arc(centerX, centerY, r, M_PI, M_PI * 2);
    }
}

void drawLine() {
    float centerX = WINDOW_WIDTH / 2.0;
    float centerY = WINDOW_HEIGHT - WINDOW_HEIGHT * 0.075;
    float lineLength = WINDOW_HEIGHT * 0.75;
    
    float angle_rad = angle * M_PI / 180.0;
    float x2 = centerX + lineLength * cos(angle_rad);
    float y2 = centerY + lineLength * sin(angle_rad);
    
    SDL_SetRenderDrawColor(renderer, 50, 250, 50, 255);
    
    // Draw thick line by drawing multiple lines
    for (int offset = -5; offset <= 5; offset++) {
        SDL_RenderDrawLine(renderer, 
            (int)centerX, (int)centerY + offset,
            (int)x2, (int)y2 + offset);
    }
}

void drawObject() {
    if (!showRedLine) return;
    
    float centerX = WINDOW_WIDTH / 2.0;
    float centerY = WINDOW_HEIGHT - 50;
    
    float angle_rad = angle * M_PI / 180.0;
    float x = distanceInPixels * cos(angle_rad);
    float y = -distanceInPixels * sin(angle_rad);
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    for (int offset = -1; offset <= 1; offset++) {
        SDL_RenderDrawLine(renderer, 
            (int)centerX + offset, (int)centerY,
            (int)(centerX + x) + offset, (int)(centerY + y));
    }
}

void serialEvent() {
    if (serial_fd == -1) return;
    
    static char buffer[256];
    static int buffer_pos = 0;
    
    char c;
    while (read(serial_fd, &c, 1) > 0) {
        if (c == '.') {
            buffer[buffer_pos] = '\0';
            
            // Trim and parse
            char* trimmed = buffer;
            while (*trimmed == ' ' || *trimmed == '\n' || *trimmed == '\r') trimmed++;
            
            // Find comma
            char* comma = strchr(trimmed, ',');
            if (comma != NULL) {
                *comma = '\0';
                angle = atoi(trimmed);
                distance = atoi(comma + 1);
                
                if (distance > 0) {
                    showRedLine = 1;
                    distanceInPixels = map_value(distance, 0, MAX_DISTANCE, 0, radarRadius);
                } else {
                    showRedLine = 0;
                    distanceInPixels = 0;
                }
            }
            
            buffer_pos = 0;
        } else if (buffer_pos < 255) {
            buffer[buffer_pos++] = c;
        }
    }
}

void draw() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 4);
    SDL_Rect fade_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 50};
    SDL_RenderFillRect(renderer, &fade_rect);
    
    drawRadar();
    drawObject();
    drawLine();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect bottom_rect = {0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &bottom_rect);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    setup();
    
    int running = 1;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
        }
        
        serialEvent();
        

        draw();
        

        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);
    }
    

    if (serial_fd != -1) close(serial_fd);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}