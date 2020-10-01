#pragma once

#include <type_traits>

#pragma region Interp Vals

template <class... Ts> struct InterpVal {};

template <class T, class... Ts>
struct InterpVal<T, Ts...> : InterpVal<Ts...>
{
	InterpVal(T t, Ts... ts) : InterpVal<Ts...>(ts...), val(t) {}

	T val;
};

template <size_t, class> struct elem_type_holder;

template <class T, class... Ts>
struct elem_type_holder<0, InterpVal<T, Ts...>> {
	typedef T type;
};

template <size_t k, class T, class... Ts>
struct elem_type_holder<k, InterpVal<T, Ts...>> {
	typedef typename elem_type_holder<k - 1, InterpVal<Ts...>>::type type;
};

template <size_t k, class... Ts>
typename std::enable_if<k == 0, typename elem_type_holder<0, InterpVal<Ts...>>::type&>::type
get(InterpVal<Ts...>& t) {
	return t.val;
}

template <size_t k, class T, class... Ts>
typename std::enable_if<k != 0, typename elem_type_holder<k, InterpVal<T, Ts...>>::type&>::type
get(InterpVal<T, Ts...>& t) {
	InterpVal<Ts...>& base = t;
	return get<k - 1>(base);
}

#pragma endregion

#pragma region Interp Param

template <class... Ts> struct InterpParam {};

template <class T, class... Ts>
struct InterpParam<T, Ts...> : InterpParam<Ts...> 
{
	InterpParam(T t, Ts... ts) : InterpParam<Ts...>(ts...), coefs(t) {}

	T coefs;
};

template <size_t, class> struct elem_type_holder;

template <class T, class... Ts>
struct elem_type_holder<0, InterpParam<T, Ts...>> {
	typedef T type;
};

template <size_t k, class T, class... Ts>
struct elem_type_holder<k, InterpParam<T, Ts...>> {
	typedef typename elem_type_holder<k - 1, InterpParam<Ts...>>::type type;
};

template <size_t k, class... Ts>
typename std::enable_if<k == 0, typename elem_type_holder<0, InterpParam<Ts...>>::type&>::type
get(InterpParam<Ts...>& t) {
	return t.coefs;
}

template <size_t k, class T, class... Ts>
typename std::enable_if<k != 0, typename elem_type_holder<k, InterpParam<T, Ts...>>::type&>::type
get(InterpParam<T, Ts...>& t) {
	InterpParam<Ts...>& base = t;
	return get<k - 1>(base);
}

template <class... Ts> 
InterpVal 

#pragma endregion
