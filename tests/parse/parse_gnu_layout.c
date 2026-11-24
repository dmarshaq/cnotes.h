/**
 * TEST: GNU `packed` and `aligned` attributes changing layout of structs, unions and enums.
 *
 * Natural struct layout with trailing padding; `packed` and `__packed__` structs; `aligned(N)` and
 * argument-less `aligned` on structs; `aligned` member raising alignment; `aligned` member inside a
 * `packed` struct; `packed` member; `packed` and `aligned` unions; `packed` enums shrinking to the
 * smallest integer holding their values, including negative values; plain enum. Every expected size
 * is checked by an array whose length is `0` on mismatch, sizes match GCC.
 *
 * --gnu
 */

struct N { int a; char b; };
struct __attribute__((packed)) P { int a; char b; };
struct __attribute__((__packed__)) P2 { char c; long long d; short e; };
struct __attribute__((aligned(16))) A { char c; };
struct __attribute__((aligned)) A2 { char c; };
struct M { char c; int x __attribute__((aligned(8))); };
struct __attribute__((packed)) PM { char c; int x __attribute__((aligned(4))); char d; };
struct MP { char c; int x __attribute__((packed)); };
union __attribute__((packed)) U { int a; char b[5]; };
union __attribute__((aligned(8))) U2 { int a; };
enum __attribute__((packed)) E1 { E1_A, E1_B = 200 };
enum __attribute__((packed)) E2 { E2_A = -1, E2_B = 300 };
enum E3 { E3_A };

int check_n[sizeof(struct N) == 8 ? 1 : 0];
int check_p[sizeof(struct P) == 5 ? 1 : 0];
int check_p2[sizeof(struct P2) == 11 ? 1 : 0];
int check_a[sizeof(struct A) == 16 ? 1 : 0];
int check_a2[sizeof(struct A2) == 16 ? 1 : 0];
int check_m[sizeof(struct M) == 16 ? 1 : 0];
int check_pm[sizeof(struct PM) == 12 ? 1 : 0];
int check_mp[sizeof(struct MP) == 5 ? 1 : 0];
int check_u[sizeof(union U) == 5 ? 1 : 0];
int check_u2[sizeof(union U2) == 8 ? 1 : 0];
int check_e1[sizeof(enum E1) == 1 ? 1 : 0];
int check_e2[sizeof(enum E2) == 2 ? 1 : 0];
int check_e3[sizeof(enum E3) == 4 ? 1 : 0];
