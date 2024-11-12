#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"


// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

// Declaración de funciones para operar el spinlock
void initlock(struct spinlock *lk, char *name);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);
int holding(struct spinlock *lk);

#endif // SPINLOCK_H
