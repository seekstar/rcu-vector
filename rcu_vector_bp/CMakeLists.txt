cmake_minimum_required(VERSION 3.2)
project(rcu_vector_bp)

add_library(rcu_vector_bp INTERFACE)
target_include_directories(rcu_vector_bp
	INTERFACE
		${PROJECT_SOURCE_DIR}
)
target_link_libraries(rcu_vector_bp
	INTERFACE
		rcu_vector_flavor
		urcu-bp
)
