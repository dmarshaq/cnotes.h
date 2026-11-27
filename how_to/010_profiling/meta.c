// The driver is the same one `005_defer` uses, with a second extension added to it.
#define CN_IMPLEMENTATION
#include "cnotes.h"

// Forward declaring both extensions, `timer_extension` lives in `timer.c` and `defer_extension` in
// `defer.c`, which is the same file `005_defer` uses.
bool timer_extension(Cn_Hooks *hooks);
bool defer_extension(Cn_Hooks *hooks);

int main(int argc, char **argv) {
    // This is the structure `005_defer` was building towards, now with something in it:
    // every extension adds its hooks, and the library calls those hooks in the order they were added.
    //
    // For every parsed function, each hook is called repeatedly until it stops reporting modifications,
    // and only then the next hook gets the reparsed function.
    //
    // `timer_extension` goes first because it produces work for `defer_hook`: it generates a
    // `[[defer]]`, reports the modification, and once that text has been parsed back into
    // ast nodes, `defer_hook` gets to see it.
    if (!cn_extension_register(timer_extension, defer_extension)) return 1;

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
