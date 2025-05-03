/*                                   */
/*       Evaluator & functions       */
/*                                   */

#include <stdio.h>
#include <stdlib.h>
#include "LISP.H"

void print_error(Index form, char *msg)
{
  printf("%s\n", msg);
  printf("At ");
  printS(form);
  putchar('\n');
  err = print_no_more;
}

Index error_(Index code, Index form)
{
  switch (code)
  {
  case Num1:
    printf("Not found");
    break;
  case Num2:
    printf("Invalid form");
    break;
  default:
    printf("Error");
  }
  printf(": ");
  printS(form);
  putchar('\n');
  err = print_no_more;
  return Nil;
}

Index atom(Index x)
{
  return (abs(tag(x)) == CELL ? Nil : T);
}

Index null(Index x)
{
  return (x == Nil ? T : Nil);
}

Index nott(Index x)
{
  return (x == Nil ? T : Nil);
}

Index cons(Index x, Index y)
{
  Index z;

  push(x);
  ec;
  push(y);
  ec;
  z = gc_getFreeCell();
  ec;
  car(z) = x;
  cdr(z) = y;
  pop();
  pop();
  return z;
}

Index rplaca(Index x, Index y)
{
  if (!is(x, CELL))
    return error("the 1st argument is an atom.");
  car(x) = y;
  return x;
}

Index rplacd(Index x, Index y)
{
  if (!is(x, CELL))
    return error("the 1st argument is an atom.");
  cdr(x) = y;
  return x;
}

Index rev_append(Index x, Index y)
{
  push(x);
  ec;
  for (; x != Nil; x = cdr(x))
  {
    push(y);
    ec;
    y = cons(car(x), y);
    ec;
    pop();
  }
  pop();
  return y;
}

Index append(Index x, Index y)
{
  return rev_append(rev_append(x, Nil), y);
}

Index assoclist(Index keys, Index values)
{
  Index indx;

  if ((keys == Nil) || (values == Nil))
    return Nil;
  indx = Nil;
  push(keys);
  ec;
  push(values);
  ec;
  while (nott(atom(keys)) && nott(atom(values)))
  {
    push(indx);
    ec;
    indx = cons(cons(car(keys), car(values)), indx);
    ec;
    keys = cdr(keys);
    values = cdr(values);
    pop();
  }
  if (nott(null(keys)))
  {
    push(indx);
    ec;
    indx = cons(cons(keys, values), indx);
    ec;
    pop();
  }
  pop();
  pop();
  return rev_append(indx, Nil);
}

Index assoc(Index key, Index lst)
{
  for (; lst != Nil; lst = cdr(lst))
    if (key == car(car(lst)))
      return cdr(car(lst));
  error_(Num1, key);
  return Nil;
}

Index def(Index var, Index val)
{
  Index env;

  push(var);
  ec;
  env = cons(cons(var, val), environment);
  if (env != Nil) /* A workaround for unquoted lambda expressions clearing the environment list. */
    environment = env;
  pop();
  return var;
}

Index isSUBR(Index x)
{
  switch (x)
  {
  case Atom:
  case Eq:
  case Car:
  case Cdr:
  case Cons:
  case Rplaca:
  case Rplacd:
  case Eval:
  case Apply:
  case Error:
  case Len:
    return T;
  default:
    return Nil;
  }
}

Index evcond(Index clauses, Index env)
{
  for (; nott(null(clauses)); clauses = cdr(clauses))
  {
    if (is(clauses, SYMBOL))
      return error("Invalid clause");
    if (is(car(clauses), SYMBOL))
      return error("Invalid clause");
    if (nott(null(eval(car(car(clauses)), env))))
      return eval(car(cdr(car(clauses))), env);
  }
  return Nil;
}

Index evlist(Index members, Index env)
{
  Index indx;

  for (indx = Nil; nott(null(members)); members = cdr(members))
  {
    push(indx);
    ec;
    indx = cons(eval(car(members), env), indx);
    ec;
    pop();
  }
  return rev_append(indx, Nil);
}

Index eval(Index form, Index env)
{
  Index result;

  push(form);
  ec;
  push(env);
  ec;
  if (form == T)
    result = T;
  else if (form == Nil)
    result = Nil;
  else if (atom(form) == T)
    result = assoc(form, env);
  else if (isSUBR(car(form)) == T ||
           (is(car(form), CELL) &&
            ((car(car(form)) == Lambda) ||
             (car(car(form))) == Funarg)))
    result = apply(car(form), evlist(cdr(form), env), env);
  else
    result = apply(car(form), cdr(form), env);
  if (err == on)
  {
    print_error(form, message);
    return Nil;
  }
  pop();
  pop();
  return result;
}

Index num(Index arg)
{
  Index num, indx, result;
  int i;

  if (!is(arg, SYMBOL))
    error("the argument is an list.");
  nameToStr(car(arg), namebuf);
  num = atoi(namebuf);
  result = 0;
  for (i = 0; i < num; i++)
  {
    indx = gc_getFreeCell();
    ec;
    car(indx) = 1;
    cdr(indx) = result;
    result = indx;
  }
  return result;
}

Index len(Index arg)
{
  Index indx;
  int i;

  if (is(arg, SYMBOL))
    error("the argument is an atom.");
  for (i = 0; arg; arg = cdr(arg))
    if (i++ < 0)
      return error("Numeric overflow.");
  sprintf(namebuf, "%d", i);
  return gc_makeSymbol(namebuf);
}

Index quit()
{
  free(cells);
  free(tags);
  exit(0);
}

Index cls()
{
  printf("\033[2J");   /* Clear the screen. */
  printf("\033[0;0H"); /* Move the cursor to (0,0). */
  err = print_no_more;
  return 0;
}

Index apply(Index func, Index args, Index env)
{
  if (atom(func) == T && func != Nil)
  {
    switch (func)
    {
    case Quote:
      return car(args);
    case Atom:
      if (atom(car(args)))
        return T;
      else
        return Nil;
    case Eq:
      if (car(args) == car(cdr(args)))
        return T;
      else
        return Nil;
    case Car:
      if (car(args) == Nil)
        return Nil;
      else if (atom(car(args)) == T)
        return error("1st item is invalid.");
      else
        return car(car(args));
    case Cdr:
      if (car(args) == Nil)
        return Nil;
      else if (atom(car(args)) == T)
        return error("1st item is invalid.");
      else
        return cdr(car(args));
    case Cons:
      return cons(car(args), car(cdr(args)));
    case Function:
      return cons(Funarg, cons(car(args), cons(env, Nil)));
    case Funarg:
      return cons(func, args);
    case Rplaca:
      return rplaca(car(args), car(cdr(args)));
    case Rplacd:
      return rplacd(car(args), car(cdr(args)));
    case Cond:
      return evcond(args, env);
    case Eval:
      return eval(car(args), car(cdr(args)));
    case Apply:
      return apply(car(args), car(cdr(args)), env);
    case Error:
      return error_(car(args), car(cdr(args)));
    case Gc:
      mark_and_sweep();
      return Nil;
    case ImportEnv:
      return environment = eval(car(args), env);
    case ExportEnv:
      return environment;
    case Def:
      return def(car(args), eval(car(cdr(args)), env));
    case Num:
      return num(car(args));
    case Len:
      return len(car(args));
    case Quit:
      return quit();
    case Cls:
      return cls();
    default:
      return eval(cons(assoc(func, env), args), env);
    }
  }
  else if (car(func) == Label)
    return eval(cons(car(cdr(cdr(func))), args),
                cons(cons(car(cdr(func)), car(cdr(cdr(func)))),
                     env));
  else if (car(func) == Funarg)
    return apply(car(cdr(func)), args, car(cdr(cdr(func))));
  else if (car(func) == Lambda)
    return eval(car(cdr(cdr(func))),
                append(assoclist(car(cdr(func)), args), env));
  else
  {
    error_(Num2, cons(func, args));
    err = print_no_more;
    return Nil;
  }
}