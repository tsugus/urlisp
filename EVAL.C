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
  for (; x != Nil; x = cdr(x))
    y = cons(car(x), y);
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
  // push(keys);
  // push(values);
  indx = Nil;
  while (nott(atom(keys)) && nott(atom(values)))
  {
    indx = cons(cons(car(keys), car(values)), indx);
    ec;
    keys = cdr(keys);
    values = cdr(values);
  }
  if (nott(null(keys)))
  {
    indx = cons(cons(keys, values), indx);
  }
  // pop();
  // pop();
  return rev_append(indx, Nil);
}
Index assoclist_wrapper(Index args, Index env)
{
  return assoclist(car(args), car(cdr(args)));
}

Index assoc(Index key, Index lst)
{
  for (; lst != Nil; lst = cdr(lst))
    if (key == car(car(lst)))
      return cdr(car(lst));
  return error("An identifier that is not in the environment list.");
}
Index assoc_wrapper(Index args, Index env)
{
  return assoc(car(args), car(cdr(args)));
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
  val = eval(val, env);
  push(var);
  push(val);
  push(env);
  ec;
  push(val);
  cdr(env) = cons(car(env), cdr(env));
  ec;
  car(env) = cons(var, val);
  ec;
  pop();
  pop();
  pop();
  return var;
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
  {
    result = assoc(exp, env);
  }
  else if (isSUBR(car(exp)) == T)
  {
    result = apply(car(exp), evlist(cdr(exp), env), env);
  }
  else
  {
    result = apply(car(exp), cdr(exp), env);
  }
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
        return error("＂第一引数がリストではない。");
      else
        return car(car(args));
    case Cdr:
      if (atom(car(args)) == T)
        return error("＂第一引数がリストではない。");
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
Index apply_wrapper(Index args, Index env)
{
  return apply(car(args), car(cdr(args)), env);
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
    indx = cons(eval(car(members), env), indx);
  return rev_append(indx, Nil);
}
