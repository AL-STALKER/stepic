#include <iostream>
#include <pthread.h>
#include <typeinfo>
#include <cstdio>
#include <unistd.h>

using namespace std;

struct sync {
	char id;
	pthread_mutex_t* mutex;
	union ptr {
		pthread_cond_t* cond;
		pthread_barrier_t* bp;
	} un_ptr;
};

void* do_nothing(void* ptr)
{	
	// cout << "" << static_cast<char*>(ptr) << endl;
	struct sync* st_sync = static_cast<struct sync*>(ptr);

	switch (st_sync->id) {
		case '0':
		{
			pthread_mutex_lock(st_sync->mutex);
			cout << "cond" << endl;
			pthread_cond_signal(st_sync->un_ptr.cond);
			pthread_mutex_unlock(st_sync->mutex);
			break;
		}
		case '1': 
		{
			cout << "barrier" << endl;
			break;
		}
	}

	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	FILE* fd = fopen("/home/stalker/main.pid", "w+");
	pthread_t threadC;
	pthread_t threadB;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_barrier_t bp;
	struct sync sync_st;

	if( fd != NULL )
	{
		pid_t pid = getpid();

		cout << "pid = " << pid << endl;
		fprintf(fd, "%u\n", pid);
		fclose(fd);
	}
	pthread_mutex_lock(&mutex);
	sync_st.id = '0';
	sync_st.mutex = &mutex;
	sync_st.un_ptr.cond = &cond;
	pthread_create(&threadC, NULL, do_nothing, (void*)&sync_st);
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);

	if( pthread_barrier_init(&bp, NULL, 1) == 0 )
		pthread_barrier_wait(&bp);
	sync_st.id = '1';
	sync_st.mutex = NULL;
	sync_st.un_ptr.bp = &bp;
	pthread_create(&threadB, NULL, do_nothing, (void*)&sync_st);
	pthread_join(threadB, NULL);
	pthread_join(threadC, NULL);
		
	sleep(10);
	// if( pthread_mutex_unlock(&mutex) == 0 )
		pthread_mutex_destroy(&mutex);
	// if( pthread_spin_unlock(&sp) == 0 )
		pthread_cond_destroy(&cond);
	// if( pthread_rwlock_unlock(&rwlock) == 0 )
		pthread_barrier_destroy(&bp);
	
	return 0;
}
