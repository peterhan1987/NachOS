// addrspace.h 
//      Data structures to keep track of executing user programs 
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"

#ifdef CHANGED
#include "synch.h"
#include "list.h"
#define MAX_FILES 5
#endif

#define UserStackSize		2048	// increase this as necessary!

class Semaphore;    //forward declaration

class AddrSpace
{
  public:
    AddrSpace (OpenFile * executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace ();		// De-allocate an address space

    void InitRegisters ();	// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState ();		// Save/restore address space-specific
    void RestoreState ();	// info on a context switch 
    
    void MultiThreadSetStackPointer(unsigned int newPositionOffset);

    // Returns how many threads the system can handle
    int GetMaxNumThreads ();

#ifdef CHANGED
        // Get's and sets the first stack's free position
    int GetAndSetFreeStackLocation ();
    // Free the given position
    void FreeStackLocation (int position);

    int increaseUserThreads();
    int decreaseUserThreads();
    int getNumberOfUserThreads();
    
    Semaphore *ExitForMain;    
    
    // Variable for Join functionality
    ListForJoin *activeThreads;
    ListForJoin *activeLocks;

    int PushTable(OpenFile *file);
    int PullTable(int index);
    int IndexSearch(OpenFile *file);
    int SearchTable(OpenFile *file);
    OpenFile *OpenSearch(int index);
    
    void setExtraArg(char *newArg);
    char* getExtraArg();
    
#endif   // END CHANGED
  private:
    TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space

#ifdef CHANGED
    int numberOfUserThreads;
    
    // Available pages
    BitMap *stackBitMap;
    Lock *stackBitMapLock;
    Lock *threadsCountLock;
    Lock *processesCountLock;

/* OpenFileProcess is a openfile table on the process level, each threads inside the same process will add new openfile objects into this level table */
    typedef struct {
      OpenFile *file; //openfile object
      int sector; //sector number return from openfile object used to make a connection between openfile table on kernel level
      bool vacant; //determine whether the cell is empty
    } OpenFileProcess;

    OpenFileProcess table[MAX_FILES];
    Lock *openLock;

    //extra variables for passing new variable in ForkExec
    bool hasArg; 
    char *arg;  // This is the 
#endif   // END CHANGED

};

#endif // ADDRSPACE_H
