T=tris

SRC=$T.c

COMP=mpicc
FLAGS=-Wall -O3
#FLAGS=-Wall -O3 -funroll-loops
RUN=mpirun
P=12
PNODE=4
N=1572864

all: tris tri_pram_seq_v1 tri_pram_seq_v2 tri_pram_omp_v1 tri_pram_omp_v2

tris: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@
	
tri_pram_seq_v1: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@
	
tri_pram_seq_v2: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@
	
tri_pram_omp_v1: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@
	
tri_pram_omp_v2: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@

exec:
ifdef HF
	$(RUN) -n $(P) -npernode $(PNODE) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) -npernode $(PNODE) $T $(N)
endif

nohyb:
ifdef HF
	$(RUN) -n $(P) -npernode $(PNODE) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) -npernode $(PNODE) $T $(N)
endif

hyb:
	# export OMP_NUM_THREADS=4
ifdef HF
	$(RUN) -n $(P) -npernode $(PNODE) -hostfile $(HF) $T $(N) -hyb
else
	$(RUN) -n $(P) -npernode $(PNODE) $T $(N) -hyb
endif

triSV1:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) tri_pram_seq_v1 $(N)
else
	$(RUN) -n $(P) tri_pram_seq_v1 $(N)
endif

triSV2:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) tri_pram_seq_v2 $(N)
else
	$(RUN) -n $(P) tri_pram_seq_v2 $(N)
endif

triOV1:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) tri_pram_omp_v1 $(N)
else
	$(RUN) -n $(P) tri_pram_omp_v1 $(N)
endif

triOV2:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) tri_pram_omp_v2 $(N)
else
	$(RUN) -n $(P) tri_pram_omp_v2 $(N)
endif

clean:
	rm $T tri_pram_seq_v1 tri_pram_seq_v2 tri_pram_omp_v1 tri_pram_omp_v2
