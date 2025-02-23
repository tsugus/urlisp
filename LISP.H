/*                                     */
/*          p u r e  L I S P           */
/*                                     */
/* This software is released under the */
/*            MIT License.             */
/*                                     */
/*                 (C) 2024-2025 Tsugu */
/*                                     */

#define CELLS_SIZE 0x8000  /* セルの総数 */
#define TEXTBUF_SIZE 100    /* 入力テキスト用バッファ */
#define SYMBOLTABLE_SIZE 64 /* シンボルを割り振るハッシュテーブルのサイズ */
#define STACK_SIZE 0x1000   /* GC 用スタックのサイズ */
#define tag(x) (tags[x])    /* ID を格納するタグ */
#define car(x) (cells[x].pair.car).index
#define cdr(x) (cells[x].pair.cdr).index
#define cas(x) (cells[x].pair.car).string
#define cds(x) (cells[x].pair.cdr).string
#define p_f(x) (cells[x].p_f)                 /* 関数ポインタ */
#define is(x, y) (abs(tag(x)) == (y) ? 1 : 0) /* x のタグの値は y か ? */
/* エラー時の大局脱出用マクロ */
#define ec \
  if (err) \
  return (0)

enum ID
{
  /* アトム用 */
  NIL,
  SYMBOL, /* ただし NIL を含まない */
  /* セル用 */
  CELL,
  POINTER, /* C の関数ポインタ */
  /* アトムの cdr 用 */
  ARGsEVAL,    /* 関数の引数を評価する */
  ARGsNotEVAL, /* 関数の引数を評価しない */
};

enum name
{
  Nil,
  T,
  Lambda,
  Quote,
  Atom,
  Eq,
  Car,
  Cdr,
  Cons,
  Cond,
  Eval,
  Label,
  ImportEnv,
  ExportEnv,
  Def,
  Gc,
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
extern char namebuf2[TEXTBUF_SIZE];
extern char *tags;
extern Cell *cells;
extern Index stack[STACK_SIZE];
extern char *txtp;
extern Index freecells, toplevel;
extern Index symbol_table[SYMBOLTABLE_SIZE];
extern int err;
extern char *message;
extern FILE *ifp;
extern int sp;            /* GC 用スタックポインタ */
extern Index environment; /* 環境リスト */
#endif

/* From "MAIN.C" */
void initCells();
/* From "IO.C" */
Index error(char *);
Index gc_getFreeCell();
Index gc_strToName(char *);
void nameToStr(Index, char *);
Index gc_makeSymbol(char *);
Index addSymbol(int, Index);
int hash(char *);
Index gc_readS(Index);
void printS(Index);
/* From "GC.C" */
Index push(Index);
Index pop();
Index purseS(Index);
void mark_and_sweep();
// Index gc_f();
/* From "EVAL.C" */
Index cons_wrapper(Index, Index);
Index atom_wrapper(Index, Index);
Index append_wrapper(Index, Index);
Index assoclist_wrapper(Index, Index);
Index assoc_wrapper(Index, Index);
Index eval(Index, Index);
Index apply(Index, Index, Index);
Index apply_wrapper(Index, Index);
Index evcond(Index, Index);
Index evlist(Index, Index);
