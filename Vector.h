// Vector.h -- interface description for Vector ADT

#ifndef _Vector_h
#define _Vector_h 1

#include <stdint.h>
#include <assert.h>

template <typename T>
struct Vector {
private:
	T* data;
	uint32_t capacity;
	uint32_t length;
public:

	uint32_t size (void) const { return length; } // return the number of elements in the vector

	Vector(void) {
		data = new T[1];
		length = 0;
		capacity = 1;
	}

	Vector(uint32_t len) {
		this->length = len;
		this->capacity = len;
		if (this->capacity == 0) {
			this->capacity = 1;
		}
		this->data = new T[capacity];
	}

	Vector(const Vector& that) : Vector(that.size()) {
		for (int i = 0; i < length; i++) {
			this->data[i] = that.data[i];
		}
	}

	~Vector(void) { delete[] data; }

	T& operator[](uint32_t k) {
		assert(k < this->length);

		return this->data[k];
	}
	
	// append x to the end
	void push_back(T x) {
		if (this->capacity == this->length) {
			this->capacity = this->capacity * 2;
			T* new_data = new T[capacity];
			for (uint32_t k = 0; k < this->length; k += 1) {
				new_data[k] = this->data[k];
			}
			delete[] this->data;
			this->data = new_data;
		}

		this->data[this->length] = x;
		this->length += 1;
	}
};

#endif /* _Vector_h */
