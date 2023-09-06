/**
 * 功能：16位与32位的启动混合代码
 */
	#include "os.h"

	// 声明本地以下符号是全局的，在其它源文件中可以访问
	.global _start, timer_int

	// 指定以下的代码生成16位的机器指令，这样才能在启动时的实模式下运行
  	.code16

	// 以下是代码区
 	.text
	// 声明os_init在外部定义
	.extern os_init
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

	cli					//关中断
	lgdt gdt_desc		//读相应数据到gdtr, 注：加中括号不识别
	lidt idt_desc		//读相应数据到idtr
	mov $1, %eax
	lmsw %ax			//讲CR0寄存器PE设为1, 表示打开保护模式
	jmp $KERNEL_COOE_SEG, $_start_32			// 这个跳转会将CS(代码段寄存器)设为8, 跳转, 之后CPU进入保护模式

	
	.org 0x1fe				//510
	.byte 0x55,0xaa			//unsinged char

	.code32					//生成32位指令
	.text

_start_32:
	mov $KERNEL_DATA_SEG ,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov %ax,%gs
	mov %ax,%fs
	mov $_start, %esp

	call os_init

	//将cr3指向页目录表
	mov $pg_dir, %eax
	mov %eax, %cr3

	//将cr4寄存器第四位置1，该位控制4Mb映射关系
	mov %cr4, %eax
	orl $(1<<4) ,%eax
	mov %eax, %cr4

	mov %cr0, %eax
	//控制分页机制的位
	orl $(1<<31) ,%eax
	mov %eax, %cr0

	sti //开中断

	jmp .

//中断处理函数
timer_int:
	push %ds
	pusha

	mov $0x20,%al
	outb %al,$0x02

	popa
	pop %ds
	iret


gdt_desc:
	.word (256*8) - 1		//-1: 16位界限
	.long gdt_table

idt_desc:
	.word (256*8) - 1		//-1: 16位界限
	.long idt_table


