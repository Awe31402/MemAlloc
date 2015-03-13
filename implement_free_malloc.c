#include <stdlib.h>
#include <stdio.h>

typedef struct block_header {
	int block_size;
	struct block_header *prev;
	struct block_header *next;
}block_header_t;

#define MEM_TO_HEADER(ptr) (block_header_t *)((unsigned long)ptr - sizeof(block_header_t))
#define HEADER_TO_MEM(ptr) (void *)((unsigned long) ptr + sizeof(block_header_t))
block_header_t *free_list_head = NULL;

void replace_header(block_header_t *old_item, block_header_t *new_item)
{
	old_item->prev->next = new_item;
	old_item->next->prev = new_item;

	new_item->prev = old_item->prev;
	old_item->next = old_item->next;
	if(old_item == free_list_head) {
		free_list_head = new_item;
		new_item->prev = NULL;
	}
	old_item->prev = old_item->next = NULL;
}

void append_header(block_header_t *item)
{
	block_header_t *current_header;
	current_header = free_list_head;

	while(current_header->next) {
		current_header = current_header->next;
	}
	
	item->prev = current_header;
	current_header->next = item;
}

void remove_header(block_header_t *item)
{
	if(item == free_list_head) {
			free_list_head = item->next;
			item->prev = item->next = NULL;
			return;
	}
	else {
		item->prev->next = item->next;
		if(item->next)
			item->next->prev = item->prev;
	}
	item->prev = item->next = NULL;
}
 
void *awe_malloc(size_t size)
{
	block_header_t *current_header, *new_header, *block_header;
	current_header = free_list_head;
	size_t size_add_header = size + sizeof(block_header_t);
	void *mem_ret; /*the memory block to return*/

	while (current_header && current_header->next) { /* if free list is exist and not tail*/
		if(current_header->block_size >= size) { /*if current block size > size we require*/
			mem_ret = HEADER_TO_MEM(current_header);

			if(current_header->block_size > size_add_header) { 
			/*if block size is large enough to add another header*/
				new_header = (block_header_t *) ((unsigned long)current_header + size + sizeof(block_header_t));
				new_header->block_size = current_header->block_size - size_add_header;
				replace_header(current_header, new_header);
			} else {
				remove_header(current_header);
			}
			return mem_ret;
		} else {
			current_header = current_header->next;
		}
	}
	
	/*
	 * Two situation could make code execute to here, 
	 * 1. the freelist could be empty
	 * 2. there is no ideal size of freelist to allocate 
         */
//	if(!current_header) { /*freelist is NULL(empty)*/
		block_header = (block_header_t *) sbrk(size_add_header);
		mem_ret = HEADER_TO_MEM(block_header);
//	} else { 
		/*
		 * there is no ideal block to allocate in free list
		 * then resize the tail item in free list
		 */
//		size_t expand_size = size - current_header->block_size;
//		block_header = (block_header_t *) sbrk(expand_size);
//		mem_ret = HEADER_TO_MEM(block_header);
//	}
	block_header->next = block_header->prev = NULL;
	block_header->block_size = size;
	return mem_ret;
}

void awe_free(void *ptr)
{
	if(free_list_head == NULL) {
		free_list_head = MEM_TO_HEADER(ptr);
		return;
	}
	append_header(MEM_TO_HEADER(ptr));
}

int main(int argc, char *argv[])
{
	void *ptr;
	char *chr_ptr;
	int *int_ptr;
	double *double_ptr;

	printf("brefore awe_malloc, sbrk(0) = %lu\n", sbrk(0));
	chr_ptr = (char *)awe_malloc(sizeof(char));
	*chr_ptr = 'x';
	printf("after allocate %lu bytes, chr_ptr = %lu, *chr_ptr= %c , sbrk(0) = %lu\n",
		sizeof(char),chr_ptr, *chr_ptr, (void *)sbrk(0));
	int_ptr = (int *)awe_malloc(sizeof(int));
	*int_ptr = 100;
	printf("after allocate %lu bytes, int_ptr = %lu, *int_ptr = %d, sbrk(0) = %lu\n",
		sizeof(int), int_ptr, *int_ptr, (void *)sbrk(0));
	double_ptr = (double *) awe_malloc(sizeof(double));
	*double_ptr = 1000000;
	printf("after allocate %lu bytes, double_ptr = %lu, *double_ptr = %lf, sbrk(0) = %lu\n", 
		sizeof(double), double_ptr, *double_ptr, (void *)sbrk(0));

	awe_free((void *)double_ptr);
	awe_free((void *)int_ptr);
	awe_free((void *)chr_ptr);
	int_ptr = (int *)awe_malloc(sizeof(int));
	*int_ptr = 200;
	printf("After awe_free, after allocate %lu bytes, int_ptr = %lu, *int_ptr = %d, sbrk(0) = %lu\n",
		sizeof(int), int_ptr, *int_ptr, (void *)sbrk(0));
	double_ptr = (double *)awe_malloc(sizeof(double));
	*double_ptr = 5000000;
	printf("After awe_free, after allocate %lu bytes, double_ptr = %lu, *double_ptr = %lf, sbrk(0) = %lu\n",
		sizeof(double), double_ptr, *double_ptr, (void *)sbrk(0));
	exit(EXIT_SUCCESS);
}
