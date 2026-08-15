#include "raylib.h"
#include <cmath>

int main() {
    // 1. Initialization
    const int screenWidth = 1000;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Happy Birthday, Pooty!");
    
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
    Music bgm = LoadMusicStream("assets/alex-morgan-birthday-celebration-tune-579468.mp3");
    bool musicLoaded = (bgm.stream.buffer != nullptr);
    
    if (musicLoaded) {
        PlayMusicStream(bgm);
    }

    // Interactive State Variable
    bool cardOpened = false;

    // Colors
    Color softCream = { 253, 246, 227, 255 };
    Color festiveRed = { 219, 48, 34, 255 };
    Color festiveGold = { 245, 197, 24, 255 };
    Color darkText = { 40, 40, 40, 255 };

    // 3. Main Loop
    while (!WindowShouldClose()) {
        
        // Keep the music stream streaming and looping smoothly
        if (musicLoaded) {
            UpdateMusicStream(bgm);
            if (!IsMusicStreamPlaying(bgm)) {
                PlayMusicStream(bgm);
            }
        }

        // 4. Update Phase (Handle Input)
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            cardOpened = !cardOpened; // Toggle state
        }

        // 5. Draw Phase
        BeginDrawing();
            ClearBackground(softCream);

            // Draw a decorative banner background shape
            DrawRectangle(0, 50, screenWidth, 80, festiveRed);

            // Draw Centered Title Text
            const char* titleText = "Happy Birthday!";
            float titleFontSize = 50.0f;
            Vector2 titleSize = MeasureTextEx(customFont, titleText, titleFontSize, 2.0f);
            DrawTextEx(customFont, titleText, 
                       {(screenWidth - titleSize.x) / 2.0f, 70.0f}, 
                       titleFontSize, 2.0f, WHITE);

            // Draw Subtitle / Recipient Name
            const char* subText = "To Pooty";
            float subFontSize = 30.0f;
            Vector2 subSize = MeasureTextEx(customFont, subText, subFontSize, 2.0f);
            DrawTextEx(customFont, subText, 
                       {(screenWidth - subSize.x) / 2.0f, 180.0f}, 
                       subFontSize, 2.0f, festiveGold);

            // Conditional Rendering based on whether the card is "opened"
            if (!cardOpened) {
                // Draw present image with a wobbling rotation effect
                if (presentTexture.id != 0) {
                    float scale = 0.35f;
                    float pWidth = presentTexture.width * scale;
                    float pHeight = presentTexture.height * scale;
                    float posX = (screenWidth - pWidth) / 2.0f;
                    float posY = 300.0f;

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
                const char* msgLine2 = "filled with joy, health, and happiness!";
                
                float msgFontSize = 24.0f;
                Vector2 msgSize1 = MeasureTextEx(customFont, msgLine1, msgFontSize, 1.0f);
                Vector2 msgSize2 = MeasureTextEx(customFont, msgLine2, msgFontSize, 1.0f);

                DrawTextEx(customFont, msgLine1, {(screenWidth - msgSize1.x) / 2.0f, 380.0f}, msgFontSize, 1.0f, darkText);
                DrawTextEx(customFont, msgLine2, {(screenWidth - msgSize2.x) / 2.0f, 430.0f}, msgFontSize, 1.0f, darkText);
            }

            // Draw static decorative confetti dots
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