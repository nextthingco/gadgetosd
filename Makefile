CC=gcc
CFLAGS=-I. -O2
DEPS = mongoose.h
OBJ = main.o mongoose.o gadgetosd.o gadget.o gadget_new.o gadget_build.o gadget_deploy.o

MONGOOSE_FLAGS=-DMG_ENABLE_HTTP_STREAMING_MULTIPART

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(MONGOOSE_FLAGS)

all: gadgetosd gadget

gadgetosd: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(MONGOOSE_FLAGS)

gadget: gadgetosd
	ln -s gadgetosd gadget

clean:
	rm *.o
