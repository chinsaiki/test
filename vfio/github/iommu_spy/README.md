# iommu_spy

This git repo contains tools to analyze the memory management of linux kernel 3.16.59

su-devmem - disables /dev/mem 1MB restriction
  `make clean; make -j`
  `sudo insmod su-devmem.ko`

dev_mem.c - read from a physical memory
  `make clean; make -j`
  `sudo ./dev_mem $((0x00000000)) r 4`

This is from `linux_src/Documentation/x86/x86_64/mm.txt`:

```
Virtual memory map with 4 level page tables:

0000000000000000 - 00007fffffffffff (=47 bits) user space, different per mm
hole caused by [48:63] sign extension
ffff800000000000 - ffff80ffffffffff (=40 bits) guard hole
ffff880000000000 - ffffc7ffffffffff (=64 TB) direct mapping of all phys. memory
ffffc80000000000 - ffffc8ffffffffff (=40 bits) hole
ffffc90000000000 - ffffe8ffffffffff (=45 bits) vmalloc/ioremap space
ffffe90000000000 - ffffe9ffffffffff (=40 bits) hole
ffffea0000000000 - ffffeaffffffffff (=40 bits) virtual memory map (1TB)
... unused hole ...
ffffff0000000000 - ffffff7fffffffff (=39 bits) %esp fixup stacks
... unused hole ...
ffffffff80000000 - ffffffffa0000000 (=512 MB)  kernel text mapping, from phys 0
ffffffffa0000000 - ffffffffff5fffff (=1525 MB) module mapping space
ffffffffff600000 - ffffffffffdfffff (=8 MB) vsyscalls
ffffffffffe00000 - ffffffffffffffff (=2 MB) unused hole

The direct mapping covers all memory in the system up to the highest
memory address (this means in some cases it can also include PCI memory
holes).

vmalloc space is lazily synchronized into the different PML4 pages of
the processes using the page fault handler, with init_level4_pgt as
reference.

Current X86-64 implementations only support 40 bits of address space,
but we support up to 46 bits. This expands into MBZ space in the page tables.

->trampoline_pgd:

We map EFI runtime services in the aforementioned PGD in the virtual
range of 64Gb (arbitrarily set, can be raised if needed)

0xffffffef00000000 - 0xffffffff00000000
-Andi Kleen, Jul 2004
```

The virtual address space is split between the kernel and user modes. The upper region
specified by the PAGE_OFFSET is the virtual address space used by the kernel address.
The lower 0x0 to PAGE_OFFSET is used by the user address space.
The PAGE_OFFSET is set to 0xffff880000000000 in x86_64.  

It is important to distinguish between the three kinds of virtual addresses that is present:
* Kernel:
  - Kernel Logical Address
  - Kernel Virtual Address
* User:
  - User Virtual Address

I know it is confusing to think about how a kernel can have a virtual address because the kernel
is not a process, so logically you would think that it would have access to the entire physical memory
and that all of its address are 'physical'. But once paging is enabled (with the CR0 register), all memory
access are now translated including kernel addresses. And the difference between kernel logical and kernel 
virtual addresses is due to a performance reason. 

Kernel Logical Addresses are the normal address space of the kernel (kmalloc()). These virtual addresses
are a fixed offset from their physical address. So, a virt: 0xffff880000000000 -> phys: 0x0000000000000000.
This makes converting between the physical and virtual addresses very easy and fast.

