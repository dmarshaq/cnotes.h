
#include <stdio.h>

#include "meta_generated.h"
#include "core/typeinfo.h"

@Introspect;
typedef struct my_struct {
    double *a;
    int b, c, d;
    struct nested {
        void *ptr @NestedStructNote;
        long long int agfdg;
    } n;
} My_Struct;


int main(void) {
    Type_Info *type = @Typeof(My_Struct);
    print_type_info(type);

    printf("Hello world!\n");
    return 0;
}





