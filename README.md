Used for CS 594 Linux Kernel Programming at the University of Illinois at Chicago

```c
// C18 standard
#include <stdio.h>

int main()
{
    printf("Hello, World!\n");
    return 0;
}
```


Setup 

Install Taskfile.dev for all users
```sh
sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin
```



The Linux kernel development process is a time-based release process, with a new major kernel release happening every two or three months. The kernel developers follow a relatively straightforward discipline with regard to the merging of patches for each release. At the beginning of each development cycle, the “merge window” is said to be open. At that time, code which is deemed to be sufficiently stable (and which is accepted by the development community) is merged into the mainline kernel. The bulk of changes for a new development cycle (and all of the major changes) will be merged during this time, at a rate approaching 1,000 changes (patches, or changesets) per day. The merge window lasts for approximately two weeks. At the end of this time, Linus Torvalds will declare that the window is closed and release the first of the “rc” kernels. Over the next six to ten weeks, only patches which fix problems should be submitted to the mainline. On occasion, a more significant change will be allowed, but such occasions are rare; developers who try to merge new features outside of the merge window tend to get an unfriendly reception1The Linux kernel development process is a time-based release process, with a new major kernel release happening every two or three months. The kernel developers follow a relatively straightforward discipline with regard to the merging of patches for each release. At the beginning of each development cycle, the “merge window” is said to be open. At that time, code which is deemed to be sufficiently stable (and which is accepted by the development community) is merged into the mainline kernel. The bulk of changes for a new development cycle (and all of the major changes) will be merged during this time, at a rate approaching 1,000 changes (patches, or changesets) per day. The merge window lasts for approximately two weeks. At the end of this time, Linus Torvalds will declare that the window is closed and release the first of the “rc” kernels. Over the next six to ten weeks, only patches which fix problems should be submitted to the mainline. On occasion, a more significant change will be allowed, but such occasions are rare; developers who try to merge new features outside of the merge window tend to get an unfriendly reception1



https://www.kernel.org/doc/html/latest/process/2.Process.html