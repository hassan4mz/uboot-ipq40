Compilation
First, clone the uboot-ipq40xx code:

git clone https://github.com/1980490718/uboot-ipq40xx.git  
Then, clone the SDK to get the toolchain:

git clone https://github.com/1980490718/openwrt-sdk-ipq806x-qsdk53  
Modify the STAGING_DIR in the top-level Makefile to point to openwrt-sdk-ipq806x/staging_dir, then run:

make  
Alternatively, modify line 47 of build.sh to set STAGING_DIR to openwrt-sdk-ipq806x/staging_dir, then run:

./build.sh cdp  
or

./build.sh ap4220  
The compiled U-Boot files are:

make → bin/openwrt-ipq40xx-u-boot-stripped.elf

./build.sh cdp → bin/openwrt-cdp-u-boot-stripped.elf

./build.sh ap4220 → bin/openwrt-ap4220-u-boot-stripped.elf

Web Firmware Upgrade
Connect the device’s LAN port to a PC via Ethernet and set the PC’s IP to 192.168.1.x.

Hold the RESET button while powering on the device, and keep it pressed for 3+ seconds before releasing.

Open a browser (recommended: Google Chrome) and visit:

http://192.168.1.1  
Follow the on-screen instructions to upgrade the firmware or access other functions from the menu.
