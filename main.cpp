#include <iostream>
#include <pthread.h>
#include <typeinfo>
#include <cstdio>
#include <unistd.h>

using namespace std;

struct sync {
	char id;
	union ptr {
		pthread_mutex_t* mutex;
		pthread_spinlock_t* sp;
		pthread_rwlock_t* rwlock;
	} un_ptr;
};

void* do_nothing(void* ptr)
{	
	// cout << "" << static_cast<char*>(ptr) << endl;
	struct sync* st_sync = static_cast<struct sync*>(ptr);

	switch (st_sync->id) {
		case '0':
		{
			pthread_mutex_lock(st_sync->un_ptr.mutex);
			cout << "mutex" << endl;
			pthread_mutex_unlock(st_sync->un_ptr.mutex);
			break;
		}
		case '1': 
		{
			pthread_spin_lock(st_sync->un_ptr.sp);
			cout << "spin" << endl;
			pthread_spin_unlock(st_sync->un_ptr.sp);
			break;
		}
		case '2':
		{
			pthread_rwlock_rdlock(st_sync->un_ptr.rwlock);
			cout << "rwlock rd" << endl;
			pthread_rwlock_unlock(st_sync->un_ptr.rwlock);
			break;
		}
		case '3':
		{
			pthread_rwlock_wrlock(st_sync->un_ptr.rwlock);
			cout << "rwlock wr" << endl;
			pthread_rwlock_unlock(st_sync->un_ptr.rwlock);
			break;
		}
	}

	//return NULL;
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	FILE* fd = fopen("/home/stalker/main.pid", "w+");
	pthread_t threadM;
	pthread_t threadS;
	pthread_t threadRWr;
	pthread_t threadRWw;
	// char* mesM = "Was working threadM.\0";
	// char* mesS = "Was working threadS.\0";
	// char* mesRWr = "Was working threadRWr.\0";
	// char* mesRWw = "Was working threadRWw.\0";
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_spinlock_t sp;
	pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
	pthread_rwlock_t rwlockW = PTHREAD_RWLOCK_INITIALIZER;
	struct sync syncM, syncSp, syncRW, syncRWw;
	int ret[8];
	int i = 0;

	if( fd != NULL )
	{
		pid_t pid = getpid();

		cout << "pid = " << pid << endl;
		fprintf(fd, "%u\n", pid);
		fclose(fd);
	}
	// cout << "mutex ptr has type: " << typeid(&mutex).name() << endl;
	
	// pthread_create(&threadM, NULL, do_nothing, (void*)mesM);
	// pthread_create(&threadS, NULL, do_nothing, (void*)mesS);
	// pthread_create(&threadRWr, NULL, do_nothing, (void*)mesRWr);
	// pthread_create(&threadRWw, NULL, do_nothing, (void*)mesRWw);
	syncM.id = '0';
	syncM.un_ptr.mutex = &mutex;
	pthread_create(&threadM, NULL, do_nothing, (void*)&syncM);
	syncSp.id = '1';
	syncSp.un_ptr.sp = &sp;
	pthread_create(&threadS, NULL, do_nothing, (void*)&syncSp);
	syncRW.id = '2';
	syncRW.un_ptr.rwlock = &rwlock;
	pthread_create(&threadRWr, NULL, do_nothing, (void*)&syncRW);
	syncRWw.id = '3';
	syncRWw.un_ptr.rwlock = &rwlockW;
	pthread_create(&threadRWw, NULL, do_nothing, (void*)&syncRWw);

	
	ret[1] = pthread_mutex_lock(&mutex);
	if( pthread_spin_init(&sp, PTHREAD_PROCESS_SHARED) == 0 )
		ret[0] = pthread_spin_lock(&sp);
	ret[2] = pthread_rwlock_rdlock(&rwlock);
	ret[3] = pthread_rwlock_wrlock(&rwlockW);

	ret[6] = pthread_mutex_unlock(&mutex);
	ret[7] = pthread_spin_unlock(&sp);
	ret[5] = pthread_rwlock_unlock(&rwlock);
	ret[4] = pthread_rwlock_unlock(&rwlockW);

	pthread_join(threadM, NULL);
	pthread_join(threadS, NULL);
	pthread_join(threadRWr, NULL);
	pthread_join(threadRWw, NULL);
		
	//cout << "Main is working, pid = " << pid << endl;
	// getchar();
	// for( i = 0; i < 8; ++i)
	// 	cout << "i = " << ret[i] << " , " << endl;
	// sleep(3);
	// if( pthread_mutex_unlock(&mutex) == 0 )
		pthread_mutex_destroy(&mutex);
	// if( pthread_spin_unlock(&sp) == 0 )
		pthread_spin_destroy(&sp);
	// if( pthread_rwlock_unlock(&rwlock) == 0 )
		pthread_rwlock_destroy(&rwlock);
	// if( pthread_rwlock_unlock(&rwlockW) == 0 )
		pthread_rwlock_destroy(&rwlockW);

	return 0;
}
