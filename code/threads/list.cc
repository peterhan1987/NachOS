// list.cc 
//
//      Routines to manage a singly-linked list of "things".
//
//      A "ListElement" is allocated for each item to be put on the
//      list; it is de-allocated when the item is removed. This means
//      we don't need to keep a "next" pointer in every object we
//      want to put on a list.
// 
//      NOTE: Mutual exclusion must be provided by the caller.
//      If you want a synchronized list, you must use the routines 
//      in synchlist.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "list.h"

//----------------------------------------------------------------------
// ListElement::ListElement
//      Initialize a list element, so it can be added somewhere on a list.
//
//      "itemPtr" is the item to be put on the list.  It can be a pointer
//              to anything.
//      "sortKey" is the priority of the item, if any.
//----------------------------------------------------------------------

ListElement::ListElement (void *itemPtr, long long sortKey)
{
    item = itemPtr;
    key = sortKey;
    next = NULL;		// assume we'll put it at the end of the list 
}

//----------------------------------------------------------------------
// List::List
//      Initialize a list, empty to start with.
//      Elements can now be added to the list.
//----------------------------------------------------------------------

List::List ()
{
    first = last = NULL;
}

//----------------------------------------------------------------------
// List::~List
//      Prepare a list for deallocation.  If the list still contains any 
//      ListElements, de-allocate them.  However, note that we do *not*
//      de-allocate the "items" on the list -- this module allocates
//      and de-allocates the ListElements to keep track of each item,
//      but a given item may be on multiple lists, so we can't
//      de-allocate them here.
//----------------------------------------------------------------------

List::~List ()
{
    // delete all the list elements
    while (Remove () != NULL);			

}

//----------------------------------------------------------------------
// List::Append
//      Append an "item" to the end of the list.
//      
//      Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//      Otherwise, put it at the end.
//
//      "item" is the thing to put on the list, it can be a pointer to 
//              anything.
//----------------------------------------------------------------------

void
List::Append (void *item)
{
    ListElement *element = new ListElement (item, 0);

    if (IsEmpty ())
    {				
        // list is empty
	    first = element;
        last = element;
    }
    else
    {				
        // else put it after last
        last->next = element;
        last = element;
    }
}

//----------------------------------------------------------------------
// List::Prepend
//      Put an "item" on the front of the list.
//      
//      Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//      Otherwise, put it at the beginning.
//
//      "item" is the thing to put on the list, it can be a pointer to 
//              anything.
//----------------------------------------------------------------------

void
List::Prepend (void *item)
{
    ListElement *element = new ListElement (item, 0);

    if (IsEmpty ())
    {				
        // list is empty
	    first = element;
	    last = element;
    }
    else
    {				
        // else put it before first
	    element->next = first;
	    first = element;
    }
}

//----------------------------------------------------------------------
// List::Remove
//      Remove the first "item" from the front of the list.
// 
// Returns:
//      Pointer to removed item, NULL if nothing on the list.
//----------------------------------------------------------------------

void *
List::Remove ()
{
    return SortedRemove (NULL);	// Same as SortedRemove, but ignore the key
}

//----------------------------------------------------------------------
// List::Mapcar
//      Apply a function to each item on the list, by walking through  
//      the list, one element at a time.
//
//      Unlike LISP, this mapcar does not return anything!
//
//      "func" is the procedure to apply to each element of the list.
//----------------------------------------------------------------------

void
List::Mapcar (VoidFunctionPtr func)
{
    for (ListElement * ptr = first; ptr != NULL; ptr = ptr->next)
    {
	   DEBUG ('l', "In mapcar, about to invoke %x(%x)\n", func, ptr->item);
	   (*func) ((int) ptr->item);
    }
}

//----------------------------------------------------------------------
// List::IsEmpty
//      Returns TRUE if the list is empty (has no items).
//----------------------------------------------------------------------

bool
List::IsEmpty ()
{
    if (first == NULL)
	return TRUE;
    else
	return FALSE;
}

//----------------------------------------------------------------------
// List::SortedInsert
//      Insert an "item" into a list, so that the list elements are
//      sorted in increasing order by "sortKey".
//      
//      Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//      Otherwise, walk through the list, one element at a time,
//      to find where the new item should be placed.
//
//      "item" is the thing to put on the list, it can be a pointer to 
//              anything.
//      "sortKey" is the priority of the item.
//----------------------------------------------------------------------

void
List::SortedInsert (void *item, long long sortKey)
{
    ListElement *element = new ListElement (item, sortKey);
    ListElement *ptr;		// keep track

    if (IsEmpty ())
    {				
        // if list is empty, put
        first = element;
        last = element;
    }
    else if (sortKey < first->key)
    {
	    // item goes on front of list
        element->next = first;
        first = element;
    }
    else
    {				// look for first elt in list bigger than item
        for (ptr = first; ptr->next != NULL; ptr = ptr->next)
        {
            if (sortKey < ptr->next->key)
            {
                element->next = ptr->next;
                ptr->next = element;
                return;
            }
        }
        last->next = element;	// item goes at end of list
        last = element;
    }
}

//----------------------------------------------------------------------
// List::SortedRemove
//      Remove the first "item" from the front of a sorted list.
// 
// Returns:
//      Pointer to removed item, NULL if nothing on the list.
//      Sets *keyPtr to the priority value of the removed item
//      (this is needed by interrupt.cc, for instance).
//
//      "keyPtr" is a pointer to the location in which to store the 
//              priority of the removed item.
//----------------------------------------------------------------------

void *
List::SortedRemove (long long *keyPtr)
{
    ListElement *element = first;
    void *thing;

    if (IsEmpty())
	return NULL;

    thing = first->item;
    if (first == last)
    {				
        // list had one item, now has none 
	    first = NULL;
	    last = NULL;
    }
    else
    {
	    first = element->next;
    }
    if (keyPtr != NULL)
	*keyPtr = element->key;
    delete element;
    return thing;
}

void *
List::GetFirst() {
    return first->item;
}

#ifdef CHANGED

void 
ListForJoin::AppendTraverse(void*item, int key) {
    List::Append(item);
    last->key = (long long) key;    
}

void *
ListForJoin::RemoveTraverse(int key) {

    void *thing = NULL;    
    ListElement *element = first;
    ListElement *ptrPre = NULL;
    ListElement *ptr = first;
    long long newKey = (long long) key;

    // special case of 1 element or no element
    if (IsEmpty()) 
        return NULL;
    if (first == last) {
        if (first->key == newKey) {
            thing = first->item;
            first = NULL;
            last = NULL;
            return thing;
        }
        else return NULL;
    }
    
    // Case if there is at least 2 nodes
    // loop through to find the item with the key
    for (ptr = first; ptr != NULL; ptr = ptr->next) {

        if (newKey == ptr->key) {
            thing = ptr -> item;
            element = ptr;

            // remove node from the list
            if (ptr == first) { 
                first = ptr -> next;
            } else if (ptr == last) { 
                last = ptrPre; 
                ptrPre->next = NULL;
            } else {
                ptrPre->next = ptr->next;            
            }
                        
            // free memory
            delete element;            
            break;
        }
        ptrPre = ptr;
    }    
    return thing;
}

bool
ListForJoin::seek(int key) {
    bool result = false;
    ListElement *ptr;
    long long newKey = (long long) key;
    
    for (ptr = first; ptr != NULL; ptr = ptr->next) {
        if (newKey == ptr->key) {
            result = true;
            break;
        }
    }
    return result;
}

void
ListForJoin::PrintContent() {
    for (ListElement * ptr = first; ptr != NULL; ptr = ptr->next) {
        DEBUG ('l', "%d->", ptr->key);
    }    
    DEBUG('l', "\n");
}

bool
ListForJoin::isEmpty() {
    return IsEmpty();
}

#endif
