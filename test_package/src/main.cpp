#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <random>

#include "rcu_vector.hpp"
#include "rcu_vector_bp.hpp"

#include <gtest/gtest.h>

template <template<typename> typename V>
void push_back_func(V<int>& v, int id, size_t num,
		uint64_t *time) {
	ASSERT_EQ(*time, 0);
	auto start_time = std::chrono::steady_clock().now();
	while (num) {
		v.push_back(id);
		num -= 1;
	}
	auto end_time = std::chrono::steady_clock().now();
	*time = std::chrono::duration_cast<std::chrono::nanoseconds>(
		end_time - start_time).count();
}

struct read_ret {
	std::vector<int> v;
	size_t read_cnt;
	uint64_t time; // nanoseconds
};

template <template<typename> typename V>
void repeat_read_until(V<int>& v, size_t size, read_ret *ret) {
	v.register_thread();
	ASSERT_TRUE(ret->v.empty());
	ASSERT_EQ(ret->read_cnt, 0);
	ASSERT_EQ(ret->time, 0);
	do {
		size_t i;
		size_t s = v.size();
		auto start_time = std::chrono::steady_clock().now();
		for (i = 0; i < std::min(s, ret->v.size()); ++i) {
			ASSERT_EQ(ret->v[i], v.read_copy(i));
		}
		auto end_time = std::chrono::steady_clock().now();
		ret->time += std::chrono::duration_cast<std::chrono::nanoseconds>(
			end_time - start_time).count();
		ret->read_cnt += s;

		for (; i < s; ++i) {
			ret->v.push_back(v.read_copy(i));
		}
	} while (ret->v.size() < size);
	v.unregister_thread();
}

template <template<typename> typename V>
void pusher_checker(size_t pusher_num, size_t each_num, size_t checker_num) {
	V<int> v;
	std::vector<std::thread> pusher;
	std::vector<uint64_t> pusher_time(pusher_num);
	std::vector<std::thread> checker;
	std::vector<struct read_ret> ret(checker_num);
	for (size_t i = 0; i < pusher_num; ++i) {
		pusher.emplace_back(push_back_func<V>, std::ref(v), (int)i, each_num,
			&pusher_time[i]);
	}
	for (size_t i = 0; i < checker_num; ++i) {
		checker.emplace_back(repeat_read_until<V>, std::ref(v),
			each_num * pusher_num, &ret[i]);
	}
	for (size_t i = 0; i < pusher_num; ++i) {
		pusher[i].join();
	}
	for (size_t i = 0; i < checker_num; ++i) {
		checker[i].join();
	}
	std::vector<size_t> cnts(pusher_num);
	ASSERT_EQ(ret[0].v.size(), each_num * pusher_num);
	for (int val : ret[0].v) {
		cnts[val] += 1;
	}
	for (size_t cnt : cnts) {
		ASSERT_EQ(cnt, each_num);
	}
	for (size_t i = 1; i < checker_num; ++i) {
		ASSERT_EQ(ret[i].v.size(), ret[0].v.size());
		for (size_t j = 0; j < ret[0].v.size(); ++j) {
			ASSERT_EQ(ret[0].v[j], ret[i].v[j]);
		}
	}
	uint64_t time = 0;
	for (size_t i = 0; i < pusher_num; ++i) {
		time += pusher_time[i];
	}
	std::cout << time / (pusher_num * each_num) << " ns for each RCU write\n";

	time = 0;
	size_t read_cnt = 0;
	for (size_t i = 0; i < checker_num; ++i) {
		time += ret[i].time;
		read_cnt += ret[i].read_cnt;
	}
	std::cout << time / read_cnt << " ns for each RCU read\n";
}

TEST(RCUVectorTest, Pusher1Checker1Memb) {
	pusher_checker<rcu_vector_memb>((size_t)1, (size_t)1 << 16, (size_t)1);
}

TEST(RCUVectorTest, Pusher1Checker4Memb) {
	pusher_checker<rcu_vector_memb>((size_t)1, (size_t)1 << 16, (size_t)4);
}

TEST(RCUVectorTest, Pusher4Checker1Memb) {
	pusher_checker<rcu_vector_memb>((size_t)4, (size_t)1 << 16, (size_t)4);
}

TEST(RCUVectorTest, Pusher4Checker4Memb) {
	pusher_checker<rcu_vector_memb>((size_t)4, (size_t)1 << 16, (size_t)4);
}

TEST(RCUVectorTest, Pusher16Checker16Memb) {
	pusher_checker<rcu_vector_memb>((size_t)16, (size_t)1 << 16, (size_t)16);
}

TEST(RCUVectorTest, Pusher1Checker1Bp) {
	pusher_checker<rcu_vector_bp>((size_t)1, (size_t)1 << 16, (size_t)1);
}

TEST(RCUVectorTest, Pusher1Checker4Bp) {
	pusher_checker<rcu_vector_bp>((size_t)1, (size_t)1 << 16, (size_t)4);
}

TEST(RCUVectorTest, Pusher4Checker1Bp) {
	pusher_checker<rcu_vector_bp>((size_t)4, (size_t)1 << 16, (size_t)4);
}

TEST(RCUVectorTest, Pusher4Checker4Bp) {
	pusher_checker<rcu_vector_bp>((size_t)4, (size_t)1 << 16, (size_t)4);
}

TEST(RCUVectorTest, Pusher16Checker16Bp) {
	pusher_checker<rcu_vector_bp>((size_t)16, (size_t)1 << 16, (size_t)16);
}

template <template<typename> typename V>
void incr_growing_array_of_counters(V<std::atomic<size_t> *> *v, size_t num,
		size_t range_expand_step, size_t seed) {
	std::minstd_rand e(seed);
	size_t range = range_expand_step;
	v->register_thread();
	for (size_t i = 0; i < num; ++i) {
		size_t index = e() % range;
		if (v->size() <= index) {
			v->lock();
			while (v->size_locked() <= index) {
				v->push_back_locked(new std::atomic<size_t>(0));
			}
			v->unlock();
		}
		std::atomic<size_t> *cnt = v->read_copy(index);
		*cnt += 1;
		range += range_expand_step;
	}
	v->unregister_thread();
}

template <template<typename> typename V>
void test_incr_growing_array_of_counters(size_t thread_num, size_t incr_each,
		size_t range_expand_step) {
	V<std::atomic<size_t> *> v;
	std::vector<std::thread> t;
	for (size_t i = 0; i < thread_num; ++i) {
		t.emplace_back(incr_growing_array_of_counters<V>, &v, incr_each,
			range_expand_step, i);
	}
	for (size_t i = 0; i < thread_num; ++i) {
		t[i].join();
	}
	ASSERT_LE(v.size_locked(), range_expand_step + incr_each);
	size_t tot = 0;
	for (size_t i = 0; i < v.size_locked(); ++i) {
		auto val = v.ref_locked(i);
		// Actually no need to be atomic
		tot += val->load(std::memory_order_relaxed);
		delete val;
	}
	ASSERT_EQ(tot, incr_each * thread_num);
}

TEST(RCUVectorTest, GrowingCounterMemb) {
	test_incr_growing_array_of_counters<rcu_vector_memb>((size_t)16,
		(size_t)1 << 22, (size_t)1);
}

TEST(RCUVectorTest, GrowingCounterBp) {
	test_incr_growing_array_of_counters<rcu_vector_bp>((size_t)16,
		(size_t)1 << 22, (size_t)1);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
