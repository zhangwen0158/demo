#include"../header/Net.h"

int main() {
    CNet *net = new CNet("192.168.0.1", 1234);
    net->cRecv();
}