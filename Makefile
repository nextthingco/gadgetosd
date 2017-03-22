CC=gcc
AR=ar
CFLAGS=-I. -O2 -std=c11 -Wall -g
LIBS=-L. -lmongoose -linih
MONGOOSE_FLAGS=-DMG_ENABLE_HTTP_STREAMING_MULTIPART

ifeq ($(OS),Windows_NT)
    CFLAGS += -D _WIN32
    LIBS += -lws2_32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -D LINUX
	    LIBS += -luuid	
    endif
    ifeq ($(UNAME_S),Darwin)
        CFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CFLAGS += -D ARM
    endif
endif

GOSD_OBJ = utils.o \
config.o \
gadgetosd.o \
gadgetosd_api_version.o\
gadgetosd_api_application_add.o \
gadgetosd_api_application_stop.o

G_OBJ = utils.o \
config.o \
gadget_project.o \
gadget.o \
gadget_init.o \
gadget_build.o \
gadget_deploy.o 

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(MONGOOSE_FLAGS)

all: gadgetosd gadget

gadgetosd: $(GOSD_OBJ) libmongoose.a libinih.a
	$(CC) -o $@ $^ $(CFLAGS) $(MONGOOSE_FLAGS) $(LIBS)

gadget: $(G_OBJ) libmongoose.a libinih.a
	$(CC) -o $@ $^ $(CFLAGS) $(MONGOOSE_FLAGS) $(LIBS)

libmongoose.a: mongoose.c mongoose.h
	${CC} -c mongoose.c $(CFLAGS) $(MONGOOSE_CFLAGS) -o mongoose.o
	${AR} rcs libmongoose.a mongoose.o

libinih.a: ini.c ini.h
	${CC} -c ini.c $(CFLAGS) -o ini.o
	${AR} rcs libinih.a ini.o

test_utils: test_utils.o utils.o libmongoose.a libinih.a
	$(CC) -o test_utils test_utils.o utils.o $(CFLAGS) $(LIBS)
clean:
	rm *.o *.a gadget gadgetosd
