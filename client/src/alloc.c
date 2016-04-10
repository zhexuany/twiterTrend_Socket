/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */

#include <stdio.h>
#include "../include/alloc.h"
#undef      malloc
#undef      realloc
#undef      calloc
void* alloc(size_t size) {
  void  *new_mem;
  /*
  ** Ask for the requested memory, and check that we really
  ** got it.
  */
  new_mem = malloc( size );
  if( new_mem == NULL ){
    printf( "Out of memory!\en" );
    exit( 1 );
  }
  return new_mem;
}


void* _alloc( void* ptr, size_t size ) {
  void* new_mem;
  /*
  ** Ask for the requested memory, and check that we really
  ** got it.
  */
  new_mem = realloc( ptr, size );
  if( new_mem == NULL ){
    printf( "Out of memory!\en" );
    exit( 1 );
  }
  return new_mem;

}
void* alloc_( size_t num, size_t size ) {
  void* new_mem;
  /*
  ** Ask for the requested memory, and check that we really
  ** got it.
  */
  new_mem = calloc( num, size );
  if( new_mem == NULL ){
    printf( "Out of memory!\en" );
    exit( 1 );
  }
  return new_mem;

}
