//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// AppSettings
//
//--------------------------------------------------------------------------------
#include "AppSettings.h"
//--------------------------------------------------------------------------------
DisplayMode::Settings DisplayMode::Value = DisplayMode::Final;
GBufferOptMode::Settings GBufferOptMode::Value = GBufferOptMode::OptEnabled;
LightOptMode::Settings LightOptMode::Value = LightOptMode::Volumes;
LightMode::Settings LightMode::Value = LightMode::Lights3x3x3;
AAMode::Settings AAMode::Value = AAMode::None;
//--------------------------------------------------------------------------------
