#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <elf.h>

//typedef struct
//{
//  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
//  Elf64_Half	e_type;			/* Object file type */
//  Elf64_Half	e_machine;		/* Architecture */
//  Elf64_Word	e_version;		/* Object file version */
//  Elf64_Addr	e_entry;		/* Entry point virtual address */
//  Elf64_Off	e_phoff;		/* Program header table file offset */
//  Elf64_Off	e_shoff;		/* Section header table file offset */
//  Elf64_Word	e_flags;		/* Processor-specific flags */
//  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
//  Elf64_Half	e_phentsize;		/* Program header table entry size */
//  Elf64_Half	e_phnum;		/* Program header table entry count */
//  Elf64_Half	e_shentsize;		/* Section header table entry size */
//  Elf64_Half	e_shnum;		/* Section header table entry count */
//  Elf64_Half	e_shstrndx;		/* Section header string table index */
//} Elf64_Ehdr;


int main(int argc, char**argv)
{
    Elf64_Ehdr ehdr;
	int fd;
	
	fd = open(argv[1], O_RDONLY);

    if(fd < 0) { return 0; }
    
	read(fd, (void*)&ehdr, sizeof(ehdr));
    
    printf("e_entry     = %s.\n", ehdr.e_ident);
    printf("e_type      = %016x.\n", ehdr.e_type);
    printf("e_machine   = %016x.\n", ehdr.e_machine);
    printf("e_version   = %016x.\n", ehdr.e_version);
    printf("e_entry     = %016x.\n", ehdr.e_entry);
    printf("e_phoff     = %016x.\n", ehdr.e_phoff);
    printf("e_shoff     = %016x.\n", ehdr.e_shoff);
    printf("e_flags     = %016x.\n", ehdr.e_flags);
    printf("e_ehsize    = %016x.\n", ehdr.e_ehsize);
    printf("e_phentsize = %016x.\n", ehdr.e_phentsize);
    printf("e_phnum     = %016x.\n", ehdr.e_phnum);
    printf("e_shentsize = %016x.\n", ehdr.e_shentsize);
    printf("e_shnum     = %016x.\n", ehdr.e_shnum);
    printf("e_shstrndx  = %016x.\n", ehdr.e_shstrndx);

    close(fd);
    
}
