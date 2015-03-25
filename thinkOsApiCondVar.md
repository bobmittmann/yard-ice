int thinkos\_cond\_alloc(void);

int thinkos\_cond\_free(int cond);

int thinkos\_cond\_wait(int cond, int mutex);

int thinkos\_cond\_timedwait(int cond, int mutex, unsigned int ms);

int thinkos\_cond\_signal(int cond);

int thinkos\_cond\_broadcast(int cond);