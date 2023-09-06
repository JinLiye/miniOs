/**
 * 功能：32位代码，完成多任务的运行
 */
#include "os.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// 表项存在
#define PDE_P (1 << 0)
// 可读
#define PDE_W (1 << 1)
// 用户模式
#define PDE_U (1 << 2)
// 以4MB做映射
#define PDE_PS (1 << 7)
// 需要映射的地址
#define MAP_ADDR 0x80000000

uint8_t map_phy_buffer[4096] __attribute__((aligned(4096))) = {0x36};
;
// 定义页目录表
uint32_t pg_dir[1024] __attribute__((aligned(4096))) = {
    [0] = (0) | PDE_P | PDE_W | PDE_U | PDE_PS,
};

// IDT表结构
struct
{
    uint16_t offset_l, selector, attr, offset_h;
} idt_table[256] __attribute__((aligned(8))) = {1};

// 定义二级页表
static uint32_t page_table[1024] __attribute__((aligned(4096))) = {PDE_U};

// 定义GDT表结构
struct
{
    uint16_t limit_l, base_l, basehl_attr, base_limit;
} gdt_table[256] __attribute__((aligned(8))) = {
    [KERNEL_COOE_SEG / 8] = {0xffff, 0x0000, 0x9a00, 0x00cf},
    [KERNEL_DATA_SEG / 8] = {0xffff, 0x0000, 0x9200, 0x00cf},
};

void outb(uint8_t data, uint16_t port)
{
    __asm__ __volatile__("outb %[v], %[p]" ::[p] "d"(port), [v] "a"(data));
}

void timer_int(void);
void os_init(void)
{
    // 初始化8259主片从片
    outb(0x11, 0x20);
    outb(0x11, 0xA0);
    outb(0x20, 0x21);
    outb(0x28, 0xA1);
    outb(1 << 2, 0x21);
    outb(2, 0xa1);
    // 设置工作模式
    outb(0x1, 0x21);
    outb(0x1, 0xA1);
    outb(0xfe, 0x21);
    outb(0xff, 0xA1);

    // 配置8253
    // 100ms产生一次中断
    // int tmo = 1193180 / 100;
    int tmo = (1193180);
    outb(0x36, 0x43);
    outb((uint8_t)tmo, 0x40);
    outb(tmo >> 8, 0x40);

    idt_table[0x20].offset_l = (uint32_t)timer_int & 0xFFFF;
    idt_table[0x20].offset_h = (uint32_t)timer_int >> 16;
    idt_table[0x20].selector = KERNEL_COOE_SEG; // 选择子选代码段
    idt_table[0x20].attr = 0x8E00;              // 含义见intel编程文档

    pg_dir[MAP_ADDR >> 22] = (uint32_t)page_table | PDE_P | PDE_W | PDE_U;
    page_table[(MAP_ADDR >> 12) & 0x3FF] = (uint32_t)map_phy_buffer | PDE_P | PDE_W | PDE_U;
}
