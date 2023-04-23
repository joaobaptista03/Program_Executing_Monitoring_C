CC = gcc

tracer: obj/tracer.o obj/tracer_execute_p.o obj/tracer_execute_u.o obj/random.o
	$(CC) $^ -o tracer

obj/tracer.o : src/tracer.c
	$(CC) -c $< -o $@

obj/tracer_execute_p.o : src/tracer_execute_p.c
	$(CC) -c $< -o $@

obj/tracer_execute_u.o : src/tracer_execute_u.c
	$(CC) -c $< -o $@

obj/random.o : src/random.c
	$(CC) -c $< -o $@

clean:
	rm -f tracer obj/*.o