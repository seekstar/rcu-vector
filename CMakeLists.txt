cmake_minimum_required(VERSION 3.15)
project(rcu-vector CXX)

add_library(rcu_vector_flavor INTERFACE)

target_include_directories(rcu_vector_flavor
	INTERFACE
		include
)
set_target_properties(rcu_vector_flavor PROPERTIES PUBLIC_HEADER "include/rcu_vector_flavor.hpp")
install(TARGETS rcu_vector_flavor)

add_library(rcu_vector INTERFACE)
target_include_directories(rcu_vector
	INTERFACE
		include
)
target_link_libraries(rcu_vector
	INTERFACE
		rcu_vector_flavor
		urcu
)
set_target_properties(rcu_vector PROPERTIES PUBLIC_HEADER "include/rcu_vector.hpp")
install(TARGETS rcu_vector)

add_library(rcu_vector_bp INTERFACE)
target_include_directories(rcu_vector_bp
	INTERFACE
		include
)
target_link_libraries(rcu_vector_bp
	INTERFACE
		rcu_vector_flavor
		urcu-bp
)
set_target_properties(rcu_vector_bp PROPERTIES PUBLIC_HEADER "include/rcu_vector_bp.hpp")
install(TARGETS rcu_vector_bp)
