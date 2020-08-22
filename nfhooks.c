// nfhooks.c
#include <crash/defs.h>

static int get_field(unsigned long addr, char *name, char *field, void* buf)
{
	unsigned long off = MEMBER_OFFSET(name, field);

	if (!readmem(addr + off, KVADDR, buf, MEMBER_SIZE(name, field), name, FAULT_ON_ERROR))
		return 0;
	return 1;
}

void do_cmd(void)
{
	unsigned long ops_addr, owner_addr;
	unsigned long list_addr, base, next;
	char name[64];

	optind++;
	base = next = list_addr = htol(args[optind], FAULT_ON_ERROR, NULL);

	do {
		// 由于list就是nf_hook_ops的第一个字段，因此就不炫技了，强转即可。
		//get_field(list_addr - MEMBER_OFFSET("nf_hook_ops", "list"), "list_head", "next", &ops_addr);
		ops_addr = list_addr = next;
		get_field(ops_addr, "nf_hook_ops", "owner", &owner_addr);
		get_field(owner_addr, "module", "name", &name[0]);
		if (list_addr != base)
			fprintf(fp, "--%s\n", name);
		} while(get_field(list_addr, "list_head", "next", &next) && next != base);
}

static struct command_table_entry command_table[] = {
	{ "nfhooks", do_cmd, NULL, 0},
	{ NULL },
};

void __attribute__((constructor)) nfhooks_init(void)
{
	register_extension(command_table);
}

void __attribute__((destructor)) nfhooks_fini(void) { }
