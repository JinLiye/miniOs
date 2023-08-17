/**
 * 功能：16位与32位的启动混合代码
 */
	#include "os.h"

	// 声明本地以下符号是全局的，在其它源文件中可以访问
	.global _start

	// 指定以下的代码生成16位的机器指令，这样才能在启动时的实模式下运行
  	.code16

	// 以下是代码区
 	.text
_start:
	mov $0,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov %ax,%gs
	mov %ax,%fs
	mov $_start, %esp


read_self_all:
	mov $_start_32, %bx	//读取到的内存地址
	mov $0x2, %cx		//ch:磁道号,c1起始扇区号
	mov $0x0240, %ax	//ah:0x02读磁盘命令, al:40 读64个扇区
	mov $0x80, %dx		//磁头号, 第一个磁盘
	int $0x13
	jc read_self_all

	jmp .
	.org 0x1fe				//510
	.byte 0x55,0xaa			//unsinged char

_start_32:
	.fill 64*1024, 1, 0x22

