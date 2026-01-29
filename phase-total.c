#include "raylib.h"
#include <math.h>
#include <stdio.h>

// ================= Screen =================
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// ================= Map ====================
#define CELL_SIZE 40
#define MAP_WIDTH 20
#define MAP_HEIGHT 15

// ================= Player =================
typedef struct Player {
    Vector2 pos;        // Player position (world space)
    Vector2 dir;        // Direction vector
    Vector2 plane;      // Camera plane (for FOV)
    float speed;        // Movement speed
    float rotSpeed;     // Rotation speed
} Player;

// ================= World Map ==============
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
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// ================= Save / Load (Phase 3) =================
void SaveMap(void)
{
    FILE *f = fopen("map.bin", "wb");
    if (!f) return;
    fwrite(world_map, sizeof(world_map), 1, f);
    fclose(f);
}

void LoadMap(void)
{
    FILE *f = fopen("map.bin", "rb");
    if (!f) return;
    fread(world_map, sizeof(world_map), 1, f);
    fclose(f);
}

// ================= Main ====================
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycasting Project - Phase 1, 2, 3");
    SetTargetFPS(60);

    // -------- Player Init (Phase 1) --------
    Player player = {0};
    player.pos = (Vector2){120, 120};
    player.dir = (Vector2){1, 0};
    player.plane = (Vector2){0.0f, 0.66f};
    player.speed = 200;
    player.rotSpeed = 2.5f;

    // -------- Phase 3 State --------
    bool editMode = false;

    // ================= Game Loop =================
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // -------- Mode Switching (Phase 3) --------
        if (IsKeyPressed(KEY_M)) editMode = !editMode;
        if (IsKeyPressed(KEY_S)) SaveMap();
        if (IsKeyPressed(KEY_L)) LoadMap();

        // -------- Movement (Phase 1) --------
        Vector2 velocity = {0};

        if (!editMode)
        {
            if (IsKeyDown(KEY_W)) velocity.x += player.dir.x * player.speed,
                                  velocity.y += player.dir.y * player.speed;
            if (IsKeyDown(KEY_S)) velocity.x -= player.dir.x * player.speed,
                                  velocity.y -= player.dir.y * player.speed;
            if (IsKeyDown(KEY_A)) velocity.x -= player.plane.x * player.speed,
                                  velocity.y -= player.plane.y * player.speed;
            if (IsKeyDown(KEY_D)) velocity.x += player.plane.x * player.speed,
                                  velocity.y += player.plane.y * player.speed;

            float nextX = player.pos.x + velocity.x * dt;
            int mapX = (int)(nextX / CELL_SIZE);
            int mapY = (int)(player.pos.y / CELL_SIZE);
            if (world_map[mapY][mapX] == 0) player.pos.x = nextX;

            float nextY = player.pos.y + velocity.y * dt;
            mapX = (int)(player.pos.x / CELL_SIZE);
            mapY = (int)(nextY / CELL_SIZE);
            if (world_map[mapY][mapX] == 0) player.pos.y = nextY;

            float rot = player.rotSpeed * dt;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT))
            {
                float angle = IsKeyDown(KEY_LEFT) ? rot : -rot;

                float oldDirX = player.dir.x;
                player.dir.x = player.dir.x * cosf(angle) - player.dir.y * sinf(angle);
                player.dir.y = oldDirX * sinf(angle) + player.dir.y * cosf(angle);

                float oldPlaneX = player.plane.x;
                player.plane.x = player.plane.x * cosf(angle) - player.plane.y * sinf(angle);
                player.plane.y = oldPlaneX * sinf(angle) + player.plane.y * cosf(angle);
            }
        }

        // -------- Map Editing (Phase 3) --------
        if (editMode)
        {
            Vector2 mouse = GetMousePosition();
            int mx = mouse.x / CELL_SIZE;
            int my = mouse.y / CELL_SIZE;

            if (mx >= 0 && my >= 0 && mx < MAP_WIDTH && my < MAP_HEIGHT)
            {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    world_map[my][mx] = 1;
                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                    world_map[my][mx] = 0;
            }
        }

        // ================= Draw =================
        BeginDrawing();
        ClearBackground(BLACK);

        if (editMode)
        {
            // -------- 2D Map View (Phase 1) --------
            for (int y = 0; y < MAP_HEIGHT; y++)
                for (int x = 0; x < MAP_WIDTH; x++)
                    DrawRectangle(x * CELL_SIZE, y * CELL_SIZE,
                                  CELL_SIZE - 1, CELL_SIZE - 1,
                                  world_map[y][x] ? DARKGRAY : LIGHTGRAY);
             // fir player
            DrawCircle((int)player.pos.x, (int)player.pos.y, 10, RED);

            // for line(direction)
            DrawLine(
                (int)player.pos.x,(int)player.pos.y,
                (int)(player.pos.x + player.dir.x * 30),
                (int)(player.pos.y + player.dir.y * 30),
                RED
            );
        }
        else
        {
            // -------- Raycasting (Phase 2) --------
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, SKYBLUE);
            DrawRectangle(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, DARKGRAY);

            for (int x = 0; x < SCREEN_WIDTH; x++)
            {
                float cameraX = 2 * x / (float)SCREEN_WIDTH - 1;
                float rayDirX = player.dir.x + player.plane.x * cameraX;
                float rayDirY = player.dir.y + player.plane.y * cameraX;

                int mapX = (int)(player.pos.x / CELL_SIZE);
                int mapY = (int)(player.pos.y / CELL_SIZE);

                float deltaDistX = fabs(1 / rayDirX);
                float deltaDistY = fabs(1 / rayDirY);

                float sideDistX, sideDistY;
                int stepX, stepY, side;

                if (rayDirX < 0)
                    stepX = -1, sideDistX = (player.pos.x / CELL_SIZE - mapX) * deltaDistX;
                else
                    stepX = 1, sideDistX = (mapX + 1 - player.pos.x / CELL_SIZE) * deltaDistX;

                if (rayDirY < 0)
                    stepY = -1, sideDistY = (player.pos.y / CELL_SIZE - mapY) * deltaDistY;
                else
                    stepY = 1, sideDistY = (mapY + 1 - player.pos.y / CELL_SIZE) * deltaDistY;

                while (world_map[mapY][mapX] == 0)
                {
                    if (sideDistX < sideDistY)
                        sideDistX += deltaDistX, mapX += stepX, side = 0;
                    else
                        sideDistY += deltaDistY, mapY += stepY, side = 1;
                }

                float perpWallDist = (side == 0)
                    ? (sideDistX - deltaDistX)
                    : (sideDistY - deltaDistY);

                int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
                int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
                int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;

                // -------- Wall Shading (Phase 2) --------
                Color wallColor = GRAY;

                // Horizontal walls are darker to create depth
                if (side == 1)
                {
                wallColor = (Color){120, 120, 120, 255};
                }

                DrawLine(x, drawStart, x, drawEnd, wallColor);


                
            }

            // -------- Minimap (Phase 1) --------
            for (int y = 0; y < MAP_HEIGHT; y++)
                for (int x = 0; x < MAP_WIDTH; x++)
                    DrawRectangle(x * 8, y * 8, 8, 8,
                        world_map[y][x] ? DARKGRAY : LIGHTGRAY);

            DrawCircle(player.pos.x / CELL_SIZE * 8,
                       player.pos.y / CELL_SIZE * 8, 3, RED);
            // -------- Player Direction (Minimap) --------
            DrawLine(
                player.pos.x / CELL_SIZE * 8,
                player.pos.y / CELL_SIZE * 8,
                (player.pos.x + player.dir.x * 40) / CELL_SIZE * 8,
                (player.pos.y + player.dir.y * 40) / CELL_SIZE * 8,
                RED
            );

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
