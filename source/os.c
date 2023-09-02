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
// 定义页目录表
uint32_t pg_dir[1024] __attribute__((aligned(4096))) = {
    [0] = (0) | PDE_P | PDE_W | PDE_U | PDE_PS,
};

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

void os_init(void)
{
    pg_dir[MAP_ADDR >> 22] = (uint32_t)page_table | PDE_P | PDE_W | PDE_U;
    page_table[(MAP_ADDR >> 12) & 0x3FF] = (uint32_t)map_phy_buffer | PDE_P | PDE_W | PDE_U;
}