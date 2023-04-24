cmake_minimum_required(VERSION 3.2)
project(rcu_vector)

add_subdirectory(./rcu_vector_flavor)
add_subdirectory(./rcu_vector)
add_subdirectory(./rcu_vector_bp)

add_subdirectory(./tests)
