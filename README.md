# cs144labs

混合学习 Stanford CS144 和 Berkeley CS168，并完成CS144教程内的作业

## check0
无法访问<http://cs144.keithw.org/hello>,改为了使用标准测试网站<http://example.com>

### webget.cc

#### 回车换行`\r\n`

#### Address类 
```
Address addr(host, "http"); 
// 把host查出IP地址，并把"http"翻译成端口80
```

#### TCPSocket类
```
TCPSocket sock; // 实例化一个对象
sock.connect(addr); // 建立连接
sock.write(字符串); // 发送文本
```

#### EOF与流式读取
```
while (!sock.eof()) {
    sock.read(buffer);
}
// 只要服务器没有关闭，就不断读取
```

### byte_stream

#### string_view
只读视窗，内部只含：一个指向原有内存的指针+数据的长度；相比普通string，零拷贝、零内存分配；如果返回普通string，C++必须在内存的堆（Heap）上申请新的空间