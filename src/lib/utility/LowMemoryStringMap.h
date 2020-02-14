#ifndef LOW_MEMORY_STRING_MAP_H
#define LOW_MEMORY_STRING_MAP_H

#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>

/*
 * StringTraits
 *
 * Defines types related to either std::string or std::wstring
 */

template <typename StringT>
class StringTraits
{
};

template <>
class StringTraits<std::string>
{
public:
	typedef char CharT;
	typedef std::ostream StreamT;

	static size_t SizeFn(const CharT* str)
	{
		return strlen(str);
	}

	static CharT* CopyFn(CharT* destination, const CharT* source, size_t num)
	{
#pragma warning(push)
#pragma warning(disable : 4996)
		return strncpy(destination, source, num);
#pragma warning(pop)
	}
};

template <>
class StringTraits<std::wstring>
{
public:
	typedef wchar_t CharT;
	typedef std::wostream StreamT;

	static size_t SizeFn(const CharT* str)
	{
		return wcslen(str);
	}

	static CharT* CopyFn(CharT* destination, const CharT* source, size_t num)
	{
#pragma warning(push)
#pragma warning(disable : 4996)
		return wcsncpy(destination, source, num);
#pragma warning(pop)
	}
};

/*
 * LowMemoryStringMap
 *
 * Map of string - value pairs, where equal suffixes of strings are used to build tree structure
 * reducing memory consumption.
 *
 * - StringT: string type (supported: std::string, std::wstring)
 * - ValueT: value type
 * - defaultVal: default value of type ValueT (cannot be stored as value)
 */

template <typename StringT, typename ValueT, ValueT defaultVal>
class LowMemoryStringMap
{
public:
	typedef typename StringTraits<StringT>::CharT CharT;
	typedef typename StringTraits<StringT>::StreamT StreamT;

	LowMemoryStringMap(): m_root(StringT()) {}

	void clear()
	{
		m_root.clear();
	}

	bool empty() const
	{
		return m_root.empty();
	}

	/*
	 * Adds a new string - value pair to the map.
	 */
	void add(const StringT& str, const ValueT& val)
	{
		if (str.size())
		{
			m_uncompressedByteSize += str.size() * sizeof(CharT) + sizeof(StringT) + sizeof(ValueT);

			Branch::addTo(&m_root, str, 0, val);
		}
	}

	/*
	 * Finds the value for a string. defaultValue is returned if the string is not found.
	 */
	ValueT find(const StringT& str) const
	{
		return m_root.find(str, 0);
	}

	void print(StreamT& os) const
	{
		m_root.print(os, 0);
	}

	/*
	 * Returns the number of bytes used to store this map.
	 */
	size_t getByteSize() const
	{
		return m_root.getByteSize();
	}

	/*
	 * Returns the number of bytes necessary to store all raw string - value pairs that were added.
	 */
	size_t getUncompressedByteSize() const
	{
		return m_uncompressedByteSize;
	}

private:
	/*
	 * StringTypes
	 *
	 * These types are used to store strings of different lengths with as little memory as possible.
	 * (std::string allocates a certain default capacity and has multiple members for storing size
	 * etc., which take up more space than necessary if string length is already known.)
	 */

	/*
	 * LongString
	 *
	 * Stores all characters in an array with terminating \0 character to avoid storing its size.
	 */
	class LongString
	{
	public:
		LongString(const StringT& str)
		{
			m_str = std::unique_ptr<CharT[]>(new CharT[str.size() + 1]);
			StringTraits<StringT>::CopyFn(m_str.get(), str.c_str(), str.size() + 1);
		}

		StringT getString() const
		{
			return StringT(m_str.get());
		}

		std::pair<bool, size_t> compareString(const StringT& str, size_t idx) const
		{
			size_t size = StringTraits<StringT>::SizeFn(m_str.get());
			return std::make_pair(str.compare(idx, size, m_str.get(), size) == 0, size);
		}

		size_t getByteSize() const
		{
			size_t c = 0;
			while (m_str.get()[c] != CharT(0))
			{
				c++;
			}

			return sizeof(CharT) * (c + 1);
		}

	private:
		std::unique_ptr<CharT[]> m_str;
	};


	/*
	 * ShortString
	 *
	 * Stores all characters in an array without terminating \0. Size is available as template argument.
	 *
	 * - Size: the number of characters
	 */
	template <size_t Size>
	class ShortString
	{
	public:
		ShortString(const StringT& str)
		{
			StringTraits<StringT>::CopyFn(m_str, str.c_str(), Size);
		}

		StringT getString() const
		{
			return StringT(m_str, Size);
		}

		std::pair<bool, size_t> compareString(const StringT& str, size_t idx) const
		{
			return std::make_pair(str.compare(idx, Size, m_str, Size) == 0, Size);
		}

		size_t getByteSize() const
		{
			return 0;
		}

	private:
		CharT m_str[Size];
	};

	/*
	 * EmptyString
	 *
	 * an empty string, taking up no memory.
	 */
	class EmptyString
	{
	public:
		EmptyString(const StringT& str) {}

		StringT getString() const
		{
			return StringT();
		}

		std::pair<bool, size_t> compareString(const StringT& str, size_t idx) const
		{
			return std::make_pair(true, 0);
		}

		size_t getByteSize() const
		{
			return 0;
		}
	};


	/*
	 * Nodes
	 *
	 * These types are used to build the tree structure. They are either branches or leafes and
	 * contain a StringType
	 */

	/*
	 * Node
	 *
	 * Base type used to provide common interface of all elements in the tree.
	 */
	class Node
	{
	public:
		virtual ~Node() = default;

		virtual ValueT getValue() const
		{
			return defaultVal;
		}

		virtual StringT getString() const = 0;

		virtual std::pair<bool, size_t> compareString(const StringT& str, size_t idx) const = 0;

		virtual size_t getByteSize() const = 0;

		virtual ValueT find(const StringT& str, size_t idx) const = 0;

		virtual void print(StreamT& os, size_t depth) const = 0;
	};


	/*
	 * Leaf
	 *
	 * Contains value.
	 */
	class Leaf: public Node
	{
	public:
		Leaf(const ValueT& val): m_value(val) {}

		ValueT getValue() const override
		{
			return m_value;
		}

		ValueT find(const StringT& str, size_t idx) const override
		{
			std::pair<bool, size_t> p = this->compareString(str, idx);

			if (p.first && str.size() - idx == p.second)
			{
				return getValue();
			}

			return defaultVal;
		}

		void print(StreamT& os, size_t depth) const override
		{
			os << StringT(depth, ' ') << this->getString() << '=' << m_value << std::endl;
		}

	private:
		ValueT m_value;
	};


	/*
	 * StringLeaf
	 *
	 * Combines Leaf and StringType to allow for creating leafes of different string lengths, that
	 * can still be stored in a collection of Node types.
	 */
	template <typename StringType>
	class StringLeaf
		: public Leaf
		, public StringType
	{
	public:
		StringLeaf(const StringT& str, const ValueT& val): Leaf(val), StringType(str) {}

		StringT getString() const override
		{
			return StringType::getString();
		}

		std::pair<bool, size_t> compareString(const StringT& str, size_t idx) const override
		{
			return StringType::compareString(str, idx);
		}

		size_t getByteSize() const override
		{
			return sizeof(*this) + StringType::getByteSize();
		}
	};


	/*
	 * Branch
	 *
	 * Has branches and leaves as children, each referenced by their first character.
	 */
	class Branch: public Node
	{
	public:
		bool empty() const
		{
			return m_children.empty();
		}

		void clear()
		{
			m_children.clear();
		}

		static void addTo(Branch* branch, const StringT& str, size_t idx, const ValueT& val)
		{
			CharT c(0);
			if (idx < str.size())
			{
				c = str[idx];
			}

			auto it = branch->m_children.find(c);
			if (it == branch->m_children.end())
			{
				size_t newIdx = idx + 1 >= str.size() ? str.size() : idx + 1;
				branch->m_children.emplace(c, branch->createLeaf(str.substr(newIdx), val));
				return;
			}

			idx++;

			std::unique_ptr<Node> child = std::move(it->second);
			StringT childStr = child->getString();

			auto p = std::mismatch(childStr.begin(), childStr.end(), str.begin() + idx, str.end());

			if (p.first == childStr.end() && p.second == str.end())
			{
				// adding same string, abort
				it->second = std::move(child);
				return;
			}

			size_t length = std::distance(childStr.begin(), p.first);

			std::unique_ptr<Branch> newBranch;
			if (!dynamic_cast<Branch*>(child.get()) || p.first != childStr.end())
			{
				newBranch = branch->split(std::move(child), length);
			}
			else
			{
				newBranch = std::unique_ptr<Branch>(dynamic_cast<Branch*>(child.release()));
			}

			addTo(newBranch.get(), str, idx + length, val);

			it->second = std::move(newBranch);
		}

		size_t getByteSize() const override
		{
			size_t s = m_children.size() * sizeof(std::pair<CharT, std::unique_ptr<Node>>);

			for (const auto& p: m_children)
			{
				s += p.second->getByteSize();
			}

			return s;
		}

		virtual ValueT find(const StringT& str, size_t idx) const override
		{
			if (idx > str.size())
			{
				return defaultVal;
			}

			std::pair<bool, size_t> p = this->compareString(str, idx);
			if (!p.first)
			{
				return defaultVal;
			}
			idx += p.second;

			CharT c(0);
			if (idx < str.size())
			{
				c = str[idx];
				idx++;
			}

			auto it = m_children.find(c);
			if (it != m_children.end())
			{
				return it->second->find(str, idx);
			}

			return defaultVal;
		}

		void print(StreamT& os, size_t depth) const override
		{
			StringT myStr = this->getString();
			if (myStr.size())
			{
				os << StringT(depth, ' ') << myStr << std::endl;
				depth += myStr.size();
			}

			for (const auto& p: m_children)
			{
				os << StringT(depth, ' ') << '|' << p.first << '|' << std::endl;
				p.second->print(os, depth + 3);
			}
		}

	private:
		virtual std::unique_ptr<Branch> createBranch(const StringT& str) const = 0;
		virtual std::unique_ptr<Leaf> createLeaf(const StringT& str, const ValueT& val) const = 0;

		std::unique_ptr<Branch> split(std::unique_ptr<Node> node, size_t idx) const
		{
			StringT str = node->getString();
			std::unique_ptr<Branch> frontBranch = createBranch(str.substr(0, idx));

			CharT c(0);
			if (idx < str.size())
			{
				c = str[idx];
				idx++;
			}

			Branch* oldBranch = dynamic_cast<Branch*>(node.get());
			if (oldBranch)
			{
				std::unique_ptr<Branch> backBranch = createBranch(str.substr(idx));
				backBranch->m_children = std::move(oldBranch->m_children);
				frontBranch->m_children.emplace(c, std::move(backBranch));
			}
			else
			{
				frontBranch->m_children.emplace(c, createLeaf(str.substr(idx), node->getValue()));
			}

			return frontBranch;
		}

		std::map<CharT, std::unique_ptr<Node>> m_children;
	};


	/*
	 * StringBranch
	 *
	 * Combines Branch and StringType to allow for creating branches of different string lengths,
	 * that can still be stored in a collection of Node types.
	 */
	template <typename StringType>
	class StringBranch
		: public Branch
		, public StringType
	{
	public:
		StringBranch(const StringT& str): StringType(str) {}

		StringT getString() const override
		{
			return StringType::getString();
		}

		std::pair<bool, size_t> compareString(const StringT& str, size_t idx) const override
		{
			return StringType::compareString(str, idx);
		}

		size_t getByteSize() const override
		{
			return sizeof(*this) + Branch::getByteSize() + StringType::getByteSize();
		}

	private:
		virtual std::unique_ptr<Branch> createBranch(const StringT& str) const override
		{
			switch (str.size())
			{
			case 0:
				return std::make_unique<StringBranch<EmptyString>>(str);
			case 1:
				return std::make_unique<StringBranch<ShortString<1>>>(str);
			case 2:
				return std::make_unique<StringBranch<ShortString<2>>>(str);
			case 3:
				return std::make_unique<StringBranch<ShortString<3>>>(str);
			case 4:
				return std::make_unique<StringBranch<ShortString<4>>>(str);
			case 5:
				return std::make_unique<StringBranch<ShortString<5>>>(str);
			case 6:
				return std::make_unique<StringBranch<ShortString<6>>>(str);
			case 7:
				return std::make_unique<StringBranch<ShortString<7>>>(str);
			case 8:
				return std::make_unique<StringBranch<ShortString<8>>>(str);
			default:
				return std::make_unique<StringBranch<LongString>>(str);
			}
		}

		virtual std::unique_ptr<Leaf> createLeaf(const StringT& str, const ValueT& val) const override
		{
			switch (str.size())
			{
			case 0:
				return std::make_unique<StringLeaf<EmptyString>>(str, val);
			case 1:
				return std::make_unique<StringLeaf<ShortString<1>>>(str, val);
			case 2:
				return std::make_unique<StringLeaf<ShortString<2>>>(str, val);
			case 3:
				return std::make_unique<StringLeaf<ShortString<3>>>(str, val);
			case 4:
				return std::make_unique<StringLeaf<ShortString<4>>>(str, val);
			case 5:
				return std::make_unique<StringLeaf<ShortString<5>>>(str, val);
			case 6:
				return std::make_unique<StringLeaf<ShortString<6>>>(str, val);
			case 7:
				return std::make_unique<StringLeaf<ShortString<7>>>(str, val);
			case 8:
				return std::make_unique<StringLeaf<ShortString<8>>>(str, val);
			default:
				return std::make_unique<StringLeaf<LongString>>(str, val);
			}
		}
	};

	StringBranch<EmptyString> m_root;

	size_t m_uncompressedByteSize = 0;
};

#endif	  // LOW_MEMORY_STRING_MAP_H
