#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif



static uintptr_t loader(PCB *pcb, const char *filename) {
  assert(get_ramdisk_size() > 0);
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr elf;
  fs_read(fd, &elf, sizeof(elf));
  Log("magic = %x", *(uint32_t *)elf.e_ident);
  assert(*(uint32_t *)elf.e_ident == 0x464c457f);       // "\x7fELF" in little endian
  
  Elf_Phdr ph;
  for (int i = 0; i < elf.e_phnum; i++) {
    fs_lseek(fd, elf.e_phoff + i * sizeof(ph), SEEK_SET);
    fs_read(fd, &ph, sizeof(ph));
    if (ph.p_type == PT_LOAD) {
      Log("ph.p_vaddr = 0x%x ph.p_offset = 0x%x ph.p_filesz = 0x%x", ph.p_vaddr, ph.p_offset, ph.p_filesz);
      // ramdisk_read((void *)ph.p_vaddr, ph.p_offset, ph.p_filesz);
      fs_lseek(fd, ph.p_offset, SEEK_SET);
      fs_read(fd, (void*)(intptr_t)(ph.p_vaddr), ph.p_filesz);
      memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
    }
  }
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
  assert(0);
}

