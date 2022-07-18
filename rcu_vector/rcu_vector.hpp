#include "rcu_vector_flavor.hpp"
#include <urcu.h>

template <typename T>
class rcu_vector_memb : public rcu_vector_flavor<T, rcu_read_lock_memb,
		rcu_read_unlock_memb, rcu_register_thread_memb,
		rcu_unregister_thread_memb, call_rcu_memb> {};

template <typename T>
class rcu_vector : public rcu_vector_memb<T> {};
