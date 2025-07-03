# pageread

If your system has /dev/mem device file, and you have access
(root permissions, CONFIG_STRICT_DEVMEM not defined) to that file,
then you can use this utility to read some data from any location in memory.
Access to memory locations is organized in page fasion (I just needed it in that way for my other tasks).

## How to build this application

```
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
```

## How to use this application

Type

```
  $ pageread --help
```

for help and the rest shall be straightforward.

