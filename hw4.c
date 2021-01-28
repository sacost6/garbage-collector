#include "memlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mm.h"

struct memory_region{
  size_t * start;
  size_t * end;
};

struct memory_region global_mem;
struct memory_region stack_mem;

void walk_region_and_mark(void* start, void* end);

// PROVIDED BY US (DO NOT CHANGE)
// ------------------------------
// grabbing the address and size of the global memory region from proc 
void init_global_range(){
  int next = 0;
  char file[100];
  char * line=NULL;
  size_t n=0;
  size_t read_bytes=0;
  size_t start, end;


  sprintf(file, "/proc/%d/maps", getpid());
  FILE * mapfile  = fopen(file, "r");
  if (mapfile==NULL){
    perror("opening maps file failed\n");
    exit(-1);
  }

  int counter=0;
  while ((read_bytes = getline(&line, &n, mapfile)) != -1) {
    // .data of hw4 executable
    if (strstr(line, "hw4") != NULL && strstr(line,"rw-p")){
      sscanf(line, "%lx-%lx", &start, &end);
      global_mem.start = (void*) start;
      global_mem.end = (void*) end;
      break;
    }

  }
  fclose(mapfile);
}

// marking related operations

int is_marked(unsigned int * chunk) {
  return ((*chunk) & 0x2) > 0;
}

void mark(unsigned int * chunk) {
  (*chunk)|=0x2;
}

void clear_mark(unsigned int * chunk) {
  (*chunk)&=(~0x2);
}

// chunk related operations

#define chunk_size(c)  ((*((unsigned int *)c))& ~(unsigned int)7 ) 
void* next_chunk(void* c) { 
  if(chunk_size(c) == 0) {
    fprintf(stderr,"Panic, chunk is of zero size.\n");
  }
  if((c+chunk_size(c)) < mem_heap_hi())
    return ((void*)c+chunk_size(c));
  else 
    return 0;
}

int in_use(void *c) { 
  return *(unsigned int *)(c) & 0x1;
}


int not_in_use(unsigned int*chunk) {
  (*chunk)&=(~0x1);
}

// FOR YOU TO IMPLEMENT
// --------------------
// determine if what "looks" like a pointer actually points to an 
// in use block in the heap. if so, return a pointer to its header 
void * is_pointer(void * ptr) { 
  // Here's a helpful print statement that shows some available memory operations and working with pointers
 // printf("checking for pointeryness of %p between %p and %p\n",ptr, mem_heap_lo(), mem_heap_hi());
  // TODO
  void * current_chunk = mem_heap_lo();
  while(chunk_size(current_chunk) != 0) {	  
	  if((current_chunk < ptr) && (ptr < next_chunk(current_chunk)))
	  	return ((void*)current_chunk);
	  current_chunk = next_chunk(current_chunk);
 } 
 return 0;
}



void mark_helper(void * ptr) {
   void * address = (void*)(*(size_t*)ptr);
   void * header = is_pointer(address);
   if(!header) {
     return;
   }
   if(is_marked(header)) {return ;}
   mark(header);
   int len = chunk_size(header);
   for(int i = 0; i < len; i++) {
     mark_helper(header + (i*2));
   }
   return;
}

// walk through memory specified by function arguments and mark
// each chunk
void walk_region_and_mark(void* start, void* end) {
  //fprintf(stderr, "walking %p - %p\n", start, end);
  
  while(start < end) {
    mark_helper(start);
    start +=8;
  } 
}

// the actual collection code
void sweep() {
  void * chunk = mem_heap_lo();
  void * end = mem_heap_hi();

  while((chunk < end) && (chunk_size(chunk) != 0) )  {
      if(is_marked(chunk) ){ clear_mark(chunk);}
      else if(in_use(chunk) && !(is_marked(chunk))) { 
	      not_in_use(chunk);
	      mm_free(chunk + 4);} 
      chunk = next_chunk(chunk);
  }

}




// PROVIDED BY US (DO NOT CHANGE)
// ------------------------------
// standard initialization 

void init_gc() {
  size_t stack_var;
  init_global_range();
  // since the heap grows down, the end is found first
  stack_mem.end=(size_t *)&stack_var;
}

void gc() {
  size_t stack_var;
  // grows down, so start is a lower address
  stack_mem.start=(size_t *)&stack_var;
  // walk memory regions
  walk_region_and_mark(global_mem.start,global_mem.end);
  walk_region_and_mark(stack_mem.start,stack_mem.end);
  sweep();
}
