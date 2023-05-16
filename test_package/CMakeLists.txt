cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(rcu-vector CONFIG REQUIRED COMPONENTS rcu_vector rcu_vector_bp)

add_executable(test src/main.cpp)

target_link_libraries(test
	pthread
	gtest
	rcu_vector
	rcu_vector_bp
)
