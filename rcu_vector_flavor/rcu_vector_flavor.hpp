#include <urcu/flavor.h>
#include <mutex>
#include <cstring>

template<typename T>
class rcu_vector_flavor {
public:
	rcu_vector_flavor(const struct rcu_flavor_struct *flavor)
		:	v_(new vec{NULL, 0, 0, rcu_head()}), flavor_(flavor) {}
	~rcu_vector_flavor() {
		delete v_;
	}
	void register_thread() const {
		flavor_->register_thread();
	}
	void unregister_thread() const {
		flavor_->unregister_thread();
	}
	void read_lock() const {
		flavor_->read_lock();
	}
	void read_unlock() const {
		flavor_->read_unlock();
	}
	T read_copy(size_t i) {
		read_lock();
		T ret = rcu_dereference(v_)->s[i];
		read_unlock();
		return ret;
	}
	size_t size() const {
		read_lock();
		size_t ret = rcu_dereference(v_)->size;
		read_unlock();
		return ret;
	}

	void lock() {
		lock_.lock();
	}
	void unlock() {
		lock_.unlock();
	}
	T& ref_locked(size_t i) {
		return v_->s[i];
	}
	size_t size_locked() const {
		return v_->size;
	}
	void push_back_locked(const T& x) {
		void (*free_func)(struct rcu_head *);
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
		flavor_->update_call_rcu(&v->rcu, free_func);
	}
	void push_back(const T& x) {
		lock();
		push_back_locked(x);
		unlock();
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
	const struct rcu_flavor_struct *flavor_;
};