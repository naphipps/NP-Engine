//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_MESSAGE_HPP
#define NP_ENGINE_NETWORK_INTERFACE_MESSAGE_HPP

#ifndef NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE
	#define NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE MEBIBYTE_SIZE
#endif

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/RapidJsonInclude.hpp"

namespace np::net
{
	enum class MessageType : ui32
	{
		None = 0,
		Disconnect,
		Ping,
		Pong,
		Blob,
		Text,
		Json,

		Max
	};

	struct MessageHeader
	{
		MessageType type = MessageType::None;
		siz bodySize = 0;

		operator bl() const
		{
			return IsValid();
		}

		bl IsValid() const
		{
			return type != MessageType::None;
		}

		void Invalidate()
		{
			type = MessageType::None;
			bodySize = 0;
		}
	};

	struct MessageBody
	{
		virtual void* GetData() = 0;

		virtual void SetSize(siz size) = 0;
	};

	struct BlobMessageBody : public MessageBody
	{
		con::vector<ui8> blob;

		virtual void* GetData() override
		{
			return blob.data();
		}

		virtual void SetSize(siz size) override
		{
			blob.resize(::std::min(size, NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE));
		}
	};

	struct TextMessageBody : public MessageBody
	{
		str content;

		virtual void* GetData() override
		{
			return content.data();
		}

		virtual void SetSize(siz size) override
		{
			content.resize(::std::min(size, NP_ENGINE_NETWORK_MAX_MESSAGE_BODY_SIZE));
		}
	};

	struct JsonMessageBody : public TextMessageBody
	{
		operator ::rapidjson::Document() const
		{
			::rapidjson::Document doc;
			doc.Parse(content.c_str());
			return doc;
		}

		JsonMessageBody& operator=(const ::rapidjson::Document& doc)
		{
			::rapidjson::StringBuffer buffer;
			::rapidjson::Writer<::rapidjson::StringBuffer> writer(buffer);
			doc.Accept(writer);
			content = buffer.GetString();
			return *this;
		}
	};

	struct Message
	{
		MessageHeader header;
		mem::sptr<MessageBody> body = nullptr;

		operator bl() const
		{
			return IsValid();
		}

		bl IsValid() const
		{
			return header.IsValid();
		}

		void Invalidate()
		{
			header.Invalidate();
			body.reset();
		}
	};
} // namespace np::net

#endif /* NP_ENGINE_NETWORK_INTERFACE_MESSAGE_HPP */