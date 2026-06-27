int foo() {
    struct my_struct {
        int x, y;
    };

    struct my_struct s;

    return s.x;
}

int main() {
    struct my_struct {
        float x, y;
    };

    struct my_struct s;

    return 0;
}


