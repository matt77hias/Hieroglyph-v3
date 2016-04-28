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
// FirstPersonCamera
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef FirstPersonCamera_h
#define FirstPersonCamera_h
//--------------------------------------------------------------------------------
#include "Camera.h"
#include "Timer.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{    
    class FirstPersonCamera : public Camera
    {
    public:
        FirstPersonCamera();
		virtual ~FirstPersonCamera();
        
        virtual bool HandleEvent( EventPtr pEvent );
        virtual std::wstring GetName();

    protected:

        void Update();        

        enum ControlKeys
        {
            ForwardKey = 0,
            BackKey,
            LeftKey,
            RightKey,
            UpKey,
            DownKey,
            SpeedUpKey,

            NumControlKeys
        };

        // Key bindings for camera movement
        static const UINT KeyBindings[NumControlKeys];

        bool m_bPressedKeys[NumControlKeys];        
        int m_iLastMouseX;
        int m_iLastMouseY;
        int m_iMouseDeltaX;
        int m_iMouseDeltaY;

        float m_fRotationX;
        float m_fRotationY;

        Timer timer;
    };
};
//--------------------------------------------------------------------------------
#endif // FirstPersonCamera_h