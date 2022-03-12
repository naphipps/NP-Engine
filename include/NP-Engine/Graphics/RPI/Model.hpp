//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/11/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_MODEL_HPP
#define NP_ENGINE_RPI_MODEL_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/TinyObjLoaderInclude.hpp"
#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Image.hpp"
#include "Vertex.hpp"

namespace np::graphics
{
	class Model
	{
	public:
		constexpr static ui32 VERTEX_STRIDE = 3;
		constexpr static ui32 TEXTURE_COORDINATE_STRIDE = 2;

	protected:
		Image _texture_image;
		::tinyobj::attrib_t _attributes;
		container::vector<::tinyobj::shape_t> _shapes;
		container::vector<::tinyobj::material_t> _materials;
		container::vector<Vertex> _vertices;
		container::vector<ui32> _indices;

	public:
		Model(str model_filename, str texture_image_filename): _texture_image(texture_image_filename)
		{
			::std::vector<::tinyobj::shape_t> shapes;
			::std::vector<::tinyobj::material_t> materials;
			::std::string warning_message;
			::std::string error_message;

			bl loaded =
				::tinyobj::LoadObj(&_attributes, &shapes, &materials, &warning_message, &error_message, model_filename.c_str());

			NP_ENGINE_ASSERT(loaded,
							 "Model '" + model_filename + "' did not load. WARNING: '" + warning_message + "', ERROR: '" +
								 error_message + "'.");

			if (loaded)
			{
				_shapes.assign(shapes.begin(), shapes.end());
				_materials.assign(materials.begin(), materials.end());
				container::umap<Vertex, ui32> vertexIndexMap;

				for (const ::tinyobj::shape_t& shape : shapes)
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
		}

		~Model() = default; // TODO: may not need

		Image& GetTextureImage()
		{
			return _texture_image;
		}

		const Image& GetTextureImage() const
		{
			return _texture_image;
		}

		::tinyobj::attrib_t& GetAttributes()
		{
			return _attributes;
		}

		const ::tinyobj::attrib_t& GetAttributes() const
		{
			return _attributes;
		}

		container::vector<::tinyobj::shape_t>& GetShapes()
		{
			return _shapes;
		}

		const container::vector<::tinyobj::shape_t>& GetShapes() const
		{
			return _shapes;
		}

		container::vector<::tinyobj::material_t>& GetMaterials()
		{
			return _materials;
		}

		const container::vector<::tinyobj::material_t>& GetMaterials() const
		{
			return _materials;
		}

		container::vector<Vertex>& GetVertices()
		{
			return _vertices;
		}

		const container::vector<Vertex>& GetVertices() const
		{
			return _vertices;
		}

		container::vector<ui32>& GetIndices()
		{
			return _indices;
		}

		const container::vector<ui32>& GetIndices() const
		{
			return _indices;
		}
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_MODEL_HPP */