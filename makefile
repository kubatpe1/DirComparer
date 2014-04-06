reqflags = -Wall -pthread

outfile = dircmp.out

obj = search.o prod_con.o pthread_wrapper.o stringstack.o thread.o main.o

build: ${outfile}

${outfile}: ${obj}
	${CC} ${CFLAGS} ${reqflags} ${obj} -o ${outfile}

search.o: search.c
	${CC} ${CFLAGS} ${reqflags} -c search.c
	
prod_con.o: prod_con.c
	${CC} ${CFLAGS} ${reqflags} -c prod_con.c
	
pthread_wrapper.o: pthread_wrapper.c
	${CC} ${CFLAGS} ${reqflags} -c pthread_wrapper.c
	
stringstack.o: stringstack.c
	${CC} ${CFLAGS} ${reqflags} -c stringstack.c
	
thread.o: thread.c
	${CC} ${CFLAGS} ${reqflags} -c thread.c
	
main.o: main.c
	${CC} ${CFLAGS} ${reqflags} -c main.c
	
clean:
	rm -f ${outfile} ${obj}
	
