#include <map>
#include <string>
#include <iostream>
using namespace std;

template<typename Obj_T> struct MemVar;

template<typename Obj>
struct MemVarBase {
    void* Obj::* var;
    MemVarBase(void* Obj::* var = nullptr) : var{ var } {}
    virtual ~MemVarBase() = default;
    template<typename U>
    const MemVar<U Obj::*> As() const {
        return reinterpret_cast<U Obj::*>(var);
    }
};

template<typename Obj, typename T>
struct MemVar<T Obj::*> : MemVarBase<Obj> {
    MemVar(T Obj::* var)
        : MemVarBase<Obj>{reinterpret_cast<void* Obj::*>(var)} {}
    T& Of(Obj& obj) const {
        return obj.*reinterpret_cast<T Obj::*>(this->var);
    }
};

template<typename Obj>
struct MemFuncBase {
    void(Obj::* func)();
    MemFuncBase(void(Obj::*func)() = nullptr) : func(func) {}
    template<typename Ret, typename... Args>
    Ret Call(Obj& obj, Args&&... args) const {
        return (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(func))
            (forward<Args>(args)...);
    }
};

template<typename Func> struct MemFunc;
template<typename Obj, typename Ret, typename... Args>
struct MemFunc<Ret(Obj::*)(Args...)> : MemFuncBase<Obj> {
    MemFunc(Ret(Obj::*func)(Args...) = nullptr)
        : MemFuncBase<Obj>{ reinterpret_cast<void(Obj::*)()>(func) } {}
};

template<typename Obj>
struct Reflection {
    static Reflection& Instance() {
        static Reflection instance;
        return instance;
    }
    
    template<typename T>
    Reflection& Regist(T Obj::* ptr, const string& name) {
        n2mv[name] = new MemVar<T Obj::*>{ptr};
        return *this;
    }
    
    template<typename Ret, typename... Args>
    Reflection& Regist(Ret(Obj::*func)(Args...), const string& name) {
        n2mf[name] = new MemFunc<Ret(Obj::*)(Args...)>(func);
        return *this;
    }
    
    template<typename T>
    const MemVar<T Obj::*> Var(const string& name) {
        return n2mv[name]->template As<T>();
    }
    
    const map<string, MemVarBase<Obj>*> Vars() const { return n2mv; }
    
    template<typename Ret = void, typename... Args>
    Ret Call(const string& name, Obj& obj, Args&&... args){
        return n2mf[name]->template Call<Ret>(obj, forward<Args>(args)...);
    }
    
private:
    map<string, MemVarBase<Obj>*> n2mv;
    map<string, MemFuncBase<Obj>*> n2mf;
    Reflection() = default;
};

struct Point {
    float x, y;
    Point Add(float diff) { return {x+diff, y+diff}; }
};

int main() {
    Reflection<Point>::Instance()
        .Regist(&Point::x, "x")
        .Regist(&Point::y, "y")
        .Regist(&Point::Add, "Add");
    Point p;
    Reflection<Point>::Instance().Var<float>("x").Of(p) = 2.f;
    Reflection<Point>::Instance().Var<float>("y").Of(p) = 3.f;
    Point q = Reflection<Point>::Instance().Call<Point>("Add", p, 1.f);
    for(auto& nv : Reflection<Point>::Instance().Vars())
        cout << nv.first << ": " << nv.second->As<float>().Of(q) << endl;
}