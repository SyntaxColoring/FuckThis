TARGET:=fj
SOURCE_DIR:=src

SOURCES:=$(wildcard $(SOURCE_DIR)/*.c)
HEADERS:=$(wildcard $(SOURCE_DIR)/*.h)

.PHONY: clean

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	$(RM) $(TARGET) 
