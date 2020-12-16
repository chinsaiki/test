#include <locale.h>

/* These are the possibilities for the first argument to setlocale.
   The code assumes that the lowest LC_* symbol has the value zero.  */
#define LC_CTYPE          __LC_CTYPE
#define LC_NUMERIC        __LC_NUMERIC
#define LC_TIME           __LC_TIME
#define LC_COLLATE        __LC_COLLATE
#define LC_MONETARY       __LC_MONETARY
#define LC_MESSAGES       __LC_MESSAGES
#define	LC_ALL		  __LC_ALL
#define LC_PAPER	  __LC_PAPER
#define LC_NAME		  __LC_NAME
#define LC_ADDRESS	  __LC_ADDRESS
#define LC_TELEPHONE	  __LC_TELEPHONE
#define LC_MEASUREMENT	  __LC_MEASUREMENT
#define LC_IDENTIFICATION __LC_IDENTIFICATION

//LC_ALL          代表 所有 部分.
//LC_COLLATE      代表 正则 表达式 匹配 (和 范围 表达式[range expressions] 以及 字符类[classes]  有关系)  和  字符串       排序.
//LC_CTYPE        代表  正则  表达式  匹配,  字符类(character classification), 转换, 区分大小写 的 比较, 以及 宽字符       函数.
//LC_MESSAGES     代表 可以 本地化的 消息 (自然语言).
//LC_MONETARY     代表 货币 格式.
//LC_NUMERIC      代表 数字 格式 (比如 小数点 和 千位分组符).
//LC_TIME         代表 时间 和 日期 格式.


//C语言是世界性语言，他支持全球的语言系统，可以处理英文、中文、拉丁等等。
//可以通过 setlocale 函数进行低于设置，改变程序的语言环境。
//地域设置是与某个地区（或者某个国家）的语言和文化相关的一些列内容，包含字符集(字符编码)、日期格式、数字格式、
//货币格式（货币符号、国际货币码）、字符处理(字符分类)，字符比较(字符排序)等等。

//设置当前的区域选项
char *setlocale(int category, const char * locale);

