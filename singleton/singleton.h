#ifndef SINGLETON_H
#define SINGLETON_H
#include<memory>
#include<mutex>
#include <memory>
#include <mutex>
#include <iostream>

template <typename T>
class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    template <typename... Args>
    static T& Instance(Args&&... args)
    {
        std::call_once(initInstanceFlag, [&](){
            instancePtr.reset(new T(std::forward<Args>(args)...));
        });
        return *instancePtr;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    static std::unique_ptr<T> instancePtr;
    static std::once_flag initInstanceFlag;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instancePtr;

template <typename T>
std::once_flag Singleton<T>::initInstanceFlag;


#endif // SINGLETON_H
