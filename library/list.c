#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

typedef struct list {
    void **data;
    size_t size;
    size_t capacity;
    free_func_t free_function;
} list_t;

list_t *list_init(size_t initial_size, free_func_t free_function) {
    void **data = malloc(sizeof(void*) * initial_size);
    assert(data != NULL); // Check if allocation was successful
    list_t *result = malloc(sizeof(list_t));
    result->capacity = initial_size;
    result->data = data;
    result->size = 0;
    result->free_function = free_function;
    return result;
}

void list_free(list_t *arr) {
    free_func_t free_f = (free_func_t) arr->free_function;
    if (arr->free_function != NULL){
        for(size_t i = 0; i < arr->size; i++){
            free_f(arr->data[i]);
        }
    }
    free(arr->data);
    free(arr);
}

size_t list_size(list_t *arr) {
    return arr->size;
}

void *list_get(list_t *arr, size_t index) {
    assert(index < arr->size); // Check that index is valid
    return arr->data[index];
}

void resize(list_t *arr) {
    if (arr->size >= arr->capacity){
        size_t new_capacity = arr->capacity * 2;
        arr->capacity = new_capacity;
        arr->data = realloc(arr->data, sizeof(void *) * new_capacity);
    }
}

void list_add(list_t *arr, void *value) {
    assert(value != NULL);
    if (arr->size >= arr->capacity){
        resize(arr);
    }
    arr->data[arr->size] = value;
    arr->size++;
}

void list_append(list_t *list1, list_t *list2) {
    assert(list1->free_function == list2->free_function);
    for(size_t i = 0; i < list_size(list2); i++) {
        list_add(list1, list_get(list2, i));
    }
    // Removes appended list after transfer of ownership
    free(list2->data);
    free(list2);
}

void *list_remove(list_t *arr, size_t idx) {
    assert(idx < arr->size);
    if (arr->size == 0) {
        return NULL;
    }
    arr->size--;
    // Shift all elements forwards and ignores last element
    void *temp = arr->data[idx];
    for (size_t i = idx; i < arr->size; i++){
        arr->data[i] = arr->data[i+1];
    }
    return temp;
}

void list_replace(list_t *list, int index, void *elem){
    assert(elem != NULL);
    list->data[index] = elem;
}

bool list_contains(list_t *list, void *elem) {
    for (size_t i = 0; i < list_size(list); i++) {
        void *p = list_get(list, i);
        if (p == elem) {
            return true;
        }
    }
    return false;
}

void list_delete(list_t *list, void *item) {
    for (size_t i = 0; i < list_size(list); i++) {
        if (list_get(list, i) == item) {
            list_remove(list, i);
            return;
        }
    }
}


free_func_t list_get_freer(list_t *list) {
    return list->free_function;
}