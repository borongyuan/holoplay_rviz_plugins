// Copyright 2022 Borong Yuan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifndef Q_MOC_RUN

#include "rviz_common/display.hpp"
#include "rviz_common/panel_dock_widget.hpp"
#include "rviz_common/render_panel.hpp"
#include "rviz_rendering/logging.hpp"

#include "HoloPlayCore.h"

#endif

namespace holoplay_rviz_plugins
{
    class LookingGlassDisplay : public rviz_common::Display
    {
        Q_OBJECT

    public:
        LookingGlassDisplay();
        ~LookingGlassDisplay();

        void onInitialize() override;

    private:
        int win_x, win_y, win_w, win_h;
        std::unique_ptr<rviz_common::RenderPanel> render_panel_;
        rviz_common::PanelDockWidget *looking_glass_panel_;

        void initializeLookingGlass();
        void setupLookingGlassPanel();
    };
} // namespace holoplay_rviz_plugins
