/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_NUM, TK_HEX, TK_REG, 
  TK_PLUS, TK_SUB, TK_MTP, TK_DID, TK_LB, TK_RB, 
  TK_EQ, TK_NEQ, TK_AND,
  TK_NEG, TK_PTR,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"^[1-9][0-9]*|0", TK_NUM},         // decimal nums (neg & pos & zero)
  {"0[xX][0-9]+", TK_HEX},            // hexadecimal-number
  {"\\$[a-z]+", TK_REG},              // reg_name
  {" +", TK_NOTYPE},                  // spaces
  {"\\+", TK_PLUS},                   // plus
  {"==", TK_EQ},                      // equal
  {"-", TK_SUB},                      // sub
  {"\\*", TK_MTP},                    // multiply
  {"/", TK_DID},                      // divide
  {"\\(", TK_LB},                     // left bracket
  {"\\)", TK_RB},                     // right bracket
  {"!=", TK_NEQ},                     // '!='
  {"&&", TK_AND},                     // '&&'
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

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

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

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
        Token new_token;
        switch (rules[i].token_type) {
          case TK_NOTYPE : 
            break;
          case TK_PLUS :
            new_token.type = TK_PLUS;
            break;
          case TK_SUB :
            new_token.type = TK_SUB;
            break;
          case TK_NEG :
            new_token.type = TK_NEG;
            break;
          case TK_MTP :
            new_token.type = TK_MTP;
            break;
          case TK_PTR :
            new_token.type = TK_PTR;
            break;
          case TK_DID :
            new_token.type = TK_DID;
            break;
          case TK_LB :
            new_token.type = TK_LB;
            break;
          case TK_RB :
            new_token.type = TK_RB;
            break;
          case TK_EQ :
            new_token.type = TK_EQ;
            break;
          case TK_NEQ :
            new_token.type = TK_NEQ;
            break;
          case TK_AND :
            new_token.type = TK_AND;
            break;
          case TK_NUM :
            new_token.type = TK_NUM;
            for (int i = 0; i < substr_len; i++) {
              new_token.str[i] = substr_start[i];
            }
            new_token.str[substr_len] = '\0';
            break;
          case TK_HEX :
            new_token.type = TK_HEX;
            for (int i = 0; i < substr_len; i++) {
              new_token.str[i] = substr_start[i];
            }
            new_token.str[substr_len] = '\0';
            break;
          default: 
            printf("Unknow token type!\n");
            return false;
        }
        tokens[nr_token++] = new_token;
        break;
      }
    }
    for (int i = 0; i < nr_token; i++) {
      if (tokens[i].type == TK_SUB) {
        if (i == 0) {
          tokens[i].type = TK_NEG;
          continue;
        }
        if (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HEX
            && tokens[i - 1].type != TK_LB && tokens[i - 1].type != TK_REG) {
              tokens[i].type = TK_NEG;
            }
        continue;
      }
      if (tokens[i].type == TK_MTP) {
        if (i == 0) {
          tokens[i].type = TK_PTR;
          continue;
        }
        if (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HEX
            && tokens[i - 1].type != TK_LB && tokens[i - 1].type != TK_REG) {
              tokens[i].type = TK_PTR;
            }
        continue;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

uint64_t eval(int p, int q, bool *valid);

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  word_t expr_value;
  bool valid = true;
  expr_value = eval(0, nr_token - 1, &valid);
  if (!valid) {
    *success = false;
    return 0;
  }
  return expr_value;
}


typedef struct{
	char data[100];
	int top;
}SqStack;

void InitStack(SqStack *S)
{
  S->top=-1;
}

void push(SqStack *S, char c) {
  if (S->top == 99) {
    panic("Expression is too long!");
  }
  S->top += 1;
  S->data[S->top] = c;
}

void pop(SqStack *S) {
  if (S->top == -1) {
    panic("Stack is empty,can't pop!");
  }
  S->top -= 1;
}

char get_element(SqStack *S) {
  if (S->top == -1) {
    panic("Stack is empty,can't get top element!");
  }
  return S->data[S->top];
}

/*
  0 means worng expression
  1 means "(...)" expression
  2 means not 1 but true expression
*/
int check_parentheses(int p, int q) {
  SqStack st;
  InitStack(&st);
  bool flag = true;                   // stack never empty until the last token
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == TK_LB) {
      push(&st, '(');
    } else if (tokens[i].type == TK_RB) {
      if (st.top == -1 || get_element(&st) != '(') {
        return 0;
      } else {
        pop(&st);
        if (i != q && st.top == -1) {
          flag = false;
        }
      }
    }
  }
  if (st.top != -1) {
    return 0;
  }
  if (tokens[p].type == TK_LB && tokens[q].type == TK_RB && flag) {
    return 1;
  }
  return 2;
}

int find_master(int p, int q) {
  int res = -1;

  /* 
  level:    
  '&&'       ----   0
  '==' '!='  ----   1
  '+'  '-'   ----   2
  '*'  '/'   ----   3
  '-'  '*'   ----   4 
  */
  int level = 4;                  
  int flag = 0;                   // for judging whether in '( ... )' or not
  for (int i = p; i <= q; i++) {
     if (flag >= 1) {
      if (tokens[i].type == TK_RB) {
        flag--;
      }
      continue;
     }
     if (tokens[i].type == TK_LB) {
      flag++;
      continue;
     }
     if (tokens[i].type == TK_AND) {
      if (level >= 0) {
        res = i;
        level = 0;
        continue;
      }
     } else if (tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ) {
      if (level >= 1) {
        res = i;
        level = 1;
        continue;
      }
     }else if (tokens[i].type == TK_PLUS || tokens[i].type == TK_SUB) {
      if (level >= 2) {
        res = i;
        level = 2;
        continue;
      }
     }else if (tokens[i].type == TK_MTP || tokens[i].type == TK_DID) {
      if (level >= 3) {
        res = i;
        level = 3;
        continue;
      }
     }else if (tokens[i].type == TK_PTR || tokens[i].type == TK_NEG) {
      if (level >= 4) {
        res = i;
        level = 4;
        continue;
      }
     }
  }
  return res;
}

uint64_t eval(int p, int q, bool *valid) {
  bool left_valid = true, right_valid = true;
  if (p > q) {
    *valid = false;
    return 0;
  } else if (p == q) {                        // single token
    if (tokens[p].type == TK_NUM) {
        return atoi((char *)tokens[p].str);
    } else if (tokens[p].type == TK_HEX) {
        char **ptr = NULL;
        return (word_t)strtol((char *)tokens[p].str, ptr, 16);
    } else {                                  // if singal token not a num
      *valid = false;
      return 0;
    }
  } else if (check_parentheses(p, q) == 1) {  // The expression is surrounded by a matched pair of parentheses.
    return eval(p+1, q-1, valid);
  } else if (check_parentheses(p, q) == 0) {  // parentheses  not match
    *valid = false;
    return 0;
  } else {
    int op = find_master(p, q);
    assert(op >= p);
    assert(op <= q);
    if (op == p) {
      if (tokens[op].type == TK_NEG) {
        word_t res = -1 * eval(p+1, q, &left_valid);
        if (left_valid) {
          return res;
        }
        *valid = false;
        return 0;
      }
      if (tokens[op].type == TK_PTR) {
        word_t res =  *((uint32_t*)eval(p+1, q, &left_valid));
        if (left_valid) {
          return res;
        }
        *valid = false;
        return 0;
      }
    }
    uint32_t val1 = eval(p, op-1, &left_valid);
    uint32_t val2 = eval(op+1, q, &right_valid);
    if (!left_valid && !right_valid) {
      *valid = false;
      return 0;
    } else if (left_valid && !right_valid) {
      return val1;
    } else if (!left_valid && right_valid) {
      return val2;
    } else {
      switch (tokens[op].type)
      {
      case TK_PLUS: return val1 + val2;
      case TK_SUB: return val1 - val2;
      case TK_MTP: return val1 * val2;
      case TK_DID: return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case TK_NEG: panic("Check your '-'");
      case TK_PTR: panic("check your '*'");
      default:
        assert(0);
      }
    }
  }
}
