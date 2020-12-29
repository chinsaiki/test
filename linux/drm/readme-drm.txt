Direct Rendering Manager - DRM 
Rendering: 演奏，表演，扮演，翻译

DRM 是Linux目前的主流图形显示框架，相比 FB 框架，DRM 更能适应当前日益
更新的显示硬件。 DRM 从模块上划分，可以简单分为 3 部分， libdrm, KMS, GEM

--------------------------------------------------
*** test/c/glibc/drm
*** test/linux/drm
*** 


--------------------------------------------------
	libva-drm.so.1 (libc6,x86-64) => /usr/lib64/libva-drm.so.1
	libdrm_radeon.so.1 (libc6,x86-64) => /usr/lib64/libdrm_radeon.so.1
	libdrm_radeon.so (libc6,x86-64) => /usr/lib64/libdrm_radeon.so
	libdrm_nouveau.so.2 (libc6,x86-64) => /usr/lib64/libdrm_nouveau.so.2
	libdrm_nouveau.so (libc6,x86-64) => /usr/lib64/libdrm_nouveau.so
	libdrm_intel.so.1 (libc6,x86-64) => /usr/lib64/libdrm_intel.so.1
	libdrm_intel.so (libc6,x86-64) => /usr/lib64/libdrm_intel.so
	libdrm_amdgpu.so.1 (libc6,x86-64) => /usr/lib64/libdrm_amdgpu.so.1
	libdrm_amdgpu.so (libc6,x86-64) => /usr/lib64/libdrm_amdgpu.so
	libdrm.so.2 (libc6,x86-64) => /usr/lib64/libdrm.so.2
	libdrm.so (libc6,x86-64) => /usr/lib64/libdrm.so

