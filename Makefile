CC  = g++
C	  = cpp
H	  = h

CFLAGS    = -g -std=c++11

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -lglut -lGL -lGLU -lm
  endif
endif

HFILES = Model.${H} View.${H} Vector.${H} Utility.${H} Camera.${H} StateVector.${H} Particle.${H} RandomGenerator.${H} Strut.${H} objtriloader.${H} Cell.${H} Lattice.${H}
OFILES = Model.o View.o Vector.o Utility.o Camera.o StateVector.o Particle.o RandomGenerator.o Strut.o objtriloader.o Cell.o Lattice.o 

PROJECT   = spooky_springy_mesh

${PROJECT}: ${PROJECT}.o ${OFILES}
	${CC} ${CFLAGS} -o ${PROJECT} ${PROJECT}.o ${OFILES} ${LDFLAGS}

${PROJECT}.o:   ${PROJECT}.${C} ${HFILES} ${INCFLAGS}
	${CC} ${CFLAGS} -c ${INCFLAGS} ${PROJECT}.${C}
	
Model.o: Model.${C} Model.${H} Vector.${H} Utility.${H}
	${CC} $(CFLAGS) -c Model.${C}

View.o: View.${C} View.${H} Camera.${H} Vector.${H} Utility.${H}
	${CC} $(CFLAGS) -c View.${C}

Camera.o: Camera.${C} Camera.${H} Vector.${H} Utility.${H}
	${CC} $(CFLAGS) -c Camera.${C}

Vector.o: Vector.${C} Vector.${H} Utility.${H}
	${CC} $(CFLAGS) -c Vector.${C}

Utility.o: Utility.${C} Utility.${H}
	${CC} $(CFLAGS) -c Utility.${C}

StateVector.o: StateVector.${C} StateVector.${H}
	${CC} $(CFLAGS) -c StateVector.${C}

Particle.o: Particle.${C} Particle.${H}
	${CC} $(CFLAGS) -c Particle.${C}

RandomGenerator.o: RandomGenerator.${C} RandomGenerator.${H}
	${CC} $(CFLAGS) -c RandomGenerator.${C}

Strut.o: Strut.${C} Strut.${H}
	${CC} $(CFLAGS) -c Strut.${C}

objtriloader.o: objtriloader.${C} objtriloader.${H}
	${CC} $(CFLAGS) -c objtriloader.${C}

Cell.o: Cell.${C} Cell.${H}
	${CC} $(CFLAGS) -c Cell.${C}

Lattice.o: Lattice.${C} Lattice.${H}
	${CC} $(CFLAGS) -c Lattice.${C}

clean:
	rm -f core.* *.o *~ .DS_Store ${PROJECT}
