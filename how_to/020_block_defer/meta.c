#define CN_IMPLEMENTATION
#include "cnotes.h"

// Forward declaring `defer_extension` that is defined in `defer.c`.
bool defer_extension(Cn_Hooks *hooks);

int main(int argc, char **argv) {
    if (!cn_extension_register(defer_extension)) return 1;

    while (true) {
        argc--;
        argv++;
        if (argc == 0) break;

        Cn_Translation_Unit tu = {0};
        if (!cn_tu_init(&tu, *argv)) return 1;

        if (cn_tu_process(&tu) != 0) {
            fprintf(stderr, "Processing failed with %ld error(s)\n", tu.data.error_count);
            cn_tu_free(&tu);
            return 1;
        }

        cn_tu_free(&tu);
    }

    return 0;
}
