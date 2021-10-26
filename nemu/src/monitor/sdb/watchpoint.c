#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[40];
  uint32_t valve;
  /* TODO: Add more members if necessary */

} WP;

void insert_wp(char *e);

WP* new_wp();
void free_wp(int n);
void print_worker();
bool debug_hook();


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

void free_wp(int n){
  WP* p = head;
  //printf("%d\n",wp->NO); 
  if(p->NO== n){
  	head = p->next;
  	p->next = free_;
  	free_=p;
  	return ;
  }
  while(p->next != NULL){
  	if(p->next->NO == n)break;
  	p=p->next;
  }
  if (p->next == NULL ){
    printf("Can't find NO.%d\n",n);
    return ;
  }
  WP *q = p->next;
  p->next =p->next->next;
  q->next = free_;
  free_ = q;
}


void print_worker(){
  WP* p = head;
  if (p == NULL)printf("NO Worker!\n");
  while (p != NULL){
    printf("NO: %d\tEXPR: %s\tVALVE: 0x%08x(%u) \n",p->NO,p->expr,p->valve,p->valve);
    p = p->next;
  }
}

void insert_wp(char *e){
  bool success;
  uint32_t valve = expr(e,&success);
  if (!success){
    printf("Please Check expr !\n");
    return ;
  }
  WP* p = new_wp();
  strcpy(p->expr, e );
  p-> valve = valve ;
}

bool debug_hook(){
  bool state = false ;
  bool success;
  for(WP *p = head ; p != NULL ;p = p -> next ){
    int new_valve = expr(p->expr,&success);
    if (new_valve != p -> valve ){
      printf("%s  has changed!\n",p -> expr);
      printf("  0x%08x -> 0x%08x\n",p->valve,new_valve);
      state = true;
    }
  }
  return state ;
}
/* TODO: Implement the functionality of watchpoint */

