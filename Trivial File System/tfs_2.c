/*
*Palmer Cone
*OS - Summer 21
*Drachova
*/

#include "tfs.h"


/* implementation of assigned functions */
unsigned int tfs_get_block( unsigned int fd ){
  //returns first block
  return(directory[fd].first_block);
}
unsigned int tfs_check_file_is_closed( unsigned int fd ){
  //Finds if file is closed
  if( directory[fd].status != CLOSED ){
    
    return( FALSE );
  }
  return( TRUE );
}


/* tfs_delete()
 *
 * deletes a closed directory entry having the given file descriptor
 *   (changes the status of the entry to unused) and releases all
 *   allocated file blocks
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is closed
 *
 * postconditions:
 *   (1) the status of the directory entry is set to unused
 *   (2) all file blocks have been set to free
 *
 * input parameter is a file descriptor
 *
 * return value is TRUE when successful or FALSE when failure
 */

unsigned int tfs_delete( unsigned int file_descriptor ){
  //Preconditions
  if( !tfs_check_fd_in_range( file_descriptor ) ) return( FALSE );
  if( !tfs_check_file_is_closed( file_descriptor ) ) return( FALSE );
  //Var assignments

  unsigned char c,b,first;
  unsigned int n_blocks = 1 + (directory[file_descriptor].size/128);
  first = b = directory[file_descriptor].first_block;
  c = b = file_allocation_table[b];
  while(c && b != LAST_BLOCK){
    //find most recent and delete as iterate
    c = file_allocation_table[b];
    blocks[b].bytes[0] = '\0';
    file_allocation_table[b] = FREE;
    b = c;
  }
  blocks[b].bytes[0] = '\0';
  file_allocation_table[b] = FREE;
  file_allocation_table[first] = FREE;
  //"Freeing info"
  directory[file_descriptor].first_block = 0;
  directory[file_descriptor].size = 0;
  directory[file_descriptor].status = UNUSED;
}


/* tfs_read()
 *
 * reads a specified number of bytes from a file starting
 *   at the byte offset in the directory into the specified
 *   buffer; the byte offset in the directory entry is
 *   incremented by the number of bytes transferred
 *
 * depending on the starting byte offset and the specified
 *   number of bytes to transfer, the transfer may cross two
 *   or more file blocks
 *
 * the function will read fewer bytes than specified if the
 *   end of the file is encountered before the specified number
 *   of bytes have been transferred
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is open
 *   (3) the file has allocated file blocks
 *
 * postconditions:
 *   (1) the buffer contains bytes transferred from file blocks
 *   (2) the specified number of bytes has been transferred
 *         except in the case that end of file was encountered
 *         before the transfer was complete
 *
 * input parameters are a file descriptor, the address of a
 *   buffer of bytes to transfer, and the count of bytes to
 *   transfer
 *
 * return value is the number of bytes transferred
 */

unsigned int tfs_read( unsigned int file_descriptor,
                       char *buffer,
                       unsigned int byte_count ){
  //preconditions
  if( !tfs_check_fd_in_range ( file_descriptor ) ) return( FALSE );
  if( !tfs_check_file_is_open ( file_descriptor ) ) return( FALSE );
  if( !tfs_check_block_in_range ( tfs_get_block(file_descriptor) )) return( FALSE );
  if( directory[file_descriptor].byte_offset > tfs_size(file_descriptor)) return(FALSE);
 //Var assignments
  unsigned char b;
  b = directory[file_descriptor].first_block;
  unsigned int offset_block = directory[file_descriptor].byte_offset/128;
  unsigned int offset_byte = directory[file_descriptor].byte_offset - (128 * offset_block);
  
  //find Block
  for (int i = 0; i < offset_block; i++){
    b =  file_allocation_table[b];
  }
  unsigned int length_buff = 0;
  while (b != LAST_BLOCK && length_buff < byte_count){
    if (offset_byte >= 128){
      offset_byte = 0;
    }
    for (length_buff=length_buff;offset_byte < 128 && length_buff < byte_count;length_buff++){
        //Read to buffer
        sprintf(buffer, "%c", blocks[b].bytes[offset_byte]);
        offset_byte++;
    }
    if (b != LAST_BLOCK){
      //Find last block
      b =  file_allocation_table[b];
    }
    else{
      return length_buff;
    }

  }
  return length_buff;
}

/* tfs_write()
 *
 * writes a specified number of bytes from a specified buffer
 *   into a file starting at the byte offset in the directory;
 *   the byte offset in the directory entry is incremented by
 *   the number of bytes transferred
 *
 * depending on the starting byte offset and the specified
 *   number of bytes to transfer, the transfer may cross two
 *   or more file blocks
 *
 * furthermore, depending on the starting byte offset and the
 *   specified number of bytes to transfer, additional file
 *   blocks, if available, will be added to the file as needed;
 *   in this case, the size of the file will be adjusted
 *   based on the number of bytes transferred beyond the
 *   original size of the file
 *
 * the function will read fewer bytes than specified if file
 *   blocks are not available
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is open
 *
 * postconditions:
 *   (1) the file contains bytes transferred from the buffer
 *   (2) the specified number of bytes has been transferred
 *         except in the case that file blocks needed to
 *         complete the transfer were not available
 *   (3) the size of the file is increased as appropriate
 *         when transferred bytes extend beyond the previous
 *         end of the file
 *
 * input parameters are a file descriptor, the address of a
 *   buffer of bytes to transfer, and the count of bytes to
 *   transfer
 *
 * return value is the number of bytes transferred
 */

unsigned int tfs_write( unsigned int file_descriptor,
                        char *buffer,
                        unsigned int byte_count ){
  // Preconditions
  if( !tfs_check_fd_in_range ( file_descriptor ) ){ 
    
    return( FALSE );
  }
  if( !tfs_check_file_is_open ( file_descriptor ) ){ 
    
    return( FALSE );
    }
  //Assignments
  unsigned int num_transferred = 0;//transferred number of bytes/retval
  //placeholders/iterators
  unsigned char b;
  unsigned char c;
  unsigned int byte_location=0;
  
  if(directory[file_descriptor].first_block == 0 ){
    //new block (FIRST FOR DIR)
    b = tfs_new_block();
    directory[file_descriptor].first_block = b;
    file_allocation_table[b] = LAST_BLOCK;
    byte_location = 0;
    
  }
  else
  {
   //NOT FIRST BLOCK FOR DIR
    b = directory[file_descriptor].first_block;
    c = b;
    c = file_allocation_table[c];
    //Find last block
    while (c != LAST_BLOCK){
      
      b = file_allocation_table[b];
      c = file_allocation_table[c];
    }
    if(directory[file_descriptor].size/128 == 0){
      //Fresh block needed
      b = tfs_new_block();
      if(b == 0){
        //Done
        return num_transferred;
      }
      file_allocation_table[c] = b;
      file_allocation_table[b] = LAST_BLOCK;
      byte_location = 0;
    }
    else{
      //Find LOC in block
      byte_location = directory[file_descriptor].size - 128 * (directory[file_descriptor].size/128);
    }
  }
  
  while(num_transferred < byte_count ){
    //Writeing
    for(byte_location = byte_location; byte_location < 128 && num_transferred < byte_count ; byte_location++){
      sprintf(blocks[b].bytes, "%c", buffer[num_transferred]);
      directory[file_descriptor].size++;
      num_transferred++;
    }
    if(num_transferred < byte_count){
      //not finished writing
      byte_location = 0;
      
      c=tfs_new_block();
      if(c == 0){
        return num_transferred;
      }
      else{
        file_allocation_table[b] = c;
        b =  file_allocation_table[b];
        file_allocation_table[c] = LAST_BLOCK;
      }

    }


  }
  return num_transferred;


}
