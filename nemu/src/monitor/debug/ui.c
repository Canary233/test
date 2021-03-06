#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
void cpu_exec(uint64_t);
void list_watchpoint();
WP *new_wp(char*st);
void free_wp(int n);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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
static int cmd_p(char *args){
  bool ifis;
  int resu=expr(args,&ifis);
  printf("%d\n",resu);
  return 0;
}
static int cmd_si(char *args) {
  if(args==NULL)
      cpu_exec(1);
  else{
     char *temp;
     int much;
     temp=strtok(args," ");
     much=atoi(temp);
     cpu_exec(much);
  }
  return 0;
}
static int cmd_info(char *args){
  if(strcmp(args,"r")==0)
  {
       for(int index=0;index<8;index++)
       {
      	 printf("%s:\t%8x\t%d\n", regsl[index], cpu.gpr[index]._32,cpu.gpr[index]._32);
       }
       printf("eip:\t%8x\t%d\n", cpu.eip,cpu.eip);

  }
  else if(strcmp(args,"w")==0)
  {
      list_watchpoint();	
  }
  return 0;
}
static int cmd_x(char *args){
  int len;
  char str[32];
  vaddr_t addr;
  vaddr_t temp;
  sscanf(args,"%d %s",&len,str);
  bool find=0;
  if(str[1]=='x')
  {
      find=1;
      addr=0;
      for(int j=2;j<strlen(str);j++)
      {
            if(str[j]>='a')
                 addr=addr*16+str[j]-'a'+10;
            else if(str[j]<'A')
                 addr=addr*16+str[j]-'0';
            else
                 addr=addr*16+str[j]-'A'+10;
       }
  }
  else if(str[1]!='x')
  {
     if(strcmp(str,"$eip")==0)
      {
             addr=cpu.eip;
	     find=1;
       }
     else
     {
            for(int index=0;index<8;index++)
            {
		 char strnew[32];
		 for(int in=1;in<4;in++)
		 {
			 strnew[in-1]=str[in];
		 }
		 strnew[3]='\0';
                 if(strcmp(regsl[index],strnew)==0)
		 {
                       addr=cpu.gpr[index]._32;
		       find=1;
		 }
            }
     }
  }
  if(find==0)
  {
	  printf("???????????????????????????\n");
	  return 0;
  }
  printf("Address    Dword block...Byte sequence\n");
  for(int i=0;i<len;i++)
  {
      temp=vaddr_read(addr,4);
      printf( "0x%08x  0x%08x...",addr,temp);
      for(int j=0;j<4;j++)
      {
           printf("%02x ",temp%256);
	   temp=temp/256;
      }
      printf("\n");
      addr+=4;
  }
  return 0;
}
static int cmd_q(char *args) {
  return -1;
}
static int cmd_w(char *args)
{
   WP *p=new_wp(args);
   printf("Set watchpoint #%d\n",p->NO);
   printf("expr      = %s\nold value =0x%08x\n",p->expr,p->old_val); 
   return 0;
}
static int cmd_d(char *args)
{
   int num;
   sscanf(args,"%d",&num);
   free_wp(num);
   return 0;
}
static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si" ,"Single step",cmd_si },
  {"info","Print register",cmd_info },
  {"x","Scan memory",cmd_x},
  {"p","Calculate",cmd_p},
  {"w","Set New Watchpoint",cmd_w},
  {"d","Delete Watchpoint",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

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

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
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

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
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
