/*                                     */
/*             U r L I S P             */
/*                                     */
/* This software is released under the */
/*            MIT License.             */
/*                                     */
/*                      (C) 2025 Tsugu */
/*                                     */

#define CELLS_SIZE 0x8000   /* Total number of cells */
#define STACK_SIZE 0x4000   /* Size of the stack for Gabage Collection */
#define TEXTBUF_SIZE 100    /* Input text buffer */
#define SYMBOLTABLE_SIZE 64 /* Size of the hash table for symbols */
#define tag(x) (tags[x])    /* Tag to store ID */
#define car(x) (cells[x].pair.car).index
#define cdr(x) (cells[x].pair.cdr).index
#define cas(x) (cells[x].pair.car).string
#define cds(x) (cells[x].pair.cdr).string
#define p_f(x) (cells[x].p_f)                 /* Function pointer */
#define is(x, y) (abs(tag(x)) == (y) ? 1 : 0) /* Is the tag value of x y ? */
/* Macro for global escape in case of error */
#define ec \
  if (err) \
  return (0)

enum ID
{
  NIL,
  SYMBOL, /* However, it does not include NIL. */
  CELL,
};

enum name
{
  Nil,
  T,
  Lambda,
  Label,
  Quote,
  Atom,
  Eq,
  Car,
  Cdr,
  Cons,
  Rplaca,
  Rplacd,
  Cond,
  Eval,
  Apply,
  Error,
  Gc,
  ImportEnv,
  ExportEnv,
  Def,
  Num,
  Len,
  Quit,
  Cls,
  Num1,
  Num2,
  Last
};

enum ERROR_NUM
{
  off,
  on,
  print_no_more,
  eof
};

typedef unsigned int Index;

typedef union WORD
{
  Index index;
  char string[sizeof(Index)];
} Word;

typedef union CELL
{
  Index (*p_f)(Index, Index);
  struct PAIR
  {
    Word car;
    Word cdr;
  } pair;
} Cell;

#ifndef MAIN
extern char textbuf[TEXTBUF_SIZE];
extern char namebuf[TEXTBUF_SIZE];
extern char *tags;
extern Cell *cells;
extern Index stack[STACK_SIZE];
extern char *txtp;
extern Index freecells, toplevel;
extern Index symbol_table[SYMBOLTABLE_SIZE];
extern int err;
extern char *message;
extern FILE *ifp;
extern int sp;            /* The stack pointer for GC */
extern Index environment; /* The Environment list */
extern int no_input_after_GC;
#endif

/* From "IO.C" */
Index error(char *);
Index gc_getFreeCell();
Index gc_strToName(char *);
void nameToStr(Index, char *);
Index gc_makeSymbol(char *);
Index addSymbol(int, Index);
int hash(char *);
Index findSymbol(int, char *);
Index gc_readS(Index);
void printS(Index);
/* From "GC.C" */
Index push(Index);
Index pop();
void mark_and_sweep();
/* From "EVAL.C" */
Index eval(Index, Index);
Index apply(Index, Index, Index);
