#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_test(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "   Continue the execution of the program", cmd_c },
  { "q", "   Exit NEMU", cmd_q },
  { "si","  si [N] steps (default 1)",cmd_si},
  {"info","info SUBCMD\n\t r: Print state of Register\n\t w: Print info of WatchPoint",cmd_info},
  {"x","   x N EXPR\n\t Get the result of EXPR; Begin with result, Output n 4 Bytes",cmd_x},
  {"test","   test EXPR\n\t Get the answer of EXPR\n",cmd_test}
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int times;
  if(arg == NULL)
    times=1;
  else
    times=atoi(arg);
  cpu_exec(times);
  return 0;
}
static int cmd_info(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  if(strcmp(arg,"r") == 0){
    isa_reg_display();
    return 0;
  }
  if(strcmp(arg,"w") == 0){
    printf("w\n");
    return 0;
  }
  printf("Unknown command '%s'\n", arg);
  return 0;
}

static int cmd_x(char *args) {
  //like x 10 0x80000000
  //could see x n add_begin
  /* extract the first and second argument */
  //i'm lazy,so the user must keep args right
  char *arg = strtok(NULL, " ");
  uint32_t n=atoi(arg);

  arg = strtok(NULL, " ");
  char *ptr;
  int address=strtoul(arg,&ptr,16);
  for(int i=0;i<n;i++){
    printf("RAM[0x%x]: 0x%02x\n",address,vaddr_read(address,1));
    //printf("RAM[0x%x]",address);
    address+=1;
  }
  //printf("Unknown command '%s'\n", arg);
  return 0;
}

static int cmd_test(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, "\0");
  bool success;
  int ans=expr(arg,&success);
  if(success)
    printf("ans is %u 0x%x\n",ans,ans);
  else 
    printf("\tERROR!\n");
  return 0;
}
void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
