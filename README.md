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
  P=<nombre de processus MPI>:48
  PNODE=<nombre de processus par machine>:4
  TH=<nombre de thread version hybride>
  N=<nombre d'éléments à trier>:1572864
  HF=<chemin vers un hostfile>
```

Des hostfiles sont fournis pour les salles 31-303, 31-304, 31-309 et 31-201.

## TODO
- calculer l'accélération et l'efficacité dans les résultats
- Benchmark
  - VERSION 1 & 2 tri_PRAM MPI pur => V1 WIN
  - VERSION 1 & 2 tri_PRAM Hybride => V2 WIN
  - Comparer performances MPI pur et hybride (PRAM et quick_sort)
  - Comparer nb machines / nb processus / nb éléments (faire qques test pour voir des valeurs pertinentes)
  - Avec/Sans optimisation -03
  - Comparer quick_sort et qsort (MPI pur)