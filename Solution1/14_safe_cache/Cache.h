// UTF-8
// ═══════════════════════════════════════════════════════════════
// 14 线程安全 Cache<K, V>
// ═══════════════════════════════════════════════════════════════
//
// shared_mutex（C++17）：读写锁，支持两种加锁模式
//   unique_lock → 写锁（独占）：调用 shared_mutex::lock()
//                  等待条件：readers_==0 && !writing_
//                  其他线程不能读也不能写
//   shared_lock → 读锁（共享）：调用 shared_mutex::lock_shared()
//                  等待条件：!writing_（有其他读者没关系）
//                  多个线程可以同时持有
//
// shared_mutex 内部简化逻辑：
//   class shared_mutex {
//       int readers_ = 0;
//       bool writing_ = false;
//       condition_variable cv_;
//   public:
//       void lock()          { wait(readers_==0 && !writing_); writing_=true; }
//       void unlock()        { writing_=false; notify_all(); }
//       void lock_shared()   { wait(!writing_); readers_++; }
//       void unlock_shared() { if(--readers_==0) notify_all(); }
//   };
//
// unique_lock 内部简化逻辑（RAII 包装器）：
//   class unique_lock {
//       Mutex* mtx_;
//   public:
//       unique_lock(Mutex& m) { mtx_->lock(); }    // 构造加锁
//       ~unique_lock()        { mtx_->unlock(); }  // 析构解锁
//       void unlock()         { mtx_->unlock(); }  // 支持手动解锁
//       void lock()           { mtx_->lock(); }    // 支持手动加锁
//   };
//   配合 shared_mutex 时调用 lock()  → 写锁
//   配合 shared_mutex 时调用 lock_shared() 的是 shared_lock
//
// 并发行为：
//   线程A: shared_lock（读）  ─────────────────
//   线程B: shared_lock（读）  ─────────────────  ← 可以同时持有
//   线程C: unique_lock（写）       ████████      ← 等A和B都释放才能进
//   线程D: shared_lock（读）            ─────    ← 等C释放才能进
//
// mutable：允许 const 成员函数修改该成员
//   get() / size() 是 const 函数，但需要加锁（修改 mtx_ 内部状态）
//   所以 mtx_ 必须是 mutable，否则编译错误
//
// std::optional<V>（C++17）：
//   表示"可能有值也可能没有"，比返回 bool + 引用参数更优雅
//   有值：return it->second;   → optional 内部存了 V
//   无值：return std::nullopt; → optional 为空
//   调用方：if (auto v = cache.get("key")) { use(*v); }
//
// Java 对照：
//   shared_mutex ≈ ReadWriteLock
//   shared_lock  ≈ readLock().lock()
//   unique_lock  ≈ writeLock().lock()
//   std::optional≈ Optional<V>
//   mutable      ≈ Java 没有直接对应，最接近的是非 final 字段在 final 方法里修改
// ═══════════════════════════════════════════════════════════════
#pragma once
#include <shared_mutex>
#include <unordered_map>
#include <optional>
#include <string>

template<typename K, typename V>
class Cache {
public:
    void set(const K& key, const V& val);
    std::optional<V> get(const K& key) const;
    void remove(const K& key);
    size_t size() const;

private:
    std::unordered_map<K, V> data_;
    mutable std::shared_mutex mtx_;  // mutable：const 函数（get/size）也能加锁
};

// ── 模板实现必须在 .h 里 ──────────────────────────────────────

template<typename K, typename V>
void Cache<K, V>::set(const K& key, const V& val) {
    // unique_lock → 调用 shared_mutex::lock() → 写锁（独占）
    // 等待所有读者和写者退出后才能进入
    std::unique_lock lock(mtx_);
    data_[key] = val;
}  // 析构 → shared_mutex::unlock() → writing_=false → notify_all

template<typename K, typename V>
std::optional<V> Cache<K, V>::get(const K& key) const {
    // shared_lock → 调用 shared_mutex::lock_shared() → 读锁（共享）
    // 只等写者退出，其他读者可以同时进入
    std::shared_lock lock(mtx_);
    auto it = data_.find(key);
    if (it == data_.end()) return std::nullopt;  // 没找到，返回空 optional
    return it->second;                           // 找到，返回值包装进 optional
}  // 析构 → shared_mutex::unlock_shared() → readers_-- → 若归零 notify_all

template<typename K, typename V>
void Cache<K, V>::remove(const K& key) {
    std::unique_lock lock(mtx_);  // 写锁，独占
    data_.erase(key);
}

template<typename K, typename V>
size_t Cache<K, V>::size() const {
    std::shared_lock lock(mtx_);  // 读锁，只读 size 不修改数据
    return data_.size();
}
