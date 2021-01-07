gcc gup_benchmark.c -o gup_benchmark.out
gcc hmm-tests.c -lhugetlbfs -pthread -o hmm-tests.out
gcc hugepage-mmap.c -o hugepage-mmap.out
gcc hugepage-shm.c -o hugepage-shm.out
gcc khugepaged.c -o khugepaged.out
gcc map_fixed_noreplace.c -o map_fixed_noreplace.out
gcc map_hugetlb.c -o map_hugetlb.out
gcc map_populate.c -o map_populate.out
gcc mlock2-tests.c -o mlock2-tests.out
gcc mlock-random-test.c -lcap -o mlock-random-test.out
gcc mremap_dontunmap.c -o mremap_dontunmap.out
gcc on-fault-limit.c -o on-fault-limit.out
gcc protection_keys.c -o protection_keys.out
gcc thuge-gen.c -o thuge-gen.out
gcc transhuge-stress.c -o transhuge-stress.out
gcc userfaultfd.c -pthread -o userfaultfd.out
gcc va_128TBswitch.c -o va_128TBswitch.out
gcc virtual_address_range.c -o virtual_address_range.out
gcc write_to_hugetlbfs.c -o write_to_hugetlbfs.out
gcc compaction_test.c -o compaction_test.out
