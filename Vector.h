#pragma once

#include <initializer_list>
#include <memory>

template<typename T>
class Vector
{
public:

	// constructor

	explicit Vector()
		: mData(nullptr)
		, mCapacity(0)
		, mSize(0)
	{}

	explicit Vector(size_t count, const T& value = T())
		: mData(new T[count])
		, mCapacity(count)
		, mSize(count)
	{
		for (size_t i = 0; i < count; i++)
		{
			mData[i] = value;
		}
	}

	Vector(std::initializer_list<T> init)
		: mData(new T[init.size()])
		, mCapacity(init.size())
		, mSize(init.size())
	{
		memcpy(mData, init.begin(), sizeof(T) * init.size());
	}

	Vector(const Vector& other)
		: mData(new T[other.mCapacity])
		, mCapacity(other.mCapacity)
		, mSize(other.mSize)
	{
		for (size_t i = 0; i < other.size(); i++)
		{
			mData[i] = other.mData[i];
		}
	}

	Vector(Vector&& other)
		: mData(std::move(other.mData))
		, mCapacity(std::move(other.mCapacity))
		, mSize(std::move(other.mSize))
	{
		other.mData = nullptr;
		other.mSize = 0;
		other.mCapacity = 0;
	}

	//

	// destructor

	~Vector()
	{
		deleteData();
	}

	//

	// operator =

	Vector& operator=(const Vector& other)
	{
		if (this != &other)
		{
			deleteData();

			mData = new T[other.mCapacity];
			mCapacity = other.mCapacity;
			mSize = other.mSize;

			for (size_t i = 0; i < other.mCapacity; i++)
			{
				mData[i] = other.mData[i];
			}
		}

		return *this;
	}

	Vector& operator=(Vector&& other)
	{
		std::swap(mData, other.mData);
		std::swap(mSize, other.mSize);
		std::swap(mCapacity, other.mCapacity);
		other.mSize = 0;

		return *this;
	}

	Vector& operator=(std::initializer_list<T> ilist)
	{
		deleteData();

		mData = new T[ilist.size()];
		mCapacity = ilist.size();
		mSize = ilist.size();

		memcpy(mData, ilist.begin(), sizeof(T) * ilist.size());

		return *this;
	}

	// Element access

	inline T& operator[](size_t pos)
	{
		return mData[pos];
	}

	inline const T& operator[](size_t pos) const
	{
		return mData[pos];
	}

	//

	// Capacity

	inline bool empty() const noexcept
	{
		return mSize == 0;
	}

	inline size_t size() const noexcept
	{
		return mSize;
	}

	inline void reserve(size_t newCapacity)
	{
		if (mCapacity >= newCapacity)
		{
			return;
		}

		mCapacity = newCapacity;

		T* newData = new T[mCapacity];
		memcpy(newData, mData, sizeof(T) * mSize);

		deleteData();
		mData = newData;
	}

	inline size_t capacity() const noexcept
	{
		return mCapacity;
	}

	//

	// Modifiers

	inline void clear() noexcept
	{
		deleteData();

		mSize = 0;
	}

	inline void push_back(const T& value)
	{
		if (mSize == mCapacity)
		{
			size_t newCapacity = static_cast<size_t>(mCapacity * 1.5f);
			if (newCapacity == mCapacity)
			{
				newCapacity++;
			}

			reserve(newCapacity);
		}

		mData[mSize++] = value;
	}

	inline void push_back(T&& value)
	{
		if (mSize == mCapacity)
		{
			size_t newCapacity = static_cast<size_t>(mCapacity * 1.5f);
			if (newCapacity == mCapacity)
			{
				newCapacity++;
			}

			reserve(newCapacity);
		}

		mData[mSize++] = std::move(value);
	}

	inline void pop_back()
	{
		mSize = mSize > 0 ? mSize - 1 : 0;
	}

	inline void resize(size_t newSize, const T& value = T())
	{
		if (newSize == mSize ||
			newSize < mSize)
		{
			mSize = newSize;
			return;
		}

		reserve(newSize);

		T* newData = new T[mCapacity];

		for (size_t i = 0; i < mSize; i++)
		{
			newData[i] = mData[i];
		}

		for (size_t i = mSize; i < mCapacity; i++)
		{
			newData[i] = value;
		}

		deleteData();
		mData = newData;
		mSize = newSize;
	}

private:
	inline void deleteData()
	{
		if (mData)
		{
			delete[] mData;
			mData = nullptr;
		}
	}

private:
	T* mData;
	size_t mCapacity;
	size_t mSize;
};
