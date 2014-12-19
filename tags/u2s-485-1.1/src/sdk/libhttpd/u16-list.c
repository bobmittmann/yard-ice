#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

struct list {
	uint16_t cnt;
	uint16_t key[];
};

static int __key_lookup(struct list * lst, unsigned int key)
{
	int imin = 0;
	int imax = lst->cnt - 1;

	if (imax < 0)
		return -1;

	while (imin < imax) {
		int imid = (imin + imax) / 2;

		if (lst->key[imid] < key) 
			imin = imid + 1;
		else
			imax = imid;
	}

	if ((imax == imin) && (lst->key[imin] == key))
		return imin;

	return -1;
}


static void __key_delete(struct list * lst, unsigned int idx)
{
	int i;

	assert(idx < lst->cnt);

	lst->cnt--;

	for (i = idx; i < lst->cnt; ++i) {
		lst->key[i] = lst->key[i + 1];
	}
}

static int __key_next(struct list * lst, unsigned int key)
{
	int imin = 0;
	int imax = lst->cnt - 1;

	if (imax < 0)
		return -1;

	if (key >= lst->key[imax])
		return -1;

	while (imin < imax) {
		int imid = (imin + imax) / 2;

		if (lst->key[imid] < key) 
			imin = imid + 1;
		else
			imax = imid;
	}

	if (lst->key[imin] <= key)
		return imin + 1;

	return imin;
}

void u16_list_insert(struct list * lst, unsigned int key)
{
	int idx;
	int i;

	idx = __key_next(lst, key);

	if (idx < 0) {
		/* Append */
		lst->key[lst->cnt++] = key;
		return;
	}

	/* Make room for the new key. Move keys by one position */
	for (i = lst->cnt; i > idx; --i)
		lst->key[i] = lst->key[i - 1];
	lst->cnt++;

	lst->key[idx] = key;
}

bool u16_list_contains(struct list * lst, unsigned int key)
{
	return __key_lookup(lst, key) < 0 ? false : true;
}

int u16_list_indexof(struct list * lst, unsigned int key)
{
	return __key_lookup(lst, key);
}

void u16_list_remove(struct list * lst, unsigned int key)
{
	int idx;

	idx = __key_lookup(lst, key);

#if DEBUG
	fprintf(stderr, "%s: key=%d idx=%d.\n", __func__, key, idx);
	fflush(stderr);
#endif

	assert(idx >= 0);

	__key_delete(lst, idx);
}

void u16_list_subtract(struct list * from, struct list * lst)
{
	int i;

	/* remove dead nodes */
	for (i = 0; i < lst->cnt; ++i) {
		int key = lst->key[i];
		int idx;

		if ((idx = __key_lookup(from, key)) >= 0);
			__key_delete(from, idx);
	}
}

void u16_list_dump(FILE * f,  struct list * lst)
{
	int i;

	fprintf(f, "-");
	for (i = 0; i < lst->cnt; ++i) {
		fprintf(f, " %04x", lst->key[i]);
	}

	fprintf(f, "\n");
	fflush(f);
}

void u16_list_init(struct list * lst)
{
	lst->cnt = 0;
}

