#include "rcu_vector_flavor.hpp"
#include <urcu-bp.h>

template <typename T>
class rcu_vector_bp : public rcu_vector_flavor<T, rcu_read_lock_bp,
		rcu_read_unlock_bp, rcu_register_thread_bp, rcu_unregister_thread_bp,
		call_rcu_bp> {};
