//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "GlyphKinect2/Kinect2Manager.h"
#include "Kinect.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
struct Kinect2Manager::Impl
{
	Impl() : 
		m_pKinectSensor(nullptr),
		m_pCoordinateMapper(nullptr),
		m_pBodyFrameReader(nullptr)
	{
	}
	//--------------------------------------------------------------------------------
	~Impl()
	{

	}
	//--------------------------------------------------------------------------------
	bool Initialize(bool enable_skeleton = true, bool enable_color = true, bool enable_depth = true)
	{
		HRESULT hr;

		hr = GetDefaultKinectSensor(&m_pKinectSensor);
		if (SUCCEEDED(hr))
		{

			if (m_pKinectSensor)
			{
				// Initialize the Kinect and get coordinate mapper and the body reader
				IBodyFrameSource* pBodyFrameSource = nullptr;

				hr = m_pKinectSensor->Open();

				if (SUCCEEDED(hr))
				{
					hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
				}

				if (SUCCEEDED(hr))
				{
					hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
				}

				if (SUCCEEDED(hr))
				{
					hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
				}

				if (pBodyFrameSource) {
					pBodyFrameSource->Release();
					pBodyFrameSource = nullptr;
				}
			}
		}

		if (!m_pKinectSensor || FAILED(hr))
		{
			MessageBox( 0, L"No Kinect was found!", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
			return false;
		}

		return true;
	}
	//--------------------------------------------------------------------------------
	void Update()
	{
	    if (!m_pBodyFrameReader)
		{
			return;
		}

		IBodyFrame* pBodyFrame = nullptr;

		HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

		if (SUCCEEDED(hr))
		{
			INT64 nTime = 0;

			hr = pBodyFrame->get_RelativeTime(&nTime);

			IBody* ppBodies[BODY_COUNT] = {0};

			if (SUCCEEDED(hr))
			{
				hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
			}

			if (SUCCEEDED(hr))
			{
				for (int i = 0; i < Kinect2BodyCount; ++i)
				{
					IBody* pBody = ppBodies[i];
					if (pBody)
					{
						BOOLEAN bTracked = false;
						hr = pBody->get_IsTracked(&bTracked);

						bodies[i].tracked = bTracked;

						if (SUCCEEDED(hr) && bTracked)
						{
							Joint joints[JointType_Count]; 
							
							hr = pBody->GetJoints(_countof(joints), joints);
							if (SUCCEEDED(hr))
							{
								for ( int j = 0; j < Kinect2JointCount; ++j )
								{
									bodies[i].jointPositions[j] = 
										Vector3f( joints[j].Position.X,
												  joints[j].Position.Y,
												  joints[j].Position.Z );
								}
							}
						}
					}
				}
			}

			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				if ( ppBodies[i] != nullptr ) {
					ppBodies[i]->Release();
					ppBodies[i] = nullptr;
				}
			}
		}

		if ( pBodyFrame != nullptr ) {
			pBodyFrame->Release();
			pBodyFrame = nullptr;
		}
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		if ( m_pBodyFrameReader ) {
			m_pBodyFrameReader->Release();
			m_pBodyFrameReader = nullptr;
		}

		if ( m_pCoordinateMapper ) {
			m_pCoordinateMapper->Release();
			m_pCoordinateMapper = nullptr;
		}

		if ( m_pKinectSensor ) {
			m_pKinectSensor->Close();
			m_pKinectSensor->Release();
			m_pKinectSensor = nullptr;
		}
	}
	//--------------------------------------------------------------------------------
	const Kinect2Skeleton& GetBody( unsigned int body )
	{
		assert( body < Kinect2BodyCount );
		return bodies[body];
	}
	//--------------------------------------------------------------------------------


	IKinectSensor*          m_pKinectSensor;
    ICoordinateMapper*      m_pCoordinateMapper;
    IBodyFrameReader*       m_pBodyFrameReader;

	Kinect2Skeleton bodies[Kinect2BodyCount];
};
//--------------------------------------------------------------------------------















//--------------------------------------------------------------------------------
Kinect2Manager::Kinect2Manager() :
	m_pImpl( new Impl() )
{
}
//--------------------------------------------------------------------------------
Kinect2Manager::~Kinect2Manager(void)
{
	delete m_pImpl;
}
//--------------------------------------------------------------------------------
bool Kinect2Manager::Initialize( bool enable_skeleton, bool enable_color, bool enable_depth )
{
	return m_pImpl->Initialize( enable_skeleton, enable_color, enable_depth );
}
//--------------------------------------------------------------------------------
void Kinect2Manager::Update()
{
	m_pImpl->Update();
}
//--------------------------------------------------------------------------------
void Kinect2Manager::Shutdown()
{
	m_pImpl->Shutdown();
}
//--------------------------------------------------------------------------------
const Kinect2Skeleton& Kinect2Manager::GetBody( unsigned int body )
{
	return m_pImpl->GetBody( body );
}
//--------------------------------------------------------------------------------