#pragma once
#include <stdio.h>
#include <stdarg.h> // for va_list
#include <string.h> // for strlen
#include <string>

#define MSVC _MSC_VER && !__INTEL_COMPILER
#if MSVC
	#define STRCPY(A, B) strcpy_s(A, B)
#else
	#define STRCPY(A, B) strcpy(A, B)
#endif

namespace smp {
	enum class fileflags : uint8_t {
		READ,
		WRITE,
		APPEND,
		READ_EXTENDED,
		WRITE_EXTENDED,
		APPEND_EXTENDED
	};

	class file
	{
	private:
		fileflags m_fileflags{};
		FILE* m_pFile{};

	public:
		file() {}

		file(const char* filePath, const fileflags& flags)
			: m_fileflags{ flags }
		{
			open(filePath, flags);
		}

		file(const std::string& filePath, const fileflags& flags)
			: file(filePath.c_str(), flags)
		{}

		// copying a file class is disallowed
		file(const file&) = delete;
		void operator=(const file&) = delete;

		// move constructor
		file(file&& other) noexcept
			: m_pFile{ other.m_pFile }, m_fileflags{ other.m_fileflags }
		{
			other.m_pFile = nullptr;
		}

		~file()
		{
			if (m_pFile)
				fclose(m_pFile);
		}

		void open(const char* filePath, const fileflags& flags)
		{
			if (m_pFile) // if we've already opened a file
				fclose(m_pFile);

			char mode[4]{};
			switch (flags)
			{
			case fileflags::READ:
				STRCPY(mode, "rb");
				break;
			case fileflags::WRITE:
				STRCPY(mode, "wb");
				break;
			case fileflags::APPEND:
				STRCPY(mode, "ab");
				break;
			case fileflags::READ_EXTENDED:
				STRCPY(mode, "rb+");
				break;
			case fileflags::WRITE_EXTENDED:
				STRCPY(mode, "wb+");
				break;
			case fileflags::APPEND_EXTENDED:
				STRCPY(mode, "ab+");
				break;
			}
#if MSVC
			fopen_s(&m_pFile, filePath, mode);
#else
			m_pFile = fopen(filePath, mode);
#endif
		}

		void close() { if (m_pFile) fclose(m_pFile); }
		void flush() { fflush(m_pFile); }

		template <typename T>
		file& write(const T& out)
		{
			static_assert(!std::is_pointer<T>::value, "Writing pointers to a file is disallowed!");
			static_assert(!std::is_array<T>::value, "Use 'writearray' instead of 'write' for arrays!");
			static_assert(!std::is_class<T>::value, "Classes aren't allowed to be written to directly!");
			fwrite(&out, sizeof(T), 1, m_pFile);
			return *this;
		}

		// write a pointer parameter to the file. issues appear if
		// you pass in a non-const pointer to this function if the
		// parameter is const
		template <typename T>
		file& writearray(T* out, unsigned int size)
		{
			fwrite(out, sizeof(T), size, m_pFile);
			return *this;
		}

		// specialization for const C strings
		file& write(const char* out)
		{
			size_t len{ strlen(out) };
			fwrite(out, sizeof(char), len, m_pFile);
			char null{ 0 };
			fwrite(&null, sizeof(null), 1, m_pFile);
			return *this;
		}

		// ugly hack so that you can pass both const and non-const C strings
		file& write(char* out)
		{
			return write(reinterpret_cast<const char*>(out));
		}

		// specialization for wide const C strings
		file& write(const wchar_t* out)
		{
			size_t len{ wcslen(out) };
			fwrite(out, sizeof(wchar_t), len, m_pFile);
			wchar_t null{ 0 };
			fwrite(&null, sizeof(null), 1, m_pFile);
			return *this;
		}

		file& write(wchar_t* out)
		{
			return write(reinterpret_cast<const wchar_t*>(out));
		}

		template <typename T, typename... Args>
		file& write(const T& first, const Args... args)
		{
			write(first).write(args...);
			return *this;
		}

		// fill a file with the symbol parameter amt times. for some reason
		// fwrite(&symbol, sizeof(uint8_t), amt, m_pFile) didn't work properly,
		// so instead this will write the largest possible data types to the file
		// in order to do the job. (so if the amt == 12, it will write a longlong
		// and then an int, to minimize OS file calls)
		// i'm sure there's a better solution to this though
		file& fill(unsigned int amt, uint8_t symbol = 0)
		{
			// create a uint64 and fill the first byte with symbol
			uint64_t int64written{ symbol };
			// now fill all the other bytes with symbol. this is
			// 7 since the first byte's already done
			for (int i{ 1 }; i < 8; i++)
			{
				int64written |= (static_cast<uint64_t>(symbol) << 8 * i);
			}

			if (amt > 0)
			{
				unsigned int remainder{ amt % 8 };
				unsigned int amtdiv8{ amt / 8 };
				if (amtdiv8 > 0)
				{
					for (unsigned int i{ 0 }; i < amtdiv8; i++)
						fwrite(&int64written, sizeof(uint64_t), 1, m_pFile);
				}
				if (remainder > 0)
				{
					fwrite(&int64written, remainder, 1, m_pFile);
				}
			}
			return *this;
		}

		// =================================================
		// the option to read is given in two ways: either return the result
		// that's been read, or store it in the given parameter directly. for
		// many arguments, only the latter can be done
		// =================================================

		// read T and return it.
		template <typename T>
		T read()
		{
			static_assert(!std::is_pointer<T>::value, "Reading pointers is disallowed!");
			static_assert(!std::is_array<T>::value, "Use 'readarray' instead of 'read' for arrays!");
			static_assert(!std::is_class<T>::value, "Classes aren't allowed to be read from directly!");

			T buffer{};
			fread_s(&buffer, sizeof(T), sizeof(T), 1, m_pFile);
			return buffer;
		}

		// read T and store the result in the parameter out
		template <typename T>
		void read(T& out)
		{
			static_assert(!std::is_pointer<T>::value, "Reading pointers is disallowed!");
			static_assert(!std::is_array<T>::value, "Use 'readarray' instead of 'read' for arrays!");
			static_assert(!std::is_class<T>::value, "Classes aren't allowed to be read from directly!");

			fread_s(&out, sizeof(T), sizeof(T), 1, m_pFile);
		}

		// same as above but for many arguments
		template <typename T, typename... Args>
		void read(T& out, Args... args)
		{
			read(out);
			read(args...);
		}

		template <typename T>
		void readarray(T* out, unsigned int size)
		{
			for (unsigned int i{ 0 }; i < size; i++)
				//out[i] = read<T>();
				read<T>(out[i]);
		}

		char getc() { static_cast<char>(fgetc(m_pFile)); }
		wchar_t getwc() { static_cast<wchar_t>(fgetwc(m_pFile)); }
		void putc(char input) { fputc(input, m_pFile); }
		void putwc(wchar_t input) { fputwc(input, m_pFile); }
		char* gets(char* buffer, int maxCount) { fgets(buffer, maxCount, m_pFile); }
		void puts(const char* str) { fputs(str, m_pFile); }
		wchar_t* getws(wchar_t* buffer, int maxCount) { fgetws(buffer, maxCount, m_pFile); }
		void putws(const wchar_t* wstr) { fputws(wstr, m_pFile); }
#if MSVC
		int vfprintf(const char* format, va_list vlist) { return vfprintf_s(m_pFile, format, vlist); }
		int fprintf(const char* format, va_list vlist) { return fprintf_s(m_pFile, format, vlist); }
#else
		int vfprintf(const char* format, va_list vlist) { return ::vfprintf(m_pFile, format, vlist); }
		int fprintf(const char* format, va_list vlist) { return ::fprintf(m_pFile, format, vlist); }
#endif

		long ftell() { return ::ftell(m_pFile); }
		bool fseek(long offset, int origin)
		{
			int result{ ::fseek(m_pFile, offset, origin) };
			// fseek returns 0 on success, nonzero value otherwise
			return retTrueOnZero(result);
		}

		bool getpos(fpos_t* pos)
		{
			int result{ fgetpos(m_pFile, pos) };
			// same as fseek
			return retTrueOnZero(result);
		}

		bool setpos(const fpos_t* pos)
		{
			int result{ fsetpos(m_pFile, pos) };
			// same as fseek
			return retTrueOnZero(result);
		}

		long getFileSize()
		{
			long curPos{ ftell() };
			fseek(0, SEEK_SET);
			long startPos{ ftell() };
			fseek(0, SEEK_END);
			long endPos{ ftell() };

			fseek(curPos, SEEK_SET);
			return endPos - startPos;
		}

		inline operator bool() { return m_pFile; }

	private:
		inline bool retTrueOnZero(int in)
		{
			if (in != 0)
				return false;
			return true;
		}
	};
}