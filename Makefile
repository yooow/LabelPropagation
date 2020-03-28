.PHONY: main
main: $(OBJ)
	gcc *.c *.h -o main

.PHONY: clean
clean:
	rm *.o