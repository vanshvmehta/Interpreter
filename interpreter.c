#include "F3.h"
#include "parsefr.h"
#include <setjmp.h>

struct FRAST copiedAst;             // store a copy of the original expr (for freeing purposes)
struct FRAST ast;                   // store the expr (a global parameter)
struct Cont cont;                   // store the continuations (a global parameter)

struct DealArg *freeList = NULL;    // store the copied arguments used for substitution 
                                    // (for freeing purposes)

void trampoline(void *(*f)()) {
  while (f) {
    void *g = f();
    f = (void *(*)())g;
  }
}

// copies the entire struct into the heap memory
// returns a new heap allocated copied struct of the "exp"
struct FRAST copyArg(struct FRAST exp){
    enum FRASTType type = exp.type;

    struct FRAST result;
    result.type = type;

    if (type == NUMBER) {
        result.n = malloc(sizeof(struct Num));
        result.n->val = exp.n->val;
        return result;
    }
    else if (type == VAR) {
        result.v = malloc(sizeof(struct Var));
        strcpy(result.v->name, exp.v->name);
        return result;
    }
    else if (type == BIN) {
        result.b = malloc(sizeof(struct Bin));
        result.b->arg1 = copyArg(exp.b->arg1);
        result.b->arg2 = copyArg(exp.b->arg2);
        result.b->op = exp.b->op;
        return result;
    }
    else if (type == APP) {
        result.a = malloc(sizeof(struct App));
        result.a->fn = copyArg(exp.a->fn);
        result.a->arg = copyArg(exp.a->arg);
        return result;
    }
    else if (type == FUN) {
        result.f = malloc(sizeof(struct Fun));
        result.f->body = copyArg(exp.f->body);
        strcpy(result.f->var, exp.f->var);
        return result;
    }
}

// subsitutes all the instances of varName with a new heap allocated copy of the arg
// returns the subsituted expr
struct FRAST substVarWithArg(struct FRAST funBody, char varName[MAXLENGTH], struct FRAST arg, int x){
    enum FRASTType bodyType = funBody.type;

    if (bodyType == NUMBER) return funBody;

    else if (bodyType == VAR) {
        struct Var *var = funBody.v;
        if (!strcmp(var->name, varName)) {
            struct FRAST result = copyArg(arg);
            freeFR(funBody, 1);

            if (x) {    // to not add the result into freeList (for freeing purposes)
                struct DealArg *tmp = malloc(sizeof(struct DealArg));
                tmp->ast = result;
                tmp->next = freeList;
                freeList = tmp;
            }

            return result;
        } else {
            return funBody;
        }
    }

    else if (bodyType == BIN) {
        struct Bin *bin = funBody.b;
        bin->arg1 = substVarWithArg(bin->arg1, varName, arg, 0);
        bin->arg2 = substVarWithArg(bin->arg2, varName, arg, 0);
        return funBody;
    }

    else if (bodyType == FUN) {
        struct Fun *func = funBody.f;
        func->body = substVarWithArg(func->body, varName, arg, x);
        return funBody;
    }

    else if (bodyType == APP) {
        struct App *app = funBody.a;
        app->arg = substVarWithArg(app->arg, varName, arg, x);
        app->fn = substVarWithArg(app->fn, app->fn.f->var, app->arg, 0);
        app->fn = substVarWithArg(app->fn, varName, arg, x);
        return funBody;
    }
}

void *applyCont();

// computes the result of applying the binary operator on integers num1 and num2
int opTrans(char op, int num1, int num2) {
    if (op == '+') return num1 + num2;
    else if (op == '-') return num1 - num2;
    else if (op == '*') return num1 * num2;
    else if (op == '/') return num1 / num2;
}

// interprets the global expr ast with the global continuation cont
void *interp() {
    if (ast.type == BIN) {
        struct Cont tmpCont;                        // create a temporary continuation
        tmpCont.type = BINL;                        // initialize it to BINL type

        tmpCont.bl = malloc(sizeof(struct BinL));

        tmpCont.bl->op = ast.b->op;

        tmpCont.bl->ast = ast.b->arg2;
        tmpCont.bl->cont = cont;

        cont = tmpCont;                             // update the global continuation to tmpCont

        ast = ast.b->arg1;
        return interp;                              // call interp
    }

    else if (ast.type == APP) {
        struct Cont tmpCont;                        // create a temporary continuation
        tmpCont.type = APPL;                        // initialize it to APPL type

        tmpCont.al = malloc(sizeof(struct AppL));

        tmpCont.al->ast = ast.a->arg;
        tmpCont.al->cont = cont;

        cont = tmpCont;                             // update the global continuation to tmpCont

        ast = ast.a->fn;
        return interp;                              // call interp
    }

    else {  // if ast.type == NUMBER or ast.type == FUN
        return applyCont;                           // call applyCont
    }
}

void *applyCont() {
    if (cont.type == BINL) {
        struct Cont tmpCont;                        // create a temporary continuation
        tmpCont.type = BINR;                        // initialize it to BINR type

        tmpCont.br = malloc(sizeof(struct BinR));

        tmpCont.br->op = cont.bl->op;
        tmpCont.br->val = ast;
        tmpCont.br->cont = cont.bl->cont;

        ast = cont.bl->ast;

        free(cont.bl);
        cont = tmpCont;                             // update the global continuation to tmpCont

        return interp;                              // call interp
    }

    else if (cont.type == BINR) {
        char op = cont.br->op;

        ast.n->val = opTrans(op, cont.br->val.n->val, ast.n->val);  // evaluate the BIN expr

        struct Cont tmpCont = cont.br->cont;
        free(cont.br);                              // free the BINR continuation from heap memory
        cont = tmpCont;

        return applyCont;                           // call applyCont
    }

    else if (cont.type == APPL) {
        struct Cont tmpCont;                        // create a temporary continuation
        tmpCont.type = APPR;                        // initialize it to APPR type

        tmpCont.ar = malloc(sizeof(struct AppR));

        tmpCont.ar->val = ast;
        tmpCont.ar->cont = cont.al->cont;

        ast = cont.al->ast;

        free(cont.al);                              // free the APPL continuation from heap memory
        cont = tmpCont;

        return interp;                              // call interp
    }

    else if (cont.type == APPR) {
        // substitute the function body
        ast = substVarWithArg(cont.ar->val.f->body, cont.ar->val.f->var, ast, 1);

        struct Cont tmpCont = cont.ar->cont;
        free(cont.ar);                              // free the APPR continuation from heap memoery
        cont = tmpCont;
        return interp;                              // call interp
    }

    else return NULL;                               // if cont.type == MT, return NULL
}

// frees the global linked list of all copied arguments "freeList" from heap memory
void freeFreeList(){
    for (struct DealArg *cur = freeList; cur;) {
        struct DealArg *tmp = cur;
        freeFR(tmp->ast, 1);
        cur = cur->next;
        free(tmp);
    }
}

// initializes the interpretation of the given expr
void init(struct FRAST expr){
    copiedAst = expr;       // initializes the copiedAse (global variable)
    ast = expr;             // initializes the global parameter "ast"
    cont.type = MT;         // initializes the global parameter "cont"

    trampoline(interp);     // begins the trampolining process

    printFR(ast);           // prints the interpretted ast
    printf("\n");
    freeFreeList();         // free the freeList
    freeFR(copiedAst, 0);   // free the copiedAst (0 is for freeing purposes)
}

