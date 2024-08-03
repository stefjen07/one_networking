#include "server/server.h"

int main() {
    Server server("localhost", 82);
    server.start();
    return 0;
}
