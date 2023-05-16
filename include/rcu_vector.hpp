#ifndef RCU_VECTOR_H_
#define RCU_VECTOR_H_

#include "rcu_vector_flavor.hpp"
#include <urcu.h>

// This should have been declared in the library
extern "C" {
extern void urcu_memb_call_rcu(struct rcu_head *head,
	void (*func)(struct rcu_head *head));
}

template <typename T>
class rcu_vector_memb : public rcu_vector_flavor<T, urcu_memb_read_lock,
		urcu_memb_read_unlock, urcu_memb_register_thread,
		urcu_memb_unregister_thread, urcu_memb_call_rcu> {};

template <typename T>
class rcu_vector : public rcu_vector_memb<T> {};

#endif // RCU_VECTOR_H_
