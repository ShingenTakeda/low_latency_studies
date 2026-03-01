#pragma once
#include "../helpers.hpp"

#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#include <atomic>

template<typename T>
struct LFQueue final
{
	std::vector<T> store;
	std::atomic<size_t> next_write_index = {0};
	std::atomic<size_t> next_read_index = {0};

	std::atomic<size_t> num_elements = {0};

	LFQueue(size_t ini_size) : store(ini_size, T()) {}

	LFQueue() = delete;
	LFQueue(const LFQueue&) = delete;
	LFQueue(const LFQueue&&) = delete;
	LFQueue &operator=(const LFQueue&) = delete;
	LFQueue &operator=(const LFQueue&&) = delete;

	auto get_next_to_write_to() noexcept
	{
		return &store[next_write_index];
	}

	auto update_write_index() noexcept
	{
		next_write_index = (next_write_index + 1) % store.size();
		num_elements++;
	}

	auto get_next_read_index() const noexcept -> const T *
	{
		return (next_read_index == next_write_index) ? nullptr : &store[next_read_index];
	}

	auto update_read_index() noexcept
	{
		next_read_index = (next_read_index + 1) % store.size();
		ASSERT(num_elements != 0, "Read an invalid element in: " + std::to_string(pthread_self()));
		num_elements--;
	}

	auto size() noexcept
	{
		return num_elements.load();
	}
};
