// This how_to builds on `001_basic_usage`, so the driver below is kept as thin as possible,
// every interesting thing happens in `defer.c`.
//
// Notice that this file is the only one that defines `CN_IMPLEMENTATION`, `defer.c` includes
// `cnotes.h` too, but only gets the header part of it. That is the usual stb-style setup and it
// is what lets a meta program grow across multiple files.
#define CN_IMPLEMENTATION
#include "cnotes.h"

// Forward declaring `defer_extension` that is defined in `defer.c`.
bool defer_extension(Cn_Hooks *hooks);

int main(int argc, char **argv) {
    // One of the ways to structure various meta programs is to give each of them its own extension,
    // that adds its own hooks. Every extension is then registered here, in one place.
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
