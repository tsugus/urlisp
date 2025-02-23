/*                                   */
/*       Evaluator & functions       */
/*                                   */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISP.H"

/* エラー表示 */
void print_error(Index form, char *msg)
{
  printf("%s\n", msg);
  printf("At ");
  printS(form);
  putchar('\n');
  err = print_no_more; /* これ以上、表示しない */
}

Index atom(Index x)
{
  return (abs(tag(x)) == CELL ? Nil : T);
}

Index cons(Index x, Index y)
{
  Index z;

  push(x);
  push(y);
  z = gc_getFreeCell();
  ec;
  car(z) = x;
  cdr(z) = y;
  pop();
  pop();
  return z;
}

Index null(Index x)
{
  return (x == Nil ? T : Nil);
}

Index nott(Index x)
{
  return (x == Nil ? T : Nil);
}

Index rev_append(Index x, Index y)
{
  push(x);
  for (; x != Nil; x = cdr(x))
  {
    push(y);
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
Index append_wrapper(Index args, Index env)
{
  return append(car(args), car(cdr(args)));
}

Index assoclist(Index keys, Index values)
{
  Index indx;

  if ((keys == Nil) || (values == Nil))
    return Nil;
  indx = Nil;
  push(keys);
  push(values);
  while (nott(atom(keys)) && nott(atom(values)))
  {
    // push(indx);
    indx = cons(cons(car(keys), car(values)), indx);
    ec;
    keys = cdr(keys);
    values = cdr(values);
    // pop();
  }
  if (nott(null(keys)))
  {
    // push(indx);
    indx = cons(cons(keys, values), indx);
    ec;
    // pop();
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
  return error("An identifier that is not in the environment list.");
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
    return T;
  default:
    return Nil;
  }
}

Index setq(Index var, Index val, Index env)
{
  push(var);
  push(env);
  push(val);
  val = eval(val, env);
  ec;
  pop();
  push(val);
  cdr(env) = cons(car(env), cdr(env));
  ec;
  car(env) = cons(var, val);
  ec;
  pop();
  pop();
  pop();
  return val;
}

Index importEnv(Index pairlist)
{
  return environment = rev_append(pairlist, Nil);
}

Index exportEnv()
{
  return environment;
}

Index eval(Index exp, Index env)
{
  Index result;

  push(exp);
  push(env);
  if (exp == T)
    result = T;
  else if (exp == Nil)
    result = Nil;
  else if (atom(exp) == T)
    result = assoc(exp, env);
  else if (isSUBR(car(exp)) == T)
    result = apply(car(exp), evlist(cdr(exp), env), env);
  else
    result = apply(car(exp), cdr(exp), env);
  if (err == on)
  {
    print_error(exp, message);
    return Nil;
  }
  pop();
  pop();
  return result;
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
      if (atom(car(args)) == T)
        return error("The first argument is not a list.");
      else
        return car(car(args));
    case Cdr:
      if (atom(car(args)) == T)
        return error("The first argument is not a list.");
      else
        return cdr(car(args));
    case Cons:
      return cons(car(args), car(cdr(args)));
    case Cond:
      return evcond(args, env);
    case Setq:
      return setq(car(args), car(cdr(args)), env);
    case ImportEnv:
      return importEnv(car(args));
    case ExportEnv:
      return exportEnv();
    case Gc:
      mark_and_sweep();
      return Nil;
    default:
      return eval(cons(assoc(func, env), args), env);
    }
  }
  else if (car(func) == Label)
    return (eval(cons(car(cdr(cdr(func))), car(cdr(cdr(func)))), env));
  else if (car(func) == Lambda)
    return eval(car(cdr(cdr(func))), append(assoclist(car(cdr(func)), evlist(args, env)), env));
  else
    return error("An invalid Expression.");
}

Index evcond(Index clauses, Index env)
{
  for (; nott(null(clauses)); clauses = cdr(clauses))
    if (nott(null(eval(car(car(clauses)), env))))
      return eval(car(cdr(car(clauses))), env);
  return Nil;
}

Index evlist(Index members, Index env)
{
  Index indx;

  for (indx = Nil; nott(null(members)); members = cdr(members))
  {
    push(indx);
    indx = cons(eval(car(members), env), indx);
    ec;
    pop();
  }
  return rev_append(indx, Nil);
}
