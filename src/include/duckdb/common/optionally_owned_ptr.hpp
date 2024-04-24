//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/common/optionally_owned_ptr.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/exception.hpp"
#include "duckdb/common/unique_ptr.hpp"
#include "duckdb/common/shared_ptr.hpp"

namespace duckdb {

template <class T>
class optionally_owned_ptr { // NOLINT: mimic std casing
public:
	optionally_owned_ptr() {
	}
	optionally_owned_ptr(T *ptr_p) : ptr(ptr_p) { // NOLINT: allow implicit creation from pointer
	}
	optionally_owned_ptr(T &ref) : ptr(&ref) { // NOLINT: allow implicit creation from reference
	}
	explicit optionally_owned_ptr(unique_ptr<T> &&owned_p) : owned(std::move(owned_p)), ptr(owned) {
	}
	// Move constructor
	explicit optionally_owned_ptr(optionally_owned_ptr &&other) : owned(std::move(other.owned)), ptr(other.ptr) {
		other.ptr = nullptr;
	}
	// Copy constructor
	explicit optionally_owned_ptr(const optionally_owned_ptr &other) : owned(nullptr), ptr(other.ptr) {
	}

	operator bool() const { // NOLINT: allow implicit conversion to bool
		return ptr;
	}
	T &operator*() {
		return *ptr;
	}
	const T &operator*() const {
		return *ptr;
	}
	T *operator->() {
		return ptr.get();
	}
	const T *operator->() const {
		return ptr.get();
	}
	T *get() { // NOLINT: mimic std casing
		return ptr.get();
	}
	const T *get() const { // NOLINT: mimic std casing
		return ptr.get();
	}
	bool is_owned() const { // NOLINT: mimic std casing
		return owned != nullptr;
	}
	// this looks dirty - but this is the default behavior of raw pointers
	T *get_mutable() const { // NOLINT: mimic std casing
		return ptr.get();
	}

	bool operator==(const optionally_owned_ptr<T> &rhs) const {
		if (owned != rhs.owned) {
			return false;
		}
		return ptr == rhs.ptr;
	}

	bool operator!=(const optionally_owned_ptr<T> &rhs) const {
		return !(*this == rhs);
	}

private:
	unique_ptr<T> owned;
	optional_ptr<T> ptr;
};

} // namespace duckdb
