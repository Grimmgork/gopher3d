#include <raylib/raylib.h>
#include <stdio.h>
#include <raylib/rcamera.h>
#include <raylib/raymath.h>

#include <gopher.h>
#include <arena.h>

#define MAX_COLUMNS 8
#define GLSL_VERSION 330
#define PIXELATION 1

int main(void)
{
	gopher_init();
	struct gopher_request* request = gopher_fetch("sdf.org", "70", "/maps");
	if (request == NULL) {
		printf("error while fetching\n");
		return 1;
	}

	char data[1024];
	while (true)
	{
		int count = gopher_recv(request, data, 1024);
		printf("%i\n", count);
		if (count == 0)
		{
			break;
		}
		if (count < 0)
		{
			printf("error while recv\n");
			break;
		}

		printf("%.*s\n", count, data);
	}

	gopher_close(request);
	gopher_end();
	return 0;

	SetConfigFlags(FLAG_VSYNC_HINT); // enable vsync
    
	InitWindow(800, 400, "gopher3d");
	const int display = GetCurrentMonitor();
	const int screenWidth = GetMonitorWidth(display);
    const int screenHeight = GetMonitorHeight(display);
	SetWindowSize(screenWidth, screenHeight);
	ToggleFullscreen();

	Shader shader = LoadShader(0, TextFormat("resources/shaders/glsl%i/shader.fs", GLSL_VERSION));

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255 };
    }

	RenderTexture2D renderTarget = LoadRenderTexture(screenWidth / PIXELATION, screenHeight / PIXELATION);

	int sl_fogDensity = GetShaderLocation(shader, "fogDensity");
	float fogDensity = 0.15f;
    SetShaderValue(shader, sl_fogDensity, &fogDensity, SHADER_UNIFORM_FLOAT);
	
	int sl_fogColor = GetShaderLocation(shader, "fogColor");
	Vector4 fogColor = ColorNormalize(BLACK);
	SetShaderValue(shader, sl_fogColor, &fogColor, SHADER_UNIFORM_VEC4);

	int sl_viewPosition = GetShaderLocation(shader, "viewPosition");
	SetShaderValue(shader, sl_viewPosition, &camera.position, SHADER_UNIFORM_VEC3);

    DisableCursor();                    // Limit cursor to relative movement inside the window
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		SetShaderValue(shader, sl_viewPosition, &camera.position, SHADER_UNIFORM_VEC3);

		// render scene to texture for hdr
		BeginTextureMode(renderTarget);
            ClearBackground(BLACK);
            BeginMode3D(camera);
				BeginShaderMode(shader);
                	DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
                	DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                	DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                	DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

                	for (int i = 0; i < MAX_COLUMNS; i++)
                	{
                    	DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
                	}
				EndShaderMode();
            EndMode3D();
        EndTextureMode();

		// appply hdr shaders to rendered scene and draw to sceen
        BeginDrawing();
			DrawTexturePro(renderTarget.texture, (Rectangle){0, 0, screenWidth / PIXELATION, -(screenHeight / PIXELATION) }, (Rectangle){ 0, 0, screenWidth, screenHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);
			DrawFPS(10, 10);
        EndDrawing();
    }

	UnloadShader(shader);
	UnloadRenderTexture(renderTarget);
    CloseWindow();        // Close window and OpenGL context
    return 0;
}