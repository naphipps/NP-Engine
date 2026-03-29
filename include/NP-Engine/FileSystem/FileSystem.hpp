//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_FSYS_FILESYSTEM_HPP
#define NP_ENGINE_FSYS_FILESYSTEM_HPP

#include <string>
#include <filesystem>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::fsys
{
	/*
		checks if the given file or dir exists
	*/
	static inline bl exists(::std::string s)
	{
		return s.size() > 0 ? ::std::filesystem::exists(::std::filesystem::path(s)) : false;
	}

	/*
		gets the parent path of the given file or dir path
	*/
	static inline ::std::string get_parent_path(::std::string s)
	{
		return s.size() > 0 ? ::std::filesystem::path(s).parent_path().string().c_str() : "";
	}

	/*
		the identity append function used for the templated append function below
	*/
	static inline std::string append(std::string a)
	{
		return a;
	}

	/*
		appends the given path args to the given path a
	*/
	template <typename... Args, std::enable_if_t<(std::is_constructible_v<std::string, Args> && ...), bl> = true>
	static inline std::string append(std::string a, Args&&... args)
	{
		std::filesystem::path p(a);
		p /= (::std::filesystem::path(append(args...)));
		return p.string().c_str();
	}

	/*
		creates all the directories throughout the given dir path
	*/
	static inline void create_directories(::std::string s)
	{
		if (s.size() > 0)
			::std::filesystem::create_directories(::std::filesystem::path(s));
	}

	/*
		gets the current working directory path
	*/
	static inline ::std::string get_current_path()
	{
		return ::std::filesystem::current_path().string().c_str();
	}

	/*
		sets the current working directory path
	*/
	static inline void set_current_path(::std::string s)
	{
		::std::filesystem::current_path(::std::filesystem::path(s));
	}
} // namespace np::fsys

#endif /* NP_ENGINE_FSYS_FILESYSTEM_HPP */