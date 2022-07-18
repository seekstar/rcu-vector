#include <urcu.h>
#include <mutex>
#include <cstring>

template<typename T>
class rcu_vector {
public:
	rcu_vector() : v_(new vec{NULL, 0, 0, rcu_head()}) {}
	~rcu_vector() {
		delete v_;
	}
	T read_copy(size_t i) {
		rcu_read_lock();
		T ret = rcu_dereference(v_)->s[i];
		rcu_read_unlock();
		return ret;
	}
	void push_back(const T& x) {
		void (*free_func)(struct rcu_head *);
		lock_.lock();
		auto nv = new vec;
		if (v_->size == v_->cap) {
			if (v_->cap == 0) {
				nv->cap = 1;
			} else {
				nv->cap = v_->cap * 2;
			}
			nv->s = (T *)malloc(nv->cap * sizeof(T));
			if (nv->s == NULL) {
				throw std::bad_alloc();
			}
			memcpy(nv->s, v_->s, v_->size * sizeof(T));
			free_func = free_vec_deep;
		} else {
			nv->cap = v_->cap;
			nv->s = v_->s;
			free_func = free_vec;
		}
		nv->s[v_->size] = x;
		nv->size = v_->size + 1;
		auto v = v_;
		rcu_assign_pointer(v_, nv);
		lock_.unlock();
		call_rcu(&v->rcu, free_func);
	}
	size_t size() const {
		rcu_read_lock();
		size_t ret = rcu_dereference(v_)->size;
		rcu_read_unlock();
		return ret;
	}
private:
	static void free_vec(struct rcu_head *head) {
		struct vec *v = caa_container_of(head, struct vec, rcu);
		delete v;
	}
	static void free_vec_deep(struct rcu_head *head) {
		struct vec *v = caa_container_of(head, struct vec, rcu);
		free(v->s);
		delete v;
	}
	std::mutex lock_;
	struct vec {
		T *s;
		size_t cap;
		size_t size;
		struct rcu_head rcu;
	};
	struct vec *v_;
};
