/*                                   */
/*               Main                */
/*                                   */

#define MAIN
#include <stdio.h>
#include <stdlib.h>
#include "LISP.H"

char textbuf[TEXTBUF_SIZE];
char namebuf[TEXTBUF_SIZE];
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
int no_input_after_GC;

/* Give a name to the cell with index n, make it a symbol, and register it in the table. */
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

  /* Freecell linking */
  for (indx = 0; indx < CELLS_SIZE - 1; indx++)
  {
    car(indx) = 0;
    cdr(indx) = indx + 1;
    tag(indx) = CELL;
  }
  car(CELLS_SIZE - 1) = 0;
  cdr(CELLS_SIZE - 1) = 0;

  /* Initializing the symbol table */
  for (i = 0; i < SYMBOLTABLE_SIZE; i++)
    symbol_table[i] = 0;

  /* Initializing the top position of freecells */
  freecells = Last; /* Indexes smaller than 'Last' are reserved. */

  /* Initialization of the stack pointer for GC */
  sp = 0;

  /* Initializing the environment list */
  environment = 0;

  /* Registering 'nil' */
  tag(0) = NIL;
  car(0) = 0;
  cdr(0) = 0;

  /* Registering system symbols */
  gc_addSystemSymbol(T, "t");
  gc_addSystemSymbol(Lambda, "lambda");
  gc_addSystemSymbol(Label, "label");
  gc_addSystemSymbol(Quote, "quote");
  gc_addSystemSymbol(Atom, "atom");
  gc_addSystemSymbol(Eq, "eq");
  gc_addSystemSymbol(Car, "car");
  gc_addSystemSymbol(Cdr, "cdr");
  gc_addSystemSymbol(Cons, "cons");
  gc_addSystemSymbol(Rplaca, "rplaca");
  gc_addSystemSymbol(Rplacd, "rplacd");
  gc_addSystemSymbol(Cond, "cond");
  gc_addSystemSymbol(Eval, "eval");
  gc_addSystemSymbol(Apply, "apply");
  gc_addSystemSymbol(Error, "error");
  gc_addSystemSymbol(Gc, "gc");
  gc_addSystemSymbol(ImportEnv, "importenv");
  gc_addSystemSymbol(ExportEnv, "exportenv");
  gc_addSystemSymbol(Def, "def");
  gc_addSystemSymbol(Num, "num");
  gc_addSystemSymbol(Len, "len");
  gc_addSystemSymbol(Quit, "quit");
  gc_addSystemSymbol(Cls, "cls");
  gc_addSystemSymbol(Num1, "1");
  gc_addSystemSymbol(Num2, "2");
}

void top_loop()
{
  txtp = textbuf;
  *txtp = '\0';
  while (err != eof)
  {
    err = off;
    toplevel = gc_readS(1);
    no_input_after_GC = 0;
    if (err != off)
    {
      char *chp;

      printf("%s\n", message);
      if ((*txtp - 1) != EOF) /* Don't display '>' when "init.txt" is loaded. */
        printf("> %c\n", *(txtp - 1));
      *txtp = '\0';
      continue;
    }
    toplevel = eval(toplevel, environment);
    if (err == off)
    {
      if (no_input_after_GC)
        putchar('\n');
      printS(toplevel);
      putchar('\n');
    }
  }
}

void greeting()
{
  printf("\n");
  printf("\t  A Minimal Pure LISP Interpreter  \n\n");
  printf("\t            U r L I S P            \n\n");
  printf("\t           Version 0.3.5           \n");
  printf("\tThis software is released under the\n");
  printf("\t            MIT License.           \n\n");
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
  ifp = fopen("INIT.TXT", "r"); /* LISP programs to load at startup */
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
