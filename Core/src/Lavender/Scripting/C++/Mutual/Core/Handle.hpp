#pragma once

namespace Lavender::Script
{

	class Entity;

	template<typename T>
	struct Handle
	{
	public:
		Handle() = default;
		Handle(T* value)
			: m_Value(value)
		{
		}
		~Handle() = default;

		inline Handle& operator = (const T& newValue)
		{
			*m_Value = newValue;
			return *this;
		}

		inline operator T ()
		{
			return *m_Value;
		}

		/*
		inline operator T& ()
		{
			return *m_Value;
		}
		*/

		inline T* operator -> ()
		{
			return m_Value;
		}

		inline T& operator * ()
		{
			return *m_Value;
		}

	private:
		T* m_Value = nullptr;
	};

}