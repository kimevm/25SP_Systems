CFLAGS = -Wall -Werror -std=gnu99
APP = mem_alloc

all: $(APP)

$(APP): $(APP).c $(APP).h main.c
	gcc $(CFLAGS) -o $(APP) $(APP).c main.c

test: $(APP).c $(APP).h test_main.c
	gcc $(CFLAGS) -o test $(APP).c test_main.c

clean:
	rm -f $(APP) test