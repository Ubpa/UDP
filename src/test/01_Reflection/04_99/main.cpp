#include <map>
#include <string>
#include <iostream>

using namespace std;

template<typename Obj_T> struct MemVar;

template<typename Obj>
struct MemVar<void* Obj::*> {
    MemVar(void* Obj::* ptr = nullptr) : ptr(ptr) {}
    virtual ~MemVar() = default;

    template<typename U>
    const MemVar<U Obj::*> As() const {
        return reinterpret_cast<U Obj::*>(ptr);
    }
    
    void* Obj::* ptr;
};

template<typename Obj, typename T>
struct MemVar<T Obj::*> : MemVar<void* Obj::*> {
    MemVar(T Obj::* ptr)
        : MemVar<void* Obj::*>{reinterpret_cast<void* Obj::*>(ptr)} {}

    T& Of(Obj& obj) const {
        return obj.*reinterpret_cast<T Obj::*>(this->ptr);
    }
};

struct MemFunc {
    struct Nil{};
    MemFunc(void(Nil::*func)(void*) = nullptr) : func(func) {}

    template<typename Ret, typename Obj, typename... Args>
    Ret Call(Obj& obj, Args&&... args) const {
        return (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(func))
            (forward<Args>(args)...);
    }
    
    void(Nil::*func)(void*);
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
        n2mf[name] = reinterpret_cast<void(MemFunc::Nil::*)(void*)>(func);
        return *this;
    }
    
    template<typename T = void*>
    const MemVar<T Obj::*> Var(const string& name) {
        return n2mv[name]->template As<T>();
    }
    
    const map<string, MemVar<void* Obj::*>*> Vars() const { return n2mv; }
    
    template<typename Ret = void, typename... Args>
    Ret Call(const string& name, Obj& obj, Args... args){
        return n2mf[name].template Call<Ret>(obj, std::forward<Args>(args)...);
    }
    
private:
    map<string, MemVar<void* Obj::*>*> n2mv;
    map<string, MemFunc> n2mf;
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