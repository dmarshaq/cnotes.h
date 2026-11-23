// Test: Initializer, designators and designations.
typedef struct vec3 {
    int x, y;
    int z;
} Vec3;

int test_simple_initializer(void) {
    int x = 5;
    int y = 5;
    int arr[2] = { 1, [0] = 2, 3 };
    int *pi = (int[3]){1, 2, 3};
    Vec3 v = { .y = 3, 4, .x = 0 };
    return v.x + v.y + v.z;
}
