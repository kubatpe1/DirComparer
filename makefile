REQFLAGS = -Wall -pthread

OUTFILE = dircmp.out

OBJ = search.o prod_con.o pthread_wrapper.o stringstack.o thread.o main.o

build: ${OUTFILE}

${OUTFILE}: ${OBJ}
	${CC} ${CFLAGS} ${REQFLAGS} ${OBJ} -o ${OUTFILE}

%.o: CFLAGS += ${REQFLAGS}
	
clean:
	rm -f ${OUTFILE} ${OBJ}
	
