#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <fstream>
#include <cstdio>
#include <thread>

#include "../helpers.hpp"
#include "../lock_free_queue/lock_free_queue.hpp"
#include "../threading/thread_utils.hpp"
#include "../time_utils.hpp"

constexpr size_t LOG_SIZE_QUEUE = 8 * 1024 * 1024;

//Missing some types
enum class LogType : uint8_t 
{
	CHAR = 0,
	INTEGER = 1, LONG_INTERGER = 2, LONG_LONG_INTERGER = 3,
	UNSIGNED_INTERGER = 4, UNSIGNED_LONG_INTERGER = 5, UNSIGNED_LONG_LONG_INTERGER = 6,
	FLOAT = 7, DOUBLE = 8
};

struct LogElement
{
	LogType type = LogType::CHAR;

	union
	{
		char c;
		int i; long l; long long ll;
		unsigned u; unsigned long ul; unsigned long long ull;
		float f; double d;
	}u;
};

struct Logger final
{
	const std::string file_name;
	std::ofstream file;
	LFQueue<LogElement> queue;
	std::atomic<bool> running = {true};
	std::thread *logger_thread = nullptr;

	explicit Logger(const std::string &filename) : file_name(filename), queue(LOG_SIZE_QUEUE)
	{
		file.open(file_name);
		ASSERT(file.is_open(), "Could not open log file: " + file_name);
		logger_thread = create_and_start_thread(-1, "Common/logger{" + file_name, [this](){flush_queue();});
		ASSERT(logger_thread != nullptr, "Could not create logger thread!");
	}

	void flush_queue() noexcept
	{
		while(running)
		{
			for(auto next = queue.get_next_read_index(); queue.size() && next; next = queue.get_next_read_index())
			{
				switch(next->type)
				{
					case LogType::CHAR:
						file << next->u.c;
						break;
					case LogType::INTEGER:
						file << next->u.i;
						break;
					case LogType::LONG_INTERGER:
						file << next->u.l;
						break;
					case LogType::LONG_LONG_INTERGER:
						file << next->u.ll;
						break;
					case LogType::UNSIGNED_INTERGER:
						file << next->u.u;
						break;
					case LogType::UNSIGNED_LONG_INTERGER:
						file << next->u.ul;
						break;
					case LogType::UNSIGNED_LONG_LONG_INTERGER:
						file << next->u.ull;
						break;
					case LogType::FLOAT:
						file << next->u.f;
						break;
					case LogType::DOUBLE:
						file << next->u.d;
						break;
				}
				queue.update_read_index();
				next = queue.get_next_read_index();
			}
			
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
	}

	~Logger()
	{
		std::cerr << "Flushing and closing for " << file_name << std::endl;

		while(queue.size())
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}

		running = false;
		logger_thread->join();
		file.close();
	}

	auto push_value(const LogElement &log_element) noexcept
	{
		*(queue.get_next_to_write_to()) = log_element;
		queue.update_write_index();
	}

	auto push_value(const char value) noexcept
	{
		push_value(LogElement{LogType::CHAR, {.c = value}});
	}

	auto push_value(const char *value) noexcept
	{
		while(*value)
		{
			push_value(LogElement{LogType::CHAR, {.c = *value}});
			value++;
		}
	}

	auto push_value(const std::string &value) noexcept
	{
		push_value(value.c_str());
	}

	auto push_value(const int value) noexcept
	{
		push_value(LogElement{LogType::INTEGER, {.i = value}});
	}

	auto push_value(const long value) noexcept
	{
		push_value(LogElement{LogType::LONG_INTERGER, {.l = value}});
	}

	auto push_value(const long long  value) noexcept
	{
		push_value(LogElement{LogType::LONG_LONG_INTERGER, {.ll = value}});
	}

	auto push_value(const unsigned value) noexcept
	{
		push_value(LogElement{LogType::UNSIGNED_INTERGER, {.u = value}});
	}

	auto push_value(const unsigned long value) noexcept
	{
		push_value(LogElement{LogType::UNSIGNED_LONG_INTERGER, {.ul = value}});
	}

	auto push_value(const unsigned long long value) noexcept
	{
		push_value(LogElement{LogType::UNSIGNED_LONG_LONG_INTERGER, {.ull = value}});
	}

	auto push_value(const float value) noexcept
	{
		push_value(LogElement{LogType::FLOAT, {.f = value}});
	}

	auto push_value(const double value) noexcept
	{
		push_value(LogElement{LogType::DOUBLE, {.d = value}});
	}

	template<typename T, typename ...A>
	auto log(const char *s, const T &value, A... args) noexcept
	{
		while(*s)
		{
			if(*s == '%')
			{
				if(UNLIKELY(*(s+1) == '%'))
				{
					++s;
				}
				else 
				{
					push_value(value);
					log(s + 1, args...);
					return;
				}
			}
			push_value(*s++);
		}
		FATAL("Extra arugments provided to log()");
	}

	auto log(const char *s)
	{
		while(*s)
		{
			if(*s == 's')
			{
				++s;
			}
			else
			{
				FATAL("Missins arguments to log()");
			}
			push_value(*s++);
		}
	}

	Logger() = delete;
	Logger(const Logger&) = delete;
	Logger(const Logger&&) = delete;
	Logger &operator=(const Logger&) = delete;
	Logger &operator=(const Logger&&) = delete;
};
