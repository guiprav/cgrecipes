GCC_OPTIONS=-lglfw3 -lGL -lXrandr -lXinerama -lXcursor -lfreetype $(shell freetype-config --cflags) -lm

text_test: util.h text_test.c
	gcc text_test.c $(GCC_OPTIONS) -o bin/text_test
