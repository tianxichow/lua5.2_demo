IDIR =../include
CC=g++
CFLAGS=

LIBS=-llua -lm 

TARGET = c_lua thread_yeild user_type
	
all :
	for i in $(TARGET); do \
		echo "build["$$i"]"; \
		$(CC) -o $$i $(addsuffix .cpp, $$i) $(LIBS); \
	done 

.PHONY: clean

clean:
	rm -f $(TARGET)