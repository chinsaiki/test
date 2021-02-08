/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 24日 星期一 08:52:25 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:19 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:28 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 09:02:19 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:53:59 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:12 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:50 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:26:57 CST. */
/**
 *	互斥锁的属性
 */

#include <pthread.h>

/**
 *	互斥锁的初始化和摧毁函数
 */


int pthread_mutex_init(pthread_mutex_t *mptr, 
						pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mptr);

// https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/mutex_options


//当pthread_mutex_t使用标准属性初始化对象时，它将创建一个私有的，非递归的，非健壮的和非优先级继承的互斥量。

//高级互斥选项
//
//为了定义互斥锁的任何其他功能，您将需要创建一个pthread_mutexattr_t对象。该对象将存储定义的futex属性。
void highlevel_mutex_option() {
    //创建互斥对象：
    pthread_mutex_t my_mutex ;

    pthread_mutexattr_t my_mutex_attr ;

    pthread_mutexattr_init(&my_mutex_attr);

    //共享和专用互斥锁：
    //共享互斥锁可以在进程之间使用，但是它们会产生更多的开销。
    pthread_mutexattr_setpshared(&my_mutex_attr，PTHREAD_PROCESS_SHARED);


    //实时优先级继承：
    //通过使用优先级继承可以避免优先级反转问题。
    pthread_mutexattr_setprotocol(&my_mutex_attr，PTHREAD_PRIO_INHERIT);

    //健壮的互斥锁：
    //所有者死亡时会释放健壮的互斥锁，但是这也会带来高昂的间接费用。_NP此字符串中的表示此选项是非POSIX或不可移植的。
    pthread_mutexattr_setrobust_np(&my_mutex_attr，PTHREAD_MUTEX_ROBUST_NP);

    //互斥量初始化：
    //设置属性后，使用这些属性初始化互斥量。
    pthread_mutex_init(&my_mutex，＆my_mutex_attr);

    //清理属性对象：
    //创建互斥锁之后，可以保留属性对象以初始化更多相同类型的互斥锁，也可以清理它。互斥量在任何情况下均不受影响。要清除属性对象，请使用_destroy命令。
    pthread_mutexattr_destroy(&my_mutex_attr);
//互斥锁现在可以正常运行pthread_mutex，并且可以照常进行锁定，解锁和销毁。
}
