/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef VCM_TYPES_H
#define VCM_TYPES_H

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/genalloc.h>
#include <linux/vcm_alloc.h>
#include <linux/list.h>

/*
 * Reservation Attributes
 *
 * Used in vcm_reserve(), vcm_reserve_at(), vcm_set_res_attr() and
 * vcm_reserve_bound().
 *
 *	VCM_READ	Specifies that the reservation can be read.
 *	VCM_WRITE	Specifies that the reservation can be written.
 *	VCM_EXECUTE	Specifies that the reservation can be executed.
 *	VCM_USER	Specifies that this reservation is used for
 *			userspace access.
 *	VCM_SUPERVISOR	Specifies that this reservation is used for
 *				supervisor access.
 *	VCM_SECURE	Specifies that the target of the reservation is
 *			secure. The usage of this setting is TBD.
 *
 *	Caching behavior as a 4 bit field:
 *		VCM_NOTCACHED		The VCM region is not cached.
 *		VCM_INNER_WB_WA		The VCM region is inner cached
 *					and is write-back and write-allocate.
 *		VCM_INNER_WT_NWA	The VCM region is inner cached and is
 *					write-through and no-write-allocate.
 *		VCM_INNER_WB_NWA	The VCM region is inner cached and is
 *					write-back and no-write-allocate.
 *		VCM_OUTER_WB_WA		The VCM region is outer cached and is
 *					write-back and write-allocate.
 *		VCM_OUTER_WT_NWA	The VCM region is outer cached and is
 *					write-through and no-write-allocate.
 *		VCM_OUTER_WB_NWA	The VCM region is outer cached and is
 *					write-back and no-write-allocate.
 *		VCM_WB_WA		The VCM region is cached and is write
 *					-back and write-allocate.
 *		VCM_WT_NWA		The VCM region is cached and is write
 *					-through and no-write-allocate.
 *		VCM_WB_NWA		The VCM region is cached and is write
 *					-back and no-write-allocate.
 */

/* Order of alignment (power of 2). Ie, 12 = 4k, 13 = 8k, 14 = 16k
 * Alignments of less than 1MB on buffers of size 1MB or greater should be
 * avoided. Alignments of less than 64KB on buffers of size 64KB or greater
 * should be avoided. Strictly speaking, it will work, but will result in
 * suboptimal performance, and a warning will be printed to that effect if
 * VCM_PERF_WARN is enabled.
 */
#define VCM_ALIGN_SHIFT		10
#define VCM_ALIGN_MASK		0x1F
#define VCM_ALIGN_ATTR(order) 	(((order) & VCM_ALIGN_MASK) << VCM_ALIGN_SHIFT)

#define VCM_ALIGN_DEFAULT	0
#define VCM_ALIGN_4K		(VCM_ALIGN_ATTR(12))
#define VCM_ALIGN_8K		(VCM_ALIGN_ATTR(13))
#define VCM_ALIGN_16K		(VCM_ALIGN_ATTR(14))
#define VCM_ALIGN_32K		(VCM_ALIGN_ATTR(15))
#define VCM_ALIGN_64K		(VCM_ALIGN_ATTR(16))
#define VCM_ALIGN_128K		(VCM_ALIGN_ATTR(17))
#define VCM_ALIGN_256K		(VCM_ALIGN_ATTR(18))
#define VCM_ALIGN_512K		(VCM_ALIGN_ATTR(19))
#define VCM_ALIGN_1M		(VCM_ALIGN_ATTR(20))
#define VCM_ALIGN_2M		(VCM_ALIGN_ATTR(21))
#define VCM_ALIGN_4M		(VCM_ALIGN_ATTR(22))
#define VCM_ALIGN_8M		(VCM_ALIGN_ATTR(23))
#define VCM_ALIGN_16M		(VCM_ALIGN_ATTR(24))
#define VCM_ALIGN_32M		(VCM_ALIGN_ATTR(25))
#define VCM_ALIGN_64M		(VCM_ALIGN_ATTR(26))
#define VCM_ALIGN_128M		(VCM_ALIGN_ATTR(27))
#define VCM_ALIGN_256M		(VCM_ALIGN_ATTR(28))
#define VCM_ALIGN_512M		(VCM_ALIGN_ATTR(29))
#define VCM_ALIGN_1GB		(VCM_ALIGN_ATTR(30))


#define VCM_CACHE_POLICY	(0xF << 0)
#define VCM_READ		(1UL << 9)
#define VCM_WRITE		(1UL << 8)
#define VCM_EXECUTE		(1UL << 7)
#define VCM_USER		(1UL << 6)
#define VCM_SUPERVISOR		(1UL << 5)
#define VCM_SECURE		(1UL << 4)
#define VCM_NOTCACHED		(0UL << 0)
#define VCM_WB_WA		(1UL << 0)
#define VCM_WB_NWA		(2UL << 0)
#define VCM_WT			(3UL << 0)


/*
 * Physical Allocation Attributes
 *
 * Used in vcm_phys_alloc().
 *
 *	Alignment as a power of 2 starting at 4 KB. 5 bit field.
 *	1 = 4KB, 2 = 8KB, etc.
 *
 *			Specifies that the reservation should have the
 *			alignment specified.
 *
 *	VCM_4KB		Specifies that the reservation should use 4KB pages.
 *	VCM_64KB	Specifies that the reservation should use 64KB pages.
 *	VCM_1MB		specifies that the reservation should use 1MB pages.
 *	VCM_ALL		Specifies that the reservation should use all
 *			available page sizes.
 *	VCM_PHYS_CONT	Specifies that a reservation should be backed with
 *			physically contiguous memory.
 *	VCM_COHERENT	Specifies that the reservation must be kept coherent
 *			because it's shared.
 */

#define VCM_4KB			(1UL << 5)
#define VCM_64KB		(1UL << 4)
#define VCM_1MB			(1UL << 3)
#define VCM_ALL			(1UL << 2)
#define VCM_PAGE_SEL_MASK       (0xFUL << 2)
#define VCM_PHYS_CONT		(1UL << 1)
#define VCM_COHERENT		(1UL << 0)


#define SHIFT_4KB               (12)

#define ALIGN_REQ_BYTES(attr) (1UL << (((attr & VCM_ALIGNMENT_MASK) >> 6) + 12))
/* set the alignment in pow 2, 0 = 4KB */
#define SET_ALIGN_REQ_BYTES(attr, align) \
	((attr & ~VCM_ALIGNMENT_MASK) | ((align << 6) & VCM_ALIGNMENT_MASK))

/*
 * Association Attributes
 *
 * Used in vcm_assoc(), vcm_set_assoc_attr().
 *
 * 	VCM_USE_LOW_BASE	Use the low base register.
 *	VCM_USE_HIGH_BASE	Use the high base register.
 *
 *	VCM_SPLIT		A 5 bit field that defines the
 *				high/low split.  This value defines
 *				the number of 0's left-filled into the
 *				split register. Addresses that match
 *				this will use VCM_USE_LOW_BASE
 *				otherwise they'll use
 *				VCM_USE_HIGH_BASE. An all 0's value
 *				directs all translations to
 *				VCM_USE_LOW_BASE.
 */

#define VCM_SPLIT		(1UL << 3)
#define VCM_USE_LOW_BASE	(1UL << 2)
#define VCM_USE_HIGH_BASE	(1UL << 1)


/*
 * External VCMs
 *
 * Used in vcm_create_from_prebuilt()
 *
 * Externally created VCM IDs for creating kernel and user space
 * mappings to VCMs and kernel and user space buffers out of
 * VCM_MEMTYPE_0,1,2, etc.
 *
 */
#define VCM_PREBUILT_KERNEL		1
#define VCM_PREBUILT_USER		2

/**
 * enum memtarget_t - A logical location in a VCM.
 *
 * VCM_START	Indicates the start of a VCM_REGION.
 */
enum memtarget_t {
	VCM_START
};


/**
 * enum memtype_t - A logical location in a VCM.
 *
 * VCM_MEMTYPE_0	Generic memory type 0
 * VCM_MEMTYPE_1	Generic memory type 1
 * VCM_MEMTYPE_2	Generic memory type 2
 *
 * A memtype encapsulates a platform specific memory arrangement. The
 * memtype needn't refer to a single type of memory, it can refer to a
 * set of memories that can back a reservation.
 *
 */
enum memtype_t {
	VCM_INVALID,
	VCM_MEMTYPE_0,
	VCM_MEMTYPE_1,
	VCM_MEMTYPE_2,
};


/**
 * vcm_handler - The signature of the fault hook.
 * @dev_id	The device id of the faulting device.
 * @data	The generic data pointer.
 * @fault_data	System specific common fault data.
 *
 * The handler should return 0 for success. This indicates that the
 * fault was handled. A non-zero return value is an error and will be
 * propagated up the stack.
 */
typedef int (*vcm_handler)(size_t dev_id, void *data, void *fault_data);


enum vcm_type {
	VCM_DEVICE,
	VCM_EXT_KERNEL,
	VCM_EXT_USER,
	VCM_ONE_TO_ONE,
};


/**
 * vcm - A Virtually Contiguous Memory region.
 * @start_addr	The starting address of the VCM region.
 * @len 	The len of the VCM region. This must be at least
 *		vcm_min() bytes.
 */
struct vcm {
	enum vcm_type type;

	size_t start_addr;
	size_t len;

	size_t dev_id; /* opaque device control */

	/* allocator dependent */
	struct gen_pool *pool;

	struct list_head res_head;

	/* this will be a very short list */
	struct list_head assoc_head;
};

/**
 * avcm - A VCM to device association
 * @vcm		The VCM region of interest.
 * @dev_id	The device to associate the VCM with.
 * @attr	See 'Association Attributes'.
 */
struct avcm {
	struct vcm *vcm_id;
	size_t dev_id;
	uint32_t attr;

	struct list_head assoc_elm;

	int is_active; /* is this particular association active */
};

/**
 * bound - A boundary to reserve from in a VCM region.
 * @vcm		The VCM that needs a bound.
 * @len		The len of the bound.
 */
struct bound {
	struct vcm *vcm_id;
	size_t len;
};


/**
 * physmem - A physical memory allocation.
 * @memtype	The memory type of the VCM region.
 * @len		The len of the physical memory allocation.
 * @attr 	See 'Physical Allocation Attributes'.
 *
 */

struct physmem {
	enum memtype_t memtype;
	size_t len;
	uint32_t attr;

	struct phys_chunk alloc_head;

	/* if the physmem is cont then use the built in VCM */
	int is_cont;
	struct res *res;
};

/**
 * res - A reservation in a VCM region.
 * @vcm		The VCM region to reserve from.
 * @len		The length of the reservation. Must be at least vcm_min()
 *		bytes.
 * @attr	See 'Reservation Attributes'.
 */
struct res {
	struct vcm *vcm_id;
	struct physmem *physmem_id;
	size_t len;
	uint32_t attr;

	/* allocator dependent */
	size_t alignment_req;
	size_t aligned_len;
	unsigned long ptr;
	size_t aligned_ptr;

	struct list_head res_elm;


	/* type VCM_EXT_KERNEL */
	struct vm_struct *vm_area;
	int mapped;
};

extern int chunk_sizes[NUM_CHUNK_SIZES];

#endif /* VCM_TYPES_H */
