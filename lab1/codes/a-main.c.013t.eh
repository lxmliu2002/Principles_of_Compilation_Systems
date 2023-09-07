
;; Function main (main, funcdef_no=0, decl_uid=2470, cgraph_uid=1, symbol_order=0)

int main ()
{
  double time;
  clock_t end;
  clock_t start;
  int f;
  int n;
  int i;
  int D.2487;

  scanf ("%d", &n);
  i = 2;
  f = 1;
  start = clock ();
  if (i <= 0) goto <D.2482>; else goto <D.2483>;
  <D.2482>:
  __builtin_puts (&"i\xe5\xb0\x8f\xe4\xba\x8e1"[0]);
  <D.2483>:
  goto <D.2479>;
  <D.2480>:
  f = f * i;
  i = i + 1;
  <D.2479>:
  n.0_1 = n;
  if (i <= n.0_1) goto <D.2480>; else goto <D.2478>;
  <D.2478>:
  if (f > 10000) goto <D.2484>; else goto <D.2485>;
  <D.2484>:
  __builtin_puts (&"\xe8\xbe\x93\xe5\x87\xba\xe8\xb6\x85\xe9\x99\x90"[0]);
  goto <D.2486>;
  <D.2485>:
  printf ("%d\n", f);
  <D.2486>:
  end = clock ();
  _2 = end - start;
  _3 = (double) _2;
  time = _3 / 1.0e+6;
  printf ("\xe7\xa8\x8b\xe5\xba\x8f\xe8\xbf\x90\xe8\xa1\x8c\xe6\x97\xb6\xe9\x97\xb4\xe4\xb8\xba\xef\xbc\x9a%f\n", time);
  D.2487 = 0;
  goto <D.2489>;
  <D.2489>:
  n = {CLOBBER};
  goto <D.2488>;
  D.2487 = 0;
  goto <D.2488>;
  <D.2488>:
  return D.2487;
}


