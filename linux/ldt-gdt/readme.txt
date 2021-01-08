GDT Global Descriptor Table全局描述符表

LDT Local Descriptor Table局部描述符表


# GDT Global Descriptor Table全局描述符表

GDT只有一张(一个处理器对应一个GDT)，可以放在内存的任何位置，CPU必须知道这个入口。
Intel设计者提供一个 GDTR 寄存器用来存放GDT的入口地址。

段选择子(Selector)由GDTR访问全局描述符表是通过“段选择子”（实模式下的寄存器）来完成的。


# LDT Local Descriptor Table局部描述符表

LDT 可以有若干张，每个人物可以有一张。

我们可以这样理解 GDT 和 LDT： GDT 为一级索引， LDT 为二级索引。

