#ifndef  _CAN_INIT_H
#define _CAN_INIT_H

#include <iostream>

#define OFFLINE_TEST 1
using namespace std;

void InitCanBus(int fd);
void can_send_raw(int fd, unsigned char *data, unsigned int len);

#endif
