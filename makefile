TARGETS = shell

.PHONY: clean

all: $(TARGETS)

%o: %.c %.h
	gcc -c -o $@ $<
shell: execute.o parse.o split.o final.o
	gcc -o shell parse.o execute.o split.o final.o
clean:
	rm -f $(TARGETS) *.o
