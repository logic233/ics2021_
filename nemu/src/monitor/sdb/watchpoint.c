#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char str[40];
  /* TODO: Add more members if necessary */

} WP;

WP* new_wp();
void free_wp(WP *wp);

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp(){
  if(free_ == NULL)assert(0);
  else{
    WP *p = free_ ;
    free_ = free_ -> next;
    p -> next = head;
    head = p;
    return p; 
  }
}
void free_wp(WP *wp){
  WP* p = head;
  //printf("%d\n",wp->NO); 
  if(p->NO==wp->NO){
  	head = p->next;
  	wp->next = free_;
  	free_=wp;
  	return ;
  }
  while(p->next != NULL){
  	if(p->next->NO == wp->NO)break;
  	p=p->next;
  }
  p->next =p->next->next;
  wp->next = free_;
  free_ = wp;
}


/* TODO: Implement the functionality of watchpoint */

