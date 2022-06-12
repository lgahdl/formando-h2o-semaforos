#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define NUM_MOLECULAS 999

void* thread_O(void* arg);
void* thread_H(void* arg);
void inicia_variaveis();
void destroi_variaveis();
void inicia_threads();
void forma_h2o();
void print_stock_and_finish_program();

pthread_mutex_t mutex;
pthread_mutex_t print_mutex;
sem_t O_sem;
sem_t H_sem;
pthread_barrier_t barreira;
int O = 0;
int H = 0;
int moleculas_lidas = 0;
int pode_imprimir = FALSE;

void inicia_variaveis() {
    pthread_barrier_init(&barreira, NULL, 3);
    sem_init(&O_sem, 0, 0);
    sem_init(&H_sem, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);
}

void destroi_variaveis() {
    sem_destroy(&H_sem);
    sem_destroy(&O_sem);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&print_mutex);
    pthread_barrier_destroy(&barreira);
}

void inicia_threads() {
    srand((unsigned)time(NULL));
    pthread_t molecules[NUM_MOLECULAS];
    int threads_ids[NUM_MOLECULAS];

    for (int i = 0; i < NUM_MOLECULAS; i++) {
        threads_ids[i] = i;
        int isH = rand() % 3;
        if (isH > 0) {
            pthread_create(&molecules[i], NULL, &thread_H, (void*)&threads_ids[i]);
        }
        else {
            pthread_create(&molecules[i], NULL, &thread_O, (void*)&threads_ids[i]);
        }
    }

    for (int i = 1; i <= NUM_MOLECULAS; i++) {
        pthread_join(molecules[i], NULL);
    }
}

void* thread_O(void* arg) {
    pthread_mutex_lock(&mutex);
    moleculas_lidas++;
    O++;
    printf("O recebido! Quantidade O = %d - Total lidas: %d (Thread: %d)\n", O, moleculas_lidas, *((int*)arg));
    fflush(stdout);
    if (H >= 2) {
        sem_post(&H_sem);
        sem_post(&H_sem);
        H = H - 2;
        sem_post(&O_sem);
        O--;
    }
    else {
        if (moleculas_lidas == NUM_MOLECULAS && H < 2) {
            print_stock_and_finish_program();
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }

    sem_wait(&O_sem);
    forma_h2o();

    pthread_barrier_wait(&barreira);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* thread_H(void* arg) {
    pthread_mutex_lock(&mutex);
    moleculas_lidas++;
    H++;
    printf("H recebido! Quantidade H = %d - Total lidas: %d (Thread: %d)\n", H, moleculas_lidas, *((int*)arg));
    fflush(stdout);
    if (H >= 2 && O >= 1) {
        sem_post(&H_sem);
        sem_post(&H_sem);
        H = H - 2;
        sem_post(&O_sem);
        O--;
    }
    else {
        if (moleculas_lidas == NUM_MOLECULAS) {
            print_stock_and_finish_program();
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }
    sem_wait(&H_sem);
    forma_h2o();

    pthread_barrier_wait(&barreira);
	return 0;
}

void print_stock_and_finish_program(){
    printf("\nTodas as moléculas lidas!\n");
    printf("Estoque H: %d\n", H);
    printf("Estoque O: %d\n", O);
    fflush(stdout);
    destroi_variaveis();
    exit(0);
}

void forma_h2o() {
    int sem_value_h;
    int sem_value_o;
    sem_getvalue(&H_sem, &sem_value_h);
    sem_getvalue(&O_sem, &sem_value_o);
    pthread_mutex_lock(&print_mutex);
    if(sem_value_h == 0 && sem_value_o == 0) {
        pode_imprimir = TRUE;
    } else {
        pode_imprimir = FALSE;
    }
    
    if(pode_imprimir == TRUE) {
        printf("\n---------------------------------------------");
        printf("\nH20 Formada! (atravessaram a barreira)\n");
        printf("Estoque H: %d\n", H);
        printf("Estoque O: %d\n", O);
        printf("---------------------------------------------\n\n");
        fflush(stdout);
        if (moleculas_lidas == NUM_MOLECULAS) {
            print_stock_and_finish_program();
        }
    }
    pthread_mutex_unlock(&print_mutex);
}

int main(int argc, char** argv) {
    inicia_variaveis();
    inicia_threads();
    return 0;
}