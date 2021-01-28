#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>

#define MEMORY_DEVICE  ("/dev/mem")

// Read bytes of physical memory starting at address into destination.
int mem_read(const unsigned long address, void *const destination, const size_t bytes);

// Write bytes from source to physical memory starting at address.
int mem_write(const unsigned long address, const void *const source, const size_t bytes);

static int mem_device = -1;

static int mem_open(void) {
  int result;

  if (mem_device != -1)
    return errno = EBADF;

  do {
    result = open(MEMORY_DEVICE, O_RDWR | O_SYNC);
  } while (result == -1 && errno == EINTR);
  if (result == -1) return errno;

  mem_device = result;
  return 0;
}

int mem_read(const unsigned long address, void *const destination, const size_t bytes) {
  unsigned char   *d = (unsigned char *)destination;
  off_t            o = (off_t)address;
  size_t           n = bytes;
  ssize_t          r;

  while (n) {
    r = pread(mem_device, d, n, o);
    if (r == (ssize_t)n) {
      return 0;
    } 
    else {
      if (r >= (ssize_t)0) {
        d += r;
        n -= r;
        o += r;
      } 
      else {
        if (r != (ssize_t)-1) {
          return errno = EIO;
        } 
        else {
          if (errno == EBADF) {
            if (mem_open()) return errno;
          } 
          else {
            if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) return errno;
          }
        }
      }
    }
  }
  return 0;
}

int mem_write(const unsigned long address, const void *const source, const size_t bytes) {
  unsigned char   *s = (unsigned char *)source;
  off_t            o = (off_t)address;
  size_t           n = bytes;
  ssize_t          w;

  while (n) {
    w = pwrite(mem_device, s, n, o);
    if (w == (ssize_t)n) {
      return 0;
    } 
    else {
      if (w >= (ssize_t)0) {
        s += w;
        n -= w;
        o += w;

      } 
      else {
        if (w != (ssize_t)-1) {
          return errno = EIO;
        } 
        else {
          if (errno == EBADF) {
            if (mem_open()) {
              return errno;
            }
          } 
          else {
            if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
              return errno;
            }
          }
        }
      }
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  uint64_t addr = std::stoull(argv[1]);

  if( strcmp(argv[2], "r") == 0 ) {
    printf("reading\n");
    unsigned long length = strtoul(argv[3], 0, 0);
    int word = 0x0;
    for( int i = 0 ; i < length; i++ ) {
      if (mem_read(addr, &word, sizeof(word))) {
        fprintf(stderr, "Cannot read %d bytes from physical memory address 0x%08lX: %s.\n",
        		(int)sizeof(word), addr, strerror(errno));
        return 2;
      }

      if(word != 0x0 && word != 0xFF) {
        printf("Physical memory address 0x%016lX = 0x%x (%d bytes).\n", addr, (int)word, (int)sizeof(word));
      }
      addr+=4;
    }
  }

  else if ( strcmp(argv[2], "w") == 0 ) {
    printf("writing\n");
    unsigned value = atoi(argv[3]);
    if (mem_write(addr, &value, sizeof(value))) {
        fprintf(stderr, "Cannot write %d bytes to physical memory address 0x%08lX: %s.\n",
                        (int)sizeof(value), addr, strerror(errno));
        return 1;
    }
  }

  return 0;
}
