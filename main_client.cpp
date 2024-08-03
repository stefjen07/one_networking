#include "client/client.h"

int main() {
    Client client("localhost", 82);
    client.start();
    return 0;
}
