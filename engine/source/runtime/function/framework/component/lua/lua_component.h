#pragma once
#include "sol/sol.hpp"
#include "runtime/function/framework/component/component.h"

namespace Piccolo
{
    REFLECTION_TYPE(LuaComponent)
    CLASS(LuaComponent : public Component, WhiteListFields)
    {
        REFLECTION_BODY(LuaComponent)

    public:
        LuaComponent() = default;

        void postLoadResource(std::weak_ptr<GObject> parent_object) override;

        void tick(float delta_time) override;

        template<typename T>
        static void set(std::weak_ptr<GObject> game_object, const char* name, T value);

        template<typename T>
        static T get(std::weak_ptr<GObject> game_object, const char* name);

        static void invoke(std::weak_ptr<GObject> game_object, const char* name);
    protected:

        // 用 SOL2 库来执行lua脚本
        sol::state m_lua_state;

        // 提醒piccolo parser，这个字段(m_lua_script)需要序列化
        META(Enable)
        std::string m_lua_script;
    };
} // namespace Piccolo
