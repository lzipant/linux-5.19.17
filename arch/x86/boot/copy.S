/* SPDX-License-Identifier: GPL-2.0-only */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 * ----------------------------------------------------------------------- */

#include <linux/linkage.h>


/*
 * 该文件内的函数使用了 fastcall 调用规则，意味着所有的函数调用参数是通过 ax, dx, cx寄存器传入的，
 * 而不是传统的通过堆栈传入
 */

/*
 * Memory copy routines
 */

	.code16
	.text

SYM_FUNC_START_NOALIGN(memcpy)
  # 将si和di压入堆栈保存
	pushw	%si
	pushw	%di
  # di和si分别表示拷贝的目标和源的起始地址
	movw	%ax, %di
	movw	%dx, %si
	pushw	%cx
  # 除以4，说明以4字节为单位进行拷贝
	shrw	$2, %cx
  # 执行拷贝cx次
	rep; movsl
  # 恢复cx
	popw	%cx
  # 处理最后不足4字节的内容
	andw	$3, %cx
	rep; movsb
  # 恢复di和si
	popw	%di
	popw	%si
	retl
SYM_FUNC_END(memcpy)

SYM_FUNC_START_NOALIGN(memset)
	pushw	%di
	movw	%ax, %di
	movzbl	%dl, %eax
	imull	$0x01010101,%eax
	pushw	%cx
	shrw	$2, %cx
	rep; stosl
	popw	%cx
	andw	$3, %cx
	rep; stosb
	popw	%di
	retl
SYM_FUNC_END(memset)

SYM_FUNC_START_NOALIGN(copy_from_fs)
	pushw	%ds
	pushw	%fs
	popw	%ds
	calll	memcpy
	popw	%ds
	retl
SYM_FUNC_END(copy_from_fs)

SYM_FUNC_START_NOALIGN(copy_to_fs)
	pushw	%es
	pushw	%fs
	popw	%es
	calll	memcpy
	popw	%es
	retl
SYM_FUNC_END(copy_to_fs)
