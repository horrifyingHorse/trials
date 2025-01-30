## RayLib
Hugh jeLL to the Raylib Doc, dawg. Cheatsheet... ok?

- Initialise the Window: InitWindow(size_x, size_y, "title")
- Set the FPS using: SetTargetFPS(int)
- The Event Loop (or Game Loop):
```c
while (!WindowShouldClose()) {
    // Game Loop
    // by default Esc key sets WindowShouldClose

    // Computation

    // Redraw the screen
    BeginDrawing();
        ClearBackground(COLOR); // Clear the screen
                                // and paint COLOR
        // .. ..
    EndDrawing();
}
```

### Shapes:
Create Shape objects(?) which can then be drawn on the window

> Rectangle {pos_x, pos_y, len_x, len_y}


### Drawing:
> DrawRectangleRec(Rectangle, COLOR)

> DrawText("Text", pos_x, pos_y, font_size, COLOR)
