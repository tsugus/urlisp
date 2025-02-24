/*                                   */
/*               Main                */
/*                                   */

#define MAIN
#include <stdio.h>
#include <stdlib.h>
#include "LISP.H"

char textbuf[TEXTBUF_SIZE];
char namebuf[TEXTBUF_SIZE];
char namebuf2[TEXTBUF_SIZE];
char *tags;
Cell *cells;
Index stack[STACK_SIZE];
char *txtp;
Index freecells, toplevel;
Index symbol_table[SYMBOLTABLE_SIZE];
int err;
char *message;
FILE *ifp;
int sp;
Index environment;

/* インデックス n のセルに name を持つシンボルを作ってテーブルに登録 */
void gc_addSystemSymbol(Index n, char *name)
{
  car(n) = gc_strToName(name);
  tag(n) = SYMBOL;
  addSymbol(hash(name), n);
}

void initCells()
{
  Index indx;
  int i;

  /* フリーセル・リストの連結 */
  for (indx = 0; indx < CELLS_SIZE - 1; indx++)
  {
    car(indx) = 0;
    cdr(indx) = indx + 1;
    tag(indx) = CELL;
  }
  car(CELLS_SIZE - 1) = 0;
  cdr(CELLS_SIZE - 1) = 0;

  /* シンボルテーブルの初期化 */
  for (i = 0; i < SYMBOLTABLE_SIZE; i++)
    symbol_table[i] = 0;

  /* フリーセルの先頭位置の初期化 */
  freecells = Last; /* last より小さいインデックスは予約済み */

  /* sp の初期化 */
  sp = 0; /* GC 用スタックポインタ */

  /* 環境リストの初期化 */
  environment = 0;

  /* nil の登録 */
  tag(0) = NIL;
  car(0) = 0;
  cdr(0) = 0;

  /* システム・シンボルの登録 */
  gc_addSystemSymbol(T, "t");
  gc_addSystemSymbol(Lambda, "lambda");
  gc_addSystemSymbol(Quote, "quote");
  gc_addSystemSymbol(Atom, "atom");
  gc_addSystemSymbol(Eq, "eq");
  gc_addSystemSymbol(Car, "car");
  gc_addSystemSymbol(Cdr, "cdr");
  gc_addSystemSymbol(Cons, "cons");
  gc_addSystemSymbol(Cond, "cond");
  gc_addSystemSymbol(Eval, "eval");
  gc_addSystemSymbol(Label, "label");
  gc_addSystemSymbol(Gc, "gc");
  gc_addSystemSymbol(ImportEnv, "importenv");
  gc_addSystemSymbol(ExportEnv, "exportenv");
  gc_addSystemSymbol(Def, "def");
}

void top_loop()
{
  txtp = textbuf;
  *txtp = '\0';
  while (err != eof)
  {
    err = off;
    toplevel = gc_readS(1);
    if (err != off)
    {
      char *chp;

      printf("%s\n", message);
      printf("> ");
      for (chp = textbuf; chp <= txtp; chp++)
        putchar(*chp);
      putchar('\n');
      *txtp = '\0';
      continue;
    }
    toplevel = eval(toplevel, environment);
    if (err == off)
    {
      printS(toplevel);
      putchar('\n');
    }
  }
}

void greeting()
{
  printf("\n");
  printf("\t     An pure LISP Interpreter\n\n");
  printf("\t           U r L I S P\n\n");
  printf("\t          Version 0.0.5\n");
  printf("\tThis software is released under the\n");
  printf("\t           MIT License.\n\n");
  printf("\t                     (C) 2025 Tsugu\n\n");
}

int main()
{
  tags = (char *)malloc(sizeof(char) * CELLS_SIZE);
  if (tags == NULL)
  {
    printf("Unable to secure a cell area.\n");
    return 0;
  }
  cells = (Cell *)malloc(sizeof(Cell) * CELLS_SIZE);
  if (cells == NULL)
  {
    printf("Unable to secure a cell area.\n");
    return 0;
  }
  ifp = stdin;
  initCells();
  ifp = fopen("init.txt", "r"); /* 起動時に読み込む LISP プログラム */
  if (ifp == NULL)
  {
    printf("\"init.txt\" is missing. Please prepare an empty init.txt file.\n");
    return 0;
  }
  err = off;
  top_loop();
  fclose(ifp);
  ifp = stdin;
  greeting();
  while (1)
  {
    err = off;
    top_loop();
    if (ifp == stdin)
      rewind(ifp);
    else
      fclose(ifp);
  }
}
