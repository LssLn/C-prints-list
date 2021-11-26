/*
Una stampante di rete memorizza per ogni stampa effettuata da un utente remoto le seguenti informazioni:
•	Identificativo dell'utente che ha inviato la stampa - preferibile usare una stringa
•	Data della stampa - AAAA/MM/GG
•	Orario della stampa - HH:MM
•	Numero di pagine stampate - ovviamente un intero (ma meglio un unsigned)
•	Tipo di stampa (fronte e retro o solo fronte) - un intero o un booleano  => quindi un intero
Scrivere un opportuno main in cui sia previsto un menu di scelta delle operazioni richieste.
Tutti i valori necessari al funzionamento devono essere passati utilizzando parametri,
non è permesso l’uso di variabili globali.
1.	Implementare una funzione che legga da file l'elenco delle
	stampe effettuate e le inserisca in una coda. (apri_file)
2.	Implementare una funzione che restituisca il numero di stampe
	fronte e retro ed il numero di stampe solo fronte effettuate da un
	utente passato come parametro. (calcola_numero_stampe)
3.	Implementare una funzione che restituisca in un opportuno vettore
	le informazioni delle stampe effettuate in una certa data passata come parametro. (stampe_per_data)
4.	Implementare una funzione che crei e restituisca in una lista l'elenco
	delle stampe effettuate da un certo utente passato come parametro. (stampe_per_clienti)
*/
#define _CRT_SECURE_NO_WARNINGS
#define STAMPA_DIM_ID	16
#define STAMPA_FRONTE	1
#define STAMPA_RETRO	2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 1. struttura dati
typedef struct s_stampa {
	char id[STAMPA_DIM_ID];
	int numero_pagine;
	int tipo_stampa;
	char data[12];
	char ora[12];
}t_stampa;
typedef struct s_nodo {
	t_stampa atomo;
	struct s_nodo* successivo;
} t_nodo;
typedef t_nodo* t_lista;


void lista_aggiungi(t_lista* pt_lista, t_stampa stampa) {
	t_nodo* aux = (t_nodo*)malloc(sizeof(t_nodo));
	if (aux == 0)
		return;
	aux->atomo = stampa;
	aux->successivo = *pt_lista;
	*pt_lista = aux;
}
void lista_svuota(t_lista* pt_lista) {
	if (*pt_lista == NULL)
		return;
	lista_svuota(&((*pt_lista)->successivo)); // Guardate bene questa istruzione
	free(*pt_lista); *pt_lista = NULL;
}
void lista_stampa_contenuto(t_lista lista) {
	t_nodo* aux = lista;
	while (aux != NULL) {
		printf("%s %d %d %s %s\n", aux->atomo.id, aux->atomo.numero_pagine, aux->atomo.tipo_stampa, aux->atomo.data, aux->atomo.ora);
		aux = aux->successivo;
	}
}

typedef struct s_coda {
	t_nodo* primo;
	t_nodo* ultimo;
} t_coda;
void coda_aggiungi(t_coda* pt_coda, t_stampa stampa) {
	t_nodo* aux = (t_nodo*)malloc(sizeof(t_nodo));
	if (aux == 0)
		return;
	aux->atomo = stampa;
	aux->successivo = NULL;
	if (pt_coda->primo == NULL) {
		pt_coda->primo = pt_coda->ultimo = aux;
	}
	else {
		pt_coda->ultimo->successivo = aux;
		pt_coda->ultimo = aux;
	}
}
void coda_stampa_contenuto(t_coda coda) {
	t_nodo* aux = coda.primo;
	while (aux != NULL) {
		printf("%s %d %d %s %s\n", aux->atomo.id, aux->atomo.numero_pagine, aux->atomo.tipo_stampa, aux->atomo.data, aux->atomo.ora);
		aux = aux->successivo;
	}
}
// 2. definizione dei prototipi della quattro funzioni
void apri_file(t_coda* pt_coda, char* nome_file);
void calcola_numero_stampe(t_coda coda, const char* id_utente, int* pt_numero_fronte, int* pt_numero_retro);
//char* id_utente	||	char id_utente[]	|| const char* id_utente :: puntatore che non deve essere modificato
int* stampe_per_data(t_coda coda, const char* data);
t_lista stampe_per_cliente(t_coda coda, const char* id);

int menu(const char* testo, unsigned numero_scelte) {
	int scelta = 0;
	do {
		printf(testo);
		printf("\n\n>> ");
		scanf("%d", &scelta);
		if (scelta < 0 || scelta > numero_scelte)
			printf("Scelta non permessa\n\n");
	} while (scelta < 0 || scelta > numero_scelte);
	return scelta;
}
int main(void) {
	int scelta = 0;
	t_coda coda_stampa = { NULL, NULL }; //inizializzo la coda (per questo a volte faccio inizializza)
	char buffer[128];
	int numero_stampe_fronte, numero_stampe_retro;
	char* testo_menu = "1. Apri file\n2. Conta stampe\n3. stampe per data\n4. stampe per cliente\n0. Esci\n";
	do {
		scelta = menu(testo_menu, 4);
		switch (scelta) {
		case 0: // Uscita
			printf("Ciao, ciao\n");
			break;
		case 1: // apri file
			printf("Inserisci nome del file: ");
			scanf("%s", buffer);
			apri_file(&coda_stampa, buffer);
			coda_stampa_contenuto(coda_stampa);
			break;
		case 2: // numero di satmpe per utente
			printf("Inserisci codice utente: ");
			scanf("%s", buffer);
			calcola_numero_stampe(coda_stampa, buffer, &numero_stampe_fronte, &numero_stampe_retro);
			printf("%d %d\n", numero_stampe_fronte, numero_stampe_retro);
			break;
		case 3:
			// da implemetare

			break;
		case 4: // stampe di un cliente
			printf("Inserisci codice utente: ");
			scanf("%s", buffer);
			t_lista lista_temporanea = stampe_per_cliente(coda_stampa, buffer);
			lista_stampa_contenuto(lista_temporanea);
			lista_svuota(&lista_temporanea);
		}
	} while (scelta != 0);
}

void apri_file(t_coda* pt_coda, char* nome_file) {
	t_stampa stampa;
	FILE* f = NULL;
	f = fopen(nome_file, "r");
	if (f == NULL)
		return;
	while (fscanf(f, "%s%d%d%s%s", stampa.id, &stampa.numero_pagine, &stampa.tipo_stampa, stampa.data, stampa.ora) == 5) {
		coda_aggiungi(pt_coda, stampa);
	}
	fclose(f);
}

void calcola_numero_stampe(t_coda coda, const char* id_utente, int* pt_numero_fronte, int* pt_numero_retro) {
	int accumulatore_fronte = 0;
	int accumulatore_retro = 0;

	t_nodo* aux = coda.primo;
	while (aux != NULL) {
		if (strcmp(id_utente, aux->atomo.id) == 0) {
			if (aux->atomo.tipo_stampa == STAMPA_FRONTE)
				accumulatore_fronte += aux->atomo.numero_pagine;
			else
				accumulatore_retro += aux->atomo.numero_pagine;
		}
		aux = aux->successivo;
	}
	*pt_numero_fronte = accumulatore_fronte;
	*pt_numero_retro = accumulatore_retro;
}

int* stampe_per_data(t_coda coda, const char* data) {
	int* v = (int*)malloc(2 * sizeof(int)); // ATTENZIONE: deve essere allocato dinamicamente, perchè?

	// per sviluppare questo esercizio dovete fare una visita della struttura
	// dati e selezionare il tipo di stampa
}

t_lista stampe_per_cliente(t_coda coda, char* id) {
	t_lista lista_uscita = NULL;
	t_nodo* aux = coda.primo;

	while (aux != NULL) {
		if (strcmp(id, aux->atomo.id) == 0) {
			lista_aggiungi(&lista_uscita, aux->atomo);
		}
		aux = aux->successivo;
	}
	return lista_uscita;
}
