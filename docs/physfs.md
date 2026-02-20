PhysFS
------

PhysFS (PhysicsFS) is a library to provide abstract access to various archives. It is used in many games and applications to handle file I/O in a platform-independent manner.

**Link:** [https://github.com/icculus/physfs](https://github.com/icculus/physfs)
**Version**: 3.0.2

---

How to Update PhysFS (Linux)
----------------------------

Download the latest PhysFS source code from its [GitHub repository](https://github.com/icculus/physfs).

Then, move the relevant source files into the `engine/external/physfs` directory:

```bash
mv physfs/src engine/external/physfs
mv physfs/extas/physfssdl3.c engine/external/physfs
mv physfs/extas/physfssdl3.h engine/external/physfs
```
