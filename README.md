# ExperimentCore

Is going to become a central repo I'll build that includes a lot of the useful
tools and libraries that make sense for us to have in one place.

The way this will work is we use `git submodule` to link any third party
utilities we require into somewhere like `/third_party/`. This allows us to
share and link to that stuff,
and extend it locally as we need to. As is usual in these projects, we'd prefer
to limit dependencies and mark them explicitly where they're used.

I'm still figuring out whether it's worth it to link some of that into here yet,
need to compare the experience on Windows too. I'm going to run Windows in the
lab and use the WSL - Windows Subsystem for Linux - because I'm good with Ubuntu
and I need to be able to compare the experiences.

A lot of work can be removed by offloading it to containers or VMs, and I'll
explain that plan longer term, but I want to setup support for microservice
architecture if it's easy to do so. The basic idea is if we want to do stuff like
setup cloud controllers, or log servers, for example. We can automatically
collate data into different sorts of databases for different sorts of analysis,
and automate everything. It's easy and multiplies what I can get done, so I plan
on taking advantage of it.

## Build Environments

* git
* cmake
* python3.8
* VS Code (and so many plugins...)
  * C/C++
  * CMake Tools
  * Docker
  * Bracket Pair Colorizer 2
  * Better Comments
  * GitLens
  * Live Share
  * Python
  * 

## Windows Build Environment

* From Win App Store:
  * Windows Terminal
  * Windows Subsystem for Linux
  * Ubuntu (18 or 20.04 LTS)
* Visual Studio 2008 (Last compiled for, support will be deprecated later)
* Visual Studio 2019 (Community)
* [Chocolatey](https://chocolatey.org/install)

``
## Chocolatey Install
`Set-ExecutionPolicy AllSigned`
`Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))`

### Dev Systems
`choco install openssh git.install vlc 7zip.install googlechrome vcredist140 dotnetfx sysinternals ccleaner python vscode salt-minion`

### Build system
`choco install openssh git.install visualstudio2019buildtools vcredist140 salt-minion`

#### Jenkins

see blbld.uchicago.edu


#### Highly Rec'd
* Notepad++ (Backup sanity check, really)
* 

### Setup for Protoc

* Start Menu > x64 Native Tools CLI for VS 2019
* Make sure git and cmake are installed and on path
* `cd E:\repos\grpc\third_party\protobuf\cmake`
* `mkdir build & cd build; mkdir release * cd release`
* `make -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../../../install ../..`
* `nmake`
* add "E:\repos\grpc\third_party\protobuf\cmake\build\release" to path
* now can run `protoc` on windows

## Notes on Protocol Buffer implementation

We're using this system for a few major reasons:

* Universally supported, very well documented, stable for ~15 years.
* Basically OS and Language-agnostic: Allows us to mix and match languages across environments
* Does what we needs.

There's a fair amount of setup code below that's maybe not necessary if you're
working in a Visual Studio environment; which has a bunch of generators for this
built in. I'm building in linux right now, which may seem like the hard way
right now so I can make a build script for
all of this, on a build server, so whenever somebody checks in a new version of a
.proto file, I can automatically rebuild these stubs on a build server. This
will make it real easy to add functionality to applications.

### Protobuffer schema

I started by writing my OpenAPI spec first. This is really just an extra
abstraction layer over the gRPC stuff we need. I did this so I could have
forwards compatibility for some ideas I have for later. It's less important for
now.

```bash
openapi-generator generate -i DaniAPI.yaml -g protobuf-schema -o ./OpenAPI-proto/
```

For the sake of a demo, though, let's limit variables and make it as simple as
possible. We're going to do helloWorld with the `./proto/helloWorld.proto` file.


## Linux Build

### Python

Required setup to make python generate our code.

#### Py Install

Required files to install:

```bash
python3 -m pip install grpcio-tools
```

#### Py Generate

This generates both server stubs and client:

```bash
# NOTE: These setvars are used below as well!
# Setting my abspath for safety.
PROJECT='/mnt/data/home/projects/uc/DaniTest/'

# output path:
mkdir pyClient
OUTPUT='pyClient'

PROTO_FILES="${PROJECT}proto/helloWorld.proto"

python3 -m grpc.tools.protoc \
--python_out="${OUTPUT}" \
--grpc_python_out="${OUTPUT}" \
--proto_path="${PROJECT}./proto/" \
$PROTO_FILES
```

### CPP

I'm following along with
[Intro to gRPC on C](https://medium.com/@andrewvetovitz/grpc-c-introduction-45a66ca9461f)
to figure this out.

#### C++ Install

```bash
sudo apt install protobuf-compiler protobuf-compiler-grpc build-essential autoconf libtool pkg-config
# In mine, I did this already:
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule init
git submodule update
cd grpc/third_party/protobuf
# sudo make install
#! This `sudo make` didn't just work. Needed to do full autoconf/automake.
```

#### How to autoconf/automake

Needed this for `make install` above to work right:

```bash
# Order looks something like:
libtoolize
aclocal
autoheader  # Not sure if <- this is in right order
autoconf
automake --add-missing
./configure
make
sudo make install
```

```bash
# Note that I got this error on the way, fixed with the `ldconfig` below. Why?
dmacdonald@amethyst:/mnt/data/home/projects/uc/DaniTest$ protoc -I $INCLUDE --grpc_out=$OUTPUT --plugin=protoc-gen-grpc=grpc_cpp_plugin $PROTO_FILES
protoc: error while loading shared libraries: libprotoc.so.22: cannot open shared object file: No such file or directory
dmacdonald@amethyst:/mnt/data/home/projects/uc/DaniTest$ sudo ldconfig
```

#### C++ Generate

##### Linux
```bash
# Now! Finally, to try compiling the c proto stubs:
OUTPUT='cpp'
PROTO_FILES='./proto/helloWorld.proto'

# So it complains about the `grpc_cpp_plugin` program if I don't reset my environment here. So subbing in abs path is `/usr/bin/grpc_cpp_plugin`:
protoc --grpc_out=$OUTPUT --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin $PROTO_FILES
protoc --cpp_out=$OUTPUT $PROTO_FILES
```

##### Windows
```powershell
# Now! Finally, to try compiling the c proto stubs:
$OUTPUT = 'cppServer'
$PROTO_FILES = './proto/helloWorld.proto'

# So it complains about the `grpc_cpp_plugin` program if I don't reset my environment here. So subbing in abs path is `/usr/bin/grpc_cpp_plugin`:
protoc --grpc_out=$OUTPUT --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin $PROTO_FILES
protoc --cpp_out=$OUTPUT $PROTO_FILES
```


## Utilizing Protocol Buffers

TODO!

Both Python and C++ are classy languages, and the best thing to do would be to
include the generated files into your project, and (I think, testing still!)
subclass from the Servicer object.

## References

Really, stuff to look into more later:

* [101 on the build process](https://thoughtbot.com/blog/the-magic-behind-configure-make-make-install)
* [autotools.io - Book on build tooling](https://autotools.io/)

