#include "allocation.h"
#include "inode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BLOCKSIZE 4096
int id = 0;


struct inode* create_file( struct inode* parent, char* name, char readonly, int size_in_bytes ) {
  if (find_inode_by_name(parent, name) || !parent) { return NULL; }

  struct inode* filenode = malloc(sizeof(struct inode));
  if (!filenode) {printf("MALLOC create_file FAILED"); exit(-1);}

  filenode->id = ++id;
  if ((filenode->name = malloc(sizeof(name)+1))) {
    strcpy(filenode->name, name);
  } else {printf("Malloc create_file name failed"); exit(-1);}

  filenode->is_directory = 0;
  filenode->is_readonly = readonly;
  filenode->filesize = size_in_bytes;
  filenode->num_entries = size_in_bytes/BLOCKSIZE + (size_in_bytes % BLOCKSIZE != 0);
  filenode->entries = malloc(filenode->num_entries*sizeof(uintptr_t));
  if (!filenode->entries) {printf("MALLOC create_file entries FAILED"); exit(-1);}
  for (int i=0; i<filenode->num_entries;++i) {
    filenode->entries[i] = allocate_block();
  }
  parent->num_entries++;
  parent->entries = realloc(parent->entries, (sizeof(uintptr_t) * parent->num_entries));
  if (!parent->entries) {printf("Realloc create_file failed"); exit(-1);}
  parent->entries[parent->num_entries-1] = (uintptr_t) filenode;

  return filenode;
}


struct inode* create_dir( struct inode* parent, char* name ) {
  if (parent && find_inode_by_name(parent, name)) { return NULL; }

  struct inode* dirnode = malloc(sizeof(struct inode));
  if (!dirnode) {printf("MALLOC INODE create_dir FAILED"); exit(-1);}

  dirnode->id = id++;
  if ((dirnode->name = malloc(sizeof(name)+1))) {
    strcpy(dirnode->name, name);
  } else {printf("Malloc name create_dir failed"); exit(-1);}

  dirnode->is_directory = 1;
  dirnode->is_readonly = 0;
  dirnode->filesize = 0;
  dirnode->num_entries = 0;
  dirnode->entries = malloc(sizeof(uintptr_t));
  if (!dirnode->entries) {printf("MALLOC entries create_dir FAILED"); exit(-1);}
  if (!parent) { return dirnode; } // hvis dette er rotnoden

  parent->entries = realloc(parent->entries, sizeof(uintptr_t)*parent->num_entries+1);
  if (!parent->entries) {printf("Realloc create_dir failed"); exit(-1);}
  parent->entries[parent->num_entries] = (uintptr_t) dirnode;

  return dirnode;
}


struct inode* find_inode_by_name( struct inode* parent, char* name ) {
  if (!strcmp(name, parent->name)) { return parent; }
  if (!parent || !parent->is_directory) { return NULL; }

  for (int i=0; i<parent->num_entries;++i) {
    struct inode* found = find_inode_by_name((struct inode*) parent->entries[i], name);
    if(found) { return found; }
    }
  return NULL;
}


struct inode* load_recursive(FILE *fptr) {

  struct inode *root = malloc(sizeof(struct inode));
  if (!root) {printf("MALLOC INODE FAILED"); exit(-1);}

  fread(&root->id, sizeof(int), 1, fptr);
  int name_len = 0; fread(&name_len, sizeof(int), 1, fptr);
  root->name = malloc(name_len);
  if (!root->name) {printf("MALLOC NAME FAILED"); exit(-1);}
  
  fread(root->name, name_len, 1, fptr);
  fread(&root->is_directory, sizeof(char), 1, fptr);
  fread(&root->is_readonly, sizeof(char), 1, fptr);
  fread(&root->filesize, sizeof(int), 1, fptr);
  fread(&root->num_entries, sizeof(int), 1, fptr);
  uintptr_t *file_entries = malloc(sizeof(uintptr_t) * root->num_entries);
  root->entries = malloc(64*root->num_entries);
  if (!root->entries || !file_entries) {printf("MALLOC ENTRIES FAILED"); exit(-1);}
  fread(file_entries, root->num_entries*sizeof(uintptr_t), 1, fptr);

  if (root->is_directory) {
    for (int i=0; i<root->num_entries; ++i){
      root->entries[i] = (uintptr_t) load_recursive(fptr);
    }
  } else {
    for (int i=0; i<root->num_entries; ++i) {
      root->entries[i] = file_entries[i];
    }
  }
  free(file_entries);
  return root;
}


struct inode* load_inodes() {

  FILE *fptr;
  fptr = fopen("master_file_table", "rb");

  if (fptr == NULL) {
    printf("File Error: Could not open \"master_file_table\"");
    exit(-1);
  }
  struct inode *root = load_recursive(fptr);
  fclose(fptr);
  return root;
}


void fs_shutdown( struct inode* inode ) {

  if (!inode) {return;} 
  if (inode->is_directory) {
    for (int i=0; i<inode->num_entries;++i) {
      fs_shutdown((struct inode*) inode->entries[i]);
    }
  }
  free(inode->name);
  free(inode->entries);
  free(inode);
}

/* This static variable is used to change the indentation while debug_fs
 * is walking through the tree of inodes and prints information.
 */
static int indent = 0;

void debug_fs( struct inode* node )
{
    if( node == NULL ) return;
    for( int i=0; i<indent; i++ )
        printf("  ");
    if( node->is_directory )
    {
        printf("%s (id %d)\n", node->name, node->id );
        indent++;
        for( int i=0; i<node->num_entries; i++ )
        {
            struct inode* child = (struct inode*)node->entries[i];
            debug_fs( child );
        }
        indent--;
    }
    else
    {
        printf("%s (id %d size %db blocks ", node->name, node->id, node->filesize );
        for( int i=0; i<node->num_entries; i++ )
        {
            printf("%d ", (int)node->entries[i]);
        }
        printf(")\n");
    }
}

