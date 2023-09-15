//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/7/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NETWORK_INTERFACE_IP_HPP
#define NP_ENGINE_NETWORK_INTERFACE_IP_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"

namespace np
{
	namespace net
	{
		enum class IpType : ui8
		{
			None = 0,
			V4,
			V6
		};

		struct Ip
		{
			virtual IpType GetType() const = 0;
		};

		struct Ipv4 : public Ip
		{
			con::array<ui8, 4> bytes{0, 0, 0, 0};

			Ipv4() {}

			Ipv4(ui8 a, ui8 b, ui8 c, ui8 d)
			{
				bytes[0] = a;
				bytes[1] = b;
				bytes[2] = c;
				bytes[3] = d;
			}

			bl operator==(const Ipv4& other) const
			{
				bl equal = true;
				for (siz i = 0; i < bytes.size(); i++)
					equal &= bytes[i] == other.bytes[i];
				return equal;
			}

			Ipv4& operator=(str s) // TODO: implement
			{
				return *this;
			}

			virtual IpType GetType() const override
			{
				return IpType::V4;
			}
		};

		struct Ipv6 : public Ip
		{
			con::array<ui16, 8> shorts{0, 0, 0, 0, 0, 0, 0, 0};

			Ipv6() {}

			Ipv6(ui16 a, ui16 b, ui16 c, ui16 d, ui16 e, ui16 f, ui16 g, ui16 h)
			{
				shorts[0] = a;
				shorts[1] = b;
				shorts[2] = c;
				shorts[3] = d;
				shorts[4] = e;
				shorts[5] = f;
				shorts[6] = g;
				shorts[7] = h;
			}

			bl operator==(const Ipv6& other) const
			{
				bl equal = true;
				for (siz i = 0; i < shorts.size(); i++)
					equal &= shorts[i] == other.shorts[i];
				return equal;
			}

			Ipv6& operator=(str s) // TODO: implement
			{
				return *this;
			}

			virtual IpType GetType() const override
			{
				return IpType::V6;
			}
		};
	} // namespace net

	static str to_str(const net::Ip& ip)
	{
		str s = "";
		switch (ip.GetType())
		{
		case net::IpType::V4:
		{
			const net::Ipv4& ipv4 = (const net::Ipv4&)ip;
			s = to_str(ipv4.bytes[0]);
			for (auto it = ipv4.bytes.begin() + 1; it != ipv4.bytes.end(); it++)
				s += "." + to_str(*it);
			break;
		}
		case net::IpType::V6:
		{
			const net::Ipv6& ipv6 = (const net::Ipv6&)ip;

			// find longest run of 0's
			siz first = ipv6.shorts.size();
			siz last = first;
			for (siz i = 0; i < ipv6.shorts.size(); i++)
			{
				siz a = i;
				siz b = a;

				while (ipv6.shorts[i] == 0)
				{
					b++;
					i++;
				}

				if (b - a > last - first)
				{
					first = a;
					last = b;
				}
			}

			// to str
			::std::stringstream ss;
			ss << ::std::hex;
			for (siz i = 0; i < ipv6.shorts.size(); i++)
			{
				if (i < first || i >= last)
				{
					if (i > 0)
						ss << ":";
					ss << ipv6.shorts[i];
				}
				else if (i == first)
				{
					ss << ":";
				}
			}
			s = ss.str();
			break;
		}
		default:
			break;
		}

		for (siz i = 0; i < s.size(); i++)
			s[i] = ::std::tolower(s[i]);

		return s;
	}
} // namespace np

namespace std
{
	template <>
	struct hash<::np::net::Ipv4>
	{
		::np::siz operator()(const ::np::net::Ipv4& ipv4) const noexcept
		{
			return ::np::mat::HashFnv1aUi64(ipv4.bytes.data(), ipv4.bytes.size());
		}
	};

	template <>
	struct hash<::np::net::Ipv6>
	{
		::np::siz operator()(const ::np::net::Ipv6& ipv6) const noexcept
		{
			return ::np::mat::HashFnv1aUi64(ipv6.shorts.data(), ipv6.shorts.size() * sizeof(::np::ui16));
		}
	};

	template <>
	struct hash<pair<::np::net::Ipv4, ::np::ui16>>
	{
		::np::siz operator()(const pair<::np::net::Ipv4, ::np::ui16>& ipv4_prt) const noexcept
		{
			::np::siz ipv4_h = hash<::np::net::Ipv4>{}(ipv4_prt.first);
			return ::np::mat::HashFnv1aUi64(&ipv4_prt.second, sizeof(::np::ui16), ipv4_h);
		}
	};

	template <>
	struct hash<pair<::np::net::Ipv6, ::np::ui16>>
	{
		::np::siz operator()(const pair<::np::net::Ipv6, ::np::ui16>& ipv6_prt) const noexcept
		{
			::np::siz ipv6_h = hash<::np::net::Ipv6>{}(ipv6_prt.first);
			return ::np::mat::HashFnv1aUi64(&ipv6_prt.second, sizeof(::np::ui16), ipv6_h);
		}
	};
} // namespace std

#endif /* NP_ENGINE_NETWORK_INTERFACE_IP_HPP */