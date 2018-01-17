NAME    := libopenweathermap
VERSION := 0.1
CC      :=  gcc 
#LIBS    := -lm -lcurl ${EXTRA_LIBS} 
DLIB    := ${NAME}.so
SLIB    := ${NAME}.a
TARGET	:= $(NAME).so.${VERSION} 
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	:= $(OBJECTS:.o=.deps)
DESTDIR := /
PREFIX  := /usr
SHAREDIR := $(PREFIX)/share/$(NAME)
MANDIR  := $(SHAREDIR)/man
BINDIR  := $(PREFIX)/bin
SHARE   := /$(PREFIX)/share/$(TARGET)
CFLAGS  := -fpie -fpic -Wall -DSHAREDIR=\"${SHAREDIR}\" -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -g -I include ${EXTRA_CFLAGS}
LDFLAGS := -pie  ${EXTRA_LDFLAGS}
ARCH := $(shell arch)

ifneq (,$(findstring 64,$(ARCH)))
	LIBDIR  := lib64
else
	LIBDIR  := lib
endif

all: $(TARGET)

$(TARGET): $(SLIB)
	$(CC) -s -pie -shared -fPIC -o $(TARGET) -Wl,-soname,$(DLIB) -Wl,--whole-archive $(SLIB) -Wl,--no-whole-archive


$(SLIB): $(OBJECTS)
	ar r $(SLIB) $(OBJECTS)

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET) $(SLIB) 

install: $(TARGET)
	cp -p $(TARGET) $(DESTDIR)$(PREFIX)/$(LIBDIR)
	ln -sf $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(TARGET) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(DLIB) 
	@mkdir -p $(DESTDIR)$(PREFIX)/include/
	cp -pr include/* $(DESTDIR)$(PREFIX)/include/


-include $(DEPS)

.PHONY: clean

