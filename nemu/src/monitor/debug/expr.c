#include "nemu.h"
#include<string.h>
#include<stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ=257,TK_UEQ=258,TK_AND=259,TK_OR=260,TK_NOT=261,TK_HEX=262,TK_DEC=263,TK_REG=264

  /* TODO: Add more token types */
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\{",'{'},          // left kuohao
  {"\\}",'}'},          //right kuohao
  {"-",'-'},            //jian
  {"\\*",'*'},          //multiply
  {"/",'/'},            //division
  {"!=",TK_UEQ},        //unequal
  {"&&",TK_AND},        //and
  {"\\|\\|",TK_OR},     //or
  {"!",TK_NOT},         //not
  {"0x[A-Fa-f0-9]{1,30}",TK_HEX}, //hex
  {"[0-9]{1,32}",TK_DEC}, //decimal
  {"$[eE]?(ax|dx|cx|bx|bp|si|di|sp)",TK_REG} //register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
         case 40:
	 {
              tokens[nr_token].type=40;
	      break;
	 }
         case 41:
	 {
              tokens[nr_token].type=41;
	      break;
	 }
         case 42:
	 {
              tokens[nr_token].type=42;
	      break;
	 }
         case 43:
	 {
              tokens[nr_token].type=43;
	      break;
	}
	  case 45:
         {
              tokens[nr_token].type=45;
              break;
         }
	  case 47:
         {
              tokens[nr_token].type=47;
              break;
         }
	 case 257:
         {
              tokens[nr_token].type=257;
              break;
         }
	  case 258:
         {
              tokens[nr_token].type=258;
              break;
         }
	 case 259:
         {
              tokens[nr_token].type=259;
              break;
         }
	case 260:
         {
              tokens[nr_token].type=260;
              break;
         }
	 case 261:
         {
              tokens[nr_token].type=261;
              break;
         }
	case 262:
	 {
	       tokens[nr_token].type=262;
	       strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
	       break;
	 }
	 case 263:
         {
               tokens[nr_token].type=263;
               strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
               break;
	}
	 case 264:
          {
               tokens[nr_token].type=264;
               strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
               break;
        }

          default:
	 {
              nr_token--;
	      break;	
        }
	nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  nr_token--;
  }
  return true;
}
bool check_parentheses(int p,int q)
{
	if(tokens[p].type!=40&&tokens[p].type!=41)
		return false;
	int num=0;
	for(int i=p;i<=q;i++)
	{
		if(tokens[i].type==40)
			num++;
		else if(tokens[i].type==41)
			num--;
		if(num==0&&i<q)
			return false;
	}
	if(num==0)
		return true;
	else
		return false;
}
int youxian(int n)
{
	if(n==259||n==260) //&&,||
		return 5;
	if(n==42||n==47)   //*,\/		
		return 2;
	if(n==43||n==45)   //+,-
		return 3;
	if(n==257||n==258)  //=,!=
		return 4;
	if(n==261)         //!
		return 1;
	else
		return 0;
}
int find_dominated_op(int p,int q)
{
	int num=0;
	int you=0;
	int pos=p;
	for(int i=p;i<=q;i++)
	{
		if(tokens[i].type==263||tokens[i].type==262||tokens[i].type==264)
			continue;
		if(tokens[i].type==40)
		{
			num++;
			i++;
			while(num!=0)
			{
				if(tokens[i].type==40)
					num++;
				else if(tokens[i].type==41)
					num--;
				i++;
			}
			if(i>q)
				break;
			i--;
		}
		else if(youxian(tokens[i].type)>=you)
		{
			you=youxian(tokens[i].type);
			pos=i;
		}
	}
	return pos;
}
uint32_t eval(int p,int q)
{
    if (p > q) {
	assert(0);	 
        /* Bad expression */
    }
    else if (p == q) {
	if(tokens[p].type==263)    
		return atoi(tokens[p].str);
	else if(tokens[p].type==262)
	{
		int sum=0;
		for(int j=2;j<strlen(tokens[p].str);j++)
		{
			sum=sum*16+tokens[p].str[j]-'0';
		}
		return sum;
	}
	else
	{
	    for(int index=0;index<8;index++)
      	    {
		 if(strcmp(regsl[index],tokens[p].str)==0)
      	               return cpu.gpr[index]._32;
       	    }
	    assert(0);
	}
    }
    else if (check_parentheses(p, q) == true) {
        return eval(p + 1, q - 1);
    }
    else {
        int op,op_type;
        op = find_dominated_op(p,q);
        op_type=tokens[op].type;
        if(op_type==261)
                return eval(op+1,q);
        uint32_t val1 = eval(p, op - 1);
        uint32_t val2 = eval(op + 1, q);
        switch (op_type) {

            case 43: return val1 + val2;
            case 45: return val1 - val2;
            case 42: return val1 * val2; 
            case 47: return val1 / val2;
	    case 257:return val1 == val2;
	    case 258:return val1 != val2;
	    case 259:return val1 && val2;
	    case 260:return val1 || val2;
            default: assert(0);
        }
    }
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return eval(0,nr_token-1);
}
