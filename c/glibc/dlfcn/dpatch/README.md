# dpatch

(work in progress) Experimental dynamic patching system.


## Quickstart

You'll need to build `libdpatch.so` and a target program to get started.

The target program must have its symbols exported as dynamic symbols, which is a known limitation of `dpatch`. The `dpatch` repository comes with several demonstration programs pre-configured for building and using.

```sh
# Configure build scripts into `./build`.
$ cmake -B build .

# Build the configured targets in ./build`.
$ cmake --build build

# See an unpatched program.
$ ./build/demo/self_patch
I am alpha!

# Try a dynamic patch.
$ LD_AUDIT=./build/dpatch/libdpatch.so DPATCH_SCRIPT=./build/demo/self_patch.patch ./build/demo/self_patch
I am bravo!

# (Optional) Install `dpatch` onto your system.
$ cmake --build build -t install
$ LD_AUDIT=/usr/local/lib/libdpatch.so DPATCH_SCRIPT=/usr/local/etc/self_patch.patch /usr/local/dpatch/demo/self_patch
Hello, world!
```


## Building, installing, and packaging

`dpatch` is built with CMake.

### Building

The basic build commands are the standard CMake commands:

```sh
# Configure build scripts in `/build` for the platform default build tool.
# This will probably be UNIX Makefiles on most Linux systems.
$ cmake -B build

# Build all targets in `./build` using the platform-default build tools.
$ cmake --build build
```

At this point, you could run `dpatch` directly from the build output directory:

```sh
$ LD_AUDIT=./build/dpatch/libdpatch.so /path/to/target/program
```


### Installing

CMake is configured with install targets for CMake.

You can install `dpatch` directly onto your system using `$ cmake --build build -t install`. Installing `dpatch` will allow you to use it from your system paths:

```sh
$ LD_AUDIT=/usr/local/lib/libdpatch.so /path/to/target/program
```

### Packaging

`dpatch` can be installed to a custom output directory to support self-packaging, or automatically packaged by CMake's `cpack` subsystem.

#### CPack

`dpatch` can generate its own packages using CMake's `cpack` system. CPack will generate a (not very) reasonable default package format for your platform. On Unix-style systems, this is likely to be a self extracting archive wrapped in a shell script.

Build a package by invoking:

```sh
$ cmake --build build -t package
```

You can change the package format by invoking `cpack` directly:

```sh
$ cpack --config build/CPackConfig.cmake -G <PackageGenerator>
```

View `cpack --help` for a list of package generator formats available for your platform.

#### Self-packaging

`dpatch`'s `install` target respects the standard `DESTDIR` environment variable to install `dpatch` to a custom path. This can be used to generate binary packages as you see fit.

## Scripting

Dpatch uses scripts to determine how to patch the target. The script path can be specified as an environment variable named `DPATCH_SCRIPT`. The environment variable must be set when the target program is started and linked with `libdpatch`.

If no script path is specified, Dpatch will attempte to find a script in the default path: `/usr/etc/patch.dpatch`.
