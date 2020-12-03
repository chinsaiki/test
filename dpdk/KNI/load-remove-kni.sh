#!/bin/bash
# 来自 DPDK 20.05 dpdk-setup.sh


#
# Unloads the rte_kni.ko module.
#
remove_kni_module()
{
	echo "Unloading any existing DPDK KNI module"
	/sbin/lsmod | grep -s rte_kni > /dev/null
	if [ $? -eq 0 ] ; then
		sudo /sbin/rmmod rte_kni
	fi
}

#
# Loads the rte_kni.ko module.
#
load_kni_module()
{
    # Check that the KNI module is already built.
	if [ ! -f $RTE_SDK/$RTE_TARGET/kmod/rte_kni.ko ];then
		echo "## ERROR: Target does not have the DPDK KNI Module."
		echo "       To fix, please try to rebuild target."
		return
	fi

    # Unload existing version if present.
	remove_kni_module

    # Now try load the KNI module.
	echo "Loading DPDK KNI module"
	sudo /sbin/insmod $RTE_SDK/$RTE_TARGET/kmod/rte_kni.ko
	if [ $? -ne 0 ] ; then
		echo "## ERROR: Could not load kmod/rte_kni.ko."
		quit
	fi
}

