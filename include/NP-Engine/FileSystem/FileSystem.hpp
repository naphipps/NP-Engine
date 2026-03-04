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

namespace np::fsys
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
		the identity append function used for the templated append function below
	*/
	static inline std::string Append(std::string a)
	{
		return a;
	}

	/*
		appends the given path args to the given path a
	*/
	template <typename... Args, std::enable_if_t<(std::is_constructible_v<std::string, Args> && ...), bl> = true>
	static inline std::string Append(std::string a, Args&&... args)
	{
		std::filesystem::path p(a);
		p /= (::std::filesystem::path(Append(args...)));
		return p.string().c_str();
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
} // namespace np::fsys

#endif /* NP_ENGINE_FILESYSTEM_HPP */