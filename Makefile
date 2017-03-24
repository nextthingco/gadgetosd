CC=gcc
AR=ar
CFLAGS=-I. -O2 -std=c11 -Wall -g
LIBS=-L. -lmongoose -linih
MONGOOSE_FLAGS=-DMG_ENABLE_HTTP_STREAMING_MULTIPART

ifeq ($(OS),Windows_NT)
#    CFLAGS += -D _WIN32
    LIBS += -lws2_32 -luuid
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
mongoose_utils.o \
gadgetosd.o \
gadgetosd_api_version.o\
gadgetosd_api_application_add.o \
gadgetosd_api_application_start.o \
gadgetosd_api_application_stop.o \
gadgetosd_api_application_delete.o \
gadgetosd_api_application_purge.o \


G_OBJ = utils.o \
config.o \
mongoose_utils.o \
gadget_project.o \
gadget.o \
gadget_init.o \
gadget_build.o \
gadget_stop.o \
gadget_start.o \
gadget_delete.o \
gadget_purge.o \
gadget_deploy.o \


%.o: %.c
	@echo "  > Building $@"
	$(CC) -c -o $@ $< $(CFLAGS) $(MONGOOSE_FLAGS)

all: gadgetosd gadget

gadgetosd: $(GOSD_OBJ) libmongoose.a libinih.a
	@echo "  > Building $@"
	$(CC) -o $@ $^ $(CFLAGS) $(MONGOOSE_FLAGS) $(LIBS)

gadget: $(G_OBJ) libmongoose.a libinih.a
	@echo "  > Building $@"
	$(CC) -o $@ $^ $(CFLAGS) $(MONGOOSE_FLAGS) $(LIBS)

libmongoose.a: mongoose.c mongoose.h
	@echo "  > Building $@"
	${CC} -c mongoose.c $(CFLAGS) $(MONGOOSE_FLAGS) -o mongoose.o
	${AR} rcs libmongoose.a mongoose.o

libinih.a: ini.c ini.h
	@echo "  > Building $@"
	${CC} -c ini.c $(CFLAGS) -o ini.o
	${AR} rcs libinih.a ini.o

test_utils: test_utils.o utils.o libmongoose.a libinih.a
	@echo "  > Building $@"
	$(CC) -o test_utils test_utils.o utils.o $(CFLAGS) $(LIBS)
clean:
	rm *.o *.a gadget gadgetosd
