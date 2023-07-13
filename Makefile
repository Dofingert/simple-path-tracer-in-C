CFLAGS		  := -O2 -Wall
INCLUDES	  := -I./
CC 			  := loongarch32r-linux-gnusf-gcc

all: $(objects) test

clean:
	rm -rf *.o

objects		  := interaction.o triangle.o math.o bvh.o trace_ray.o simple_scene.o render.o
test: $(objects) test_main.o
	$(CC) -o test_main $(objects) test_main.o

main: $(objects) main.o
	$(CC) -o main $(objects) main.o

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $<