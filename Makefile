CC=gcc
CFLAGS=-I. -O2
DEPS = mongoose.h
OBJ = gadget.o gadgetosd.o mongoose.o main.o

MONGOOSE_FLAGS=-DMG_ENABLE_HTTP_STREAMING_MULTIPART

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(MONGOOSE_FLAGS)

all: gadgetosd gadget

gadgetosd: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

gadget: gadgetosd
	ln -s gadgetosd gadget

clean:
	rm *.o
