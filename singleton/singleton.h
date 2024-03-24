#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
public:
    template <typename... Args>
    static T& Instance(const Args&... rest)
    {
        //pthread_once(&ponce_, &Singleton::init);
        if (nullptr == value_)
        {
            value_ = new T(rest...);
        }
        return *value_;
    };

    static void destroy()
    {
        if(nullptr != value_) {
            delete value_;
        }
    }

private:
    Singleton() = delete;
    ~Singleton() = delete;

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    //static pthread_once_t ponce_;
    static T*             value_;
};

//template<typename T>
//pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = nullptr;

#endif // SINGLETON_H
