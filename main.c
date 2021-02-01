#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cp_functions.h"

#define True  1
#define False 0
#define WindowTitle  "Breakout_6110405949_Jittin Jindanoraseth"
#define WindowWidth  800
#define WindowHeight 700

Sound hit_paddle_sound, hit_brick_sound;
Sound hit_top_sound, end_sound;
Texture paddle_texture, ball_texture;
Texture brick_texture, background_texture;
Font big_font, small_font;

// Structure for storing info for objects, i.e. Paddle, Brick, Ball.
typedef struct
{
    float x, y;
    float width, height;
    float vel_x, vel_y;
    int destroyed;
    // Add damage and durability of Object.
    int damage, durability;
} Object;

// Collision Detection between two objects a and b
int collide(Object a, Object b)
{
    if (a.x + a.width  < b.x || b.x + b.width  < a.x ||
        a.y + a.height < b.y || b.y + b.height < a.y)
        return False;
    else
        return True;
}

// Initial routine to load sounds, textures, and fonts.
int game_init()
{
    hit_paddle_sound = cpLoadSound("hitDown.wav");
    hit_brick_sound = cpLoadSound("hitUp.wav");
    hit_top_sound = cpLoadSound("hitTop.wav");
    end_sound = cpLoadSound("theEnd.wav");

    paddle_texture = cpLoadTexture("paddle.png");
    ball_texture = cpLoadTexture("ball.png");
    brick_texture = cpLoadTexture("brick.png");
    background_texture = cpLoadTexture("background.png");

    big_font = cpLoadFont("THSarabun.ttf", 60);
    small_font = cpLoadFont("THSarabun.ttf", 30);

    if (hit_paddle_sound == NULL || hit_brick_sound == NULL ||
        hit_top_sound == NULL || end_sound == NULL ||
        paddle_texture == NULL || ball_texture == NULL ||
        brick_texture == NULL || background_texture == NULL ||
        big_font == NULL || small_font == NULL)
        return False;
    return True;
}

int main(int argc, char *args[])
{
    enum { BALL_VEL_Y = -5, PADDLE_VEL_X = 7 };
    int running, n_bricks = 120, n_hits = 0, score = 0, lifes = 3;
    char msg[80];
    Object bricks[n_bricks];
    Object ball = {WindowWidth/2-12, 350, 24, 24, 0, BALL_VEL_Y, False, -1, -1};
    Object paddle = {WindowWidth/2-62, WindowHeight-50, 124, 18, 0, 0, False, -1, -1};
    Event event;

    if (cpInit(WindowTitle, WindowWidth, WindowHeight) == False) {
        fprintf(stderr, "Window initialization failed!\n");
        exit(1);
    }

    if (game_init() == False) {
        fprintf(stderr, "Game initialization failed!\n");
        exit(1);
    }

    // Build bricks.
    for (int n = 0, x = -10, y = 80, d = 3; n < n_bricks; n++) {
        bricks[n].width = 55;
        bricks[n].height = 18;
        bricks[n].x = x;
        bricks[n].y = y;
        bricks[n].destroyed = False;
        bricks[n].damage = 0;
        bricks[n].durability = d;

        x += bricks[n].width;
        if(n==14 || n==29 || n==44 || n==59 || n==74 || n==89 || n==104)
        {
            x = -10;
            y = y + 18;
            // Adjust durability.
            if(n==29 || n==74){
                d = d - 1;
            }
        }
    }

    running = True;
    while (running) {
        cpClearScreen();
        cpDrawTexture(255, 255, 255,
            0, 0, WindowWidth, WindowHeight, background_texture);
        cpDrawTexture(255, 255, 255,
            paddle.x, paddle.y, paddle.width, paddle.height, paddle_texture);
        cpDrawTexture(255, 255, 255,
            ball.x, ball.y, ball.width, ball.height, ball_texture);
        sprintf(msg, "คะแนน %d", score);
        cpDrawText(255, 255, 255, 3, 3, msg, small_font, 0);
        sprintf(msg, "ชีวิตคงเหลือ %d ชีวิต", lifes);
        cpDrawText(255, 255, 255, 640, 3, msg, small_font, 0);

        for (int n = 0; n < n_bricks; n++) {
            if (!bricks[n].destroyed){

                // Draw each brick pattern.
                int R=255, G=255, B=255; 
                if(bricks[n].durability == 3)
                {
                    if(bricks[n].damage==0){R=230;G=0;B=92;}
                    else if(bricks[n].damage==1){R=255;G=26;B=117;}
                    else if(bricks[n].damage==2){R=255;G=102;B=163;}
                }
                else if(bricks[n].durability == 2)
                {
                    if(bricks[n].damage==0){R=0;G=230;B=0;}
                    else if(bricks[n].damage==1){R=77;G=255;B=77;}
                }
                else if(bricks[n].durability == 1)
                {
                    R=230;G=247;B=255;
                }
                cpDrawTexture(R, G, B,
                bricks[n].x, bricks[n].y, bricks[n].width, bricks[n].height,
                brick_texture);
            } 
        }

        if (ball.y + ball.width > WindowHeight || n_hits == n_bricks) {
            cpPlaySound(end_sound);
            
            // Show the message after death. 
            if(n_hits == n_bricks){
                cpDrawText(255, 255, 255, 400, 250, "ชนะแล้ว สมแล้วที่ท่านเป็นผู้กล้า ^0^", big_font, 1);
                cpDrawText(255, 255, 255, 400, 350, "<กดปุ่ม ESC - เพื่อออกจากเกม>", big_font, 1);
            }
            else if(lifes>0 && n_hits != n_bricks){
                cpDrawText(255, 255, 255, 400, 250, "พยายามเข้า มาลองใหม่กันเถอะ =0=", big_font, 1);
                cpDrawText(255, 255, 255, 400, 350, "<กดปุ่ม R - เพื่อเล่นต่อ>", big_font, 1);
                cpDrawText(255, 255, 255, 400, 450, "<กดปุ่ม ESC - เพื่อออกจากเกม>", big_font, 1);
            }
            else if(lifes==0){
                cpDrawText(255, 255, 255, 400, 250, "ตายซะแล้ว โลกนี้จบสิ้นละ T^T", big_font, 1);
                cpDrawText(255, 255, 255, 400, 350, "<กดปุ่ม ESC - เพื่อออกจากเกม>", big_font, 1);
            }

            cpSwapBuffers();
            while (1) {
                cbEventListener(&event);
                
                //Press the button R to Continue playing.
                if (event.key.keysym.sym == K_r && n_hits < n_bricks && lifes >0) {
                    ball.x = WindowWidth/2-12;
                    ball.y = 350;
                    ball.vel_x = 0;
                    ball.vel_y = BALL_VEL_Y;

                    paddle.x = WindowWidth/2-62;
                    paddle.vel_x = 0;
                    paddle.vel_y = 0;

                    lifes--;
                    break;
                }

                else if (event.type == QUIT ||
                    event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                    running = False;
                    break;
                }
            }
        }
        cpSwapBuffers();

        while (cbEventListener(&event)) {
            if (event.type == QUIT ||
                 event.type == KEYUP && event.key.keysym.sym == K_ESCAPE) {
                running = False;
                break;
            }

            if (event.type == KEYDOWN) {
                if (event.key.keysym.sym == K_LEFT)
                    paddle.vel_x = -abs(PADDLE_VEL_X);
                if (event.key.keysym.sym == K_RIGHT)
                    paddle.vel_x = abs(PADDLE_VEL_X);
            }
            else
            if (event.type == KEYUP) {
                if (event.key.keysym.sym == K_LEFT)
                    paddle.vel_x = 0;
                if (event.key.keysym.sym == K_RIGHT)
                    paddle.vel_x = 0;
            }
        }
        paddle.x += paddle.vel_x;

        if (paddle.x < 0)
            paddle.x = 0;
        if (paddle.x + paddle.width > WindowWidth)
            paddle.x = WindowWidth - paddle.width;

        ball.x += ball.vel_x;
        ball.y += ball.vel_y;

        if (ball.x < 0 || ball.x + ball.width > WindowWidth)
            ball.vel_x = -ball.vel_x;

        if (ball.y < 0) {
            cpPlaySound(hit_top_sound);
            ball.vel_y = -ball.vel_y;
        }

        for (int n = 0; n < n_bricks; n++) {
            if (!bricks[n].destroyed &&
                collide(ball, bricks[n]) == True) {
                cpPlaySound(hit_brick_sound);
                ball.vel_y = -ball.vel_y;
                
                bricks[n].damage += 1;
                if(bricks[n].damage == bricks[n].durability){
                    bricks[n].destroyed = True;
                    n_hits++;
                }
                score += 10;
                break;
            }
        }

        if (collide(ball, paddle) == True) {
            cpPlaySound(hit_paddle_sound);
            
            //The ball can bounce on the paddle.
            ball.vel_y = -ball.vel_y;
            if(fabs(ball.x - paddle.x) == 50)
            {
                continue;
            }
            else if(ball.x - paddle.x > (paddle.width)*0.75)
            {
                ball.vel_x = 6;
            }
            else if(ball.x - paddle.x > (paddle.width)*0.5)
            {
                ball.vel_x = 4;
            }
            else if(ball.x - paddle.x > (paddle.width)*0.25)
            {
                ball.vel_x = -4;
            }
            else
            {
                ball.vel_x = -6;
            }
        }

        cpDelay(10);
    }
    cpCleanUp();
    return 0;
}
