//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_FILESYSTEM_HPP
#define NP_ENGINE_FILESYSTEM_HPP

#include <string>
#include <filesystem>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::fs
{
	/*
		checks if the given file or dir exists
	*/
	static inline bl Exists(::std::string s)
	{
		bl exists = false;

		if (s.size() > 0)
		{
			exists = ::std::filesystem::exists(::std::filesystem::path(s));
		}

		return exists;
	}

	/*
		gets the parent path of the given file or dir path
	*/
	static inline ::std::string GetParentPath(::std::string s)
	{
		::std::string parent_path;

		if (s.size() > 0)
		{
			parent_path = ::std::filesystem::path(s).parent_path().string().c_str();
		}

		return parent_path;
	}

	/*
		appends the given path b to the given path a
	*/
	static inline ::std::string Append(::std::string a, ::std::string b)
	{
		::std::filesystem::path pa(a);
		::std::filesystem::path pb(b);
		return (pa / pb).string().c_str();
	}

	/*
		creates all the directories throughout the given dir path
	*/
	static inline void CreateDirectories(::std::string s)
	{
		if (s.size() > 0)
		{
			::std::filesystem::create_directories(::std::filesystem::path(s));
		}
	}

	/*
		gets the current working directory path
	*/
	static inline ::std::string GetCurrentPath()
	{
		return ::std::filesystem::current_path().string().c_str();
	}

	/*
		sets the current working directory path
	*/
	static inline void SetCurrentPath(::std::string s)
	{
		::std::filesystem::current_path(::std::filesystem::path(s));
	}
} // namespace np::fs

#endif /* NP_ENGINE_FILESYSTEM_HPP */