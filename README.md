# PPAR Project
*Tris parallèles*

## Compilation
```bash
make
```

## Exécution
```bash
make exec <options>

options:
  P=<nombre de processus MPI>:4
  PNODE=<nombre de processus par machine>:4
  TH=<nombre de thread version hybride>
  N=<nombre d'éléments à trier>:100000
  HF=<chemin vers un hostfile>
  SORT=<algorithme de tri>:1
    1: PRAM - MPI pur
    2: PRAM - Hybride
    3: quick_sort - MPI pur
    4: quick_sort - Hybride
    5: qsort (stdlib.h) - MPI pur
```

Des hostfiles sont fournis pour les salles 31-303, 31-304, 31-309 et 31-201.

## Benchmark
```bash
make bench <options>
```

Ecris la ligne de résultat dans le fichier correspondant du dossier *bench*

Format fichier: SORT-N-P-PNODE[-TH]

Format ligne: temps   SORT   N   P   TH
