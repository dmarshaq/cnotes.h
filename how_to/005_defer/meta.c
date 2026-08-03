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

// Forward declaring `defer_handler` that is defined in `defer.c`.
Cn_Message_Response defer_handler(Cn_Message *message);

Cn_Message_Response msg_handler(Cn_Message *message) {
    Cn_Message_Response response = CN_MESSAGE_RESPONSE_NONE;

    // One of the ways to structure handling a ton of various meta programs it to 
    // delegate messages to handlers, this can be done in variety of ways.
    // Following structure will make sense once more handlers will be called.
    response = defer_handler(message);
    if (response != CN_MESSAGE_RESPONSE_NONE) return response;

    return response;
}
