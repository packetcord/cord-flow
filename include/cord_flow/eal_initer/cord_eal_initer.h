#ifndef CORD_EAL_INITER_H
#define CORD_EAL_INITER_H

#ifdef ENABLE_DPDK_DATAPLANE

int cord_eal_init(int argc, char **argv);
void cord_eal_cleanup(void);

#endif // ENABLE_DPDK_DATAPLANE

#endif // CORD_EAL_INITER_H