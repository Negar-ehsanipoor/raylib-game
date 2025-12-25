#include "raylib.h"
#include <math.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 40
#define MAP_WIDTH 20
#define MAP_HEIGHT 15

// ----------------------------------------
typedef struct Player {
    Vector2 pos;       
    Vector2 dir;      
    Vector2 plane;     
    float speed;       
    float rotSpeed;    
} Player;

// -------------------- map--------------------
int world_map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Phase 2 - Vectors & Movement");
    SetTargetFPS(60);

    // -------------------- player init--------------------
    Player player = {0};
    player.pos = (Vector2){ 120.0f, 120.0f };
    player.dir = (Vector2){ 1.0f, 0.0f };      
    player.plane = (Vector2){ 0.0f, 1.0f };  //pb
    player.speed = 200.0f;
    player.rotSpeed = 2.5f;

    // ----------------------------------------
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ----------(WASD) ----------
        Vector2 velocity = {0};

        if (IsKeyDown(KEY_W)) {
            velocity.x += player.dir.x * player.speed;
            velocity.y += player.dir.y * player.speed;
        }
        if (IsKeyDown(KEY_S)) {
            velocity.x -= player.dir.x * player.speed;
            velocity.y -= player.dir.y * player.speed;
        }
        if (IsKeyDown(KEY_A)) {
            velocity.x -= player.plane.x * player.speed;
            velocity.y -= player.plane.y * player.speed;
        }
        if (IsKeyDown(KEY_D)) {
            velocity.x += player.plane.x * player.speed;
            velocity.y += player.plane.y * player.speed;
        }

        // ----------- Collision + Wall Sliding -----------

        // X axis
        float nextX = player.pos.x + velocity.x * dt;
        int mapX = (int)(nextX / CELL_SIZE);
        int mapY = (int)(player.pos.y / CELL_SIZE);

        if (world_map[mapY][mapX] == 0) {
            player.pos.x = nextX;
        }

        // Y axis
        float nextY = player.pos.y + velocity.y * dt;
        mapX = (int)(player.pos.x / CELL_SIZE);
        mapY = (int)(nextY / CELL_SIZE);

        if (world_map[mapY][mapX] == 0) {
            player.pos.y = nextY;
        }


        // ---------- چرخش (Arrow Keys) ----------
        float rot = player.rotSpeed * dt;

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT))
        {
            float angle = (IsKeyDown(KEY_LEFT)) ? rot : -rot;

            //[cosθ  -sinθ] [x]
            //[sinθ   cosθ] [y]
            
            // x' = x cosθ − y sinθ
            //y' = x sinθ + y cosθ
            float oldDirX = player.dir.x;
            player.dir.x = player.dir.x * cosf(angle) - player.dir.y * sinf(angle);
            player.dir.y = oldDirX * sinf(angle) + player.dir.y * cosf(angle);

            float oldPlaneX = player.plane.x;
            player.plane.x = player.plane.x * cosf(angle) - player.plane.y * sinf(angle);
            player.plane.y = oldPlaneX * sinf(angle) + player.plane.y * cosf(angle);
        }

        // ----------draw----------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // for map
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                Color c = (world_map[y][x] == 1) ? DARKGRAY : LIGHTGRAY;
                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE,
                              CELL_SIZE - 1, CELL_SIZE - 1, c);
            }
        }

        // fir player
        DrawCircle((int)player.pos.x, (int)player.pos.y, 10, RED);

        // for line(direction)
        DrawLine(
            (int)player.pos.x,(int)player.pos.y,
            (int)(player.pos.x + player.dir.x * 30),
            (int)(player.pos.y + player.dir.y * 30),
            RED
        );

        DrawText("WASD: Move | Arrows -> <-: Rotate", 10, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}



