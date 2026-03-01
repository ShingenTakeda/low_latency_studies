#pragma once

#include <cstddef>
#include <vector>
#include <string>

#include "../helpers.hpp"

//TODO:Actually get the type instead of using a bunch of autos

template<typename T>
struct MemPool final
{
	struct ObjBlock
	{
		T object;
		bool is_free = true;
	};
	
	std::vector<ObjBlock> store;
	std::size_t next_free_index = 0;

	explicit MemPool(std::size_t num_elements) : store(num_elements, {T(), true})
	{
		ASSERT(reinterpret_cast<const ObjBlock*>(&(store[0].object)) == &(store[0]), "T Object member should be first member of object block");
	}

	MemPool() = delete;
	MemPool(const MemPool&) = delete;
	MemPool(const MemPool&&) = delete;
	MemPool& operator=(const MemPool&) = delete;
	MemPool& operator=(const MemPool&&) = delete;

	template<typename... Args>
	T *allocate(Args... args) noexcept
	{
		auto obj_block = &(store[next_free_index]);
		ASSERT(obj_block->is_free, "Expected free Object block at index:" + std::to_string(next_free_index));

		T *ret = &(obj_block->object);
		ret = new(ret) T(args...);//placement new
		obj_block->is_free = false;

		update_next_free_index();
	
		return ret;
	}

	auto update_next_free_index() noexcept
	{
		const auto initial_free_index = next_free_index;
		while(!store[next_free_index].is_free)
		{
			++next_free_index;
			if(UNLIKELY(next_free_index == store.size()))
			{
				next_free_index = 0;
			}
			if(UNLIKELY(initial_free_index == next_free_index))
			{
				ASSERT(initial_free_index != next_free_index, "Memory pool out of space!");
			}
		}
	}

	auto deallocate(const T *elem) noexcept
	{
		const auto elem_index = (reinterpret_cast<const ObjBlock *>(elem) - &store[0]);
		ASSERT(elem_index >= 0 && static_cast<size_t>(elem_index) < store.size(), "Element beign deallocated does not belong");
		ASSERT(!store[elem_index].is_free, "Expected in-use object at index:" + std::to_string(elem_index));

		store[elem_index].is_free = true;
	}
};
