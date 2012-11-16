T=tris

SRC=$T.c

COMP=mpicc
FLAGS=-Wall -O3
RUN=mpirun

P=48
PNODE=4
N=1572864
SORT=1

# si P>PNODE & !HF => ERREUR

ifdef TH
THOPT=-x OMP_NUM_THREADS=$(TH)
endif

ifdef HF
HFOPT=-hostfile $(HF)
# else
# $(error Préciser un fichier hostfile)
endif

all: tris

tris: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@

exec: 
	$(RUN) $(THOPT) -n $(P) -npernode $(PNODE) $(HFOPT) $T $(N) $(SORT)

clean:
	rm $T
