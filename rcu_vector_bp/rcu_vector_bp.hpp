#ifndef RCU_VECTOR_BP_H_
#define RCU_VECTOR_BP_H_

#include "rcu_vector_flavor.hpp"
#include <urcu-bp.h>

// This should have been declared in the library
extern "C" {
extern void urcu_bp_call_rcu(struct rcu_head *head,
	void (*func)(struct rcu_head *head));
}

template <typename T>
class rcu_vector_bp : public rcu_vector_flavor<T, urcu_bp_read_lock,
		urcu_bp_read_unlock, urcu_bp_register_thread, urcu_bp_unregister_thread,
		urcu_bp_call_rcu> {};

#endif // RCU_VECTOR_BP_H_
