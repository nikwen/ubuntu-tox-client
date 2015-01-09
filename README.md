ubuntu-tox-client
=================

Attempt to create a [Tox](https://tox.im) client licensed under the GPL v3 using the Ubuntu SDK.

Building
========

Build it as usual using the Ubuntu SDK, but add the respective CMake option for the architecture you are building for:

```
-DFOR_AMD64=ON
-DFOR_I386=ON
-DFOR_ARMHF=ON
```

New versions of the shipped toxcore and libsodium binaries can be compiled using the following set of scripts: https://github.com/nikwen/ubuntu-toxcore-cross-compilation-scripts

Credits
=======

ubuntu-tox-client uses the following libraries:

* [toxcore](https://github.com/irungentoo/toxcore) (GPL v3)
* [libsodium](https://github.com/jedisct1/libsodium) (ISC license)

ubuntu-tox-client uses code from the following projects:

* [qTox](https://github.com/tux3/qTox) (GPL v3)
* [uTox](https://github.com/notsecure/uTox) (GPL v3)
