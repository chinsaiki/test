#include <sys/auxv.h>

//retrieve a value from the auxiliary vector
//	从辅助 vector 中检索一个值
//
//retrieve: 找回，取回，恢复
//auxiliary: 辅助设备，支援的，辅助人员
unsigned long getauxval(unsigned long type);

