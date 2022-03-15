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

#include <OgreCamera.h>
#include <OgreColourValue.h>

#include "rviz_common/display.hpp"
#include "rviz_common/display_context.hpp"
#include "rviz_common/panel_dock_widget.hpp"
#include "rviz_common/render_panel.hpp"
#include "rviz_common/view_manager.hpp"
#include "rviz_common/properties/float_property.hpp"
#include "rviz_rendering/logging.hpp"
#include "rviz_rendering/render_window.hpp"

#include "HoloPlayCore.h"

#endif

namespace holoplay_rviz_plugins
{
    class LookingGlassDisplay : public rviz_common::Display, public Ogre::RenderTargetListener
    {
        Q_OBJECT

    public:
        LookingGlassDisplay();
        ~LookingGlassDisplay();

        void update(float wall_dt, float ros_dt) override;

        void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;
        void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override;

    protected:
        void onInitialize() override;

    private:    
        int win_x_, win_y_, win_w_, win_h_, tile_x_, tile_y_;

        std::vector<Ogre::Camera *> virtual_cameras_;

        rviz_common::properties::FloatProperty *near_clip_property_;
        rviz_common::properties::FloatProperty *far_clip_property_;

        std::unique_ptr<rviz_common::RenderPanel> holoplay_panel_;
        rviz_common::PanelDockWidget *looking_glass_panel_;

        Ogre::SceneNode * background_scene_node_;

        void initializeLookingGlass();
        void setupHoloPlayPanel();
        void setupLookingGlassPanel();
        void updateCamera();
    };
} // namespace holoplay_rviz_plugins
