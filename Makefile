T=tris

SRC=$T.c

COMP=mpicc
FLAGS=-Wall -O3
RUN=mpirun

P=48
PNODE=4
TH=4
N=1572864

ifdef HF
HFOPT=-hostfile $(HF)
endif

all: tris

tris: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@

exec: 
	make nohyb HF=hostfile201
	make hyb HF=hostfile201 P=12 PNODE=1

nohyb:
	$(RUN) -n $(P) -npernode $(PNODE) $(HFOPT) $T $(N)

hyb:
	$(RUN) -x OMP_NUM_THREADS=$(TH) -n $(P) -npernode $(PNODE) $(HFOPT) $T $(N) -hyb

clean:
	rm $T
