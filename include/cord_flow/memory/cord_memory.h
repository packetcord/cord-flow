#ifndef CORD_MEMORY_H
#define CORD_MEMORY_H

#define CORD_BUFFER(name, size) typedef uint8_t name[size] __attribute__ ((aligned (64)))

#endif // CORD_MEMORY_H
