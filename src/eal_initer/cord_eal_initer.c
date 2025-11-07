#ifdef ENABLE_DPDK_DATAPLANE

#include <eal_initer/cord_eal_initer.h>
#include <cord_error.h>
#include <cord_retval.h>
#include <cord_type.h>
#include <rte_eal.h>
#include <rte_errno.h>

int cord_eal_init(int argc, char **argv)
{
    // Initialize the DPDK EAL
    int ret = rte_eal_init(argc, argv);
    if (ret < 0)
    {
        CORD_LOG("[CordApp] Error: Failed to initialise DPDK EAL: %s\n", rte_strerror(rte_errno));
        return CORD_ERR;
    }

    CORD_LOG("[CordApp] DPDK EAL initialized successfully.\n");
}

#endif