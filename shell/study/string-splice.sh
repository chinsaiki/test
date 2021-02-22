#!/bin/bash
# 字符串分割 方法
# 荣涛 2021年2月22日

#${var#*/}
#${var##*/}
#${var%/*}
#${var%%/*}
#${var:start:len}
#${var:start}
#${var:0-start:len}
#${var:0-start}

#使用#和%进行截取字符串的方法在文章上面已经详细介绍了，这里就不做过多说明了。
#${file#*/}：  表示删除掉第一条 / 及其左边的字符串，即结果为：dir1/dir2/dir3/my.file.txt
#${file##*/}： 表示删除掉最后一条 / 及其左边的字符串，即结果为：my.file.txt
#${file#*.}：  表示删除掉第一个 .  及其左边的字符串，即结果为：file.txt
#${file##*.}： 表示删除掉最后一个 .  及其左边的字符串，即结果为：txt
#${file%/*}：  表示删除掉最后一条 / 及其右边的字符串，即结果为：/dir1/dir2/dir3
#${file%%/*}： 表示删除掉第一条 / 及其右边的字符串，即结果为：(空值)
#${file%.*}：  表示删除掉最后一个 .  及其右边的字符串，即结果为：/dir1/dir2/dir3/my.file
#${file%%.*}： 表示删除掉第一个 .  及其右边的字符串，即结果为：/dir1/dir2/dir3/my

#获得字符串的长度
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
length=${#str}
echo "length: [${length}]"


## 最小限度从前面截取word
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
#分割符为'/'
substr=${str#*/}
echo "substr: [${substr}]"

### 最大限度从前面截取word
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
#分割符为'/'
substr=${str##*/}
echo "substr : [${substr}]"

#% 最小限度从后面截取word
#!/bin/bash
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
substr=${str%/*}
echo "substr : [${substr}]"

#%% 最大限度从后面截取word
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
substr=${str%%/*}
echo "substr : [${substr}]"

#指定从左边第几个字符开始以及子串中字符的个数
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
#其中的 0 表示左边第一个字符开始，7 表示子字符的总个数。
substr=${str:0:7}
echo "substr : [${substr}]"

#从左边第几个字符开始一直到结束
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
#其中的7表示左边第8个字符开始  (如果是${str:7:5},就表示从左边第8个字符开始截取，截取5个字符)
substr=${str:7}
echo "substr : [${substr}]"


#从右边第几个字符开始以及字符的个数
#语法：${var:0-start:len}；即${var:0-8,3} 和 ${var:2-10:3} 和 ${var:5:13:3} 是一样的，即从右边第8个开始截取，截取3个字符。 即8-0=10-2=13-5=8 
#!/bin/bash
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
#其中的 0-23 表示右边算起第23个字符开始，5 表示字符的个数
substr=${str:0-23:5}
echo "substr : [${substr}]"


#从右边第几个字符开始一直到结束
#!/bin/bash
str="http://www.kevin.com/shibo/anhuigrace"
echo "string: [${str}]"
 
#其中的 0-6 表示右边算起第6个字符开始
substr=${str:0-6}
echo "substr : [${substr}]"





