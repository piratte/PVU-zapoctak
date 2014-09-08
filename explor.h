#ifndef __EXPLOR_H
#define __EXPLOR_H

char *get_src();
int explor(char *dirname, bool top);
void *thr_run(void *x);
void add(char *name);

#endif /* __EXPLOR_H */