#if 0
// TODO: Make this name so that everyone knows they should not use it.
#define GET_MACRO(_1,_2,_3,NAME,...) NAME


#define FOO1(a) printf("Doing FOO1 with args: %s\n", a);
#define FOO2(a,b) printf("Doing FOO2 with args: %s %s\n", a, b);
#define FOO3(a,b,c) printf("Doing FOO3 with args: %s %s %s\n", a, b, c);
#define FOO(...) GET_MACRO(__VA_ARGS__, FOO3, FOO2, FOO1)(__VA_ARGS__)
#endif
