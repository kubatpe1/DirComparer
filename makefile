CFLAGS = ${CFLAGS} -Wall -pthread

outfile = dircmp.out

obj = search.o prod_con.o pthread_wrapper.o stringstack.o thread.o main.o

build: ${outfile}

${outfile}: ${obj}
	${CC} ${CFLAGS} ${obj} -o ${outfile}

search.o: search.c
	${CC} ${CFLAGS} -c search.c
	
prod_con.o: prod_con.c
	${CC} ${CFLAGS} -c prod_con.c
	
pthread_wrapper.o: pthread_wrapper.c
	${CC} ${CFLAGS} -c pthread_wrapper.c
	
stringstack.o: stringstack.c
	${CC} ${CFLAGS} -c stringstack.c
	
thread.o: thread.c
	${CC} ${CFLAGS} -c thread.c
	
main.o: main.c
	${CC} ${CFLAGS} -c main.c
	
clean:
	rm -f ${outfile} ${obj}
	
