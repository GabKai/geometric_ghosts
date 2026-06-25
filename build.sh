windres icon.rc -O coff -o icon.o
gcc src/main.c src/menu.c src/game.c icon.o -o geometric_ghosts.exe -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lopengl32 -lgdi32 -lwinmm
# -mwindows
