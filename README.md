# ARP POISONING

### Instructions

```bash
$ mkdir bin
$ make
$ sudo ./bin/arpoison 192.168.0.1 192.168.0.2 enp0s3
```

first argument is gateway's IP
second argument is victim's IP
third argument is interface name