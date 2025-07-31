#ifndef CORD_RETVAL_H
#define CORD_RETVAL_H

typedef enum cord_retval_t
{
    CORD_OK = 0,
    CORD_ERR = -1,
    CORD_ERR_TIMEOUT = -2,
    CORD_ERR_INVALID = -3,
    CORD_ERR_NO_MEMORY = -4,
    CORD_ERR_UNSUPPORTED = -5,
    CORD_ERR_AGAIN = -6,
    CORD_ERR_NOT_FOUND = -7,
    CORD_ERR_ALREADY_EXISTS = -8,
} cord_retval_t;

#endif // CORD_RETVAL_H
