// SPDX-License-Identifier: GPL-2.0-only
/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 * ----------------------------------------------------------------------- */

/*
 * Prepare the machine for transition to protected mode.
 */

#include "boot.h"
#include <asm/segment.h>

/*
 * Invoke the realmode switch hook if present; otherwise
 * disable all interrupts.
 */
static void realmode_switch_hook(void)
{
  // 如果存在realmode_switch回调函数则调用它来禁止NMI中断
	if (boot_params.hdr.realmode_swtch) {
		asm volatile("lcallw *%0"
			     : : "m" (boot_params.hdr.realmode_swtch)
			     : "eax", "ebx", "ecx", "edx");
	} else {
    // 禁止中断设置
		asm volatile("cli"); // 清除中断标志，外部中断就被禁止了
		outb(0x80, 0x70); /* Disable NMI */ // 通过写 0x80 进 CMOS 地址寄存器 0x70来禁止NMI中断
		io_delay(); // 进行了短暂的延时以等待 I/O 操作完成
	}
}

/*
 * Disable all interrupts at the legacy PIC.
 */
static void mask_all_interrupts(void)
{
	outb(0xff, 0xa1);	/* Mask all interrupts on the secondary PIC */
	io_delay();
	outb(0xfb, 0x21);	/* Mask all but cascade on the primary PIC */
	io_delay();
}

/*
 * Reset IGNNE# if asserted in the FPU.
 */
static void reset_coprocessor(void)
{
	outb(0, 0xf0);
	io_delay();
	outb(0, 0xf1);
	io_delay();
}

/*
 * Set up the GDT
 */

struct gdt_ptr {
	u16 len;
	u32 ptr;
} __attribute__((packed)); // packed表示不对齐优化

static void setup_gdt(void)
{
	/* There are machines which are known to not boot with the GDT
	   being 8-byte unaligned.  Intel recommends 16 byte alignment. */
  /*
   * 使用 boot_gdt 数组定义了需要引入 GDTR 寄存器的段描述符信息
   * aligned(16)表示16字节对齐
   */
	static const u64 boot_gdt[] __attribute__((aligned(16))) = {
		/* CS: code, read/execute, 4 GB, base 0 */
    // GDT_ENTRY_BOOT_CS是2，所以第0和1项是空项
    /*
     * GDT_ENTRY接受三个参数：
     * * 段描述符的标志 - 0xc09b
     * * 基地址 - 0
     * * 段长度 - 0xfffff
     */
		[GDT_ENTRY_BOOT_CS] = GDT_ENTRY(0xc09b, 0, 0xfffff),
		/* DS: data, read/write, 4 GB, base 0 */
		[GDT_ENTRY_BOOT_DS] = GDT_ENTRY(0xc093, 0, 0xfffff),
		/* TSS: 32-bit tss, 104 bytes, base 4096 */
		/* We only have a TSS here to keep Intel VT happy;
		   we don't actually use it for anything. */
    // TSS 段并不会被使用到，存在的唯一目的就是让 Intel 处理器能够正确进入保护模式
		[GDT_ENTRY_BOOT_TSS] = GDT_ENTRY(0x0089, 4096, 103),
	};
	/* Xen HVM incorrectly stores a pointer to the gdt_ptr, instead
	   of the gdt_ptr contents.  Thus, make it static so it will
	   stay in memory, at least long enough that we switch to the
	   proper kernel GDT. */
	static struct gdt_ptr gdt;

  // 设置gdt的起始地址和长度
	gdt.len = sizeof(boot_gdt)-1;
	gdt.ptr = (u32)&boot_gdt + (ds() << 4);

  // 执行 lgdtl 指令将 GDT 信息写入 GDTR 寄存器
	asm volatile("lgdtl %0" : : "m" (gdt));
}

/*
 * Set up the IDT
 */
static void setup_idt(void)
{
  /*
   * 由于 null_idt 没有设定中断描述符表的长度（长度为 0 ），
   * 所以这段指令执行之后，实际上没有任何中断调用被设置成功（所有中断调用都是空的）
   */
	static const struct gdt_ptr null_idt = {0, 0};
	asm volatile("lidtl %0" : : "m" (null_idt));
}

/*
 * Actual invocation sequence
 */
void go_to_protected_mode(void)
{
	/* Hook before leaving real mode, also disables interrupts */
	realmode_switch_hook();

	/* Enable the A20 gate */
  // 激活A20地址线
	if (enable_a20()) {
		puts("A20 gate not responding, unable to boot...\n");
		die();
	}

	/* Reset coprocessor (IGNNE#) */
  // 通过将 0 写入 I/O 端口 0xf0 和 0xf1 以复位数字协处理器
	reset_coprocessor();

	/* Mask all interrupts in the PIC */
  /*
   * 这个函数调用屏蔽了从中断控制器 (注：中断控制器的原文是 Programmable Interrupt Controller) 的所有中断，
   * 和主中断控制器上除IRQ2以外的所有中断（IRQ2是主中断控制器上的级联中断，
   * 所有从中断控制器的中断将通过这个级联中断报告给 CPU ）。
   */
	mask_all_interrupts();

	/* Actual transition to protected mode... */
  // 设置中断描述符表IDT
	setup_idt();
  // 设置全局描述符表GDT
	setup_gdt();
  /*
   * 切换进入保护模式，传入了两个参数：保护模式代码的入口（eax）和boot_params 结构的地址（edx）
   * 该入口是文件boot/compressed/head_64.S
   */
	protected_mode_jump(boot_params.hdr.code32_start,
			    (u32)&boot_params + (ds() << 4));
}
