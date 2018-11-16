# Data type in kernel

This code show errors under ycm, but gcc and clang can compile it without any complain, maybe this is chance to fix ycm
```
(int)((void *)(&c.t)   - (void *)&c)
```

### `module` and `modules_install`
https://superuser.com/questions/770730/what-is-the-difference-between-make-modules-and-make-modules-install
https://www.kernel.org/doc/Documentation/kbuild/modules.txt

### what's -ENODEV

### oh !!! I can not find message from dmesg | grep !
[linux debug](https://elinux.org/Debugging_by_printing)

In the front of kernel, I'am too young too simple.
Having changed the module alike hello-2.ko, and use insmod, the result of dmesg is amazing:

```
[35185.530566] arch  Align:  char  short  int  long   ptr long-long  u8 u16 u32 u64
[35185.530572] arch Align: x86_64          1     2     4     8     8     8        1   2   4   8
[35201.676216] arch  Align:  char  short  int  long   ptr long-long  u8 u16 u32 u64
[35201.676225] arch Align: x86_64          1     2     4     8     8     8        1   2   4   8
[35215.702610] arch  Align:  char  short  int  long   ptr long-long  u8 u16 u32 u64
[35215.702617] arch Align: x86_64          1     2     4     8     8     8        1   2   4   8
```
What a shit, why another two msg appear two, that means, in fact, the function exec correctly and leave msg in the kernel, but demgs | grep doesn't show it!
