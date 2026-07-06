#pragma once

#include <sys/types.h>
#include <stddef.h>

ssize_t recvAll(int socketFD, void *buffer, size_t size);
