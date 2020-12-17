#include <bfd.h>

bfd_boolean bfd_get_section_contents   (bfd *abfd, asection *section, void *location, file_ptr offset,
                                        bfd_size_type count);

