ENTRY(_start)

SECTIONS
{
	. = 2M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

    .pagedir BLOCK(4K) : ALIGN(4K)
	{
		page_dir = .;
		. += 4K;
	}

    .pagetables BLOCK(4K) : ALIGN(4K)
    {
        page_tables = .;
        . += 4M;
        endkernel = .;
    }
}
