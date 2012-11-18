T=tris

SRC=$T.c

COMP=mpicc
FLAGS=-Wall -O3
RUN=mpirun

P=4
PNODE=4
N=100000
SORT=1

ifdef TH
THOPT=-x OMP_NUM_THREADS=$(TH)
THB=-$(TH)
endif

ifdef HF
HFOPT=-hostfile $(HF)
endif

all: tris

$T: $(SRC)
	$(COMP) $(FLAGS) $@.c -fopenmp -o $@

exec:
	@if [ $(P) -gt $(PNODE) ] && [ -n $(HF) ]; then \
		echo "/!\\ Préciser un fichier hostfile pour lancer l'exécution sur plusieurs machines. /!\\"; \
	else \
		echo $(RUN) -n $(P) -npernode $(PNODE) $(THOPT) $(HFOPT) $T $(N) $(SORT); \
		$(RUN) -n $(P) -npernode $(PNODE) $(THOPT) $(HFOPT) $T $(N) $(SORT); \
	fi

bench-compile: $(SRC)
	$(COMP) $(FLAGS) $(T).c -fopenmp -D BENCH -o $T

bench: bench-compile
	$(RUN) -n $(P) -npernode $(PNODE) $(THOPT) $(HFOPT) $T $(N) $(SORT) 2>> bench/$(N)-$(P)-$(PNODE)$(THB)

clean:
	rm $T
