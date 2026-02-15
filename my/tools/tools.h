#pragma once
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>

void errif(bool, const char *);
void setnonblocking(int fd);