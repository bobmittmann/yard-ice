int thinkos\_sem\_alloc(unsigned int value);

int thinkos\_sem\_free(int sem);

int thinkos\_sem\_init(int sem, unsigned int value);

int thinkos\_sem\_wait(int sem);

int thinkos\_sem\_timedwait(int sem, unsigned int ms);

int thinkos\_sem\_post(int sem);