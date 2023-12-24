#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_set>

namespace Piccolo
{
    extern bool                            g_is_editor_mode;
    extern std::unordered_set<std::string> g_editor_tick_component_types;

    class PiccoloEngine
    {
        friend class PiccoloEditor;

        static const float s_fps_alpha;

    public:
        /**
         * Start and shutdown the engine
         * start 和 shutdown 刚好相反
         */
        void startEngine(const std::string& config_file_path);
        void shutdownEngine();

        /**
         * Initialize and clear the engine
         * These two functions are called before and after the engine starts
         * You can use them to initialize and clear the engine
         * 这是两个空的占位函数，暂时不用关心
         */
        void initialize();
        void clear();

        bool isQuit() const { return m_is_quit; }

        /**
         * The main loop of the engine
         * The engine will call the tickOneFrame function in a loop
         * The engine will exit the loop when the window is closed
         */
        void run();
        /**
         * Tick one frame
         * 逻辑和渲染分离架构
         * 逻辑tick
         * 渲染tick
         */
        bool tickOneFrame(float delta_time);

        int getFPS() const { return m_fps; }

    protected:
        void logicalTick(float delta_time);
        bool rendererTick(float delta_time);

        void calculateFPS(float delta_time);

        /**
         *  Each frame can only be called once
         */
        float calculateDeltaTime();

    protected:
        bool m_is_quit {false};

        std::chrono::steady_clock::time_point m_last_tick_time_point {std::chrono::steady_clock::now()};

        float m_average_duration {0.f};
        int   m_frame_count {0};
        int   m_fps {0};
    };

} // namespace Piccolo
