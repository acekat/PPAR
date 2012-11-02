T=tris
O=tri_pram_seq_v1
P=tri_pram_seq_v2
Q=tri_pram_omp_v1
R=tri_pram_omp_v2

SRC=$T.c

COMP=mpicc
FLAGS=-Wall
RUN=mpirun
P=4

all : tris tri_pram_seq_v1 tri_pram_seq_v2 tri_pram_omp_v1 tri_pram_omp_v2

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
	$(RUN) -n $(P) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) $T $(N)
endif

triSV1:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) tri_pram_seq_v1 $(N)
endif

triSV2:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) tri_pram_seq_v2 $(N)
endif

triOV1:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) tri_pram_omp_v1 $(N)
endif

triOV2:
ifdef HF
	$(RUN) -n $(P) -hostfile $(HF) $T $(N)
else
	$(RUN) -n $(P) tri_pram_omp_v2 $(N)
endif

clean:
	rm tris tri_pram_seq_v1 tri_pram_seq_v2 tri_pram_omp_v1 tri_pram_omp_v2
