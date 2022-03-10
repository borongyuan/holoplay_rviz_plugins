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

#include "holoplay_rviz_plugins/looking_glass_display.hpp"

namespace holoplay_rviz_plugins
{
    LookingGlassDisplay::LookingGlassDisplay() = default;

    LookingGlassDisplay::~LookingGlassDisplay() = default;

    void LookingGlassDisplay::onInitialize()
    {
        setupLookingGlassPanel();
    }

    void LookingGlassDisplay::setupLookingGlassPanel()
    {
        render_panel_ = std::make_unique<rviz_common::RenderPanel>();
        setAssociatedWidget(render_panel_.get());
        looking_glass_panel_ = getAssociatedWidgetPanel();
        looking_glass_panel_->setFloating(true);
        looking_glass_panel_->setGeometry(QApplication::desktop()->screenGeometry(1));
        looking_glass_panel_->showFullScreen();
    }
} // namespace holoplay_rviz_plugins

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(holoplay_rviz_plugins::LookingGlassDisplay, rviz_common::Display)
