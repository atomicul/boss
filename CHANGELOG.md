# v0.1.0
Minimal bootable setup displaying a friendly greeting

# v0.2.0
Code structure rework and paging!

* The kernel now resides at a higher half vitrual address (0xC0000000)
* Added an API to request memory through paging
* Added multiple lower level APIs to manage paging data structures and physical memory
* Added a partial implementation of libc to be used by the kernel code (to help deal with strings and IO)
