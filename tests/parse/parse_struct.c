// Test: Struct and union parsing
struct Point {
    int x;
    int y;
};

struct Rectangle {
    struct Point top_left;
    struct Point bottom_right;
};

union Data {
    int i;
    float f;
    char c;
};

struct WithPointer {
    int *ptr;
    struct WithPointer *next;
};

struct Point global_point;

int test_struct_access(void) {
    struct Point p;
    p.x = 10;
    p.y = 20;
    return p.x + p.y;
}

int test_pointer_access(struct Point *p) {
    p->x = 5;
    p->y = 10;
    return p->x + p->y;
}
