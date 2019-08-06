#include "Condition.h"
#include "time.h"

namespace hhl{

    int Condition::waitForSeconds(int s){
        struct timespec abstime;
        abstime.tv_sec = s;

        MutexLock::UnassignGuard ug(mutex_);
        return pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
    }
}