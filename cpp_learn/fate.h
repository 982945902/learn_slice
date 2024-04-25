#pragma once

#include <any>
#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#ifdef FATE_PYBIND11_SUPPORT
#include "pybind11/pybind11.h"
#endif

#ifdef FATE_PYBIND11_SUPPORT
namespace py = pybind11;
// https://github.com/pybind/pybind11/issues/2765
#define PYBIND11_PY_CAST_THREAD_UNSAFE_BUG
#endif

class Fate {
 public:
  template <typename FUNC>
  struct FuncTraits;

  template <typename Ret, typename... Args>
  struct FuncTraits<Ret(Args...)> {
    using RetType = Ret;
    using ArgTypes = std::tuple<Args...>;
  };

  template <typename Ret, typename... Args>
  struct FuncTraits<Ret (*)(Args...)> : FuncTraits<Ret(Args...)> {};
  template <typename Class, typename Ret, typename... Args>
  struct FuncTraits<Ret (Class::*)(Args...)> : FuncTraits<Ret(Args...)> {};

  template <typename Class, typename Ret, typename... Args>
  struct FuncTraits<Ret (Class::*)(Args...) const> : FuncTraits<Ret(Args...)> {
  };

  template <typename T>
  struct remove_all_const : std::remove_const<T> {};

  template <typename T>
  struct remove_all_const<T*> {
    typedef typename remove_all_const<T>::type* type;
  };

  template <typename T>
  struct remove_all_const<T* const> {
    typedef typename remove_all_const<T>::type* type;
  };

  template <typename T>
  auto ConvertAny(std::any& arg) {
    if constexpr (std::is_pointer_v<T>) {
      using ct = std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>>;
      auto atid = arg.type().name();
      if (strcmp(atid, typeid(ct).name()) == 0)
        return (T)std::any_cast<ct>(arg);
    }

    return std::any_cast<T>(arg);
  }

  template <typename PARAM_TYPE>
  auto ConvertArgs(std::any v) {
    using PARAM_TYPE_REMOVE =
        remove_all_const<std::remove_reference_t<PARAM_TYPE>>::type;
    return std::make_tuple(ConvertAny<PARAM_TYPE_REMOVE>(v));
  }

  template <size_t params_num, typename PARAMS_TYPE>
  auto ConvertArgs(std::vector<std::any>& args, PARAMS_TYPE params_type) {
    if constexpr (params_num == 1) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]));
    } else if constexpr (params_num == 2) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertArgs<decltype(std::get<1>(params_type))>(args[1]));
    } else if constexpr (params_num == 3) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertArgs<decltype(std::get<2>(params_type))>(args[2]));
    } else if constexpr (params_num == 4) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertArgs<decltype(std::get<2>(params_type))>(args[2]),
          ConvertArgs<decltype(std::get<3>(params_type))>(args[3]));
    } else if constexpr (params_num == 5) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertArgs<decltype(std::get<2>(params_type))>(args[2]),
          ConvertArgs<decltype(std::get<3>(params_type))>(args[3]),
          ConvertArgs<decltype(std::get<4>(params_type))>(args[4]));
    } else if constexpr (params_num == 6) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertArgs<decltype(std::get<2>(params_type))>(args[2]),
          ConvertArgs<decltype(std::get<3>(params_type))>(args[3]),
          ConvertArgs<decltype(std::get<4>(params_type))>(args[4]),
          ConvertArgs<decltype(std::get<5>(params_type))>(args[5]));
    } else if constexpr (params_num == 7) {
      return std::tuple_cat(
          ConvertArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertArgs<decltype(std::get<2>(params_type))>(args[2]),
          ConvertArgs<decltype(std::get<3>(params_type))>(args[3]),
          ConvertArgs<decltype(std::get<4>(params_type))>(args[4]),
          ConvertArgs<decltype(std::get<5>(params_type))>(args[5]),
          ConvertArgs<decltype(std::get<6>(params_type))>(args[6]));
    }
  }

#ifdef FATE_PYBIND11_SUPPORT

  template <typename T>
  auto ConvertPyArg(py::object arg) {
    if (arg.is(py::none())) {
      if constexpr (std::is_pointer_v<T>)
        return (T) nullptr;
      else
        return T();
    }

    return arg.cast<T>();
  }

  template <typename PARAM_TYPE>
  auto ConvertPyArgs(py::object v) {
    using PARAM_TYPE_REMOVE =
        remove_all_const<std::remove_reference_t<PARAM_TYPE>>::type;
    return std::make_tuple(ConvertPyArg<PARAM_TYPE_REMOVE>(v));
  }

  template <size_t params_num, typename PARAMS_TYPE>
  auto ConvertPyArgs(py::args args, PARAMS_TYPE params_type) {
    if constexpr (params_num == 1) {
      return std::tuple_cat(
          ConvertPyArgs<decltype(std::get<0>(params_type))>(args[0]));
    } else if constexpr (params_num == 2) {
      return std::tuple_cat(
          ConvertPyArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertPyArgs<decltype(std::get<1>(params_type))>(args[1]));
    } else if constexpr (params_num == 3) {
      return std::tuple_cat(
          ConvertPyArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertPyArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertPyArgs<decltype(std::get<2>(params_type))>(args[2]));
    } else if constexpr (params_num == 4) {
      return std::tuple_cat(
          ConvertPyArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertPyArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertPyArgs<decltype(std::get<2>(params_type))>(args[2]),
          ConvertPyArgs<decltype(std::get<3>(params_type))>(args[3]));
    } else if constexpr (params_num == 5) {
      return std::tuple_cat(
          ConvertPyArgs<decltype(std::get<0>(params_type))>(args[0]),
          ConvertPyArgs<decltype(std::get<1>(params_type))>(args[1]),
          ConvertPyArgs<decltype(std::get<2>(params_type))>(args[2]),
          ConvertPyArgs<decltype(std::get<3>(params_type))>(args[3]),
          ConvertPyArgs<decltype(std::get<4>(params_type))>(args[4]));
    }
  }
#endif

  using FuncObj = std::function<std::any(std::vector<std::any>&)>;

  std::unordered_map<std::string, FuncObj> funcMap_;

#ifdef FATE_PYBIND11_SUPPORT

#ifdef PYBIND11_PY_CAST_THREAD_UNSAFE_BUG
  using PyFuncObj = std::function<std::any(py::args, py::kwargs)>;
#else
  using PyFuncObj = std::function<py::object(py::args, py::kwargs)>;
#endif

  std::unordered_map<std::string, PyFuncObj> pyfuncMap_;
#else
  std::unordered_map<std::string, std::string> place_holder_;
#endif
 public:
  template <typename FUNC>
  void Register(const std::string& func_name, FUNC func) {
    funcMap_[func_name] = [func = func,
                           this](std::vector<std::any>& args) -> std::any {
      using ParseFuncArgs = FuncTraits<FUNC>::ArgTypes;
      ParseFuncArgs params_type;
      auto converted_args =
          ConvertArgs<std::tuple_size<ParseFuncArgs>{}>(args, params_type);

      if constexpr (std::is_void<typename FuncTraits<FUNC>::RetType>::value) {
        std::apply(func, converted_args);
        return std::any{};
      } else {
        return std::apply(func, converted_args);
      }

      return std::any{};
    };
  }

  std::any Call(const std::string& func_name, std::vector<std::any> param) {
    auto iter = funcMap_.find(func_name);
    if (iter == funcMap_.end()) {
      SCU_THROW("not find function[{}]", func_name);
    }
    auto& func = iter->second;
    return func(param);
  }

#ifdef FATE_PYBIND11_SUPPORT
  template <typename FUNC>
  void RegisterPy(const std::string& func_name, FUNC func) {
    pyfuncMap_[func_name] =
#ifdef PYBIND11_PY_CAST_THREAD_UNSAFE_BUG
        [func = func, this](py::args args, py::kwargs kwargs) -> std::any {
      using ParseFuncArgs = FuncTraits<FUNC>::ArgTypes;
      ParseFuncArgs params_type;
      auto converted_args =
          ConvertPyArgs<std::tuple_size<ParseFuncArgs>{}>(args, params_type);

      if constexpr (std::is_void<typename FuncTraits<FUNC>::RetType>::value) {
        std::apply(func, converted_args);
        return std::any{};
      } else {
        return std::apply(func, converted_args);
      }

      return std::any{};
      ;
    };
#else
        [func = func, this](py::args args, py::kwargs kwargs) -> py::object {
      using ParseFuncArgs = FuncTraits<FUNC>::ArgTypes;
      ParseFuncArgs params_type;
      auto converted_args =
          ConvertPyArgs<std::tuple_size<ParseFuncArgs>{}>(args, params_type);

      if constexpr (std::is_void<typename FuncTraits<FUNC>::RetType>::value) {
        std::apply(func, converted_args);
        return py::none();
      } else {
        return py::cast(std::apply(func, converted_args));
      }

      return py::none();
      ;
    };
#endif
  }

#ifdef PYBIND11_PY_CAST_THREAD_UNSAFE_BUG
  std::any CallPy(const std::string& func_name, py::args args,
                  py::kwargs kwargs){
#else
  py::object CallPy(const std::string& func_name, py::args args,
                    py::kwargs kwargs) {
#endif
      auto iter = pyfuncMap_.find(func_name);
  if (iter == pyfuncMap_.end()) {
    SCU_THROW("not find function[{}]", func_name);
  }
  auto& func = iter -> second;
  return func(args, kwargs);
}
#endif
};