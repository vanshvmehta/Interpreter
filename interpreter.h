#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsefr.h"

enum CONTType {MT = 0, APPR, APPL, BINR, BINL};

struct AppR;
struct AppL;
struct BinL;
struct BinR;

struct Cont {
  enum CONTType type;
  union {
    struct AppR *ar;
    struct AppL *al;
    struct BinR *br;
    struct BinL *bl;
  };
};

struct AppL {
  struct FRAST ast;
  struct Cont cont;
};

struct AppR {
  struct FRAST val;
  struct Cont cont;
};

struct BinL {
  char op;
  struct FRAST ast;
  struct Cont cont;
};

struct BinR {
  char op;
  struct FRAST val;
  struct Cont cont;
};

// linked list to deal with Arguments that are substituted
struct DealArg {
  struct FRAST ast;
  struct DealArg *next;
};

void init(struct FRAST expr);
