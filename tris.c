// Projet PPAR
// 
// CASSAT Valentin
// DUCAMAIN Myriam


#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define NMAX 1572864	// nombre maximum d'éléments à écrire dans le fichier
#define TAG_TAB 0
#define TAG_RES 1
#define TAG_CHECK 2

#define P0 if (my_rank == 0)

int my_rank;	// rang du processus
int nb_proc;	// nombre de processus
int k;			// nombre d'éléments par processus

char usage[] = "\
\nUsage:\n\
  tris <nb elem> <type tri>\n\
\n\
  <type tri>:\n\
	1: PRAM - MPI pur\n\
	2: PRAM - hybride\n\
	3: quick_sort - MPI pur\n\
	4: quick_sort - hybride\n\
	5: qsort (stdlib.h) - MPI pur\n\
";

/**
 * Affiche le contenu du tableau.
 * @param tab tableau à afficher
 * @param len longueur du tableau
 */
void print_tab(int *tab, int len)
{
	int i;
	printf("\t[");
	for (i = 0; i < len-1; i++) {
		printf("%d, ",tab[i]);
	}
	printf("%d]\n", tab[len-1]);
}

/**
 * Affiche le temps maximal de calcul
 * @param total temps total de chaque processus
 */
void print_results(double total)
{
	int i;
	double tmp;

	if (my_rank != 0)
		MPI_Send(&total, 1, MPI_DOUBLE, 0, TAG_RES, MPI_COMM_WORLD);
	else {
		for (i = 1; i < nb_proc; i++) {
			MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_RES, MPI_COMM_WORLD, NULL);
			total = tmp > total ? tmp : total;
		}
		printf("\tTri en %f sec\n", total);

		// TODO: calculer l'accélération et l'efficacité

		#ifdef BENCH
		fprintf(stderr, "%f", total);
		#endif
	}
}

/**
 * Initialise le tableau avec des valeurs aléatoires.
 * @param tab tableau à initialiser
 * @param len longueur du tableau
 */
void init_rand(int *tab, int len)
{	
	int i;
	for (i = 0; i < len; i++) {
		srandom(time(NULL)+i*my_rank);
		tab[i] = random();		
	}
}

/**
 * Fonction de comparaison pour qsort de stdlib.h
 * @param  a 
 * @param  b 
 * @return    0 si a = b
 *           <0 si a < b
 *           >0 si a > b
 */
int compare(void const *a, void const *b)
{
   int const *pa = a;
   int const *pb = b;

   return *pa - *pb;
}

/**
 * Vérifie si les éléments locaus sont correctement triés
 * @param  tab tableau des éléments locaux
 * @param  len longueur du tableau
 * @return     1: tri incorrect
 *             0: tri correct
 */
int check_local(int *tab, int len)
{
	int i = 1;
	while (i < len) {
		if (tab[i-1] <= tab[i]) 
			i++;
		else {
			printf("%d : /!\\ TRI LOCAL INCORRECT /!\\ \n", my_rank);
			return 1;
		}
	}
	return 0;
}

/**
 * Vérifie si tous les éléments sont triés correctement
 * @param  tab tableau des éléments locaux
 * @param  len longueur du tableau
 * @return     1: tri incorrect
 *             0: tri correct
 */
int check_sort(int *tab, int len)
{
	int min = tab[0];
	int max = tab[len-1];
	
	if (my_rank != nb_proc-1)
		MPI_Send(&max, 1, MPI_INT, my_rank+1, TAG_CHECK, MPI_COMM_WORLD);

	if (my_rank != 0)
		MPI_Recv(&min, 1, MPI_INT, my_rank-1, TAG_CHECK, MPI_COMM_WORLD, NULL);
	
	if (check_local(tab, len) || (min > tab[0])) {
		printf("%d : /!\\ TRI GLOBAL INCORRECT /!\\ \n", my_rank);
		return 1;
	}

	return 0;
}

/**
 * Algorithme du tri PRAM
 * @param tab_in  non trié
 * @param tab_out trié
 */
void PRAM(int *tab_in, int *tab_out)
{
	int i, j, cpt;
	int *count = (int *)calloc(k, sizeof(int));

	if (count == NULL) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}

	// comparaisons
	// VERSION 1
	for (i = 0; i < k; i++) {
		for (j = i+1; j < k; j++) {
			if (tab_in[i] > tab_in[j])
				count[i]++;
			else
				count[j]++;
		}
	}

	// VERSION 2 
	// for (i = 0; i < k; i++) {
	// 	for (j = 0; j < k; j++) {
	// 		if (((j<i) && (tab_in[i] == tab_in[j])) || (tab_in[i] > tab_in[j]))
	// 			count[i]++;
	// 	}
	// }
	
	// réarrangement
	for (i = 0; i < k; i++) {
		cpt = count[i];
		tab_out[cpt] = tab_in[i];
	}

	free(count);
}

/**
 * Algorithme du tri PRAM en version openMP
 * @param tab_in  non trié
 * @param tab_out trié
 */
void PRAM_omp(int *tab_in, int *tab_out)
{
	int i, j, cpt;
	int *count = (int *)calloc(k, sizeof(int));

	if (count == NULL) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}
	
	// comparaisons
	// VERSION 1
	// #pragma omp parallel for private(j)
	// for (i = 0; i < k; i++) {
	// 	for (j = i+1; j < k; j++) {
	// 		if (tab_in[i] > tab_in[j]) {
	// 			#pragma omp atomic
	// 			count[i]++;
	// 		}
	// 		else {
	// 			#pragma omp atomic
	// 			count[j]++;
	// 		}
	// 	}
	// }


	// VERSION 2
	#pragma omp parallel for private(j) 
	for (i = 0; i < k; i++) {
		for (j = 0; j < k; j++) {
			if (((j<i) && (tab_in[i] == tab_in[j])) || (tab_in[i] > tab_in[j]))
			count[i]++;
		}
	}

	// réarrangement
	#pragma omp parallel for private(cpt)
	for (i = 0; i < k; i++) {
		cpt = count[i];
		tab_out[cpt] = tab_in[i];
	}

	free(count);
}

/**
 * Méthode de sélection d'un pivot par valeur médiane du premier, dernier et 
 * élément centrale du tableau.
 * @param  tab tableau dans lequel calculer le pivot
 * @param  len longueur du tableau
 * @return     indice du pivot
 */
int pivot(int *tab, int len)
{
	int ibeg=0, imid=len/2, iend=len-1;
	int beg=tab[ibeg], mid=tab[imid], end=tab[iend];

	if(beg < mid) {
		if(mid < end)
			return imid;		
		if(end < beg)
			return ibeg;
	} else {
		if(beg < end) 
			return ibeg;
		if(end < mid)
			return imid;
	}
	return iend;
}

/**
 * Echange les éléments aux adresses mémoires en paramètre.
 * @param i adresse du premier élément
 * @param j adresse du second élément
 */
void swap(int *i, int *j)
{
	int tmp = *i;
	*i = *j;
	*j = tmp;
}

/**
 * Algorithme de tri rapide
 * @param tab tableau à trier
 * @param len longueur du tableau
 */
void quick_sort(int *tab, int len)
{
	int i=0, j=len, ipivot;
	
	// pas de tri à faire
	if (len <= 1) 
		return;

	// pas assez d'éléments pour choisir un pivot
	if (len < 3) {
		if (tab[1] < tab[0])
			swap(&tab[1], &tab[0]);
		return;
	}
	
	// choix d'un pivot
	ipivot = pivot(tab, len);
	swap(&tab[0], &tab[ipivot]);

	while(1) {
		do i++; while (i < len && tab[i] < tab[0]);
		do j--; while (tab[j] > tab[0]);
		if (j < i) break;
		swap(&tab[i], &tab[j]);
	}

	swap(&tab[0], &tab[j]);
	quick_sort(tab, j);
	quick_sort(tab+j+1, len-j-1);
}

/**
 * Algorithme de tri rapide parallélisé par tâche
 * @param tab tableau à trié
 * @param len longueur du tableau
 */
void _quick_sort_omp(int *tab, int len)
{
	int i=0, j=len, ipivot;
	
	// pas de tri à faire
	if (len <= 1) 
		return;

	// pas assez d'éléments pour choisir un pivot
	if (len < 3) {
		if (tab[1] < tab[0])
			swap(&tab[1], &tab[0]);
		return;
	}
	
	// choix d'un pivot
	ipivot = pivot(tab, len);
	swap(&tab[0], &tab[ipivot]);

	while(1) {
		do i++; while (i < len && tab[i] < tab[0]);
		do j--; while (tab[j] > tab[0]);
		if (j < i) break;
		swap(&tab[i], &tab[j]);
	}

	swap(&tab[0], &tab[j]);

	#pragma omp task
	_quick_sort_omp(tab, j);
	
	#pragma omp task
	_quick_sort_omp(tab+j+1, len-j-1);
}

/**
 * Lanceur de l'algorithme de tri rapide pour une parallélisation par tâche
 * @param tab tableau à trier
 * @param len longueur du tableau
 */
void quick_sort_omp(int *tab, int len)
{
	#pragma omp parallel
	{
		#pragma omp single
		_quick_sort_omp(tab, len);
	}
}

/**
 * Réarrange en triant les éléments des deux tableaux.
 * Les plus petits dans tab1 et les plus grands dans tab2.
 * @param tab1 
 * @param tab2 
 */
void merge_sort(int *tab1, int *tab2)
{
	int ind, ind1, ind2;
	int *tmp = (int *)malloc(2*k*sizeof(int));

	if (tmp == NULL) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}

	ind1 = 0;
	ind2 = 0;
	
	for (ind = 0; ind < k*2; ind++) {
		// recopier la fin de tab2 dans tmp
		if (ind1 >= k) {
			memcpy(tmp+ind, tab2+ind2, (k-ind2)*sizeof(int)); 
			break;
		}
		// recopier la fin de tab1 dans tmp
		if (ind2 >= k) {
			memcpy(tmp+ind, tab1+ind1, (k-ind1)*sizeof(int));
			break;
		}
		
		if (tab1[ind1] < tab2[ind2]) {
			tmp[ind] = tab1[ind1];
			ind1++;
		}
		else {
			tmp[ind] = tab2[ind2];
			ind2++;	
		}		
	}
	
	// decouper tmp dans tab1 et tab2
	memcpy(tab1, tmp, k*sizeof(int));
	memcpy(tab2, tmp+k, k*sizeof(int));

	free(tmp);
}



int main(int argc, char* argv[])
{
	if (argc != 3) {
		fprintf(stderr, "%s\n", usage);
		return 0;
	}

	/* Initialisation */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	
	MPI_Status  status;

	// TODO: tester != 0
	int nb_elem = atoi(argv[1]);	// nombre d'éléments total
	int sort_type = atoi(argv[2]);	// type de l'algorithme de tri

	k = nb_elem / nb_proc;

	int *tab_sort = (int *)malloc(k*sizeof(int));
	int *tab_tmp = (int *)malloc(k*sizeof(int));
	
	if ((tab_tmp == NULL) || (tab_sort == NULL)) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		MPI_Finalize();
		exit(1);
	}

	int left = my_rank-1;
	int right = my_rank+1;

	// initialise le tableau local
	P0 printf("Initialisation du tableau...");
	switch (sort_type) {
		case 1:	
		case 2: init_rand(tab_tmp, k); break;
		case 3:
		case 4:
		case 5: init_rand(tab_sort, k); break;
	}
	P0 printf(" OK!\n");

	P0 printf("Calcul...\n");

	// début du chronométrage
	double start, end;
	start = MPI_Wtime();
	
	// choix de l'algorithme de tri
	switch (sort_type) {
		case 1: PRAM(tab_tmp, tab_sort); break;
		case 2: PRAM_omp(tab_tmp, tab_sort); break;
		case 3: quick_sort(tab_sort, k); break;
		case 4: quick_sort_omp(tab_sort, k); break;
		case 5: qsort(tab_sort, k, sizeof(int), compare); break;
	}

	// tri pair-impair
	int step;
	for (step = 0; step < nb_proc; step++) {
		if ((my_rank%2) - (step%2) == 0) {
			if (my_rank != nb_proc-1) {
				MPI_Recv(tab_tmp, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD, &status);
				merge_sort(tab_sort, tab_tmp);
				MPI_Send(tab_tmp, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD);
			}
		}
		else {
			if (my_rank != 0) {
				MPI_Send(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD);
				MPI_Recv(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD, &status);
			}
		}
	}
	
	// fin du chronométrage
	end = MPI_Wtime();

	print_results(end - start);

	// écriture dans le fichier 	
	if (nb_elem <= NMAX) {
		P0 printf("Ecriture du fichier...");

		MPI_File file; 
		MPI_Offset my_offset;
		char filename[strlen("/Vrac/ppar_cassat_ducamain_sort")+1];
		strcpy(filename, "/Vrac/ppar_cassat_ducamain_sort");	

		my_offset = my_rank * sizeof(int) * k;
		MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file);
		MPI_File_write_at(file, my_offset, tab_sort, k, MPI_INT, &status);
		// printf("(%d) a écrit: ", my_rank);
		// print_tab(tab_sort, k);
		
		// attends que tous aient écrit
		MPI_Barrier(MPI_COMM_WORLD);
		
		MPI_File_read_ordered(file, tab_sort, k, MPI_INT, &status);
		MPI_File_close(&file);
		// printf("(%d) a lu: ", my_rank);
		// print_tab(tab_sort, k);

		P0 printf(" OK!\n");
	}

	// Vérification du tri
	P0 printf("Vérification du tri...\n");

	if (!check_sort(tab_sort, k) && (my_rank == 0))
		printf("\tTri correct!\n");

	#ifdef BENCH
	P0 fprintf(stderr, "\t%d\t%d", nb_elem, nb_proc);

	#ifdef _OPENMP
	#pragma omp parallel
	{
		if ((my_rank == 0) && (omp_get_thread_num() == 0))
			fprintf(stderr, "\t%d", omp_get_num_threads());
	}
	#endif
	
	P0 fprintf(stderr, "\n");
	#endif 

	free(tab_sort);
	free(tab_tmp);

	/* Desactivation */
	MPI_Finalize();
	return 0;
}
