//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/24/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SERIALIZABLE_HPP
#define NP_ENGINE_SERIALIZABLE_HPP

#include <fstream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/FileSystem/FileSystem.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Vendor/RapidJsonInclude.hpp"

//TODO: our serialization needs have changes so this class is up for redesign

namespace np::srlz
{
	/**
	 this class acts as the basis for all serializable objects
	 the entry points still enable inherited behavior while the protected methods allow for serialization coordination
	 throughout inheritance
	 */
	class Serializable
	{
	protected:
		/**
		 protected template Save method so child classes can directly call the Save of their parents
		 */
		template <typename T>
		bl SaveAs(str filepath, const T* object) const
		{
			bl saved = false;

			str parent_path = fsys::GetParentPath(filepath);

			if (parent_path.size() > 0)
			{
				fsys::CreateDirectories(parent_path);
			}

			if ((parent_path.size() == 0 && filepath.size() > 0) || fsys::Exists(parent_path))
			{
				::std::ofstream os;
				os.open(filepath);
				object->T::Insertion(os, filepath);
				os.close();

				saved = true;
			}

			return saved;
		}

		/**
		protected template Load method so child classes can directly call the Load of their parents
		*/
		template <typename T>
		bl LoadAs(str filepath, T* object)
		{
			bl loaded = false;

			if (fsys::Exists(filepath))
			{
				::std::ifstream is;
				is.open(filepath);
				object->T::Extraction(is, filepath);
				is.close();

				loaded = true;
			}

			return loaded;
		}

	public:
		/**
		 save ourselves to the given json object
		 */
		virtual bl SaveToJson(::rapidjson::Document& json) const
		{
			return false;
		}

		/**
		 load ourselves from the given json object
		 */
		virtual bl LoadFromJson(const ::rapidjson::Document& json)
		{
			return false;
		}

		/**
		 save oursellves as the given filepath
		 return if the save was successful or not
		 */
		bl SaveAs(str filepath) const
		{
			bl saved = false;

			str parent_path = fsys::GetParentPath(filepath);

			if (parent_path.size() > 0)
			{
				fsys::CreateDirectories(parent_path);
			}

			if ((parent_path.size() == 0 && filepath.size() > 0) || fsys::Exists(parent_path))
			{
				::std::ofstream os;
				os.open(filepath);
				Insertion(os, filepath);
				os.close();

				saved = true;
			}

			return saved;
		}

		/**
		 load outselves as the given filepath
		 return if the load was successful or not
		 */
		bl LoadAs(str filepath)
		{
			bl loaded = false;

			if (fsys::Exists(filepath))
			{
				::std::ifstream is;
				is.open(filepath);
				Extraction(is, filepath);
				is.close();

				loaded = true;
			}

			return loaded;
		}

		/**
		 serialization method for us to write
		 we require our objects to know which filepath they are a part of
		 */
		virtual ::std::ostream& Insertion(::std::ostream& os, str filepath) const = 0;

		/**
		 deserialization method for us to read
		 we require our objects to know which filepath they are a part of
		 */
		virtual ::std::istream& Extraction(::std::istream& is, str filepath) = 0;

		/**
		 save oursellves inside the given dirpath
		 return if the save was successful or not
		 */
		virtual bl SaveTo(str dirpath) const = 0;

		/**
		 load outselves from the given dirpath
		 return if the load was successful or not
		 */
		virtual bl LoadFrom(str dirpath) = 0;
	};
} // namespace np::srlz

#endif /* NP_ENGINE_SERIALIZABLE_HPP */
