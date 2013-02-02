// CODE COPIED FROM PDF FILES, there might be some format errors 
/* Programma che, acquisita una espressione aritmetica semplice da file, 
 * inserisce le parentesi riflettenti la precedenza e la associatività degli operatori 
 * (i due unari, i tre moltiplicativi e i tre additivi ),  tenendo conto anche delle 
 * eventuali parentesi inserite. Il programma calcola inoltre il tipo dell'espressione 
 * acquisita, assumendo che le costanti che variabili con una lettera tra a ed l 
 * sono di tipo int e le altre di tipo double).
 * Studente: Elvis Ciotti (Matr. 200244)
 * Prof: Marco Bernardo
 * C.d.l in informatica applicata - Programmazione degli elaboratori 7/02/2002 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define FILE_INPUT "input.dat"    /* Nome del file di input*/
#define FILE_OUTPUT "output.dat"  /* Nome del file di output */
int tipo = 0; /* tiene in memoria il tipo dell'espressione */
int n_carat(FILE *file_f);
void scansione_file(char *a, FILE *file_f);
int contr_espress(char *a, int n);
void risolvi_tutto(char *a, int *b, int n);
char pros_par_val(char *a, int part, int *fine);
void risolvi_sottoesp(char *a, int *b, int part, int fine);
void metti_par(char *a, int *b, int indice_op, int prima, int dopo);
int is_op(char c);
int is_molt(char c);
int is_ci_double(char c);
void stampa_vett(char *a, int *b, int n); /* facoltativa */
void calcola_tipo(char *a, int n);
void stampa_su_file(char *a, int *b, int n, FILE *output_f);

/**
 * MAIN
*/
int main(void) {
    FILE *input_f = NULL;
    FILE *output_f = NULL;
    int n = n_carat(input_f);
    /* contiene n.di caratteri nel file */ char *vet_espr; /* vettore con espressione */
    int *vet_par; /* vettore con i numeri di parentesi */
    if (n) /* se il file esiste e non è vuoto */ {
        /* allocamento memoria ai vettori di lavoro */
        vet_espr = (char *) calloc((size_t) (n + 2), sizeof (char));
        /* allocamento memoria */ 
        vet_par = (int *) calloc((size_t) (n + 2), sizeof (int));
        /* controllo disponibilita memoria */ 
        if (vet_espr != NULL && vet_par != NULL) {
            scansione_file(vet_espr, input_f);
            if (contr_espress(vet_espr, n)) {
                risolvi_tutto(vet_espr, vet_par, n);
                stampa_vett(vet_espr, vet_par, n);
                calcola_tipo(vet_espr, n);
                stampa_su_file(vet_espr, vet_par, n, output_f);
            }
            free(vet_espr); /* disalloca memoria utilizzata */
            free(vet_par);
        } else
            printf("Memoria non disponibile.");
    } else
        printf("ERRORE: file di input %s inesistente o vuoto.\n", FILE_INPUT);
    return (0);
}

/* 
 * restituisce 0 se il file non esiste o non contiene nessun carattere,
 * altrimenti restituisce il numero di caratteri presenti tralasciando gli spazi 
 */
int n_carat(FILE *file_f) {
    int n = 0;
    char tmp;
    file_f = fopen(FILE_INPUT, "r");
    if (file_f != NULL) /* se il file esiste */ {
        while (fscanf(file_f, "%c", &tmp) != EOF)
            /* conta i caratteri tralasciando spazi e \n*/
            if (tmp != ' ' && tmp != '\n') n++;
        fclose(file_f); 
    }
    return n; /* ritorna numero di caratteri */
}

/* 
 * riversa contenuto del file nel vettore passato tralasciando gli spazi 
 */
void scansione_file(char *a, FILE *file_f) {
    int i = 1;
    char tmp; /* variabile char temporanea */
    a[0] = '('; /* inserisce parentesi di apertura */
    /* apertura file, il controllo della sua esistenza e'
     *  gia stato eseguito dalla funzione precedente */
    file_f = fopen(FILE_INPUT, "r");
    /* scansione contenuto file sul vettore tralasciando gli spazi e\n */
    while (fscanf(file_f, "%c", &tmp) != EOF)
        if (tmp != ' ' && tmp != '\n') {
            a[i] = tmp;
            i++;
        }
    a[i] = ')'; /* inserisce parentesi di chiusura */
    fclose(file_f);
}

/* 
 * valuta se il vettore fino alla n-esima posizione contiene una espressione corretta 
 */
int contr_espress(char *a, int n) {
    int i = 1,
            ret = 1, /* variabile di ritorno (vale 1 se non ci sono errori) */
            n_ap = 0,
            n_ch = 0,
            n_punti = 0;
    while (i < n + 2) {
        /* controllo moltiplicativo e par.chiusa che siano precedute da solo da alnum, sottotratti o altre par.chiuse */
        if (is_molt(a[i]) || a[i] == ')') {
            if (!isalnum(a[i - 1]) && a[i - 1] != '_' && a[i - 1] != ')') {
                printf("Errore al carattere %d\n", i);
                ret = 0;
            }
            i++;
            /* rilascia contatore alla posizione seguente per controlli successivi */
        }
        /* controllo par. aperte precedute da altre par.chiuse */
        if (i < n + 2 && a[i] == '(') {
            if (a[i - 1] == ')') {
                printf("Errore al carattere %d, manca operatore\n", i);
                ret = 0;
                i++; /* rilascia contatore alla posizione seguente */
            }
            i++; /* rilascia contatore alla posizione seguente */
        }
        /* controllo identificatori di variabili */
        if (i < n + 2 && (isalpha(a[i]) || a[i] == '_'))  {
            i++;
            /* finchè non arrivi alla fine dell'identificatore (cioe ad un operatore che non
            sia parentesi aperta ) */
            while (!is_op(a[i]) || a[i] == '(')  {
                if (!isalnum(a[i]) && a[i] != '_')  {
                    printf("Errore al carattere %d non valido per un'identificatore di variabile\n", i);
                    ret = 0;
                }
                i++;
            }
            i++;
        }
    
        /* controllo costante numerica */
        if (i < n + 2 && isdigit(a[i])) {
            i++; /* parti dal carattere successivo */
            while (!is_op(a[i]) || a[i] == '(') {
                if (!isdigit(a[i]) && a[i] != '.') /* se trovi carattere non valido per costante numerica */ {
                    printf("Errore al carattere %d non valido per una costante numerica.\n", i);
                    ret = 0;
                }
                /* incrementa n. di punti trovati */
                if (a[i] == '.')
                    n_punti++; 
                i++;
            }
            
            /* se hai trovato piu di un punto nella stessa costante */ 
            if (n_punti > 1) {
                printf("Errore: una costante numerica non deve contenere %d punti\n", n_punti);
                ret = 0;
                n_punti = 0; 
            }
            /* se hai trovato piu di un punto nella stessa costante */ 
            if (a[i - 1] == '.') /* controllo eventuale punto alla fine della costante */ {
                printf("Errore al carattere %d: una costante non puo terminare con il punto\n", i);

                ret = 0;
            }
            i++;
        }
    
        /* in caso trovi + e - incrementa lo stesso il contatore anche se non occorre eseguire alcun controllo (qualsiasi carattere di una espr. e valido prima di questi) */
        if ((i < n + 2 && a[i] == '+') || a[i] == '-')
            i++;
        /* controllo caratteri singoli non accettabili in una espressione */
        if (i < n + 2 && !is_op(a[i]) && !isalnum(a[i]) && a[i] != '_') {
            printf("Errore al carattere %d non valido\n", i);
            ret = 0;
            ret = 0;
        }
        i++; /* rilascia contatore alla posizione seguente l'operatore */
    }
    /* in caso trovi + e - incrementa lo stesso il contatore anche se non occorre eseguire alcun controllo */
    /* fine while */

    for (i = 0; i < n + 2; i++) {
        if (a[i] == '(')
            n_ap++;
        if (a[i] == ')')
            n_ch++;
    }
    /* controllo n.par */
    if (n_ap != n_ch) {
        printf("Errore: %d parentesi aperte e %d parentesi chiuse.\n", n_ap, n_ch);
        ret = 0;
    }
    if (ret == 0) /* notifica dopo la stampa degli errori */ printf(" Il file di output non é stato creato.\n");
    return ret;
}

/* inserisce gli indici parentesi su tutto il vettore */
void risolvi_tutto(char *a, int *b, int n) {
    int i,
        k; 
    /* fino a che la espressione radice non è risolta */ 
    while (a[0] == '(') 
        for (i = 0; (i <= n); i++)
            if (a[i] == '(' && pros_par_val(a, i + 1, &k) == ')')
                risolvi_sottoesp(a, b, i, k); 
    /* annullamento della sostituzione temporanea effettuata dalla funzione risolvi_sottoesp */
    for (i = 1; (i < n); i++)
        switch (a[i]) {
            case 15:
                a[i] = '+';
                break;
            case 16:
                a[i] = '-';
                break;
            case 17:
                a[i] = '*';
                break;
            case 18:
                a[i] = '/';
                break;
            case 19:
                a[i] = '%';

                break;
        }
}

/* 
 * Restituisce la N-esima posizione. 
 */
char pros_par_val(char *a, int part, int *fine) {
    while (a[part] != ')' && a[part] != '(')
        part++;
    *fine = part; /* restituisce anche per indirizzo la posizione della parentesi trovata nel vettore */

    return a[part]; /* ritorna il carattere par.chiusa o aperta */
}

/* risolve la sottoespressione dalla part-esima alla fine-esima riga
 * collocando nel vettore b gli indici di parentesi relativi all'espressione del vettore a
 */
void risolvi_sottoesp(char *a, int *b, int part, int fine) {
    int i,
            k;
    for (i = fine - 1; (i > part); i--)
        if ((a[i] == '+' || a[i] == '-') && is_op(a[i - 1])){
            b[i]++; /* incrementa indice parentesi corrispondende all'unario */
            /* inserisce parentesi di chiusura relative all'operatore nella posizione i */
            metti_par(a, b, i, 0, 1); 
        }
    /* inserimento indici parentesi per op. moltiplicativi */
    for (i = part + 1; (i < fine); i++)
        /* se la i-esima posizione contiene un operatore moltiplicativo */ 
        metti_par(a, b, i, 1, 1); 
    /* inserisce parentesi prima e dopo relative all'operatore nella posizione i*/
        if (is_molt(a[i])) 
    /* inserimento indici parentesi per op. additivi */
    for (i = part + 1; (i < fine); i++)
        if ((a[i] == '+' || a[i] == '-') && !is_op(a[i - 1])) /* se la i-esima posizione contiene un operatore additivo */
            metti_par(a, b, i, 1, 1); /* inserisce parentesi prima e dopo relative all'operatore nella posizione i*/

    /* ricerca errori di costanti o id.var di tipo double agenti su operatore di modulo */ for (i = part + 1; (i < fine); i++) {
        if (tipo != -1 && a[i] == '%') /* se non hai già trovato errore e se trovi operatore
di modulo */ {
            /* ricerca costanti e id.var. double all'indietro prima di %*/
            for (k = i - 1;
                    (a[k] != '(' && a[k] != '+' && a[k] != '-'); /* fino a che non arrivi
all'inizio dell'espressione o a op. additivo */ k--)

                /* se trovi punto oppure un id. double */
                if (a[k] == '.' || (is_ci_double(a[k]) && is_op(a[k - 1])))
                    tipo = -1;
            /* ricerca costanti e id.var. double in avanti dopo il % */
            for (k = i + 1; /* salto eventuali unari sulla costante successiva al % */
                    (a[k] == '+' || a[k] == '-'); k++);
            if (is_ci_double(a[k])) /* se trovi identificatore double subito dopo gli unari */
                tipo = -1;
            /* ricerca costanti double entro il prossimo operatore */
            for (k = k + 1; 
                    (!is_op(a[k]));
                    k++)
                if (a[k] == '.') tipo = -1;
        }
    }

    /* sostituzione parentesi quadre con tonde a indicare la sottoespressione compresa risolta */
    a[part] = '[';
    a[fine] = ']';
    /* sostituzione temporanea degli operatori in modo da non essere
    nuovamene tenuti in considerazione quando si risolve l'espressione madre */
    for (i = part + 1;
            (i < fine);
            i++)
        switch (a[i]) {
            case '+':
                a[i] = 15;
                break;

            case '-': a[i] = 16;
                break;
            case '*':
                a[i] = 17;
                break;
            case '/':
                a[i] = 18;
                break;
            case '%':
                a[i] = 19;
                break;
        }
}

/* mette nel vettore b i numeri di parentesi corrispondenti all'espressione contenuta nel vettore a
e relativi all'operatore nella indice_op-esima posizione.
Se prima (succ) e diverso da 0 allora viene messo in vettore b l'indice parentesi prima (dopo) del precedente (successivo) operatore del
vettore a (gli intervalli di parentesi risolti vengono saltati sfruttando la somma parziale degli indici parentesi intermedi nel vettore b).*/
void metti_par(char *a, int *b, int indice_op, int prima, int dopo) {
    int i,
        somma, /* contatore somma parziale indici utilizzata in vettore */ 
        cond; /* variabile sentinella */
    /* inserimento indice parentesi aperta prima (se l'operatore era unario non viene eseguita) */
    if (prima) {
        /* ricerca all'indietro del precedente operatore in vettore a */ 
        for (i = indice_op - 1, somma = 0, cond = 1;
                (cond); i--) {
            somma += b[i]; /* aggiornamento somma parzialedegli indici */
            /* se la somma parziale vale 0 oppure esiste un'operatore nella posizione precedente del vettore espressione corrispondente */
            if (somma == 0 && (is_op(a[i - 1]) || a[i - 1] == '[')) {
                b[i]++; /* incremento indice parentesi dell'elemento in vettore b */
                cond = 0;
                /* esce dal ciclo - parentesi messa */
            }
        }
    }

    /* inserimento indice parentesi chiusa dopo */ 
    if (dopo) {
        /* ricerca in avanti del successivo operatore in vettore a */
        for (i = indice_op + 1, somma = 0, cond = 1; (cond);
                i++) {
            somma += b[i]; /* aggiornamento somma parziale degli indici */
            /* se la somma parziale vale 0  oppure esiste un'operatore nella
               posizione successiva del vettore espressione a corrispondente */
            if (somma == 0 && (is_op(a[i + 1]) || a[i + 1] == ']')) {
                b[i]--;
                cond = 0;
            }
        }
    }
}

/* restuisce vero se il carattere è un operatore o una parentesi */
int is_op(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '(' || c == ')');
}

/* ritorna vero se il char passato è operatore moltiplicativo */
int is_molt(char c) {
    return (c == '*' || c == '/' || c == '%');
}

/* ritorna vero se il char passato puo essere inizio di id. di var. double, ovvero se il carattere è compreso fra la m e la z oppure e un sottotratto */
int is_ci_double(char c) {
    return ((c >= 'm' && c <= 'z') || (c >= 'M' && c <= 'Z') || c == '_');
}

/* stampa a video i due vettori allineati per migliore comprensione dell'algoritmo di risoluzione.
Puo essere rimossa senza compromettere il funzionamento dle programma.*/
void stampa_vett(char *vet_espr, int *vet_par, int n) {
    int i;
    printf("Vettore espres : ");
    for (i = 0; (i < n + 2); i++)
        printf("%2c", vet_espr[i]);
    printf("\nVettore parent : ");
    for (i = 0; (i < n + 2); i++)
        printf("%2d", vet_par[i]);
    printf("\n");
}

/* Restituisce 0 o 1 a seconda che l'espressione sia rispettivamente int o double. Per essere double basta che esista un identificatore di costante double di lettere
dell'alfabeto (fra m e z) o numerica (contenente il carattere '.'), sempre che non si siano rilevati errori di operatore di modulo su operandi double */
void calcola_tipo(char *a, int n) {
    int i;
    if (tipo != -1) /* esegue calcolo tipo solo se non ci sono operatori di modulo su costanti o id. double */ {
        /* ricerca costanti double */
        for (i = 1;
                (i < n + 1);
                i++)
            if (((a[i] == '[' || a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/') && is_ci_double(a[i + 1])) || a[i + 1] == '.')
                tipo = 1;
    }
}

/* Stampa su file l'espressione con le parentesi e il tipo */
void stampa_su_file(char *a, int *b, int n, FILE *output_f) {
    int i;
    output_f = fopen(FILE_OUTPUT, "w");
    fprintf(output_f, "L'espressione contenuta nel file %s con le parentesi riflettenti\nprecedenza e associatività degli operatori è la seguente:\n", FILE_INPUT);
    for (i = 1;
            (i <= n);
            i++) /* stampa combinata dell'espressione con parentesi */ {
        if (b[i] == 0 && a[i] != '[' && a[i] != ']') /* stampa caratteri escluse le par. quadre*/
            fprintf(output_f, "%c", a[i]);
        if (b[i] > 0) /* indice vettore parentesi con numero positivo di parentesi (aperte)
*/ {/* stampa tante parentesi aperte quanto indica l'i-esimo elemento del vettore parentesi */
            while (b[i] > 0) {
                fprintf(output_f, "("); /* stampa tonde aperte */
                b[i]--;
            }
            fprintf(output_f, "%c", a[i]);
            /* stampa il carattere i-esimo della espress. */
        }
        /* indice vettore parentesi con numero di negativo parentesi (chiuse) */
        if (b[i] < 0)  {
            fprintf(output_f, "%c", a[i]); /* stampa carattere i-esimo */
            /* stampa tante parentesi aperte chiuse quanto indica i-esimo elemento del
            vettore parentesi */ while (b[i] < 0) {
                fprintf(output_f, ")");
                b[i]++;
            }
        }
    }
    /* stampa del tipo a seconda del valore contenuto nella var globale */
    switch (tipo) {
        case 0:
            fprintf(output_f, "\nEspressione di tipo INT.\n");
            break;
        case 1:
            fprintf(output_f, "\nEspressione di tipo DOUBLE.\n");
            break;
        case -1:
            fprintf(output_f, "\nErrore, l'operatore di modulo agisce su operandi double, tipo della espressione non determinabile.\n");
            break;
    }
    fclose(output_f);
    printf("Espressione risolta correttamente nel file %s.\n", FILE_OUTPUT);
}