cmake_minimum_required(VERSION 3.2)
project(rcu_vector)

add_library(rcu_vector INTERFACE)
target_include_directories(rcu_vector
	INTERFACE
		${PROJECT_SOURCE_DIR}
)
target_link_libraries(rcu_vector
	INTERFACE
		rcu_vector_flavor
		urcu
)
