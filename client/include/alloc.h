/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */

#include <stdlib.h>
#define     malloc
#define     realloc
#define     calloc
#define     MALLOC(num, type)        (type *)alloc( (num) * sizeof(type))
#define     CALLOC(num, type)        (type *)alloc_(num, num*sizeof(type));

#define     REALLOC(ptr, num, type)  (type*)_alloc((type*)ptr, num*sizeof(type))
extern      void* alloc(size_t size);

extern      void* _alloc(void* ptr, size_t size);

extern void* alloc_(size_t num, size_t size);
