#ifndef CORD_EAL_INITER_H
#define CORD_EAL_INITER_H

#ifdef ENABLE_DPDK_DATAPLANE

int cord_eal_init(int argc, char **argv);

#endif

#endif