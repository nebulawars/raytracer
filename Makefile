all:
	gcc render.c raytracer.c -lopengl32 -lglut32 -lglu32 && ./a.exe
