#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>
#include <stdbool.h>

/**
 * A growable array of pointers.
 * Can store values of any pointer type (e.g. vector_t*, body_t*).
 * The list automatically grows its internal array when more capacity is needed.
 */
typedef struct list list_t;

/**
 * A function that can be called on list elements to release their resources.
 * Examples: free, body_free
 */
typedef void (*free_func_t)(void *);

/**
 * Allocates memory for a new list with space for the given number of elements.
 * The list is initially empty.
 * Asserts that the required memory was allocated.
 *
 * @param initial_size the number of elements to allocate space for
 * @param freer if non-NULL, a function to call on elements in the list
 *   in list_free() when they are no longer in use
 * @return a pointer to the newly allocated list
 */
list_t *list_init(size_t initial_size, free_func_t freer);

/**
 * Releases the memory allocated for a list.
 *
 * @param list a pointer to a list returned from list_init()
 */
void list_free(list_t *list);

/**
 * Gets the size of a list (the number of occupied elements).
 * Note that this is NOT the list's capacity.
 *
 * @param list a pointer to a list returned from list_init()
 * @return the number of elements in the list
 */
size_t list_size(list_t *list);

/**
 * Gets the element at a given index in a list.
 * Asserts that the index is valid, given the list's current size.
 *
 * @param list a pointer to a list returned from list_init()
 * @param index an index in the list (the first element is at 0)
 * @return the element at the given index, as a void*
 */
void *list_get(list_t *list, size_t index);

/**
 * Removes the element at a given index in a list and returns it,
 * moving all subsequent elements towards the start of the list.
 * Asserts that the index is valid, given the list's current size.
 *
 * @param list a pointer to a list returned from list_init()
 * @return the element at the given index in the list
 */
void *list_remove(list_t *list, size_t index);

/**
 * Removes the first occurence of an item in a list by equivalence
 *
 * @param list a pointer to a list returned from list_init()
 * @return the element at the given index in the list
 */
void list_delete(list_t *list, void *item);

/**
 * Appends an element to the end of a list.
 * If the list is filled to capacity, resizes the list to fit more elements
 * and asserts that the resize succeeded.
 * Also asserts that the value being added is non-NULL.
 *
 * @param list a pointer to a list returned from list_init()
 * @param value the element to add to the end of the list
 */
void list_add(list_t *list, void *value);

/**
 * Appends a list to the end of a list.
 * If the list is filled to capacity, resizes the list to fit more elements
 * and asserts that the resize succeeded.
 * Also asserts that the free_func_t of lists are equivalent.
 *
 * @param list a pointer to a list returned from list_init()
 * @param value the element to add to the end of the list
 */
void list_append(list_t *list1, list_t *list2);

/**
 * Resizes given list to capactiy size.
 * 
 * @param list a pointer to the list being resized
 * @param size the new size of the list
*/
void list_resize(list_t *list, size_t size);

/**
 * Replaces the existing element at index i with vector.
 * 
 * @param list a pointer to the list
 * @param index index to replace
 * @param vector vector to replace in list
*/
void list_replace(list_t *list, int index, void *elem);

/**
 * Checks whether an element is in a list.
 * 
 * @param list a pointer to the list
 * @param elem the element to check if in the list
 * @return whether the element is in a list.
*/
bool list_contains(list_t *list, void *elem);

free_func_t list_get_freer(list_t *list);

#endif // #ifndef __LIST_H__