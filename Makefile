T=tris

SRC=$T.c

COMP=mpicc
FLAGS=-W
RUN=mpirun
P=4


$T: $(SRC)
	$(COMP) $(FLAGS) $(SRC) -fopenmp -o $@

exec:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) $T $(N)
endif

clean:
	rm $(T)
