TARGET:=ft
SOURCE_DIR:=src

SOURCES:=$(wildcard $(SOURCE_DIR)/*.c)
HEADERS:=$(wildcard $(SOURCE_DIR)/*.h)

CFLAGS+=-std=c90 -Wall -Wextra -Wpedantic -Werror

.PHONY: clean

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	$(RM) $(TARGET) 
