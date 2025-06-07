## 编译

首先clone uboot代码uboot-ipq40xx：
```
git clone https://github.com/1980490718/uboot-ipq40xx.git
```
然后通过clone sdk来获得toolchain：
```
git clone https://github.com/1980490718/openwrt-sdk-ipq806x-qsdk53
```
修改顶层Makefile的STAGING_DIR，指向openwrt-sdk-ipq806x/staging_dir，然后执行：
```
make
```
或者修改顶层的build.sh中的第47行的STAGING_DIR，指向openwrt-sdk-ipq806x/staging_dir，然后执行：
```
./build.sh cdp
```
或者：
```
./build.sh ap4220
```
## 生成关系
```
make
```
生成的uboot为：bin/openwrt-ipq40xx-u-boot-stripped.elf

```
./build.sh cdp
```

生成的uboot为：bin/openwrt-cdp-u-boot-stripped.elf

```
./build.sh ap4220
```

生成的uboot为：bin/openwrt-ap4220-u-boot-stripped.elf

## web升级

1. 将设备的LAN口通过网线接到电脑，并设置电脑IP为192.168.1.x
2. 按住RESET键上电，并保持3s以上后松开即可。
3. 在浏览器（推荐Google浏览器）输入：
访问http://192.168.1.1来升级固件,或者点击索引菜单中的其他项目进行升级或者操作。