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
	jmp .

	.org 0x1fe				//510
	; 在第一个扇区后添加0xaa.0x55，使得bios将其加载到0x7c00
	.byte 0x55,0xaa			//unsinged char

