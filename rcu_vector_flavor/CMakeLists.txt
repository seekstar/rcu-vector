cmake_minimum_required(VERSION 3.2)
project(rcu_vector_flavor)

add_library(rcu_vector_flavor INTERFACE)

target_include_directories(rcu_vector_flavor
	INTERFACE
		${PROJECT_SOURCE_DIR}
)
