#pragma once

namespace Lavender::Script
{

	class Entity;

	// TODO: Improve this system
	template<typename T>
	struct Handle
	{
	public:
		Handle() = default;
		Handle(const T& value)
			: m_CodedValue(value)
		{
		}
		Handle(T* value)
			: m_Value(value)
		{
		}
		~Handle() = default;

		inline Handle& operator = (const T& newValue)
		{
			if (m_Value)
				*m_Value = newValue;
			else
				m_CodedValue = newValue;

			return *this;
		}

		inline operator T ()
		{
			if (m_Value)
				return *m_Value;

			return m_CodedValue;
		}

		inline T* operator -> ()
		{
			if (m_Value)
				return m_Value;

			return &m_CodedValue;
		}

		inline T& operator * ()
		{
			if (m_Value)
				return *m_Value;

			return m_CodedValue;
		}

	private:
		T* m_Value = nullptr;
		T m_CodedValue = {};
	};

	// TODO: Improve this system
	template<typename T>
	struct FnHandle
	{
	public:
		using SetFn = void (*)(T);
		using GetFn = T (*)();
	public:
		FnHandle() = default;
		FnHandle(const T& value)
			: m_CodedValue(value)
		{
		}
		FnHandle(SetFn setter, GetFn getter)
			: m_Setter(setter), m_Getter(getter)
		{
		}
		~FnHandle() = default;

		inline FnHandle& operator = (const T& newValue)
		{
			if (m_Setter)
				m_Setter(newValue);
			else
				m_CodedValue = newValue;

			return *this;
		}

		inline operator T ()
		{
			if (m_Getter)
				return m_Getter();

			return m_CodedValue;
		}

	private:
		SetFn m_Setter = nullptr;
		GetFn m_Getter = nullptr;

		T m_CodedValue = {};
	};

}