int thinkos\_ev\_alloc(void);

int thinkos\_ev\_free(int ev);

int thinkos\_ev\_wait(int ev);

int thinkos\_ev\_timedwait(int ev, unsigned int ms);

int thinkos\_ev\_raise(int ev);