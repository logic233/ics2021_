#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int pos = 0;
#define NUM_LEN_MAX 3
#define EXPR_LEN_MAX 30
//int len; 
int gen_expr =0;
static void gen(char c){
  //if (pos>EXPR_LEN_MAX) return;
  
  buf[pos++]=c;
}


static void gen_num(){
  int len = rand()% (NUM_LEN_MAX-1)+1;
  for(int i=0;i<len;i++){
    int num_single = rand()%10;
    if (num_single == 0 && i == 0)num_single = rand()%9 +1 ;
    gen('0'+num_single);
  }
}
static void gen_rand_op(){
  int op = rand() % 4 ;
  switch(op){
    case 0:gen('+');break;
    case 1:gen('-');break;
    case 2:gen('*');break;
    case 3:gen('/');break;
  } 
}

static void gen_rand_expr() {
  int choose = rand()%3;
  gen_expr++;
  if(gen_expr > EXPR_LEN_MAX )choose = 0 ;
  if(gen_expr < 2)choose = 2 ;
  switch(choose){
    case 0: gen_num();break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

void init_gen(){
  pos = gen_expr = 0 ;
}



int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    // input buff from begin
    init_gen();
    gen_rand_expr();
    buf[pos]='\0';
    
    //printf("%s\n",buf);
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
