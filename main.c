long int a;
long long b;
long c;
long long int d;

typedef struct vec2f {
    float x, y;
} Vec2f;

Vec2f vec_a;

struct vec2f vec_b;

int main(void) {
    a + 5;
}

// // // Function pointers
// void                  (*y)(void);
// int                  (*z)(int, char *);
//  
// // Pointer to function returning pointer
// int                  *(*aa)(int, char *);
//  
// // Function taking function pointer
// int                   bb(int (*)(char *), double);
//  
// // Function returning function pointer
//  int                  (*cc(int))(char *);
//  
// // Nested function pointers — the main stress test
// // pointer to function taking (pointer to function taking char* returning int) returning int*
// int                  *(*dd)(int (*)(char *));
//  
// struct my_struct       *ff;
// struct my_struct      (*gg)(int, char *);
// // struct my_struct      *(*hh)[30];
// /* ============================================================
//  * struct_tag_tests.c
//  * Test cases for struct tag handling, scoping, completion,
//  * shadowing, and incomplete-type semantics.
//  *
//  * Each test is wrapped in its own function so they don't
//  * interfere with each other in terms of file-scope state.
//  * For testing file-scope behavior, the top-level declarations
//  * before main() exercise that.
//  *
//  * Expected behavior is noted in comments. Some sections are
//  * intended to PRODUCE ERRORS — those are clearly marked
//  * with "// EXPECT ERROR".
//  * ============================================================ */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 1: file-scope forward declaration and completion
//  * Tests basic incomplete -> complete transition at file scope.
//  * Expected: one tag binding 'Forward' at file scope, type
//  * starts incomplete, becomes complete. Same Cn_Type * throughout.
//  * ------------------------------------------------------------ */
// 
// struct Forward;                 /* introduce incomplete struct Forward */
// struct Forward *fwd_ptr;        /* OK: pointer to incomplete is allowed */
// 
// struct Forward {                /* complete the same tag */
//     int x;
//     int y;
// };
// 
// struct Forward fwd_obj;         /* OK: complete by this point */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 2: inline definition (no prior forward decl)
//  * Expected: creates and completes in one step.
//  * ------------------------------------------------------------ */
// 
// struct Inline {
//     int a;
//     int b;
// } inline_obj;
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 3: self-referential struct (linked list node)
//  * Tests that pointer-to-incomplete works inside the very
//  * struct being declared. The pointer member must reference
//  * the in-progress incomplete type, and remain valid after
//  * the body closes (type completes).
//  * ------------------------------------------------------------ */
// 
// struct ListNode {
//     int value;
//     struct ListNode *next;      /* refers to in-progress incomplete ListNode */
// };
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 4: mutually recursive structs via forward decls
//  * Tests cross-references through pointers, with forward
//  * declarations breaking the cycle.
//  * ------------------------------------------------------------ */
// 
// struct A;                       /* forward decl A */
// struct B;                       /* forward decl B */
// 
// struct A {
//     struct B *b;                /* pointer to incomplete B */
//     int       data;
// };
// 
// struct B {
//     struct A *a;                /* A is now complete by this point */
//     int       data;
// };
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 5: typedef of struct
//  * Tests that typedef binding (ordinary namespace) and tag
//  * binding (tag namespace) can coexist with the same string.
//  * 'Vec2' becomes a typedef; 'Vec2' is also a tag.
//  * ------------------------------------------------------------ */
// 
// typedef struct Vec2 {
//     float x;
//     float y;
// } Vec2;                          /* typedef name Vec2 in ordinary namespace */
// 
// Vec2         v_via_typedef;     /* uses typedef */
// struct Vec2  v_via_tag;         /* uses tag — same type as v_via_typedef */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 6: opaque type via forward decl + typedef
//  * Tests the standard "opaque handle" idiom.
//  * The struct is never completed in this TU; you can only
//  * hold pointers to it.
//  * ------------------------------------------------------------ */
// 
// typedef struct OpaqueHandle OpaqueHandle;
// OpaqueHandle *handle1;
// OpaqueHandle *handle2;
// /* No `OpaqueHandle obj;` because that requires complete type. */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 7: redefinition (SHOULD ERROR)
//  * Same tag, same scope, both with bodies — redefinition error.
//  * Uncomment to test. Leave commented in normal runs.
//  * ------------------------------------------------------------ */
// 
// /*
// struct Redef { int x; };
// struct Redef { int y; };        // EXPECT ERROR: redefinition of struct Redef
// */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 8: object of incomplete type at file scope (SHOULD ERROR)
//  * Per C, declaring an object of incomplete type is an error
//  * (the tentative-definition is never completed).
//  * Uncomment to test.
//  * ------------------------------------------------------------ */
// 
// /*
// struct NeverDefined;
// struct NeverDefined never_obj;  // EXPECT ERROR: incomplete type
// */
// 
// 
// /* ============================================================
//  * Function-scope tests
//  * ============================================================ */
// 
// void test_block_scope(void) {
//     /* SECTION 9: local struct, shadows file-scope tag */
//     /* File scope has struct Forward; here we introduce a local one. */
//     struct Forward {            /* new local tag, shadows outer */
//         char z;
//     };
//     struct Forward local_fwd;
//     local_fwd.z = 0;
//     /* fwd_obj.x is the outer one; not used here */
// }
// 
// void test_nested_scopes(void) {
//     /* SECTION 10: deeply nested local structs with the same tag */
//     struct Local { int a; };
//     struct Local outer_local;
//     outer_local.a = 1;
// 
//     {
//         /* New scope: shadow with a different struct */
//         struct Local { float b; };
//         struct Local inner_local;
//         inner_local.b = 2.0f;
//         /* outer_local still has type from outer scope */
//     }
//     /* Back to outer scope; struct Local is the int-a version again */
//     outer_local.a = 3;
// }
// 
// void test_forward_then_complete_in_same_scope(void) {
//     /* SECTION 11: local forward decl + completion in same scope */
//     struct LocalFwd;
//     struct LocalFwd *p;         /* OK: pointer to incomplete */
//     struct LocalFwd {
//         int v;
//     };
//     struct LocalFwd obj;
//     obj.v = 0;
//     (void)p;
// }
// 
// void test_use_position_introduces_locally(void) {
//     /* SECTION 12: use position that introduces a fresh local tag.
//      * If no outer tag NewHere exists, this both uses AND introduces.
//      */
//     struct NewHere *p;          /* introduces incomplete struct NewHere */
//     /* p is pointer to incomplete; we never complete it.
//      * That's fine because we never dereference it.
//      */
//     (void)p;
// }
// 
// void test_typedef_of_forward(void) {
//     /* SECTION 13: typedef can reference an incomplete tag */
//     typedef struct Hidden Hidden;
//     Hidden *p;                  /* OK: pointer to incomplete typedef target */
//     (void)p;
// }
// 
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 14: nested struct definition
//  * Inner struct is declared as a member type. C says the inner
//  * struct's tag is actually scoped to the enclosing scope (NOT
//  * to the outer struct), but the type itself is reachable
//  * via member access.
//  * ------------------------------------------------------------ */
// 
// struct Outer {
//     int x;
//     struct Inner {
//         int y;
//     } inner_member;
// };
// 
// /* In strict C, `struct Inner` is now visible at file scope (not inside Outer).
//  * Your tool might choose to scope it to the outer struct instead, but
//  * standard C says file scope. Test what your implementation does.
//  */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 15: anonymous struct (no tag) declared with variable
//  * Each anonymous struct decl creates a fresh type.
//  * ------------------------------------------------------------ */
// 
// struct {
//     int p;
//     int q;
// } anon_obj1;
// 
// struct {
//     int p;
//     int q;
// } anon_obj2;
// /* anon_obj1 and anon_obj2 have different types in strict C
//  * (two anonymous struct declarations = two distinct types). */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 16: pointer-to-struct deduplication
//  * Both `struct Forward *` declarations should resolve to the
//  * SAME canonical pointer type, since the structural interner
//  * dedupes pointer types by their pointee.
//  * ------------------------------------------------------------ */
// 
// struct Forward *fwd_ptr_a;
// struct Forward *fwd_ptr_b;
// /* Type of fwd_ptr_a should == type of fwd_ptr_b (same Cn_Type *). */
// 
// 
// /* ------------------------------------------------------------
//  * SECTION 17: struct containing pointer to itself, then array
//  * ------------------------------------------------------------ */
// 
// struct Tree {
//     int           value;
//     struct Tree  *left;
//     struct Tree  *right;
// };
// 
// 
// /* ============================================================
//  * Main: orchestrates the runtime tests (mostly to ensure
//  * the program is syntactically valid as a whole TU).
//  * ============================================================ */
// 
// int main(void) {
//     fwd_obj.x      = 1;
//     fwd_obj.y      = 2;
//     inline_obj.a   = 3;
//     v_via_typedef.x = 1.0f;
//     v_via_tag.y     = 2.0f;
// 
//     test_block_scope();
//     test_nested_scopes();
//     test_forward_then_complete_in_same_scope();
//     test_use_position_introduces_locally();
//     test_typedef_of_forward();
// 
//     /* Test pointer equality conceptually: both point to the
//      * same struct Forward type, so cross-assignment is fine. */
//     fwd_ptr_a = fwd_ptr_b;
// 
//     /* Self-referential type usage */
//     struct ListNode n1;
//     struct ListNode n2;
//     n1.value = 10;
//     n2.value = 20;
//     n1.next  = &n2;
//     n2.next  = (struct ListNode *)0;
// 
//     /* Tree usage */
//     struct Tree root;
//     root.value = 0;
//     root.left  = (struct Tree *)0;
//     root.right = (struct Tree *)0;
// 
//     return 0;
// }
// 
