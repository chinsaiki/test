
# 1. 参考

1. [《CentOS Linux实时性配置要点》](https://github.com/Rtoax/MoreThanDocs/blob/master/Linux/RealTime/CentOS%20Linux%E5%AE%9E%E6%97%B6%E6%80%A7%E9%85%8D%E7%BD%AE%E8%A6%81%E7%82%B9.md)
2. [《红帽 RedHat Linux实时内核配置要点全面分析》](https://rtoax.blog.csdn.net/article/details/113782050)

# 2. 依赖

## 2.1. 安装 rt-tests

从官网下载或者直接在本仓库中编译安装：

```bash
cd  WHERE/YOU/PUT/TEST/test/linux/real-time/rt-tests-1.10
make 
sudo make install
```

## 2.2. 安装 rteval

