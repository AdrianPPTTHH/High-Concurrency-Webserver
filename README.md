# C++ webserver
C++  高性能Webserver （线程池、EPOLL、消息队列、日志、缓冲、heaptimer）



## 注意事项

**设置防火墙（允许端口接收tcp的流量）**
sudo iptables -L
sudo iptables -A INPUT -p tcp --dport 8876 -j ACCEPT

**安装mysql库**
（ mysql连接库 #include <mysql/mysql.h> )
sudo apt-get install libmysqlclient-dev
sudo apt-get install default-libmysqlclient-dev

添加用户:
 //%是通配符 表示所有地址连接webserver都可以 也可以写localhost
CREATE USER 'web_admin'@'%' IDENTIFIED BY 'password';

赋予权限:
GRANT ALL PRIVILEGES ON webserver.* TO 'web_admin'@'%';

刷新:
FLUSH PRIVILEGES;
