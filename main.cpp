	// C++ Dining Phylosophers Problem
	#include <stdio.h>
	#include <pthread.h>
	#include <semaphore.h>

	// Set TEST_COUNT to a negative number to run forever, but then you have to stop the program manually
	// Some big numbers like 10000 may be enough to check if there are any deadlocks
	const int TEST_COUNT = 10000;

	enum State{
		THINKING = 0,
		EATING = 1,
		HUNGRY = 2
	};

	struct Philosopher {
		int id;
		State state;
		sem_t* chopticks;
		pthread_cond_t* conditions;
		pthread_mutex_t* mutex;
		Philosopher* all;

		void pickup(){
			pthread_mutex_lock(mutex);
			state = HUNGRY;
			printf("p%d is hungry\n", id);
			fflush(stdout);
			test();
			while (state != EATING){
				pthread_cond_wait(conditions + id, mutex);
			}
			printf("p%d is eating\n", id);
			fflush(stdout);
			pthread_mutex_unlock(mutex);
		}

		void putdown(){
			pthread_mutex_lock(mutex);
			state = THINKING;
			printf("p%d is thinking\n", id);
			fflush(stdout);
			all[(id+1) % 5].test();
			all[(id+4) % 5].test();
			pthread_mutex_unlock(mutex);
		}

		void test(){
			if (all[(id + 4) % 5].state != EATING && state == HUNGRY && all[(id + 1) % 5].state != EATING){
				state = EATING;
				pthread_cond_signal(conditions + id);
			}
		}
	};

	void* life(void* arg){
		Philosopher* ph = (Philosopher*)arg;
		printf("p%d started to think\n", ph->id);
		fflush(stdout);
		int count_down = TEST_COUNT < 0 ? 1 : TEST_COUNT;
		while (count_down){
			ph->pickup();
			ph->putdown();
			count_down -= TEST_COUNT < 0 ? 0 : 1;
		}
		return NULL;
	}

	int main(){
		sem_t chopsticks[5];
		pthread_cond_t conditions[5];
		pthread_mutex_t mutex;


		for (int i = 0; i < 5; i++){
			sem_init(&chopsticks[i], 0, 1);
			pthread_cond_init(&conditions[i], NULL);
		}
		pthread_mutex_init(&mutex, NULL);

		pthread_t lives[5];
		Philosopher philosophers[5];
		for (int i = 0; i < 5; i++){
			philosophers[i] = Philosopher {
				.id = i,
				.state = THINKING,
				.chopticks = chopsticks,
				.conditions = conditions,
				.mutex = &mutex,
				.all = philosophers
			};
		}
		for (int i = 0; i < 5; i++){
			pthread_create(&lives[i], NULL, &life, &philosophers[i]);
		}

		for (int i =0; i < 5; i++){
			pthread_join(lives[i], NULL);
		}

		printf("All threads are finished\n");
		
		exit(0);
	}