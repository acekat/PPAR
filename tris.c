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

#define N 1572864			// nombre d'éléments par défaut
#define TAG_TAB 0
#define TAG_RES 1
#define TAG_CHECK 2

// TODO
// Optimisations:
// - allouer moins de ressources pour les tableau
// - faire le calcul dans chaque noeud permet d'économiser des messages (à tester)
// - tester si chaque processus calcul la fusion

int my_rank;	// rang du processus
int left_rank;	// rang du processus de gauche
int nb_proc;	// nombre de processus
int k;			// nombre d'éléments par processus

/**
 * Affiche le contenu du tableau.
 * @param tab
 */
void print_tab(int *tab)
{
	int i;
	printf("\t[");
	for (i = 0; i < k-1; i++) {
		printf("%d, ",tab[i]);
	}
	printf("%d]\n", tab[k-1]);
}

/**
 * Affiche le temps maximal de calcul
 * @param total   temps total de chaque processus
 * @param nb_proc 
 */
void print_results(double total, int nb_proc)
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
		printf("\tTri en %g sec\n", total);

		// TODO: calculer l'accélération et l'efficacité
	}
}

/**
 * Initialise le tableau avec des valeurs aléatoires.
 * @param tab
 */
void init_rand(int *tab)
{	
	int i;
	for (i = 0; i < k; i++) {
		srandom(time(NULL)+i*my_rank);
		tab[i] = random();		
	}
}

/**
 * Vérifie si les éléments locaus sont correctement triés
 * @param  tab tableau des éléments locaux
 * @return     1: tri incorrect
 *             0: tri correct
 */
int check_local(int *tab)
{
	int i = 1;
	while (i < k) {
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
 * @return     1: tri incorrect
 *             0: tri correct
 */
int check_sort(int *tab)
{
	int min = tab[0];
	int max = tab[k-1];
	
	if (my_rank != nb_proc-1)
		MPI_Send(&max, 1, MPI_INT, my_rank+1, TAG_CHECK, MPI_COMM_WORLD);

	if (my_rank != 0)
		MPI_Recv(&min, 1, MPI_INT, my_rank-1, TAG_CHECK, MPI_COMM_WORLD, NULL);
	
	if (check_local(tab) || (min > tab[0])) {
		printf("%d : /!\\ TRI GLOBAL INCORRECT /!\\ \n", my_rank);
		return 1;
	}

	return 0;
}

/**
 * Algorithme du tri PRAM
 * @param tab_in  
 * @param tab_out 
 */
void tri_PRAM(int *tab_in, int *tab_out)
{
	int i, j, cpt;
	// int count[k];
	int *count = (int *)malloc(k*sizeof(int));

	if (count == NULL) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}

	// initialise à 0 les cases du tableau
	memset(count, 0, sizeof(count));
	
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
 * @param tab_in  
 * @param tab_out 
 */
void tri_PRAM_omp(int *tab_in, int *tab_out)
{
	int i, j, cpt;
	// int count[k];
	int *count = (int *)malloc(k*sizeof(int));

	if (count == NULL) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}

	// initialise à 0 les cases du tableau
	memset(count, 0, sizeof(count));
	
	// comparaisons
	// VERSION 1
	#pragma omp parallel for private(j)
	for (i = 0; i < k; i++) {
		for (j = i+1; j < k; j++) {
			if (tab_in[i] > tab_in[j]) {
				#pragma omp atomic
				count[i]++;
			}
			else {
				#pragma omp atomic
				count[j]++;
			}
		}
	}

	// VERSION 2
	// #pragma omp parallel for private(j) 
	// for (i = 0; i < k; i++) {
	// 	#pragma omp parallel for private(j) 
	// 	for (j = 0; j < k; j++) {
	// 		if (((j<i) && (tab_in[i] == tab_in[j])) || (tab_in[i] > tab_in[j]))
	//  		count[i]++;
	// 	}
	// }


	// réarrangement
	#pragma omp parallel for private(cpt)
	for (i = 0; i < k; i++) {
		cpt = count[i];
		tab_out[cpt] = tab_in[i];
	}

	free(count);
}

/**
 * Réarrange les éléments des tableaux en mettant les plus petits éléments
 * dans tab1 et les plus grands dans tab2.
 * @param tab1 
 * @param tab2 
 */
void tri_fusion(int *tab1, int *tab2)
{
	int ind, ind1, ind2, i, j;
	// int tmp[2*k];
	int *tmp = (int *)malloc(2*k*sizeof(int));

	if (tmp == NULL) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}

	ind1 = 0;
	ind2 = 0;
	
	for (ind = 0; ind < k*2; ind++) {
		if (ind1 >= k) {
			// recopier la fin de tab2 dans tmp
			// memcpy(tmp+(ind*sizeof(int)), tab2+(ind2*sizeof(int)), (k-ind2)*sizeof(int)); 
			for (i = ind; i < k*2; i++, ind++, ind2++) {
				tmp[ind] = tab2[ind2];
			}			
			// printf("tab1 vide, ind2 = %d ; ind = %d\n", ind2, ind);
			break;
		}
		if (ind2 >= k) {
			// recopier la fin de tab1 dans tmp
			// memcpy(tmp+(ind*sizeof(int)), tab1+(ind1*sizeof(int)), (k-ind1)*sizeof(int));
			for (i = ind; i < k*2; i++, ind++, ind1++) {
				tmp[ind] = tab1[ind1];  
			}
			// printf("tab2 vide, ind1 = %d ; ind = %d\n", ind1, ind);
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
	// memcpy(tab1, tmp, k*sizeof(int));
	// memcpy(tab2, tmp+(k*sizeof(int)), k*sizeof(int));
	for (i = 0, j = 0; i < k; i++, j++) {
		tab1[i] = tmp[i];
		tab2[j] = tmp[k+j];
	}

	free(tmp);
}

int main(int argc, char* argv[])
{
	/* Initialisation */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	
	MPI_Status  status;

	int nb_elem = N;	// nombre d'éléments total
	int hybride = 0;	// 0: tri MPI pur, 1: tri hybride

	if (argc > 1)
		nb_elem = atoi(argv[1]);
	if (argc > 2)
		hybride = !strcmp(argv[2], "-hyb");

	k = nb_elem / nb_proc;
	// int tab_sort[k];
	// int tab_tmp[k];

	int *tab_sort = (int *)malloc(k*sizeof(int));
	int *tab_tmp = (int *)malloc(k*sizeof(int));
	
	if ((tab_tmp == NULL) || (tab_sort == NULL)) {
		fprintf(stderr, "Erreur allocation mémoire du tableau \n");
		exit(1);
	}

	int left = my_rank-1;
	int right = my_rank+1;

	// initialise le tableau local
	init_rand(tab_tmp);

	// #pragma omp parallel
	// {
	// 	#ifdef _OPENMP
	// 	omp_set_num_threads(4);
	// 	printf("== %d : %d threads\n", my_rank, omp_get_num_threads());
	// 	printf("== %d : le %d\n", my_rank, omp_get_thread_num());
	// 	#endif
	// }

	if (my_rank == 0)
		printf("Calcul...\n");

	// début du chronométrage
	double start, end;
	start = MPI_Wtime();
	
	// tri le tableau local
	if (hybride)
		tri_PRAM_omp(tab_tmp, tab_sort);
	else 
		tri_PRAM(tab_tmp, tab_sort);

	int step;
	// VERSION 1
	for (step = 1; step <= nb_proc; step++) {
		if (my_rank%2 == 0) {
			if (step%2 != 0) {
				if (my_rank != nb_proc-1) {
					MPI_Recv(tab_tmp, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD, &status);
					tri_fusion(tab_sort, tab_tmp);
					MPI_Send(tab_tmp, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD);
				}
			}
			else {
				if (my_rank != 0) {
					MPI_Recv(tab_tmp, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD, &status);
					tri_fusion(tab_tmp, tab_sort);
					MPI_Send(tab_tmp, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD);
				}
			}
		}
		else {
			if (step%2 != 0) {
				MPI_Send(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD);
				MPI_Recv(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD, &status);
			}
			else {
				if (my_rank != nb_proc-1) {
					MPI_Send(tab_sort, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD);
					MPI_Recv(tab_sort, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD, &status);
				}
			}
		}
	}
	
	// VERSION 2
	// for (step = 1; step <= nb_proc; step++) {
	// 	if (my_rank%2 == 0) {
	// 		if (step%2 != 0) {
	// 			if (my_rank != nb_proc-1) {
	// 				MPI_Send(tab_sort, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD);
	// 				MPI_Recv(tab_tmp, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD, &status);
	// 				tri_fusion(tab_sort, tab_tmp);
	// 			}
	// 		}
	// 		else {
	// 			if (my_rank != 0) {
	// 				MPI_Send(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD);
	// 				MPI_Recv(tab_tmp, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD, &status);
	// 				tri_fusion(tab_tmp, tab_sort);
	// 			}
	// 		}
	// 	}
	// 	else {
	// 		if (step%2 != 0) {
	// 			MPI_Send(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD);
	// 			MPI_Recv(tab_sort, k, MPI_INT, left, TAG_TAB, MPI_COMM_WORLD, &status);
	// 			tri_fusion(tab_tmp, tab_sort);
	// 		}
	// 		else {
	// 			if (my_rank != nb_proc-1) {
	// 				MPI_Send(tab_sort, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD);
	// 				MPI_Recv(tab_sort, k, MPI_INT, right, TAG_TAB, MPI_COMM_WORLD, &status);
	// 				tri_fusion(tab_sort, tab_tmp);
	// 			}
	// 		}
	// 	}
	// }
	
	// fin du chronométrage
	end = MPI_Wtime();

	print_results(end - start, nb_proc);


	// écriture dans le fichier 	
	if (nb_elem <= N) {
		if (my_rank == 0)
			printf("Ecriture du fichier...\n");

		MPI_File file; 
		MPI_Offset my_offset;
		char filename[strlen("/Vrac/ppar_cassat_ducamain_sort")+1];
		strcpy(filename, "/Vrac/ppar_cassat_ducamain_sort");	

		my_offset = my_rank * sizeof(int) * k;
		MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file);
		MPI_File_write_at(file, my_offset, tab_sort, k, MPI_INT, &status);
		// printf("(%d) a écrit: ", my_rank);
		// print_tab(tab_sort);
		
		// attends que tous aient écrit
		MPI_Barrier(MPI_COMM_WORLD);
		
		MPI_File_read_ordered(file, tab_sort, k, MPI_INT, &status);
		MPI_File_close(&file);
		// printf("(%d) a lu: ", my_rank);
		// print_tab(tab_sort);
	}

	// Vérification du tri
	if (my_rank == 0)
		printf("Vérification du tri...\n");

	if (!check_sort(tab_sort) && (my_rank == 0))
		printf("\tTri correct!\n");

	free(tab_sort);
	free(tab_tmp);

	/* Desactivation */
	MPI_Finalize();
	return 0;
}

// N: nombre d'éléments à trier
// P: nombre de processus
// R: rang du processus [0..P-1]
// E: numéro de l'étape [1..P]

// k = N/P
// R.gauche = R-1
// R.droite = R+1

// Pour P étapes
// 	Si R pair
// 		Si E impair
// 			Si R+1 == P
// 				continue
// 			recevoir tab de R.droite (bloquant)
// 			fusion trié du tableau reçu et du tableau local
// 			envoyer k plus grands éléments à R.droite
// 		Sinon
// 			Si R == 0
// 				continue
// 			recevoir tab de R.gauche (bloquant)
// 			fusion trié du tableau reçu et du tableau local
// 			envoyer k plus petits éléments à R.gauche
// 	Sinon
// 		Si E impair
// 			envoyer tab à R.gauche
// 			recevoir tab de R.gauche (bloquant)
// 		sinon
// 			Si R+1 == P
// 				continue
// 			envoyer tab à R.droite
// 			recevoir tab de R.droite (bloquant)

// Si R == 0
// 	ecrire tab dans le fichier
// 	envoyer ECRIT à R.droite
// else
// 	recevoir ECRIT de R.gauche (bloquant)
// 	ecrire tab à la suite du fichier
// 	Si R < P-1
// 		envoyer ECRIT à R.droite
