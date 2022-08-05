/*
 * list.h - Interface for doubly linked circular list.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

#ifndef LIST_DEBUG
#define LIST_DEBUG 0
#endif

struct list_node
{
	struct list_node *next;
	struct list_node *prev;
	bool sentinel;
	void *data;
};

struct list
{
	struct list_node *sentinel;
	size_t size;
	bool (*compare)(void *, void *);
	void (*delete_)(void *);
};

typedef struct list list_t;


struct list *list_new(bool (*compare)(void *, void *), void (*delete_)(void *));

struct list_node *list_insert(struct list *self, int pos, struct list_node *n);
struct list_node *list_search(struct list *self, void *data);
struct list_node *list_delete(struct list *self, int pos);
void  list_remove(struct list *self, struct list_node * node);


struct list_node *list_push_back(struct list *self, struct list_node *n);
struct list_node *list_push_front(struct list *self, struct list_node *n);

struct list_node *list_pop_back(struct list *self) ;
struct list_node *list_pop_front(struct list *self);

void *list_back(struct list *self) ;
void *list_front(struct list *self);

struct list_node *list_head(struct list *self);
struct list_node *list_tail(struct list *self);

struct list_node *list_index(struct list *self, int pos);

size_t list_size(struct list *self) ;
bool list_empty(struct list *self)  ;
bool list_end(struct list_node *n)  ;
struct list *list_concat(struct list *a, struct list *b);

void list_free(struct list *self);

struct list_node *list_node_new(void *data,size_t siz);
struct list_node *list_node_link(struct list_node *a, struct list_node *b);
struct list_node *list_node_unlink(struct list_node *b);

#endif /* LIST_H */
