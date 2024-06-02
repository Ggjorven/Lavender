#pragma once

#include <string>
#include <functional>
#include <stdexcept>
#include <memory>

namespace Insight
{

    /*
    template <typename T>
    struct function_traits : public function_traits<decltype(&T::operator())>
    {
    };

    template<typename R, typename ...Args>
    struct function_traits<std::function<R(Args...)>>
    {
        typedef R result_type;
        typedef typename std::function<R(Args...)> type;
        typedef typename std::function<void(Args...)> typeNoRet;
    };

    template<typename R, typename ...Args>
    struct function_traits<R(*)(Args...)>
    {
        typedef R result_type;
        typedef typename std::function<R(Args...)> type;
        typedef typename std::function<void(Args...)> typeNoRet;
    };

    template<typename R, typename cls, typename ...Args>
    struct function_traits<R(cls::*)(Args...)>
    {
        typedef R result_type;
        typedef typename std::function<R(Args...)> type;
        typedef typename std::function<void(Args...)> typeNoRet;
    };



    class FunctionHolder
    {
    private:
        struct BaseHolder
        {
            BaseHolder() = default;
            virtual ~BaseHolder() = default;
        };

        template <typename T>
        struct Holder : public BaseHolder
        {
            Holder(T arg)
                : m_FuncPtr(arg)
            {
            }

            template<typename... Args>
            void Call(Args&&...args)
            {
                m_FuncPtr(std::forward<Args>(args)...);
            }

            template<typename R, typename... Args>
            R CallRet(Args&&...args)
            {
                return m_FuncPtr(std::forward<Args>(args)...);
            }

            T m_FuncPtr;
        };

    public:
        template<typename T>
        FunctionHolder(T t)
            : m_BaseHolder(new Holder<typename function_traits<T>::type>(t)),
            m_BaseHolderNoRet(new Holder<typename function_traits<T>::typeNoRet>(t))
        {
        }

        template<typename T, typename...Args>
        FunctionHolder(T&& t, Args&&... args)
            : m_BaseHolder(new Holder<typename function_traits<T>::type>
                (std::bind(std::forward<T>(t), std::forward<Args>(args)...))),
            m_BaseHolderNoRet(new Holder<typename function_traits<T>::typeNoRet>
                (std::bind(std::forward<T>(t), std::forward<Args>(args)...))) {}

        void operator()()
        {
            this->operator() < > ();
        }

        template<typename... Args>
        void operator()(Args&&... args)
        {
            auto f = dynamic_cast<Holder<std::function < void(Args...) > >*>(m_BaseHolderNoRet.get());
            if (f)
            {
                f->Call(std::forward<Args>(args)...);
                return;
            }

            throw std::invalid_argument("");
        }

        template<typename R, typename... Args>
        R call(Args&&... args)
        {
            auto f = dynamic_cast<Holder<std::function<R(Args...)>>*>(m_BaseHolder.get());
            if (f)
                return f->template CallRet<R>(std::forward<Args>(args)...);

            throw std::invalid_argument("");
        }

    private:
        std::unique_ptr<BaseHolder> m_BaseHolder = nullptr;
        std::unique_ptr<BaseHolder> m_BaseHolderNoRet = nullptr;
    };
    */

}