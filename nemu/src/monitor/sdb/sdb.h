#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);
void insert_wp(char *expr);
void free_wp(int n);
void print_worker();
bool debug_hook();
#endif
