// The driver is the same one `005_defer` uses, with a second handler added to it.
#define CN_IMPLEMENTATION
#include "cnotes.h"

Cn_Message_Response msg_handler(Cn_Message *message);

int main(int argc, char **argv) {
    while (true) {
        argc--;
        argv++;
        if (argc == 0) break;

        Cn_Translation_Unit tu = {0};
        if (!cn_tu_init(&tu, *argv)) return 1;

        cn_message_handler = msg_handler;

        if (cn_tu_process(&tu) != 0) {
            fprintf(stderr, "Processing failed with %ld error(s)\n", tu.data.error_count);
            cn_tu_free(&tu);
            return 1;
        }

        cn_tu_free(&tu);
    }

    return 0;
}

// Forward declaring both handlers, `timer_handler` lives in `timer.c` and `defer_handler` in
// `defer.c`, which is the same file `005_defer` uses.
Cn_Message_Response timer_handler(Cn_Message *message);
Cn_Message_Response defer_handler(Cn_Message *message);

// This is the structure `001_basic_usage` and `005_defer` were building towards, now with
// something in it: every message is offered to each handler in turn, and the first one to report
// a modification ends the round, because the library is about to reparse and send the message
// again anyway.
//
// `timer_handler` goes first because it produces work for `defer_handler`: it generates a
// `[[defer]]`, reports the modification, and only on the next round, once that text has been
// parsed back into ast nodes, does `defer_handler` get to see it.
Cn_Message_Response msg_handler(Cn_Message *message) {
    Cn_Message_Response response = CN_MESSAGE_RESPONSE_NONE;

    response = timer_handler(message);
    if (response != CN_MESSAGE_RESPONSE_NONE) return response;

    response = defer_handler(message);
    if (response != CN_MESSAGE_RESPONSE_NONE) return response;

    return response;
}
