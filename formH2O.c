#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

void* thread_O(void* arg);
void* thread_H(void* arg);
void inicia_variaveis();
void destroi_variaveis();
void inicia_threads();
void forma_h2o();

pthread_mutex_t mutex;
sem_t O_sem;
sem_t H_sem;
pthread_barrier_t barreira;
int O = 0;
int H = 0;
int moleculas_lidas = 0;

void inicia_variaveis() {
    pthread_barrier_init(&barreira, NULL, 3);
    sem_init(&O_sem, 0, 0);
    sem_init(&H_sem, 0, 0);
    pthread_mutex_init(&mutex, NULL);
}

void destroi_variaveis() {
    sem_destroy(&H_sem);
    sem_destroy(&O_sem);
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barreira);
}

void inicia_threads() {
    srand((unsigned)time(NULL));
    int num_of_molecules = 99;
    pthread_t molecules[num_of_molecules];
    int threads_ids[num_of_molecules];

    for (int i = 0; i <= num_of_molecules; i++) {
        threads_ids[i] = i;
        int isH = rand() % 3;
        if (isH > 0) {
            pthread_create(&molecules[i], NULL, &thread_H, (void*)&threads_ids[i]);
        }
        else {
            pthread_create(&molecules[i], NULL, &thread_O, (void*)&threads_ids[i]);
        }
    }

    for (int i = 1; i <= num_of_molecules; i++) {
        pthread_join(molecules[i], NULL);
    }
}

void* thread_O(void* arg) {
    pthread_mutex_lock(&mutex);
    moleculas_lidas++;
    O++;
    printf("O recebido! quantidade total = %d - Thread: %d\n", O, *((int*)arg));
    if (H >= 2) {
        sem_post(&H_sem);
        sem_post(&H_sem);
        H = H - 2;
        sem_post(&O_sem);
        O--;
    }
    else {
        pthread_mutex_unlock(&mutex);
    }

    sem_wait(&O_sem);

    forma_h2o();

    fflush(stdout);
    pthread_barrier_wait(&barreira);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* thread_H(void* arg) {
    pthread_mutex_lock(&mutex);
    moleculas_lidas++;
    H++;
    printf("H recebido! quantidade total = %d - Thread: %d\n", H, *((int*)arg));
    if (H >= 2 && O >= 1) {
        sem_post(&H_sem);
        sem_post(&H_sem);
        H = H - 2;
        sem_post(&O_sem);
        O--;
    }
    else {
        pthread_mutex_unlock(&mutex);
    }    
    sem_wait(&H_sem);
    forma_h2o();
    fflush(stdout);
    pthread_barrier_wait(&barreira);
	return 0;
}

void forma_h2o() {
    // int sem_value_h;
    // int sem_value_o;
    // sem_getvalue(&H_sem, &sem_value_h);
    // sem_getvalue(&O_sem, &sem_value_o);
    // printf("\n---------------------------------------------\n\n");
    printf("\nH20 Formada! (Threads atravessaram a barreira)\n");
    // printf("Estoque H: %d\n", H);
    // printf("Estoque O: %d\n", O);
    // printf("Sem O Value: %d\n", sem_value_o);
    // printf("Sem H Value: %d\n", sem_value_h);
    printf("Total Moleculas Recebidas: %d\n", moleculas_lidas);
    // printf("---------------------------------------------\n\n");
    // if ((sem_value_h < 1 && sem_value_o < 1) && (moleculas_lidas == 100)) {
    //     if(H > 0 || O > 0) {
    //         printf("Impossível Produzir H2O (Deadlock).\n");
    //     } else {
    //         printf("Todas as Moleculas Possiveis Utilizadas!\n");
    //     }
    //     exit(0);
    // }
}

int main(int argc, char** argv) {
    inicia_variaveis();
    inicia_threads();
    destroi_variaveis();
    return 0;
}