#include "runtime/engine.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/meta/reflection/reflection_register.h"

#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/particle/particle_manager.h"
#include "runtime/function/physics/physics_manager.h"
#include "runtime/function/render/debugdraw/debug_draw_manager.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

namespace Piccolo
{
    bool                            g_is_editor_mode {false};
    std::unordered_set<std::string> g_editor_tick_component_types {};

    void PiccoloEngine::startEngine(const std::string& config_file_path)
    {
        // register meta data
        // 注册元数据（信息）
        Reflection::TypeMetaRegister::metaRegister();

        // start system!
        // Piccolo::RuntimeGlobalContext::startSystems()...
        g_runtime_global_context.startSystems(config_file_path);

        // 打印log
        LOG_INFO("engine start");
    }

    void PiccoloEngine::shutdownEngine()
    {
        // 打印log
        LOG_INFO("engine shutdown");

        // shutdown system!
        // Piccolo::RuntimeGlobalContext::shutdownSystems()...
        g_runtime_global_context.shutdownSystems();

        // unregister meta data
        // 注销元数据（信息）
        Reflection::TypeMetaRegister::metaUnregister();
    }

    void PiccoloEngine::initialize() {}
    void PiccoloEngine::clear() {}

    void PiccoloEngine::run()
    {
        // get window system
        std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
        ASSERT(window_system);

        // check window system should NOT close
        while (!window_system->shouldClose())
        {
            const float delta_time = calculateDeltaTime();
            // tick one frame
            tickOneFrame(delta_time);
        }
    }

    float PiccoloEngine::calculateDeltaTime()
    {
        float delta_time;
        {
            using namespace std::chrono;

            steady_clock::time_point tick_time_point = steady_clock::now();
            duration<float> time_span = duration_cast<duration<float>>(tick_time_point - m_last_tick_time_point);
            delta_time                = time_span.count();

            m_last_tick_time_point = tick_time_point;
        }
        return delta_time;
    }

    bool PiccoloEngine::tickOneFrame(float delta_time)
    {
        // 逻辑tick
        logicalTick(delta_time);
        calculateFPS(delta_time);

        // single thread
        // exchange data between logic and render contexts
        // 逻辑和渲染之间交换数据
        g_runtime_global_context.m_render_system->swapLogicRenderData();

        // 渲染tick
        rendererTick(delta_time);

        // 以下为一些不便于划分到渲染和逻辑里面的东西
        // 例如：物理调试渲染、粒子系统渲染
#ifdef ENABLE_PHYSICS_DEBUG_RENDERER
        g_runtime_global_context.m_physics_manager->renderPhysicsWorld(delta_time);
#endif
        // 例如：窗口系统轮询事件
        g_runtime_global_context.m_window_system->pollEvents();

        // 例如：窗口标题设置
        g_runtime_global_context.m_window_system->setTitle(
            std::string("Piccolo - " + std::to_string(getFPS()) + " FPS").c_str());

        // 例如：窗口关闭检测
        const bool should_window_close = g_runtime_global_context.m_window_system->shouldClose();
        return !should_window_close;
    }

    void PiccoloEngine::logicalTick(float delta_time)
    {
        // world tick
        // world 是引擎里所有逻辑上的实体的载体，由一个一个level组成
        g_runtime_global_context.m_world_manager->tick(delta_time);
        // input tick
        g_runtime_global_context.m_input_system->tick();
    }

    bool PiccoloEngine::rendererTick(float delta_time)
    {
        g_runtime_global_context.m_render_system->tick(delta_time);
        return true;
    }

    const float PiccoloEngine::s_fps_alpha = 1.f / 100;
    void        PiccoloEngine::calculateFPS(float delta_time)
    {
        m_frame_count++;

        if (m_frame_count == 1)
        {
            m_average_duration = delta_time;
        }
        else
        {
            m_average_duration = m_average_duration * (1 - s_fps_alpha) + delta_time * s_fps_alpha;
        }

        m_fps = static_cast<int>(1.f / m_average_duration);
    }
} // namespace Piccolo
