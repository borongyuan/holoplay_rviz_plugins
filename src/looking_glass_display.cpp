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
#include <iostream>
#include "holoplay_rviz_plugins/looking_glass_display.hpp"

namespace holoplay_rviz_plugins
{
    using rviz_common::properties::StatusLevel;

    LookingGlassDisplay::LookingGlassDisplay()
    {
        near_clip_property_ = new rviz_common::properties::FloatProperty("Near Clip", 0.0, "", this);

        far_clip_property_ = new rviz_common::properties::FloatProperty("Far Clip", 0.0, "", this);
    }

    LookingGlassDisplay::~LookingGlassDisplay()
    {
        if (initialized())
        {
            rviz_rendering::RenderWindowOgreAdapter::removeListener(holoplay_panel_->getRenderWindow(), this);
            hpc_CloseApp();
        }
    }

    void LookingGlassDisplay::update(float wall_dt, float ros_dt)
    {
        (void)wall_dt;
        (void)ros_dt;
        // rviz_rendering::RenderWindowOgreAdapter::getOgreCamera(holoplay_panel_->getRenderWindow())->setPosition(0, 0, 0);
        Ogre::ColourValue *bg = new Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f);
        rviz_rendering::RenderWindowOgreAdapter::setBackgroundColor(holoplay_panel_->getRenderWindow(), bg);
    }

    void LookingGlassDisplay::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt)
    {
        (void)evt;
    }

    void LookingGlassDisplay::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt)
    {
        (void)evt;
    }

    void LookingGlassDisplay::onInitialize()
    {
        try
        {
            initializeLookingGlass();
            setupHoloPlayPanel();
            setupLookingGlassPanel();

            setStatus(StatusLevel::Ok, "HoloPlay", "OK");
        }
        catch (const std::exception &e)
        {
            setStatus(StatusLevel::Error, "HoloPlay", e.what());
        }
    }

    void LookingGlassDisplay::initializeLookingGlass()
    {
        hpc_client_error errco = hpc_InitializeApp("HoloPlay RViz", hpc_LICENSE_NONCOMMERCIAL);
        if (errco)
        {
            switch (errco)
            {
            case hpc_CLIERR_NOSERVICE:
                throw std::runtime_error("HoloPlay Service not running");
                break;
            case hpc_CLIERR_SERIALIZEERR:
                throw std::runtime_error("Client message could not be serialized");
                break;
            case hpc_CLIERR_VERSIONERR:
                throw std::runtime_error("Incompatible version of HoloPlay Service");
                break;
            case hpc_CLIERR_PIPEERROR:
                throw std::runtime_error("Interprocess pipe broken");
                break;
            case hpc_CLIERR_SENDTIMEOUT:
                throw std::runtime_error("Interprocess pipe send timeout");
                break;
            case hpc_CLIERR_RECVTIMEOUT:
                throw std::runtime_error("Interprocess pipe receive timeout");
                break;
            default:
                throw std::runtime_error("Unknown error");
                break;
            }
        }

        char buf[1000];
        hpc_GetHoloPlayCoreVersion(buf, 1000);
        RVIZ_RENDERING_LOG_INFO_STREAM("HoloPlay Core version " << buf << ".");
        hpc_GetHoloPlayServiceVersion(buf, 1000);
        RVIZ_RENDERING_LOG_INFO_STREAM("HoloPlay Service version " << buf << ".");
        int num_displays = hpc_GetNumDevices();
        RVIZ_RENDERING_LOG_INFO_STREAM(num_displays << " devices connected.");
        if (num_displays < 1)
        {
            throw std::runtime_error("Couldn't find looking glass");
        }
        else
        {
            RVIZ_RENDERING_LOG_INFO_STREAM("Device information for display " << 0 << ":");
            hpc_GetDeviceHDMIName(0, buf, 1000);
            RVIZ_RENDERING_LOG_INFO_STREAM("\tDevice name: " << buf);
            hpc_GetDeviceType(0, buf, 1000);
            RVIZ_RENDERING_LOG_INFO_STREAM("\tDevice type: " << buf);

            win_x_ = hpc_GetDevicePropertyWinX(0);
            win_y_ = hpc_GetDevicePropertyWinY(0);
            win_w_ = hpc_GetDevicePropertyScreenW(0);
            win_h_ = hpc_GetDevicePropertyScreenH(0);
            RVIZ_RENDERING_LOG_INFO_STREAM("Window parameters for display " << 0 << ":");
            RVIZ_RENDERING_LOG_INFO_STREAM("\tPosition: (" << win_x_ << ", " << win_y_ << ")");
            RVIZ_RENDERING_LOG_INFO_STREAM("\tSize: (" << win_w_ << ", " << win_h_ << ")");
            RVIZ_RENDERING_LOG_INFO_STREAM("\tAspect ratio: " << hpc_GetDevicePropertyDisplayAspect(0));

            RVIZ_RENDERING_LOG_INFO_STREAM("Shader uniforms for display " << 0 << ":");
            RVIZ_RENDERING_LOG_INFO_STREAM("\tPitch: " << hpc_GetDevicePropertyPitch(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tTilt: " << hpc_GetDevicePropertyTilt(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tCenter: " << hpc_GetDevicePropertyCenter(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tSubpixel width: " << hpc_GetDevicePropertySubp(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tView cone: " << hpc_GetDevicePropertyFloat(0, "/calibration/viewCone/value"));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tFringe: " << hpc_GetDevicePropertyFringe(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tRI: " << hpc_GetDevicePropertyRi(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tBI: " << hpc_GetDevicePropertyBi(0));
            RVIZ_RENDERING_LOG_INFO_STREAM("\tinvView: " << hpc_GetDevicePropertyInvView(0));

            tile_x_ = hpc_GetDevicePropertyTileX(0);
            tile_y_ = hpc_GetDevicePropertyTileY(0);
        }
    }

    void LookingGlassDisplay::setupHoloPlayPanel()
    {
        holoplay_panel_ = std::make_unique<rviz_common::RenderPanel>();
        holoplay_panel_->initialize(context_, true);
        setAssociatedWidget(holoplay_panel_.get());

        static int count = 0;
        holoplay_panel_->getRenderWindow()->setObjectName("HoloPlayRenderWindow" + QString::number(count++));

        holoplay_panel_->setViewController(context_->getViewManager()->getCurrent());
        rviz_rendering::RenderWindowOgreAdapter::addListener(holoplay_panel_->getRenderWindow(), this);
    }

    void LookingGlassDisplay::setupLookingGlassPanel()
    {
        looking_glass_panel_ = getAssociatedWidgetPanel();
        looking_glass_panel_->setTitleBarWidget(new QWidget(looking_glass_panel_));
        looking_glass_panel_->setFloating(true);
        looking_glass_panel_->setGeometry(win_x_, win_y_, win_w_, win_h_);
        looking_glass_panel_->setWindowState(looking_glass_panel_->windowState() | Qt::WindowFullScreen);
    }

    void LookingGlassDisplay::updateCamera()
    {
    }
} // namespace holoplay_rviz_plugins

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(holoplay_rviz_plugins::LookingGlassDisplay, rviz_common::Display)
