#include "rcu_vector_flavor.hpp"
#include <urcu.h>

template <typename T>
class rcu_vector : public rcu_vector_flavor<T> {
	rcu_vector() : rcu_vector_flavor(rcu_flavor_memb) {}
};
