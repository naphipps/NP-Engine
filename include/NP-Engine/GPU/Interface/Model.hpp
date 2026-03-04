////##===----------------------------------------------------------------------===##//
////
////  Author: Nathan Phipps 3/11/22
////
////##===----------------------------------------------------------------------===##//
//
//#ifndef NP_ENGINE_GPU_INTERFACE_MODEL_HPP
//#define NP_ENGINE_GPU_INTERFACE_MODEL_HPP
//
//#include <utility>
//
//#include "NP-Engine/String/String.hpp"
//#include "NP-Engine/Container/Container.hpp"
//#include "NP-Engine/Primitive/Primitive.hpp"
//#include "NP-Engine/Geometry/Geometry.hpp"
//#include "NP-Engine/FileSystem/FileSystem.hpp"
//
//#include "NP-Engine/Vendor/GlmInclude.hpp"
//#include "NP-Engine/Vendor/RapidJsonInclude.hpp"
//
//#include "Texture.hpp"
//#include "Vertex.hpp"
//
//// TODO: I think we should just use tinyobjloader/tinygltfloader internally, then transition all relevant data to our own
///structures, like how we treat lodepng and our Image.hpp / TODO: ^ no need to worry about allocators, just let it run and then
///we get rid of the results after we transition them
//
//// TODO: I feel like we need to load the skeleton somewhere around here, and them base it off our root bone
//
//namespace np::gpu
//{
//	class Model
//	{
//	public:
//		constexpr static ui32 VERTEX_STRIDE = 3;
//		constexpr static ui32 TEXTURE_COORDINATE_STRIDE = 2;
//
//		struct Accessor
//		{
//			siz bufferView = -1;
//			siz byteOffset = 0;
//			siz componentType = -1;
//			siz count = 0;
//			con::vector<flt> max;
//			con::vector<flt> min;
//			str type = "";
//		};
//
//		struct Animation {};
//		struct Buffer {};
//		struct BufferView {};
//		struct Material {};
//		struct Mesh {};
//		struct Node {};
//		struct Skin {};
//		struct Sampler {};
//		struct Scene {};
//
//
//	protected:
//
//		//bl _hot_reloadable;
//		// TODO: ^ I almost think we need some delegates for the actual model/image/light to call their renderable object for
//		// SetOutOfDate(false) calls to happen automatically
//		// TODO: ^ mem::Delegate _on_change_delegate;
//
//		str _filename;
//
//		con::vector<Accessor> _accessors;
//		con::vector<Animation> _animations;
//		con::vector<Buffer> _buffers;
//		con::vector<BufferView> _buffer_views;
//		con::vector<Material> _materials;
//		con::vector<Mesh> _meshes;
//		con::vector<Node> _nodes;
//		con::vector<Texture> _textures;
//		con::vector<Image> _images;
//		con::vector<Skin> _skins;
//		con::vector<Sampler> _samplers;
//		con::vector<Scene> _scenes;
//
//		i32 _scene;
//
//		geom::FltAabb3D _original_aabb;
//		geom::Transform _transform;
//
//		con::vector<Accessor> ExtractAccessors(const ::rapidjson::Value& accessors)
//		{
//			NP_ENGINE_ASSERT(accessors.IsArray(), "ExtractAccessors requires array");
//
//			con::vector<Accessor> extracted{};
//			for (auto it = accessors.Begin(); it != accessors.End(); it++)
//			{
//				/*
//				{
//					"bufferView" : 1,
//						"byteOffset" : 0,
//						"componentType" : 5126,
//						"count" : 36,
//						"max" : [
//							1.000000,
//								1.000000,
//								1.000001
//						] ,
//						"min" : [
//							-1.000000,
//								-1.000000,
//								-1.000000
//						] ,
//						"type" : "VEC3"
//				},
//					*/
//
//				Accessor a{};
//
//				a.bufferView = (*it)["bufferView"].GetInt();
//
//				extracted.emplace_back(a);
//			}
//
//			return extracted;
//		}
//
//	public:
//		Model(str filename = "") : _filename(filename), _scene(0)
//		{
//			Load(_filename);
//		}
//
//		// TODO: copy and move constructors
//
//		virtual ~Model() = default;
//
//		// TODO: copy and move assign operators
//
//		bl Load(str filename)
//		{
//			Clear();
//
//			_filename = filename;
//
//			::std::ifstream ifile(_filename);
//			::rapidjson::Document json{};
//			::rapidjson::ParseResult result = json.ParseStream(ifile);
//
//			result.IsError(); //TODO: handle parse errors
//
//			_accessors = ExtractAccessors(json["accessors"].GetArray());
//
//
//			::rapidjson::Value accessors = json["accessor"].GetArray();
//
//			ExtractAccessors(accessors);
//			accessors.Size();
//			accessors[0].GetObject();
//
//			for (auto it = accessors.Begin(); it != accessors.End(); it++)
//			{
//				Accessor a{};
//
//
//				break;
//			}
//
//
//
//
//			return false;
//		}
//
//		void Clear()
//		{
//			_filename.clear();
//			_accessors.clear();
//
//			_original_aabb = {};
//			_transform = {};
//		}
//
//		void SetHotReloadable(bl hot_reloadable = true)
//		{
//			_hot_reloadable = hot_reloadable;
//		}
//
//		bl IsHotReloadable() const
//		{
//			return _hot_reloadable;
//		}
//
//		geom::Transform& GetTransform()
//		{
//			return _transform;
//		}
//
//		const geom::Transform& GetTransform() const
//		{
//			return _transform;
//		}
//
//		geom::FltObb3D GetObb() const
//		{
//			geom::FltObb3D obb;
//			obb.center = _original_aabb.center + _transform.position; // TODO: may not be correct
//			obb.halfLengths = _original_aabb.halfLengths * _transform.scale; // TODO: may not be correct
//			obb.orientation = _transform.orientation;
//			return obb;
//		}
//
//		Texture& GetTexture()
//		{
//			return _texture;
//		}
//
//		const Texture& GetTexture() const
//		{
//			return _texture;
//		}
//
//		::tinyobj::attrib_t& GetAttributes()
//		{
//			return _attributes;
//		}
//
//		const ::tinyobj::attrib_t& GetAttributes() const
//		{
//			return _attributes;
//		}
//
//		con::vector<::tinyobj::shape_t>& GetShapes()
//		{
//			return _shapes;
//		}
//
//		const con::vector<::tinyobj::shape_t>& GetShapes() const
//		{
//			return _shapes;
//		}
//
//		con::vector<::tinyobj::material_t>& GetMaterials()
//		{
//			return _materials;
//		}
//
//		const con::vector<::tinyobj::material_t>& GetMaterials() const
//		{
//			return _materials;
//		}
//
//		con::vector<Vertex>& GetVertices()
//		{
//			return _vertices;
//		}
//
//		const con::vector<Vertex>& GetVertices() const
//		{
//			return _vertices;
//		}
//
//		con::vector<ui32>& GetIndices()
//		{
//			return _indices;
//		}
//
//		const con::vector<ui32>& GetIndices() const
//		{
//			return _indices;
//		}
//	};
//} // namespace np::gpu
//
//#endif /* NP_ENGINE_GPU_INTERFACE_MODEL_HPP */
//
///*
//class Model {
// public:
//  Model() = default;
//  DEFAULT_METHODS(Model)
//
//  bool operator==(const Model &) const;
//
//  std::vector<Accessor> accessors;
//  std::vector<Animation> animations;
//  std::vector<Buffer> buffers;
//  std::vector<BufferView> bufferViews;
//  std::vector<Material> materials;
//  std::vector<Mesh> meshes;
//  std::vector<Node> nodes;
//  std::vector<Texture> textures;
//  std::vector<Image> images;
//  std::vector<Skin> skins;
//  std::vector<Sampler> samplers;
//  std::vector<Camera> cameras;
//  std::vector<Scene> scenes;
//  std::vector<Light> lights;
//  std::vector<AudioEmitter> audioEmitters;
//  std::vector<AudioSource> audioSources;
//
//  int defaultScene{-1};
//  std::vector<std::string> extensionsUsed;
//  std::vector<std::string> extensionsRequired;
//
//  Asset asset;
//
//  Value extras;
//  ExtensionMap extensions;
//
//  // Filled when SetStoreOriginalJSONForExtrasAndExtensions is enabled.
//  std::string extras_json_string;
//  std::string extensions_json_string;
//};
//*/
//
//
///* Example:
//{
//   "accessors" : [
//	  {
//		 "bufferView" : 0,
//		 "byteOffset" : 0,
//		 "componentType" : 5123,
//		 "count" : 36,
//		 "max" : [
//			35
//		 ],
//		 "min" : [
//			0
//		 ],
//		 "type" : "SCALAR"
//	  },
//	  {
//		 "bufferView" : 1,
//		 "byteOffset" : 0,
//		 "componentType" : 5126,
//		 "count" : 36,
//		 "max" : [
//			1.000000,
//			1.000000,
//			1.000001
//		 ],
//		 "min" : [
//			-1.000000,
//			-1.000000,
//			-1.000000
//		 ],
//		 "type" : "VEC3"
//	  },
//	  {
//		 "bufferView" : 2,
//		 "byteOffset" : 0,
//		 "componentType" : 5126,
//		 "count" : 36,
//		 "max" : [
//			1.000000,
//			1.000000,
//			1.000000
//		 ],
//		 "min" : [
//			-1.000000,
//			-1.000000,
//			-1.000000
//		 ],
//		 "type" : "VEC3"
//	  },
//	  {
//		 "bufferView" : 3,
//		 "byteOffset" : 0,
//		 "componentType" : 5126,
//		 "count" : 36,
//		 "max" : [
//			1.000000,
//			-0.000000,
//			-0.000000,
//			1.000000
//		 ],
//		 "min" : [
//			0.000000,
//			-0.000000,
//			-1.000000,
//			-1.000000
//		 ],
//		 "type" : "VEC4"
//	  },
//	  {
//		 "bufferView" : 4,
//		 "byteOffset" : 0,
//		 "componentType" : 5126,
//		 "count" : 36,
//		 "max" : [
//			1.000000,
//			1.000000
//		 ],
//		 "min" : [
//			-1.000000,
//			-1.000000
//		 ],
//		 "type" : "VEC2"
//	  }
//   ],
//   "asset" : {
//	  "generator" : "VKTS glTF 2.0 exporter",
//	  "version" : "2.0"
//   },
//   "bufferViews" : [
//	  {
//		 "buffer" : 0,
//		 "byteLength" : 72,
//		 "byteOffset" : 0,
//		 "target" : 34963
//	  },
//	  {
//		 "buffer" : 0,
//		 "byteLength" : 432,
//		 "byteOffset" : 72,
//		 "target" : 34962
//	  },
//	  {
//		 "buffer" : 0,
//		 "byteLength" : 432,
//		 "byteOffset" : 504,
//		 "target" : 34962
//	  },
//	  {
//		 "buffer" : 0,
//		 "byteLength" : 576,
//		 "byteOffset" : 936,
//		 "target" : 34962
//	  },
//	  {
//		 "buffer" : 0,
//		 "byteLength" : 288,
//		 "byteOffset" : 1512,
//		 "target" : 34962
//	  }
//   ],
//   "buffers" : [
//	  {
//		 "byteLength" : 1800,
//		 "uri" : "Cube.bin"
//	  }
//   ],
//   "images" : [
//	  {
//		 "uri" : "Cube_BaseColor.png"
//	  },
//	  {
//		 "uri" : "Cube_MetallicRoughness.png"
//	  }
//   ],
//   "materials" : [
//	  {
//		 "name" : "Cube",
//		 "pbrMetallicRoughness" : {
//			"baseColorTexture" : {
//			   "index" : 0
//			},
//			"metallicRoughnessTexture" : {
//			   "index" : 1
//			}
//		 }
//	  }
//   ],
//   "meshes" : [
//	  {
//		 "name" : "Cube",
//		 "primitives" : [
//			{
//			   "attributes" : {
//				  "NORMAL" : 2,
//				  "POSITION" : 1,
//				  "TANGENT" : 3,
//				  "TEXCOORD_0" : 4
//			   },
//			   "indices" : 0,
//			   "material" : 0,
//			   "mode" : 4
//			}
//		 ]
//	  }
//   ],
//   "nodes" : [
//	  {
//		 "mesh" : 0,
//		 "name" : "Cube"
//	  }
//   ],
//   "samplers" : [
//	  {}
//   ],
//   "scene" : 0,
//   "scenes" : [
//	  {
//		 "nodes" : [
//			0
//		 ]
//	  }
//   ],
//   "textures" : [
//	  {
//		 "sampler" : 0,
//		 "source" : 0
//	  },
//	  {
//		 "sampler" : 0,
//		 "source" : 1
//	  }
//   ]
//}
//
//
//*/