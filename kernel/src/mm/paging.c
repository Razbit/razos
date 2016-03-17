/* This file is a part of the RazOS project
 *
 * paging.c -- internals of the paging system
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <asm/x86.h>
#include <errno.h>
#include <kmalloc.h>
#include <string.h>
#include <console.h>
#include <panic.h>

#include "paging.h"
#include "kvm.h"

static uint32_t identity_end = 0;
struct page_dir_t* cur_page_dir = NULL;

/* We use a bitmap to keep track of frames */
#define FRAMES_FROM_BYTES(b) ((b)/PAGE_SIZE/32)
static uint32_t frames[FRAMES_FROM_BYTES(0xFFFFFFFF)];
static uint32_t memsize;

#define frame_index(a) ((a)/32)  /* Index in frames[] */
#define frame_offset(a) ((a)%32) /* Bit number in the uint32_t */

/* Bitmap fiddling functions */
/* Mark frame as used */
static inline void set_frame(uint32_t frame)
{
	frame >>= 12; /* Remove offset from physical address */
	frames[frame_index(frame)] |= (1 << frame_offset(frame));
}

/* Mark frame as free */
static inline void clear_frame(uint32_t frame)
{
	frame >>= 12; /* Remove offset from physical address */
	frames[frame_index(frame)] &= ~(1 << frame_offset(frame));
}

/* Find a free frame, return it's address */
static inline uint32_t find_free_frame()
{
	uint32_t i, j;
	for (i = 0; i < frame_index(memsize); i++)
		if (frames[i] != 0xFFFFFFFF)
			for (j = 0; j < 32; j++)
				if ((frames[i] & ((uint32_t)1 << j)) == 0)
					return (i * 32 + j) << 12;

	/* If no free frame was found, return -1 */
	return (uint32_t)-1;
}

/* Allocate a frame, return its physical address */
uint32_t frame_alloc()
{
	uint32_t addr = find_free_frame();
	if (addr == (uint32_t)-1)
	{
		errno = ENOMEM;
		return 0;
	}

	set_frame(addr);
	return addr;
}

/* Free a frame. Takes physical frame address */
void frame_free(uint32_t addr)
{
	clear_frame(addr);
}

/* Flush the Translation Lookaside Buffer */
static inline void flush_tlb(uint32_t addr)
{
	__asm__ __volatile__("invlpg (%0)" :: "r"(addr) : "memory");
}

/* Enable paging */
static inline void enable_paging()
{
	set_cr0(CR0_PG);
}

/* Create a page directory */
struct page_dir_t* create_page_dir()
{
	struct page_dir_t* ret = \
		(struct page_dir_t*)kmalloc_pa(sizeof(struct page_dir_t));

	if (ret == NULL)
		return NULL;

	memset(ret, 0, sizeof(struct page_dir_t));

	return ret;
}

/* Clone a page directory */
void clone_page_dir(struct page_dir_t* new, struct page_dir_t* old)
{
	/* Link kernel page tables */
	for (size_t i = 0; i < KSTACK_BEGIN / (1024 * PAGE_SIZE - 1); i++)
	{
		new->tables[i] = old->tables[i];
		new->tables_phys[i] = old->tables_phys[i];
	}

	/* Copy user memory and kernel stack. */
	/* TODO: this takes a HUGE amount of time. Optimize frame_alloc? */
	for (size_t i = KSTACK_BEGIN; i < USTACK_END; i += PAGE_SIZE)
	{
		/* To prevent integer overflow */
		if (i < PAGE_SIZE)
			break;

		/* Check if page is present */
		uint32_t flags = page_flags(i, old);
		if ((flags & PF_PRES) == 0)
			continue;

		/* Allocate a frame and temporarily map it at NULL */
		uint32_t frame = frame_alloc();
		page_map(0, frame, PF_PRES | PF_RW, cur_page_dir);
		memcpy(NULL, (void*)i, PAGE_SIZE);
		page_map(0, 0, 0, cur_page_dir);
		page_map(i, frame, flags, new);
	}
}

/* Clear a page directory */
void clear_page_dir(struct page_dir_t* page_dir)
{
	/* Free each page table after the kernel memory */
	for (size_t i = KSTACK_BEGIN / (1023 * PAGE_SIZE); i < 1024; i++)
		if (page_dir->tables[i])
			kfree(page_dir->tables[i]);
}

uint32_t get_page_dir_phys(struct page_dir_t* page_dir)
{
	return get_phys((uint32_t)page_dir->tables_phys, page_dir);
}

/* Load the specified page directory to CR3 */
void set_page_dir(struct page_dir_t* page_dir)
{
	uint32_t phys_addr = get_page_dir_phys(page_dir);
	__asm__ __volatile__("mov %0, %%cr3" :: "r"(phys_addr) : "memory");
}

/* Map frame to address, set flags */
void* page_map(uint32_t addr, uint32_t frame, uint32_t flags, \
               struct page_dir_t* page_dir)
{
	addr &= ~(PAGE_SIZE-1); /* Round down */

	uint32_t tab_i = (addr >> 12) & 1023;
	uint32_t dir_i = (addr >> 22) & 1023;

	/* Create page table if it doesn't exist */
	if (page_dir->tables[dir_i] == NULL)
	{
		page_dir->tables[dir_i] = \
			(struct page_tab_t*)kmalloc_pa(sizeof(struct page_tab_t));
		if (page_dir->tables[dir_i] == NULL)
		{
			errno = ENOMEM;
			return NULL;
		}

		flush_tlb((uint32_t)(page_dir->tables[dir_i]));
		memset(page_dir->tables[dir_i], 0, PAGE_SIZE);

		/* Set page table flags */
		page_dir->tables_phys[dir_i].present = 1;
		page_dir->tables_phys[dir_i].rw = (flags & PF_RW)?1:0;
		page_dir->tables_phys[dir_i].user = (flags & PF_USER)?1:0;
		page_dir->tables_phys[dir_i].wt_caching = (flags & PF_WTC)?1:0;
		page_dir->tables_phys[dir_i].nocache = (flags & PF_NOC)?1:0;
		page_dir->tables_phys[dir_i].accessed = (flags & PF_ACC)?1:0;
		page_dir->tables_phys[dir_i].zero = 0;
		page_dir->tables_phys[dir_i].size = 0; /* Use 4K pages */
		page_dir->tables_phys[dir_i].global = (flags & PF_GLO)?1:0;
		page_dir->tables_phys[dir_i].table = \
			get_phys((uint32_t)page_dir->tables[dir_i], page_dir) >> 12;

		flush_tlb((uint32_t)(page_dir->tables[dir_i]));
	}

	/* Page flags */
	page_dir->tables[dir_i]->entry[tab_i].present = (flags & PF_PRES)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].rw = (flags & PF_RW)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].user = (flags & PF_USER)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].wt_caching = (flags & PF_WTC)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].nocache = (flags & PF_NOC)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].accessed = (flags & PF_ACC)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].dirty = (flags & PF_DIRTY)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].zero = 0;
	page_dir->tables[dir_i]->entry[tab_i].global = (flags & PF_GLO)?1:0;
	page_dir->tables[dir_i]->entry[tab_i].frame = frame >> 12;

	/* The table is writable if there is page that is writable */
	page_dir->tables_phys[dir_i].rw |= (flags & PF_RW)?1:0;

	flush_tlb(addr);

	return (void*)addr;
}

/* Convert virtual address to physical address */
uint32_t get_phys(uint32_t addr, struct page_dir_t* page_dir)
{
	uint32_t offset = addr & (PAGE_SIZE - 1);
	uint32_t tab_i = (addr >> 12) & 1023;
	uint32_t dir_i = (addr >> 22) & 1023;

	/* Check if the page table exists */
	if (page_dir->tables[dir_i] == NULL)
		return 0;

	/* Check if the page exists */
	if (page_dir->tables[dir_i]->entry[tab_i].present == 0)
		return 0;

	return (page_dir->tables[dir_i]->entry[tab_i].frame << 12) + offset;
}

/* Get page and page table flags */
uint32_t page_flags(uint32_t addr, struct page_dir_t* page_dir)
{
	uint32_t tab_i = (addr >> 12) & 1023;
	uint32_t dir_i = (addr >> 22) & 1023;

	uint32_t flags = 0;

	flags |= page_dir->tables_phys[dir_i].present << 0;
	flags |= page_dir->tables_phys[dir_i].rw << 1;
	flags |= page_dir->tables_phys[dir_i].user << 2;
	flags |= page_dir->tables_phys[dir_i].wt_caching << 3;
	flags |= page_dir->tables_phys[dir_i].nocache << 4;
	flags |= page_dir->tables_phys[dir_i].accessed << 5;
	flags |= page_dir->tables_phys[dir_i].zero << 6;
	flags |= page_dir->tables_phys[dir_i].size << 7;
	flags |= page_dir->tables_phys[dir_i].global << 8;

	flags <<= 16;

	/* Check if the page exists */
	if ((page_dir->tables[dir_i] != NULL) \
	    && ((flags & (PF_PRES << 16)) == (PF_PRES << 16)))
	{
		flags |= page_dir->tables[dir_i]->entry[tab_i].present << 0;
		flags |= page_dir->tables[dir_i]->entry[tab_i].rw << 1;
		flags |= page_dir->tables[dir_i]->entry[tab_i].user << 2;
		flags |= page_dir->tables[dir_i]->entry[tab_i].wt_caching << 3;
		flags |= page_dir->tables[dir_i]->entry[tab_i].nocache << 4;
		flags |= page_dir->tables[dir_i]->entry[tab_i].accessed << 5;
		flags |= page_dir->tables[dir_i]->entry[tab_i].dirty << 6;
		flags |= page_dir->tables[dir_i]->entry[tab_i].zero << 7;
		flags |= page_dir->tables[dir_i]->entry[tab_i].global << 8;
	}

	return flags;
}

/* These two figure out how much physical RAM we have. The information
 * resides in the multiboot info structure */
static size_t register_mem_region(struct memory_map* region)
{
	size_t ret = 0;

	for (size_t offset = 0; offset + PAGE_SIZE <= region->length_low; \
	     offset += PAGE_SIZE)
	{
		uint32_t addr = region->base_addr_low + offset;
		ret++;

		/* Free frames that are outside of identity-mapped region */
		if (addr < identity_end)
			continue;

		frame_free(addr);
	}

	return ret;
}

static size_t register_mem(struct multiboot_info* mb)
{
	struct memory_map* mmap = (void*)mb->mmap_addr;
	size_t ret = 0;
	for (size_t i=0; i < mb->mmap_length/sizeof(struct memory_map); i++)
	{
		if (mmap[i].type == 1) /* Memory map is available */
			ret += register_mem_region(mmap+i);
	}

	return ret;
}

static void identity_map()
{
	/* Round up to next page table boundary */
	if ((identity_end & (PAGE_SIZE * 1024 - 1)) != 0)
	{
		identity_end += PAGE_SIZE * 1024;
		identity_end &= ~(PAGE_SIZE * 1024 - 1);
	}

	/* Mark frame 0 as used so that we wont use it */
	set_frame(0);

	/* Identity-map the kernel area. Leave null-page unmapped */
	for (size_t i = PAGE_SIZE; i < identity_end; i += PAGE_SIZE)
	{
		set_frame(i);
		page_map(i, i, PF_PRES | PF_RW, cur_page_dir);
	}
}

/* Create the syscall/kernel stack */
static void create_kstack()
{
	for (size_t i = KSTACK_BEGIN; i < KSTACK_END; i += PAGE_SIZE)
		page_map(i, frame_alloc(), PF_PRES | PF_RW, cur_page_dir);
}

/* Grow identity-mapped region. If addr == 0, return current end */
void* set_alloc_start(void* addr)
{
	if ((uint32_t)addr > identity_end)
		identity_end = (uint32_t)addr;
	return (void*)identity_end;
}

/* Initialize paging */
void paging_init(struct multiboot_info* mb)
{
	identity_end = 0;
	memset(&frames[0], 0, sizeof(frames));

	extern uint32_t end_of_image; /* In linker.ld, boot.s */
	uint32_t initrd_end = *(uint32_t*)(mb->mods_addr+4);

	set_alloc_start(&end_of_image);
	set_alloc_start((uint32_t*)initrd_end);

	/* Get size of available memory. Round down to a multiple of
	 * 32 frames because of the bitmap */
	memsize = register_mem(mb) & ~31;

	kprintf("%u MiB available memory (0x%x pages)\n", \
	        memsize * PAGE_SIZE / 0x100000, memsize);

	cur_page_dir = create_page_dir();
	if (cur_page_dir == NULL)
		panic("Page directory could not be created");

	/* Create kernel page tables */
	for (size_t dir_i = 0; dir_i < KSTACK_END/(PAGE_SIZE*1023); dir_i++)
	{
		cur_page_dir->tables[dir_i] = \
			(struct page_tab_t*)kmalloc_pa(sizeof(struct page_tab_t));
		if (cur_page_dir->tables[dir_i] == NULL)
			panic("Page tables could not be created");

		memset(cur_page_dir->tables[dir_i], 0, PAGE_SIZE);

		/* Set page table flags */
		cur_page_dir->tables_phys[dir_i].present = 1;
		cur_page_dir->tables_phys[dir_i].rw = 1;
		cur_page_dir->tables_phys[dir_i].user = 0;
		cur_page_dir->tables_phys[dir_i].wt_caching = 0;
		cur_page_dir->tables_phys[dir_i].nocache = 0;
		cur_page_dir->tables_phys[dir_i].accessed = 0;
		cur_page_dir->tables_phys[dir_i].zero = 0;
		cur_page_dir->tables_phys[dir_i].size = 0; /* Use 4K pages */
		cur_page_dir->tables_phys[dir_i].global = 0;
		cur_page_dir->tables_phys[dir_i].table = \
			(uint32_t)(cur_page_dir->tables[dir_i]) >> 12;
	}

	identity_map();
	create_kstack();

	kvm_init((void*)identity_end);

	set_page_dir(cur_page_dir);
	enable_paging();
}
