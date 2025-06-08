/*
 * SPDX-License-Identifier:GPL-2.0-or-later
*/

#ifndef _IPQ40XX_AP_DK01_1_C2
#define _IPQ40XX_AP_DK01_1_C2

#include <configs/ipq40xx_cdp.h>


#define CONFIG_QCA_MMC

#ifdef CONFIG_QCA_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_EFI_PARTITION
#define CONFIG_GENERIC_MMC
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV  0
#endif


#define MTDPARTS_DEFAULT	"mtdparts=nand2:0x1E40000@0x1C0000(firmware)"
#define MTDIDS_DEFAULT		"nand2=nand2"

#define CONFIG_FACTORY_IMG_FILENAME    "habanero.bin"

#define CONFIG_EXTRA_ENV_SETTINGS							\
	"bootcmd=run setup && run bootlinux\0"						\
	"setup=partname=1 && setenv bootargs ${args_common}\0"				\
	"args_common=rootfstype=squashfs\0"						\
	"bootlinux=run boot0 boot1 boot2 boot3 boot4 || reset\0"				\
	"boot0=usb_boot_file\0"								\
	"boot1=echo Booting from partition: ${partname}\0"				\
	"boot2=nand device 2\0"								\
	"boot3=nboot firmware\0"							\
	"boot4=bootm\0"									\
	"do_recovery=run rec1 rec2 rec3 rec4; reset\0"					\
	"rec1=echo Doing firmware recovery!\0"						\
	"rec2=sleep 2 && tftpboot ${tftp_loadaddr} ${recovery_file}\0"			\
	"rec3=nand device 2 && nand erase.part firmware\0"				\
	"rec4=nand write ${fileaddr} firmware ${filesize}\0"				\
	"tftp_loadaddr=0x84000000\0"							\
	"recovery_file=fwupdate.bin\0"							\
	"mtdparts=" MTDPARTS_DEFAULT "\0"						\

#endif /* _IPQ40XX_AP_DK01_1_C2 */
