#include "std_include.hpp"
#include "utils/library.hpp"

namespace utils
{
	inline namespace library
	{
		dynlib dynlib::load(std::string name)
		{
			return dynlib(name, true);
		}

		dynlib dynlib::get_by_address(void* address)
		{
			HMODULE handle = nullptr;
			GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCSTR>(address), &handle);
			return dynlib(handle);
		}

		DLL_DIRECTORY_COOKIE dynlib::add_load_path(std::string path)
		{
			DLL_DIRECTORY_COOKIE result = nullptr;

			char out_buffer[MAX_PATH] = { 0 };
			if (_fullpath(out_buffer, path.data(), sizeof(out_buffer)))
			{
				path = out_buffer;
				result = AddDllDirectory(std::wstring(path.begin(), path.end()).data());
				SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
			}

			return result;
		}

		void dynlib::remove_load_path(DLL_DIRECTORY_COOKIE cookie)
		{
			if (cookie)
			{
				RemoveDllDirectory(cookie);
			}
		}

		dynlib::dynlib()
		{
			this->handle = GetModuleHandleA(nullptr);
		}

		dynlib::dynlib(std::string name, bool load)
		{
			if (load) this->handle = LoadLibraryA(name.data());
			else this->handle = GetModuleHandleA(name.data());
		}

		dynlib::dynlib(HMODULE handle)
		{
			this->handle = handle;
		}

		dynlib::operator bool()
		{
			return this->is_valid();
		}

		bool dynlib::operator==(const dynlib &obj) const
		{
			return this->handle == obj.handle;
		}

		PIMAGE_NT_HEADERS dynlib::get_nt_headers()
		{
			if (!this->is_valid()) return nullptr;
			return reinterpret_cast<PIMAGE_NT_HEADERS>(this->get_ptr() + this->get_dos_header()->e_lfanew);
		}

		PIMAGE_DOS_HEADER dynlib::get_dos_header()
		{
			return reinterpret_cast<PIMAGE_DOS_HEADER>(this->get_ptr());
		}

		PIMAGE_OPTIONAL_HEADER dynlib::get_optional_header()
		{
			if (!this->is_valid()) return nullptr;
			return &this->get_nt_headers()->OptionalHeader;
		}

		std::vector<PIMAGE_SECTION_HEADER> dynlib::get_section_headers()
		{
			std::vector<PIMAGE_SECTION_HEADER> headers;

			auto ntheaders = this->get_nt_headers();
			auto section = IMAGE_FIRST_SECTION(ntheaders);

			for (uint16_t i = 0; i < ntheaders->FileHeader.NumberOfSections; ++i, ++section)
			{
				if (section) headers.push_back(section);
				else OutputDebugStringA("There was an invalid section :O");
			}

			return headers;
		}

		std::uint8_t* dynlib::get_ptr()
		{
			return reinterpret_cast<std::uint8_t*>(this->handle);
		}

		void dynlib::unprotect()
		{
			if (!this->is_valid()) return;

			DWORD protection;
			VirtualProtect(this->get_ptr(), this->get_optional_header()->SizeOfImage, PAGE_EXECUTE_READWRITE, &protection);
		}

		size_t dynlib::get_relative_entry_point()
		{
			if (!this->is_valid()) return 0;
			return this->get_nt_headers()->OptionalHeader.AddressOfEntryPoint;
		}

		void* dynlib::get_entry_point()
		{
			if (!this->is_valid()) return nullptr;
			return this->get_ptr() + this->get_relative_entry_point();
		}

		bool dynlib::is_valid()
		{
			return this->handle != nullptr && this->get_dos_header()->e_magic == IMAGE_DOS_SIGNATURE;
		}

		std::string dynlib::get_name()
		{
			if (!this->is_valid()) return "";

			std::string path = this->get_path();
			auto pos = path.find_last_of("/\\");
			if (pos == std::string::npos) return path;

			return path.substr(pos + 1);
		}

		std::string dynlib::get_folder()
		{
			if (!this->is_valid()) return "";

			std::string path = this->get_path();
			auto pos = path.find_last_of("/\\");
			if (pos == std::string::npos) return ".\\";

			return path.substr(0, pos + 1);
		}

		std::string dynlib::get_path()
		{
			if (!this->is_valid()) return "";

			char name[MAX_PATH] = { 0 };
			GetModuleFileNameA(this->handle, name, sizeof name);

			return name;
		}

		void dynlib::free()
		{
			if (this->is_valid())
			{
				FreeLibrary(this->handle);
				this->handle = nullptr;
			}
		}

		HMODULE dynlib::get_handle()
		{
			return this->handle;
		}

		size_t dynlib::get_code_size()
		{
			if (this->is_valid())
			{
				auto headers = this->get_section_headers();
				for (auto& header : headers)
				{
					using namespace std::literals;
					if (LPSTR(header->Name) == ".text"s)
					{
						return header->SizeOfRawData;
					}
				}
			}

			return 0;
		}

		void* dynlib::get_code_start()
		{
			if (this->is_valid())
			{
				auto headers = this->get_section_headers();
				for (auto& header : headers)
				{
					using namespace std::literals;
					if (LPSTR(header->Name) == ".text"s)
					{
						return this->get_ptr() + header->VirtualAddress;
					}
				}
			}

			return nullptr;
		}

		void** dynlib::get_iat_entry(std::string moduleName, std::string procName)
		{
			if (!this->is_valid()) return nullptr;

			dynlib other_module(moduleName);
			if (!other_module.is_valid()) return nullptr;

			void* target_function = other_module.getProc<void*>(procName);
			if (!target_function) return nullptr;

			auto* header = this->get_optional_header();
			if (!header) return nullptr;

			auto* import_descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(this->get_ptr() + header->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

			while (import_descriptor->Name)
			{
				if (!_stricmp(reinterpret_cast<char*>(this->get_ptr() + import_descriptor->Name), moduleName.data()))
				{
					auto* original_thunk_data = reinterpret_cast<PIMAGE_THUNK_DATA>(import_descriptor->OriginalFirstThunk + this->get_ptr());
					auto* thunk_data = reinterpret_cast<PIMAGE_THUNK_DATA>(import_descriptor->FirstThunk + this->get_ptr());

					while (original_thunk_data->u1.AddressOfData)
					{
						LPSTR name = nullptr;

						if (IMAGE_SNAP_BY_ORDINAL(original_thunk_data->u1.Ordinal))
						{
							name = MAKEINTRESOURCEA(IMAGE_ORDINAL(original_thunk_data->u1.Ordinal));
						}
						else
						{
							name = PIMAGE_IMPORT_BY_NAME(this->get_ptr() + original_thunk_data->u1.ForwarderString)->Name;
						}

						if (GetProcAddress(other_module.handle, name) == target_function)
						{
							return reinterpret_cast<void**>(&thunk_data->u1.Function);
						}

						++original_thunk_data;
						++thunk_data;
					}

					break;
				}

				++import_descriptor;
			}

			return nullptr;
		}

#ifdef _DELAY_IMP_VER
		bool dynlib::delay_import()
		{
			return SUCCEEDED(__HrLoadAllImportsForDll(this->get_name().data()));
		}
#endif

		dynlib operator"" _lib(const char* str, size_t len)
		{
			std::string libname(str, len);
			return dynlib(libname);
		}
	}
}