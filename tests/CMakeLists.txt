add_executable(test
	EXCLUDE_FROM_ALL
		main.cpp
)

target_link_libraries(test
	PRIVATE
		pthread
		gtest
		rcu_vector
		rcu_vector_bp
)
