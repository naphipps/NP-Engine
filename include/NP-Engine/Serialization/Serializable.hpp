//
//  Serializable.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/24/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_SERIALIZABLE_HPP
#define NP_ENGINE_SERIALIZABLE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/JSON/JSON.hpp"

namespace np
{
	namespace serialization
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

				str parent_path = fs::get_parent_path(filepath);

				if (parent_path.size() > 0)
				{
					fs::create_directories(parent_path);
				}

				if ((parent_path.size() == 0 && filepath.size() > 0) || fs::exists(parent_path))
				{
					ofstrm os;
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

				if (fs::exists(filepath))
				{
					ifstrm is;
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
			virtual bl SaveToJson(nlohmann::json& json) const
			{
				return false;
			}

			/**
			 load ourselves from the given json object
			 */
			virtual bl LoadFromJson(const nlohmann::json& json)
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

				str parent_path = fs::get_parent_path(filepath);

				if (parent_path.size() > 0)
				{
					fs::create_directories(parent_path);
				}

				if ((parent_path.size() == 0 && filepath.size() > 0) || fs::exists(parent_path))
				{
					ofstrm os;
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

				if (fs::exists(filepath))
				{
					ifstrm is;
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
			virtual ostrm& Insertion(ostrm& os, str filepath) const = 0;

			/**
			 deserialization method for us to read
			 we require our objects to know which filepath they are a part of
			 */
			virtual istrm& Extraction(istrm& is, str filepath) = 0;

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
	} // namespace serialization
} // namespace np

#endif /* NP_ENGINE_SERIALIZABLE_HPP */
