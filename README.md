# About

A filesystem simulation of UNIX-systems implemented in C++. The stored information for the directories and files are stored in the postfix.di file which is created and updated on the runtime.

## Run & Invocation

Compile the project by running:
```
make all
```

#### Command line options:
 * *flags*             : {-c|-a|-x|-m|-d|-p|-j}
 * *archive-file*      : path of archive file
 * *list-of-files/dir* : path of file(s)/dir(s)

Invocation:

    ./mydiz {-c|-a|-x|-m|-d|-p|-j} <archive-file> <list-of-files/dirs>

### Archive-file_Structure ###

Two are the main data structures :
 * Archive_metadata = is the header of the file and consists of the file's:
   * name.
   * size (including its metadata and other files inside).
   * type.
   * permissions.
   * dates of lastly modified, lastly opened etc.

 * File_metadata = is the metadata of the file/folder and consists of:
   * the file/folder name.
   * another structure (=struct stat) for an easier compress/decompress.
   * an offset that points to the end of the file/folder including its files. If it's a directory it points to the next folder of the directory

The whole structure is based on the Sequential file organization which is used for the creation of '.tar' files. In this way, at the beggining of the archive-file, the header & metadata is being saved and afterwards there are sequentially added any metadata of the included files and their information. 

E.g. 
* archive_metadata/File_metadata/File_info/File1_metadata/File1_data.../EOF
* archive_metadata/Dir_metadata/Dir_file_metadata/Dir_file_data.../EOF

As far as the hierarchy of inodes, two basic data structures were used; a stack and a list. More specifically, the stack represents the root directory and each list represents a nested directory, so if a directory is found a new list will be created with the aforementioned data and will be pushed to the stack. This operation is recursive.
