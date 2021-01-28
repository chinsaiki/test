Virtual Function I/O

是一种现代化的设备直通方案，它允许利用了VT-d/AMD-Vi技术提供的DMA Remapping 和 Interrupt 
Remapping 特性，在保证直通设备的 DMA 安全性同时可以达到接近物理设备的 IO 的性能。
用户态进程可以直接使用 VFIO 驱动直接访问硬件，并且由于整个国政是在 IOMMU 
的保护下进行的，因此十分安全，而且非特权用户也是可以直接使用。换句话说，VFIO 
是一套完整 
用户态驱动方案，因为它可以安全地把设备IO、中断、DMA等能力呈现给用户空间。

-----------------------------------
*** test/vfio
*** test/linux/vfio
*** test/c/glibc/linux/vfio
*** test/dpdk/vfio
*** test/dpdk/vfio/20.05 
