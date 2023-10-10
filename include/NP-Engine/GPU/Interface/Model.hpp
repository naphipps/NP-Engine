//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/11/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_MODEL_HPP
#define NP_ENGINE_GPU_INTERFACE_MODEL_HPP

#include <utility>

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Geometry/Geometry.hpp"

#include "NP-Engine/Vendor/TinyObjLoaderInclude.hpp"
#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Texture.hpp"
#include "Vertex.hpp"

// TODO: I feel like we need to load the skeleton somewhere around here, and them base it off our root bone

namespace np::gpu
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
		geom::FltAabb3D _original_aabb;
		geom::Transform _transform;

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

		virtual ~Model() = default;

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
				::glm::vec3 min{FLT_MAX, FLT_MAX, FLT_MAX};
				::glm::vec3 max{FLT_MIN, FLT_MIN, FLT_MIN};

				for (const ::tinyobj::shape_t& shape : _shapes)
				{
					for (const ::tinyobj::index_t& index : shape.mesh.indices)
					{
						// TODO: extract Obb and Aabb here

						Vertex vertex;

						vertex.position = {_attributes.vertices[VERTEX_STRIDE * (siz)index.vertex_index + 0],
										   _attributes.vertices[VERTEX_STRIDE * (siz)index.vertex_index + 1],
										   _attributes.vertices[VERTEX_STRIDE * (siz)index.vertex_index + 2]};

						vertex.uv = {_attributes.texcoords[TEXTURE_COORDINATE_STRIDE * (siz)index.texcoord_index + 0],
									 1.0f - _attributes.texcoords[TEXTURE_COORDINATE_STRIDE * (siz)index.texcoord_index + 1]};

						vertex.color = {1.0f, 1.0f, 1.0f};

						if (vertexIndexMap.find(vertex) == vertexIndexMap.end())
						{
							vertexIndexMap[vertex] = (ui32)_vertices.size(); // get the index of the vertex we add next line
							_vertices.emplace_back(vertex);
						}

						_indices.emplace_back(vertexIndexMap[vertex]);

						min.x = ::std::min(min.x, vertex.position.x);
						min.y = ::std::min(min.y, vertex.position.y);
						min.z = ::std::min(min.z, vertex.position.z);
						max.x = ::std::max(max.x, vertex.position.x);
						max.y = ::std::max(max.y, vertex.position.y);
						max.z = ::std::max(max.z, vertex.position.z);
					}
				}

				_original_aabb.halfLengths = {max.x - min.x, max.y - min.y, max.z - min.z};
				_original_aabb.halfLengths.x /= 2.f;
				_original_aabb.halfLengths.y /= 2.f;
				_original_aabb.halfLengths.z /= 2.f;
				_original_aabb.center.x = min.x + _original_aabb.halfLengths.x;
				_original_aabb.center.y = min.y + _original_aabb.halfLengths.y;
				_original_aabb.center.z = min.z + _original_aabb.halfLengths.z;
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

		geom::Transform& GetTransform()
		{
			return _transform;
		}

		const geom::Transform& GetTransform() const
		{
			return _transform;
		}

		geom::FltObb3D GetObb() const
		{
			geom::FltObb3D obb;
			obb.center = _original_aabb.center + _transform.position; // TODO: may not be correct
			obb.halfLengths = _original_aabb.halfLengths * _transform.scale; // TODO: may not be correct
			obb.orientation = _transform.orientation;
			return obb;
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
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_MODEL_HPP */