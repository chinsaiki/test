Libvirt是用于管理虚拟化平台的开源的API，后台程序和管理工具。它可以用于管理KVM、Xen、V
Mware ESX，QEMU和其他虚拟化技术。这些API在云计算的解决方案中广泛使用.
libvirt是提供了一个方便的方式来管理虚拟机和其他虚拟化功能的软件的集合，如存储和网络接口管理。
这些软件包括一个API库，一个守护进程（Libvirtd），和一个命令行实用程序（virsh）。

libvirt的首要目标是能够管理多个不同的虚拟化供应商/虚拟机管理程序提供一个单一的方式。
例如，命令“virsh列表等等都可以用于任何支持现有的虚拟机管理程序列表（KVM、Xen、VMware ESX，等）
不需要学习管理程序特定的工具！

支持的虚拟化平台编辑
	LXC ： lightweight Linux container system
　　OpenVZ ：lightweight Linux container system
　　Kernel-based Virtual Machine/QEMU (KVM) ： open-source hypervisor for Linux and SmartOS
　　Xen ： Bare-Metal hypervisor
　　User-mode Linux (UML) ：paravirtualized kernel
　　VirtualBox ： hypervisor by Oracle (formerly by Sun) for Windows, Linux, Mac OS X, and Solaris
　　VMware ESX and GSX ： hypervisors for Intel hardware
　　VMware Workstation and Player ： hypervisors for Windows and Linux
　　Hyper-V ： hypervisor for Windows by Microsoft
　　PowerVM ： hypervisor by IBM for AIX, Linux and IBM i
　　Parallels Workstation ： hypervisor for Mac by Parallels IP Holdings GmbH
　　Bhyve ： hypervisor for FreeBSD 10+

