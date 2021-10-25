#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
#define REG_NUM 32

void isa_reg_display() {
  printf("pc: %x\n",cpu.pc);
  printf("---------------------HERE ARE 32 GPRS---------------------\n");
  for(int i=0;i<32;i++){
    printf("%3s: %8x  ",regs[i],cpu.gpr[i]._32);
    if(i%4==3)
      printf("\n");
  }
}


word_t isa_reg_str2val(const char *s, bool *success) {
  for(int i=0;i<REG_NUM;i++){
    if(strcmp(regs[i],s) == 0){
      return cpu.gpr[i]._32;
    }
  }
  if(strcmp("pc",s)==0)
    return cpu.pc;
  *success = false ;
  return 0;
}
