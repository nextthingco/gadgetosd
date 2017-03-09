CC=gcc
CFLAGS=-I. 
DEPS = mongoose.h
OBJ = gadgetosd.o mongoose.o

MONGOOSE_FLAGS=-DMG_ENABLE_HTTP_STREAMING_MULTIPART

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(MONGOOSE_FLAGS)

all: gadgetosd

gadgetosd: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o
