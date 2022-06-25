CFLAGS		  := -O2 -Wall
INCLUDES	  := -I./

all: $(objects) test

clean:
	rm -rf *.o

objects		  := interaction.o triangle.o math.o bvh.o trace_ray.o simple_scene.o render.o
test: $(objects) test_main.o
	gcc -o test_main $(objects) test_main.o

main: $(objects) main.o
	gcc -o main $(objects) main.o

%.o: %.c
	gcc $(CFLAGS) $(INCLUDES) -c $<

%.o: %.S
	gcc $(CFLAGS) $(INCLUDES) -c $<