cmake_minimum_required(VERSION 3.2)
project(rcu_vector)

add_library(rcu_vector INTERFACE)

target_include_directories(${PROJECT_NAME}
	INTERFACE
		${PROJECT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}
	INTERFACE
		urcu
)
