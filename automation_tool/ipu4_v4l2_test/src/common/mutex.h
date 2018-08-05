#ifndef  MUTEX_H
#define  MUTEX_H

#include <pthread.h>
#include <sys/time.h>
#include <stdint.h>

class Mutex {
    friend class Cond;

public:
    Mutex() {
        pthread_mutex_init(&mMutex, NULL);
    }
    virtual ~Mutex() {
        pthread_mutex_destroy(&mMutex);
    }

    void lock() {
        pthread_mutex_lock(&mMutex);
    }
    void unlock() {
        pthread_mutex_unlock(&mMutex);
    }

private:
    pthread_mutex_t mMutex;
};

class Cond {

public:
    Cond(){
        pthread_cond_init(&mCond, NULL);
    }
    ~Cond(){
        pthread_cond_destroy(&mCond);
    }

    int wait(Mutex &mutex) {
        return pthread_cond_wait(&mCond, &mutex.mMutex);
    }
    int timedwait(Mutex &mutex, uint32_t time_in_us) {
        struct timeval now;
        struct timespec abstime;

        gettimeofday(&now, NULL);
        now.tv_usec += time_in_us;
        abstime.tv_sec += now.tv_sec + now.tv_usec / 1000000;
        abstime.tv_nsec =(now.tv_usec % 1000000) * 1000;

        return pthread_cond_timedwait(&mCond, &mutex.mMutex, &abstime);
    }

    int signal() {
        return pthread_cond_signal(&mCond);
    }
    int broadcast() {
        return pthread_cond_broadcast(&mCond);
    }
private:
    pthread_cond_t mCond;
};

class SmartLock {
public:
    SmartLock(Mutex &mutex): mMutex(mutex) {
        mMutex.lock();
    }
    virtual ~SmartLock() {
        mMutex.unlock();
    }
private:
    Mutex &mMutex;
};
#endif // MUTEX_H