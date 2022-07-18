#include "rcu_vector_flavor.hpp"
#include <urcu-bp.h>

template <typename T>
class rcu_vector_bp : public rcu_vector_flavor<T> {
	rcu_vector_bp() : rcu_vector_flavor(rcu_flavor_bp) {}
};
