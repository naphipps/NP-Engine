//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/11/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_MODEL_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_MODEL_HPP

#include <utility>

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/TinyObjLoaderInclude.hpp"
#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Texture.hpp"
#include "Vertex.hpp"

namespace np::gfx
{
	class Model
	{
	public:
		constexpr static ui32 VERTEX_STRIDE = 3;
		constexpr static ui32 TEXTURE_COORDINATE_STRIDE = 2;

	protected:
		Texture _texture;
		bl _hot_reloadable;
		// TODO: ^ I almost think we need some delegates for the actual model/image/light to call their renderable object for
		// SetOutOfDate(false) calls to happen automatically
		// TODO: ^ mem::Delegate _on_change_delegate;
		str _filename;
		::tinyobj::attrib_t _attributes;
		con::vector<::tinyobj::shape_t> _shapes;
		con::vector<::tinyobj::material_t> _materials;
		con::vector<Vertex> _vertices;
		con::vector<ui32> _indices;

	public:
		Model(str model_filename, str model_texture_filename, bl hot_reloadable = false):
			_texture(model_texture_filename, hot_reloadable),
			_hot_reloadable(hot_reloadable)
		{
			Load(model_filename);
		}

		Model(const Model& other):
			_texture(other._texture),
			_hot_reloadable(other._hot_reloadable),
			_filename(other._filename),
			_attributes(other._attributes),
			_shapes(other._shapes),
			_materials(other._materials),
			_vertices(other._vertices),
			_indices(other._indices)
		{}

		Model(Model&& other) noexcept:
			_texture(::std::move(other._texture)),
			_hot_reloadable(::std::move(other._hot_reloadable)),
			_filename(::std::move(other._filename)),
			_attributes(::std::move(other._attributes)),
			_shapes(::std::move(other._shapes)),
			_materials(::std::move(other._materials)),
			_vertices(::std::move(other._vertices)),
			_indices(::std::move(other._indices))
		{}

		Model& operator=(const Model& other)
		{
			_texture = other._texture;
			_filename = other._filename;
			_attributes = other._attributes;
			_shapes = other._shapes;
			_materials = other._materials;
			_vertices = other._vertices;
			_indices = other._indices;
			return *this;
		}

		Model& operator=(Model&& other) noexcept
		{
			_texture = ::std::move(other._texture);
			_filename = ::std::move(other._filename);
			_attributes = ::std::move(other._attributes);
			_shapes = ::std::move(other._shapes);
			_materials = ::std::move(other._materials);
			_vertices = ::std::move(other._vertices);
			_indices = ::std::move(other._indices);
			return *this;
		}

		bl Load(str filename)
		{
			_filename = filename;
			::std::vector<::tinyobj::shape_t> shapes;
			::std::vector<::tinyobj::material_t> materials;
			::std::string warning_message;
			::std::string error_message;

			bl loaded =
				::tinyobj::LoadObj(&_attributes, &shapes, &materials, &warning_message, &error_message, _filename.c_str());

			NP_ENGINE_ASSERT(loaded,
							 "Model '" + _filename + "' did not load. WARNING: '" + warning_message + "', ERROR: '" +
								 error_message + "'.");

			if (loaded)
			{
				_shapes.assign(::std::make_move_iterator(shapes.begin()), ::std::make_move_iterator(shapes.end()));
				_materials.assign(::std::make_move_iterator(materials.begin()), ::std::make_move_iterator(materials.end()));
				con::umap<Vertex, ui32> vertexIndexMap;

				for (const ::tinyobj::shape_t& shape : _shapes)
				{
					for (const ::tinyobj::index_t& index : shape.mesh.indices)
					{
						Vertex vertex;

						vertex.Position = {_attributes.vertices[VERTEX_STRIDE * (siz)index.vertex_index + 0],
										   _attributes.vertices[VERTEX_STRIDE * (siz)index.vertex_index + 1],
										   _attributes.vertices[VERTEX_STRIDE * (siz)index.vertex_index + 2]};

						vertex.TextureCoordinate = {
							_attributes.texcoords[TEXTURE_COORDINATE_STRIDE * (siz)index.texcoord_index + 0],
							1.0f - _attributes.texcoords[TEXTURE_COORDINATE_STRIDE * (siz)index.texcoord_index + 1]};

						vertex.Color = {1.0f, 1.0f, 1.0f};

						if (vertexIndexMap.count(vertex) == 0)
						{
							vertexIndexMap[vertex] = (ui32)_vertices.size(); // get the index of the vertex we add next line
							_vertices.emplace_back(vertex);
						}

						_indices.emplace_back(vertexIndexMap[vertex]);
					}
				}
			}

			return loaded;
		}

		void Clear()
		{
			_texture.Clear();
			_attributes = {};
			_shapes.clear();
			_materials.clear();
			_vertices.clear();
			_indices.clear();
			_filename.clear();
			_hot_reloadable = false;
		}

		void SetHotReloadable(bl hot_reloadable = true)
		{
			_hot_reloadable = hot_reloadable;
		}

		bl IsHotReloadable() const
		{
			return _hot_reloadable;
		}

		Texture& GetTexture()
		{
			return _texture;
		}

		const Texture& GetTexture() const
		{
			return _texture;
		}

		::tinyobj::attrib_t& GetAttributes()
		{
			return _attributes;
		}

		const ::tinyobj::attrib_t& GetAttributes() const
		{
			return _attributes;
		}

		con::vector<::tinyobj::shape_t>& GetShapes()
		{
			return _shapes;
		}

		const con::vector<::tinyobj::shape_t>& GetShapes() const
		{
			return _shapes;
		}

		con::vector<::tinyobj::material_t>& GetMaterials()
		{
			return _materials;
		}

		const con::vector<::tinyobj::material_t>& GetMaterials() const
		{
			return _materials;
		}

		con::vector<Vertex>& GetVertices()
		{
			return _vertices;
		}

		const con::vector<Vertex>& GetVertices() const
		{
			return _vertices;
		}

		con::vector<ui32>& GetIndices()
		{
			return _indices;
		}

		const con::vector<ui32>& GetIndices() const
		{
			return _indices;
		}
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_MODEL_HPP */