add_executable(
	test
	main.cpp
)

target_link_libraries(test
	PRIVATE
		pthread
		gtest
		rcu_vector
		rcu_vector_bp
)
