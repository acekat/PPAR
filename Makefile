T=tris

SRC=$T.c

COMP=mpicc
FLAGS=-Wall -O3
#FLAGS=-Wall -O3 -funroll-loops
RUN=mpirun
P=48
PNODE=4
TH=4
N=1572864

all: tris

tris: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@

exec: 
	make nohyb HF=hostfile201
	make hyb HF=hostfile201 P=12 PNODE=1

nohyb:
ifdef HF
	$(RUN) -n $(P) -npernode $(PNODE) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) -npernode $(PNODE) $T $(N)
endif

hyb:
ifdef HF
	$(RUN) -x OMP_NUM_THREADS=$(TH) -n $(P) -npernode $(PNODE) -hostfile $(HF) $T $(N) -hyb
else
	$(RUN) -x OMP_NUM_THREADS=$(TH) -n $(P) -npernode $(PNODE) $T $(N) -hyb
endif

clean:
	rm $T
