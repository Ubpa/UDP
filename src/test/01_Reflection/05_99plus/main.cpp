#include <UDP/Basic/vtable.h>

#include <map>
#include <string>
#include <iostream>
#include <utility>

using namespace std;
using namespace Ubpa;

template<typename Obj, typename T> struct MemVar;
template<typename Obj>
struct MemVarBase {
    MemVarBase(void* Obj::* var = nullptr) : var{ var } {}
    virtual void Set(Obj& obj, void* value) const = 0;
    template<typename T>
    const MemVar<Obj, T> As() const { return reinterpret_cast<T Obj::*>(var); }
    void* Obj::* var;
};

template<typename Obj, typename T>
struct MemVar : MemVarBase<Obj> {
    MemVar(T Obj::* var = nullptr) : MemVarBase<Obj>{reinterpret_cast<void* Obj::*>(var)} {}
    virtual void Set(Obj& obj, void* value) const override {
        obj.*reinterpret_cast<T Obj::*>(this->var) = *reinterpret_cast<T*>(value);
    }
    T& Of(Obj& obj) const { return obj.*reinterpret_cast<T Obj::*>(this->var); }
};

template<typename Obj, typename Ret, typename... Args> struct MemFunc;
template<typename Obj>
struct MemFuncBase {
    MemFuncBase(void(Obj::*func)() = nullptr) : func{ func } {}
    virtual void Call(Obj& obj, void* rst, void* argsTuple) const = 0;
    template<typename Ret, typename... Args>
    Ret Call(Obj& obj, Args&&... args) const {
        return (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(func))(forward<Args>(args)...);
    }
    void(Obj::* func)();
};

template<typename Obj, typename Ret, typename... Args>
struct MemFunc : MemFuncBase<Obj> {
    MemFunc(Ret(Obj::*func)(Args...) = nullptr) : MemFuncBase<Obj>{ reinterpret_cast<void(Obj::*)()>(func) } {}
    virtual void Call(Obj& obj, void* rst, void* argsTuple) const override {
        ImplCall(obj, reinterpret_cast<Ret*>(rst), reinterpret_cast<tuple<Args...>*>(argsTuple), make_index_sequence<sizeof...(Args)>{});
    }
private:
    template<size_t... Ns>
    void ImplCall(Obj& obj, Ret* rst, tuple<Args...>* args, index_sequence<Ns...>) const {
        if constexpr (is_void_v<Ret>)
            (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(this->func))(get<Ns>(*args)...);
        else
            *rst = (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(this->func))(get<Ns>(*args)...);
    }
};

template<typename Obj> struct Reflection;

struct ReflectionBase {
    virtual void Set(void* obj, const string& name, void* value) = 0;
    virtual void Call(void* obj, const string& name, void* rst, void* argTuple) = 0;
};

struct ReflMngr {
    static ReflMngr& Instance() {
        static ReflMngr instance;
        return instance;
    }
    
    template<typename T>
    void Set(void* obj, const string& name, T&& value) {
        vtable2Refl[vtable(obj)]->Set(obj, name, reinterpret_cast<void*>(&value));
    }

    template<typename Ret = void, typename... Args>
    Ret Call(void* obj, const string& name, Args&&... args) {
        tuple<Args...> argTuple(std::forward<Args>(args)...);
        if constexpr (!is_void_v<Ret>) {
            Ret rst;
            vtable2Refl[vtable(obj)]->Call(obj, name, reinterpret_cast<void*>(&rst), reinterpret_cast<void*>(&argTuple));
            return rst;
        }
        else
            vtable2Refl[vtable(obj)]->Call(obj, name, nullptr, reinterpret_cast<void*>(&argTuple));
    }

private:
    template<typename Obj> friend struct Reflection;
    template<typename Obj>
    void Regist(ReflectionBase* refl) { vtable2Refl[vtable_of<Obj>::get()] = refl; }
    map<const void*, ReflectionBase*> vtable2Refl;
    ReflMngr() = default;
};

template<typename Obj>
struct Reflection : ReflectionBase {
    static Reflection& Instance() {
        static Reflection instance;
        return instance;
    }

    template<typename T>
    Reflection& Regist(T Obj::* ptr, const string& name) {
        n2mv[name] = new MemVar<Obj, T>{ ptr };
        return *this;
    }

    template<typename Ret, typename... Args>
    Reflection& Regist(Ret(Obj::* func)(Args...), const string& name) {
        n2mf[name] = new MemFunc<Obj, Ret, Args...>(func);
        return *this;
    }

    template<typename T>
    const MemVar<Obj, T> Var(const string& name) { return n2mv[name]->As<T>(); }

    const map<string, MemVarBase<Obj>*> Vars() const { return n2mv; }

    template<typename Ret = void, typename... Args>
    Ret Call(const string& name, Obj& obj, Args&&... args) {
        return n2mf[name]->template Call<Ret>(obj, std::forward<Args>(args)...);
    }

    virtual void Set(void* obj, const string& name, void* value) override {
        n2mv[name]->Set(*reinterpret_cast<Obj*>(obj), value);
    }

    virtual void Call(void* obj, const string& name, void* rst, void* argTuple) override {
        n2mf[name]->Call(*reinterpret_cast<Obj*>(obj), rst, argTuple);
    }

private:
    map<string, MemVarBase<Obj>*> n2mv;
    map<string, MemFuncBase<Obj>*> n2mf;
    Reflection() { ReflMngr::Instance().Regist<Obj>(this); }
};

struct Point {
    virtual ~Point() = default;
    Point(float x = 0.f, float y = 0.f) :x(x), y(y) {}
    float x, y;
    Point Add(float diff) { return { x + diff, y + diff }; }
};

int main() {
    Reflection<Point>::Instance()
        .Regist(&Point::x, "x")
        .Regist(&Point::y, "y")
        .Regist(&Point::Add, "Add");

    Point p;
    Reflection<Point>::Instance().Var<float>("x").Of(p) = 1.f;
    void* ptr = reinterpret_cast<void*>(&p);
    ReflMngr::Instance().Set(ptr, "y", 2.f);
    Point q = ReflMngr::Instance().Call<Point>(ptr, "Add", 10.f);
    Point w = Reflection<Point>::Instance().Call<Point>("Add", q, 100.f);
    for (auto& nv : Reflection<Point>::Instance().Vars())
        cout << nv.first << ": " << nv.second->As<float>().Of(w) << endl;
}
