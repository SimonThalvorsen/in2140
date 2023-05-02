lese master-file table filen fra disk og laster inodene inn i minnet

pointers to the blocks on disk where the file's data is stored.

To read the inode table from disk and load the inodes into memory in a Unix-like operating system, the following steps are typically taken:

A file system driver or library is used to access the file system. In Linux, for example, the driver may be part of the kernel, while in macOS it may be part of the file system framework.

The driver or library accesses the file system's superblock, which contains metadata about the file system as a whole, including the location of the inode table.

Using the information in the superblock, the driver or library reads the inode table from disk and parses its contents to extract the inode information.

Once the inode information has been extracted, it is loaded into memory by allocating memory space for each inode and copying the information from the inode table into the allocated memory.

The file system driver or library can then use the inode information in memory to access files and directories on the file system.

The specific details of how the inode table is read from disk and loaded into memory can vary depending on the file system and the implementation of the driver or library being used. However, the general process outlined above is common to many Unix-like file systems.


## Har noe memory leaks på create_example1
