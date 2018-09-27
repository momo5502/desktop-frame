#pragma once

#ifndef _WIN32
#error "Only supported on Windows"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>
#include <functional>

#include <delayimp.h>
#pragma comment(lib, "delayimp.lib")

namespace utils
{
	inline namespace library
	{
		class dynlib
		{
		public:
			static dynlib load(std::string module);
			static dynlib get_by_address(void* address);
			static DLL_DIRECTORY_COOKIE add_load_path(std::string path);
			static void remove_load_path(DLL_DIRECTORY_COOKIE cookie);

			dynlib();
			dynlib(std::string name, bool load = true);
			dynlib(HMODULE handle);

			dynlib(const dynlib& a) : dynlib(a.handle) {}

			operator bool();
			bool operator!=(const dynlib &obj) const { return !(*this == obj); };
			bool operator==(const dynlib &obj) const;

			void unprotect();
			void* get_entry_point();
			size_t get_relative_entry_point();

			bool is_valid();
			std::string get_name();
			std::string get_path();
			std::string get_folder();
			std::uint8_t* get_ptr();
			void free();

			HMODULE get_handle();

			template <typename T>
			T getProc(std::string process)
			{
				if (!this->is_valid()) nullptr;
				return reinterpret_cast<T>(GetProcAddress(this->handle, process.data()));
			}

			template <typename T>
			std::function<T> get(std::string process)
			{
				if (!this->is_valid()) std::function<T>();
				return reinterpret_cast<T*>(this->getProc<void*>(process));
			}

			template<typename T, typename... Args>
			T invoke(std::string process, Args... args)
			{
				auto method = this->get<T(__cdecl)(Args...)>(process);
				if (method) return method(args...);
				return T();
			}

			template<typename T, typename... Args>
			T invoke_pascal(std::string process, Args... args)
			{
				auto method = this->get<T(__stdcall)(Args...)>(process);
				if (method) return method(args...);
				return T();
			}

			template<typename T, typename... Args>
			T invoke_this(std::string process, void* thisPtr, Args... args)
			{
				auto method = this->get<T(__thiscall)(void*, Args...)>(thisPtr, process);
				if (method) return method(args...);
				return T();
			}

			std::vector<PIMAGE_SECTION_HEADER> get_section_headers();

			PIMAGE_NT_HEADERS get_nt_headers();
			PIMAGE_DOS_HEADER get_dos_header();
			PIMAGE_OPTIONAL_HEADER get_optional_header();

			size_t get_code_size();
			void* get_code_start();

			void** get_iat_entry(std::string moduleName, std::string procName);

#ifdef _DELAY_IMP_VER
			bool delay_import();
#endif

		private:
			HMODULE handle;
		};

		dynlib operator"" _lib(const char* str, size_t len);
	}
}