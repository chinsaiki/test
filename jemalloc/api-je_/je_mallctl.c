#include <stdio.h>
#include <jemalloc/jemalloc.h>

int main()
{
    unsigned nbins, i;
    size_t mib[4];
    size_t len, miblen;

    len = sizeof(nbins);
    je_mallctl("arenas.nbins", &nbins, &len, NULL, 0);

    miblen = 4;
    je_mallctlnametomib("arenas.bin.0.size", mib, &miblen);
    for (i = 0; i < nbins; i++) {
        size_t bin_size;

        mib[2] = i;
        len = sizeof(bin_size);
        je_mallctlbymib(mib, miblen, (void *)&bin_size, &len, NULL, 0);
        /* Do something with bin_size... */
        printf("bin_size = %d\n", bin_size);
//        bin_size = 8
//        bin_size = 16
//        bin_size = 32
//        bin_size = 48
//        bin_size = 64
//        bin_size = 80
//        bin_size = 96
//        bin_size = 112
//        bin_size = 128
//        bin_size = 160
//        bin_size = 192
//        bin_size = 224
//        bin_size = 256
//        bin_size = 320
//        bin_size = 384
//        bin_size = 448
//        bin_size = 512
//        bin_size = 640
//        bin_size = 768
//        bin_size = 896
//        bin_size = 1024
//        bin_size = 1280
//        bin_size = 1536
//        bin_size = 1792
//        bin_size = 2048
//        bin_size = 2560
//        bin_size = 3072
//        bin_size = 3584
//        bin_size = 4096
//        bin_size = 5120
//        bin_size = 6144
//        bin_size = 7168
//        bin_size = 8192
//        bin_size = 10240
//        bin_size = 12288
//        bin_size = 14336
    }
}


//"arenas":{
//	"narenas":17,
//	"dirty_decay_ms":10000,
//	"muzzy_decay_ms":0,
//	"quantum":16,
//	"page":4096,
//	"tcache_max":32768,
//	"nbins":36,
//	"nhbins":41,
//	"bin":[
//			{
//			"size":8,
//			"nregs":512,
//			"slab_size":4096,
//			"nshards":1
//			},
//			{
//			"size":16,
//			"nregs":256,
//			"slab_size":4096,
//			"nshards":1
//			},
//		  ],
//	"nlextents":196,
//	"lextent":[
//		{"size":16384},
//		{"size":20480},
//		...],
//	}

