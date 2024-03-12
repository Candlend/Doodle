#include "Singleton.h"

namespace RhyEngine
{

template <typename T>
T &Singleton<T>::GetInstance()
{
    static Singleton s_Instance;
    return s_Instance;
}

} // namespace RhyEngine