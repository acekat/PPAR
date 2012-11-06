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
  P=<nombre de processus MPI>:12
  PNODE=<nombre de processus par machine>:4
  N=<nombre d'éléments à trier>:100
  HF=<chemin vers un hostfile>
```

Des hostfiles sont fournis pour les salles 31-303, 31-304, 31-309 et 31-201.

## TODO
- Benchmark
  - VERSION 1 & 2 tri_PRAM MPI only
  - VERSION 1 & 2 tri_PRAM Hybride
  - calcul de la fusion sur chaque processus
  - Comparer performances MPI pur et hybride
  - Comparer nb machines / nb processus / nb éléments (faire qques test pour voir des valeurs pertinentes)
  - Avec/Sans optimisation -03