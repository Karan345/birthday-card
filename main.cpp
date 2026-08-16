#include "raylib.h"
#include <cmath>
#include <vector>
#include <cstdlib>

// Confetti particle structure
struct ConfettiParticle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float size;
    float rotation;
    float rotationSpeed;
};

// Function to spawn an explosive burst of confetti from a target center point
void SpawnConfetti(std::vector<ConfettiParticle>& particles, Vector2 origin) {
    int particleCount = 120;
    Color colors[] = { 
        { 219, 48, 34, 255 },   // festiveRed
        { 245, 197, 24, 255 },  // festiveGold
        { 52, 152, 219, 255 },  // Blue
        { 46, 204, 113, 255 },  // Green
        { 155, 89, 182, 255 },  // Purple
        { 230, 126, 34, 255 }   // Orange
    };

    for (int i = 0; i < particleCount; i++) {
        ConfettiParticle p;
        p.position = origin;
        
        // Random angle and velocity for an explosive burst
        float angle = (float)GetRandomValue(0, 360) * (PI / 180.0f);
        float speed = (float)GetRandomValue(150, 500);
        
        p.velocity = { cosf(angle) * speed, sinf(angle) * speed - 220.0f }; // Slight upward bias
        p.color = colors[GetRandomValue(0, 5)];
        p.size = (float)GetRandomValue(6, 14);
        p.rotation = (float)GetRandomValue(0, 360);
        p.rotationSpeed = (float)GetRandomValue(-400, 400);
        
        particles.push_back(p);
    }
}

int main() {
    // 1. Initialization
    const int screenWidth = 1000;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Happy Birthday, Auntie Pooty!");
    
    // Initialize the audio device for music/sound playback
    InitAudioDevice();

    SetTargetFPS(60);

    // 2. Load Assets (Font, Present Image, and Background Music)
    Font customFont = LoadFontEx("assets/PTSans-BoldItalic.ttf", 96, 0, 0);
    if (customFont.texture.id == 0) {
        customFont = GetFontDefault();
    } else {
        SetTextureFilter(customFont.texture, TEXTURE_FILTER_BILINEAR);
    }

    Texture2D presentTexture = LoadTexture("assets/present.png");

    // Load background music using your exact filename
    Music bgm = LoadMusicStream("assets/gr0za-birthday-happy-birthday-503371.mp3");
    bool musicLoaded = (bgm.stream.buffer != nullptr);
    
    if (musicLoaded) {
        PlayMusicStream(bgm);
    }

    // Interactive State Variables
    bool cardOpened = false;
    std::vector<ConfettiParticle> confettiParticles;
    bool confettiSpawned = false;

    // Colors
    Color softCream = { 253, 246, 227, 255 };
    Color festiveRed = { 219, 48, 34, 255 };
    Color festiveGold = { 245, 197, 24, 255 };
    Color darkText = { 40, 40, 40, 255 };

    // 3. Main Loop
    while (!WindowShouldClose()) {
        
        float dt = GetFrameTime();

        // Keep the music stream streaming and looping smoothly
        if (musicLoaded) {
            UpdateMusicStream(bgm);
            if (!IsMusicStreamPlaying(bgm)) {
                PlayMusicStream(bgm);
            }
        }

        // Calculate present dimensions/bounds for click collision check
        float scale = 0.35f;
        float pWidth = (presentTexture.id != 0) ? (presentTexture.width * scale) : 120.0f;
        float pHeight = (presentTexture.id != 0) ? (presentTexture.height * scale) : 120.0f;
        float posX = (screenWidth - pWidth) / 2.0f;
        float posY = 300.0f;
        Rectangle presentBounds = { posX, posY, pWidth, pHeight };

        // 4. Update Phase (Handle Input)
        Vector2 mousePos = GetMousePosition();
        bool clickedPresent = (CheckCollisionPointRec(mousePos, presentBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));

        if (IsKeyPressed(KEY_SPACE) || clickedPresent) {
            cardOpened = !cardOpened; // Toggle state
            
            // Trigger confetti burst the moment the card opens via click
            if (cardOpened && !confettiSpawned) {
                SpawnConfetti(confettiParticles, { posX + pWidth / 2.0f, posY + pHeight / 2.0f });
                confettiSpawned = true;
            } else if (!cardOpened) {
                confettiSpawned = false; // Reset if toggled closed
            }
        }

        // Update Confetti Physics (Gravity, Movement, Rotation)
        for (auto it = confettiParticles.begin(); it != confettiParticles.end(); ) {
            it->position.x += it->velocity.x * dt;
            it->position.y += it->velocity.y * dt;
            it->velocity.y += 500.0f * dt; // Gravity pulling downward
            it->rotation += it->rotationSpeed * dt;

            // Remove particles when they fall off screen
            if (it->position.y > (float)screenHeight + 50) {
                it = confettiParticles.erase(it);
            } else {
                ++it;
            }
        }

        // 5. Draw Phase
        BeginDrawing();
            ClearBackground(softCream);

            // Draw a decorative banner background shape
            DrawRectangle(0, 50, screenWidth, 210, festiveRed);

            // Draw Centered Title Text
            const char* titleText = "Happy Birthday";
            float titleFontSize = 90.0f;
            Vector2 titleSize = MeasureTextEx(customFont, titleText, titleFontSize, 2.0f);
            DrawTextEx(customFont, titleText, 
                       {(screenWidth - titleSize.x) / 2.0f, 70.0f}, 
                       titleFontSize, 2.0f, WHITE);

            // Draw Subtitle / Recipient Name
            const char* subText = "To  Auntie  Pooty!";
            float subFontSize = 90.0f;
            Vector2 subSize = MeasureTextEx(customFont, subText, subFontSize, 2.0f);
            DrawTextEx(customFont, subText, 
                       {(screenWidth - subSize.x) / 2.0f, 180.0f}, 
                       subFontSize, 2.0f, festiveGold);

            // Conditional Rendering based on whether the card is "opened"
            if (!cardOpened) {
                // Draw present image with a wobbling rotation effect
                if (presentTexture.id != 0) {
                    // Calculate a smooth wobble angle using time
                    float wobbleRotation = sinf((float)GetTime() * 10.0f) * 6.0f; // 6 degrees tilt max

                    Rectangle sourceRec = { 0.0f, 0.0f, (float)presentTexture.width, (float)presentTexture.height };
                    Rectangle destRec = { posX + pWidth / 2.0f, posY + pHeight / 2.0f, pWidth, pHeight };
                    Vector2 origin = { pWidth / 2.0f, pHeight / 2.0f };

                    DrawTexturePro(presentTexture, sourceRec, destRec, origin, wobbleRotation, WHITE);
                } else {
                    // Fallback text if image isn't found
                    const char* promptText = "Click to Open Card";
                    float promptFontSize = 24.0f;
                    Vector2 promptSize = MeasureTextEx(customFont, promptText, promptFontSize, 1.0f);
                    DrawTextEx(customFont, promptText, 
                               {(screenWidth - promptSize.x) / 2.0f, 400.0f}, 
                               promptFontSize, 1.0f, darkText);
                }
            } else {
                // Revealed Personal Message Box
                DrawRectangleRounded({200, 320, 600, 250}, 0.1f, 4, WHITE);
                DrawRectangleRoundedLines({200, 320, 600, 250}, 0.1f, 4, festiveGold);

                const char* msgLine1 = "Wishing you a wonderful year";
                const char* msgLine2 = "filled with joy, health, happiness and success!";
                
                float msgFontSize = 34.0f; // Adjusted to cleanly fit box size
                Vector2 msgSize1 = MeasureTextEx(customFont, msgLine1, msgFontSize, 1.0f);
                Vector2 msgSize2 = MeasureTextEx(customFont, msgLine2, msgFontSize, 1.0f);

                DrawTextEx(customFont, msgLine1, {(screenWidth - msgSize1.x) / 2.0f, 380.0f}, msgFontSize, 1.0f, darkText);
                DrawTextEx(customFont, msgLine2, {(screenWidth - msgSize2.x) / 2.0f, 430.0f}, msgFontSize, 1.0f, darkText);
            }

            // Draw Confetti Particles on top of everything
            for (const auto& p : confettiParticles) {
                DrawRectanglePro(
                    { p.position.x, p.position.y, p.size, p.size * 0.6f }, 
                    { p.size / 2.0f, p.size * 0.3f }, 
                    p.rotation, 
                    p.color
                );
            }

            // Draw static decorative confetti dots in background corners
            DrawCircle(150, 300, 8, festiveRed);
            DrawCircle(850, 350, 10, festiveGold);
            DrawCircle(200, 500, 6, festiveGold);
            DrawCircle(800, 500, 7, festiveRed);

        EndDrawing();
    }

    // 6. Cleanup
    if (customFont.texture.id != 0) {
        UnloadFont(customFont);
    }
    if (presentTexture.id != 0) {
        UnloadTexture(presentTexture);
    }
    if (musicLoaded) {
        UnloadMusicStream(bgm);
    }
    
    CloseAudioDevice();
    CloseWindow();

    return 0;
}