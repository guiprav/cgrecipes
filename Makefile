GCC_OPTIONS=-lglfw -lGL -lfreetype $(shell freetype-config --cflags)

text_test: util.h text_test.c
	gcc text_test.c $(GCC_OPTIONS) -o bin/text_test
