// Test: Char literal parsing
char global_c = 'g';
char global_arr[3] = { 'x', 'y', 'z' };

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int test_chars(void) {
    char c = 'a';
    char upper = 'Z';
    char digit = '7';
    char space = ' ';
    char quote = '"';

    char newline = '\n';
    char tab = '\t';
    char carriage = '\r';
    char nul = '\0';
    char backslash = '\\';
    char single_quote = '\'';
    char double_quote = '\"';
    char question = '\?';
    char alert = '\a';
    char backspace = '\b';
    char form_feed = '\f';
    char vertical_tab = '\v';

    char octal = '\101';
    char octal_short = '\7';
    char hex = '\x41';
    char hex_upper = '\x7F';

    int i = 'a';
    unsigned char uc = 'b';
    signed char sc = 'c';
    char arr[4] = { 'a', 'b', '\n', '\0' };
    char sized['A' - '0'];

    i = c - 'a';
    i = 'a' + 1;
    c = (char)('a' + i);
    i = c == 'a' ? 'b' : 'c';
    i = is_digit('5');
    i = sizeof('a');
    arr[0] = 'q';

    if (c != '\0') {
        c = 'x';
    }

    switch (c) {
        case 'a': i = 1; break;
        case '\n': i = 2; break;
        case '\x41': i = 3; break;
        case 'z' - 1: i = 4; break;
        default: i = 0; break;
    }

    return i;
}
