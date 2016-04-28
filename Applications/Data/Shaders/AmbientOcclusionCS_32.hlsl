//-----------------------------------------------------------------------------
// AmbientOcclusionCS.hlsl
//
// This compute shader utilizes a linear depth buffer of a scene to compute
// the ambient occlusion term for each pixel in the input.  The memory sampling
// is greatly improved over previous versions of the implementation since the
// sampling is amortized over an entire thread group instead of each pixel
// being it's own thread.
//
// This version of the shader operates on the texture one thread group at a
// time.  The constants defined below need to be updated as follows:
// 
// For example, for a 128x128 texture set, if I use 8x8 gridsize for my dispatch
// call, then the size parameters would be 16x16.  The other definitions are 
// based on these two, so no other changes are needed.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//-----------------------------------------------------------------------------

Texture2D					DepthNormalBuffer : register( t0 );
RWTexture2D<float>			AmbientOcclusionTarget : register( u0 );

SamplerState DepthSampler
{
	Filter			= MIN_MAG_MIP_POINT;
	AddressU		= CLAMP;
	AddressV		= CLAMP;
	AddressW		= CLAMP;
};

cbuffer OcclusionDataBuffer
{
	float4 Resolution;	// x:width, y:height, z:1/width, w:1/height
	float4 Perspective; // x:fov, y:aspect, z:yn, w:xn
	float4 Frustum;     // x:znear, y:zfar, znear/zfar, zfar/zfar
};


#define kernel_x 16
#define kernel_y 16

// Group size
#define size_x 32
#define size_y 32

// Group size with the extra perimeter
#define padded_x (kernel_x + size_x + kernel_x)
#define padded_y (kernel_y + size_y + kernel_y)

//#define USE_GSM

#ifdef USE_GSM
// Declare enough shared memory for the padded thread group size
groupshared float LoadedDepths[padded_x][padded_y];
#endif


// vRecipDepthBufferSize = 1.0 / depth buffer width and height in pixels.
// p_vCameraFrustrumSize = Full width and height of camera frustum at the 
// camera’s near plane in world space.
//--------------------------------------------------------------------------------
float3 ScreenPosToViewPos( uint3 DispatchID, float fDepth )
{
	const float2 vRecipDepthBufferSize = float2( Resolution.z, Resolution.w );
	const float2 vCameraFrustrumSize = float2( Perspective.w, Perspective.z );
	
	float2 vViewSpaceUV = (float2)DispatchID.xy * vRecipDepthBufferSize;
    
    vViewSpaceUV = vViewSpaceUV * float2( 2.0f, -2.0f );   // From 0..1 to to 0..2
    vViewSpaceUV = vViewSpaceUV + float2( -1.0f, 1.0f );   // From 0..2 to -1..1     
    vViewSpaceUV = vViewSpaceUV * vCameraFrustrumSize * 0.5f;

    return float3( vViewSpaceUV.x, vViewSpaceUV.y, 1.0f ) * fDepth;
};
//--------------------------------------------------------------------------------
uint3 ViewPosToScreenPos( float3 ViewPos )
{
	const float2 p_vRecipDepthBufferSize = float2( Resolution.z, Resolution.w );
	const float2 p_vCameraFrustrumSize = float2( Perspective.w, Perspective.z );

	float2 SamplePoint = ViewPos.xy / ViewPos.z;

	SamplePoint = SamplePoint / ( p_vCameraFrustrumSize * 0.5f );
	SamplePoint = SamplePoint + float2( 1.0f, -1.0f );
	SamplePoint = SamplePoint * float2( 0.5f, -0.5f );
	SamplePoint = SamplePoint * float2( Resolution.x, Resolution.y );

	return( uint3( (uint)SamplePoint.x, (uint)SamplePoint.y, 0 ) );
};
//--------------------------------------------------------------------------------
float3 reflect( float3 vSample, float3 vNormal )
{
    return( vSample - 2.0f * dot( vSample, vNormal ) * vNormal );
};
//--------------------------------------------------------------------------------
#ifdef USE_GSM
float LoadDepth( int3 offset )
{
	offset = clamp(offset, int3(0,0,0), int3(padded_x,padded_y,0));
	return( LoadedDepths[offset.x][offset.y] );
};
#endif // USE_GSM
//--------------------------------------------------------------------------------

// Declare one thread for each texel of the group.
[numthreads(size_x, size_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	int3 globaltexturelocation = int3( 0, 0, 0 );
	globaltexturelocation = DispatchThreadID;
	
	int3 texturelocation = globaltexturelocation;

#ifdef USE_GSM
	// Load the required depth samples the group shared memory ()
	//
	//  0 1 ==> x y
	//  2 3 ==> z w
	// 

	int3 OffsetLocation = int3( GroupID.x * size_x - kernel_x, GroupID.y * size_y - kernel_y, 0 );
	int3 ThreadLocation = GroupThreadID * 2;
	
//#define USE_GATHER
#ifndef USE_GATHER
	int3 Sample0 = ThreadLocation + OffsetLocation;
	LoadedDepths[ThreadLocation.x][ThreadLocation.y] =  DepthNormalBuffer.Load(Sample0).w * Frustum.y;

	ThreadLocation = ThreadLocation + int3(1,0,0);
	int3 Sample1 = ThreadLocation + OffsetLocation;
	LoadedDepths[ThreadLocation.x][ThreadLocation.y] =  DepthNormalBuffer.Load(Sample1).w * Frustum.y;
	
	ThreadLocation = ThreadLocation + int3(-1,1,0);
	int3 Sample2 = ThreadLocation + OffsetLocation;
	LoadedDepths[ThreadLocation.x][ThreadLocation.y] =  DepthNormalBuffer.Load(Sample2).w * Frustum.y;

	ThreadLocation = ThreadLocation + int3(1,0,0);
	int3 Sample3 = ThreadLocation + OffsetLocation;
	LoadedDepths[ThreadLocation.x][ThreadLocation.y] =  DepthNormalBuffer.Load(Sample3).w * Frustum.y;
#else
	float2 fGatherSample;
	fGatherSample.x = ((float)GroupID.x * (float)size_x - (float)kernel_x + (float)GroupThreadID.x * 2.0f ) / Resolution.x;
	fGatherSample.y = ((float)GroupID.y * (float)size_y - (float)kernel_y + (float)GroupThreadID.y * 2.0f ) / Resolution.y;
	
	float4 fDepths = DepthNormalBuffer.GatherAlpha( DepthSampler, fGatherSample + float2( 0.5f / Resolution.x, 0.5f / Resolution.y ) ) * Frustum.y;
	LoadedDepths[ThreadLocation.x][ThreadLocation.y] = fDepths.w;
	LoadedDepths[ThreadLocation.x+1][ThreadLocation.y] = fDepths.z;
	LoadedDepths[ThreadLocation.x+1][ThreadLocation.y+1] = fDepths.y;
	LoadedDepths[ThreadLocation.x][ThreadLocation.y+1] = fDepths.x;
#endif // USE_GATHER

	//GroupMemoryBarrier();
	GroupMemoryBarrierWithGroupSync();
#endif // USE_GSM


	// Define the sampling kernel that will be used for this thread.  
	const float3 avKernel[8] =
	{
		normalize( float3(  1, 1, 1 ) ),
		normalize( float3( -1,-1,-1 ) ),
		normalize( float3( -1,-1, 1 ) ),
		normalize( float3( -1, 1,-1 ) ),
		normalize( float3( -1, 1 ,1 ) ),
		normalize( float3(  1,-1,-1 ) ),
		normalize( float3(  1,-1, 1 ) ),
		normalize( float3(  1, 1,-1 ) )
	};

	// Create a set of lengths to apply to the offset vectors
	const float avScales[8] =
	{
		0.5625f,
		0.6250f,
		0.6875f,
		0.7500f,
		0.8125f,
		0.8750f,
		0.9375f,
		1.0000f
	};

	// Define reflection vectors
	const float3 rotation[16][16] = 
	{
		{float3(0.979213,-0.00394083,-0.202798),float3(0.459437,-0.571893,-0.6796),float3(-0.267886,-0.963319,-0.0159409),float3(0.141673,-0.892674,-0.427858),float3(-0.605668,-0.522737,0.599927),float3(0.36067,-0.521283,-0.773422),float3(-0.617904,-0.785726,0.0288041),float3(0.0271558,-0.817716,0.574981),float3(0.874356,-0.283914,0.393566),float3(0.44629,-0.447495,0.774967),float3(-0.98568,-0.0282295,-0.166246),float3(0.0136833,-0.366321,0.930388),float3(0.928053,-0.0280628,0.37139),float3(-0.100809,-0.99463,0.0234069),float3(-0.30039,-0.947823,0.106757),float3(0.752661,-0.499962,-0.428415)},
		{float3(0.134703,-0.988616,0.0670385),float3(0.92656,-0.17868,-0.331),float3(-0.449829,-0.624531,0.638447),float3(-0.0541511,-0.997785,-0.038643),float3(-0.698716,-0.373442,0.610194),float3(-0.222204,-0.130609,0.966213),float3(-0.0552836,-0.991832,-0.114946),float3(0.0777883,-0.99334,0.0849931),float3(-0.558428,-0.413204,-0.71932),float3(-0.360388,-0.633497,0.684691),float3(0.0069461,-0.00218526,0.999973),float3(-0.328396,-0.937604,-0.114253),float3(0.538485,-0.743599,0.39635),float3(-0.0657227,-0.475524,-0.877244),float3(0.108545,-0.926043,0.361473),float3(-0.0938633,-0.846157,-0.524602)},
		{float3(0.245239,-0.0276785,-0.969068),float3(0.489094,-0.763624,0.421504),float3(-0.227781,-0.960828,0.157877),float3(-0.660053,-0.482526,0.575759),float3(0.026194,-0.999055,0.0346879),float3(-0.694114,-0.716796,-0.0664092),float3(0.0194243,-0.991762,-0.126615),float3(-0.233739,-0.715253,-0.65862),float3(-0.291034,-0.950516,-0.108716),float3(-0.636915,-0.735691,0.230432),float3(-0.208723,-0.97321,-0.0964224),float3(0.578281,-0.459402,0.674196),float3(0.13214,-0.97326,-0.187893),float3(-0.00136498,-0.998328,0.0577794),float3(-0.633131,-0.699423,-0.331591),float3(0.756454,-0.507435,-0.412658)},
		{float3(0.00428661,-0.999767,0.0211337),float3(-0.591534,-0.806201,0.0112951),float3(0.13438,-0.44801,-0.883872),float3(0.258792,-0.431263,-0.864314),float3(-0.468439,-0.818806,-0.331844),float3(0.0495516,-0.974266,0.219887),float3(-0.832018,-0.0970927,0.546186),float3(0.32747,-0.464091,-0.823032),float3(-0.821471,-0.525847,0.220612),float3(0.437797,-0.566836,0.697876),float3(-0.840761,-0.537812,-0.0622878),float3(0.882256,-0.47055,-0.0143863),float3(-0.641713,-0.742038,-0.193865),float3(0.621326,-0.771081,-0.13924),float3(-0.376758,-0.837599,0.395577),float3(-0.644653,-0.762986,-0.0476969)},
		
		{float3(0.848258,-0.37783,0.371084),float3(-0.0961239,-0.490588,-0.866074),float3(-0.049984,-0.998345,0.0284381),float3(0.0733058,-0.974027,0.214236),float3(-0.23468,-0.151068,0.960262),float3(-0.141234,-0.987227,-0.0737266),float3(0.0786212,-0.996381,0.0322987),float3(-0.747721,-0.456474,-0.482228),float3(0.101159,-0.990994,0.0877404),float3(-0.805487,-0.0457108,-0.590848),float3(-0.671006,-0.726708,0.147126),float3(0.552159,-0.578811,-0.600081),float3(-0.834493,-0.477467,-0.275041),float3(-0.0022514,-0.999977,0.0064049),float3(0.0089757,-0.0863984,-0.99622),float3(-0.0365092,-0.973077,-0.227569)},
		{float3(-0.0587858,-0.994739,-0.0838924),float3(-0.813127,-0.467471,0.346837),float3(-0.279966,-0.877412,-0.389572),float3(-0.334297,-0.469933,-0.816951),float3(0.675523,-0.339342,-0.654611),float3(0.0971135,-0.83556,-0.540749),float3(0.541337,-0.783955,0.303923),float3(0.668761,-0.42753,-0.608258),float3(-0.688827,-0.490755,0.53355),float3(-0.22283,-0.949345,0.221567),float3(0.307464,-0.714531,0.62842),float3(0.190957,-0.00509423,0.981585),float3(-0.363043,-0.566577,0.739723),float3(0.280616,-0.615653,0.73636),float3(-0.00765945,-0.985324,-0.170524),float3(0.927412,-0.350709,0.13004)},
		{float3(-0.595018,-0.69791,0.398592),float3(-0.0323572,-0.990107,-0.136531),float3(0.115298,-0.98098,0.156153),float3(-0.461571,-0.812747,-0.35552),float3(0.129801,-0.980962,0.14445),float3(-0.245229,-0.96911,0.0262309),float3(-0.192345,-0.917425,-0.348331),float3(-0.00183823,-0.999856,0.016877),float3(-0.131949,-0.813586,0.566275),float3(0.72007,-0.563188,0.405361),float3(0.00616074,-0.532637,-0.846321),float3(-0.000215262,-0.991551,-0.129716),float3(0.878643,-0.302293,-0.369602),float3(0.938532,-0.292933,0.182616),float3(-0.14868,-0.20629,-0.967129),float3(-0.750408,-0.31835,-0.579259)},
		{float3(0.497183,-0.656287,-0.567535),float3(0.162598,-0.961815,0.220169),float3(-0.374517,-0.918189,0.129101),float3(0.147128,-0.989003,0.0150618),float3(-0.279292,-0.951703,0.127504),float3(-0.456355,-0.434446,0.776529),float3(-0.295755,-0.922251,0.248961),float3(-0.193802,-0.977814,-0.0795076),float3(0.610035,-0.548326,-0.572011),float3(-0.0506886,-0.984272,0.169231),float3(0.404417,-0.69247,0.597438),float3(-0.264889,-0.94481,0.192791),float3(-0.238864,-0.963653,-0.119651),float3(0.00337932,-0.999909,0.0130613),float3(-0.647,-0.723332,0.241209),float3(-0.307253,-0.927514,0.212872)},

		{float3(-0.371527,-0.571004,-0.732067),float3(-0.0957842,-0.970517,0.221186),float3(-0.900048,-0.267351,0.344148),float3(-0.0895738,-0.751118,-0.654063),float3(-0.0334791,-0.86161,0.506465),float3(0.533498,-0.837193,-0.120366),float3(0.297318,-0.821022,0.487366),float3(0.370811,-0.33527,-0.866079),float3(-0.640932,-0.444547,0.625767),float3(0.757135,-0.468683,0.455064),float3(0.231501,-0.247314,0.940874),float3(-0.229709,-0.901328,0.367209),float3(-0.143615,-0.989366,-0.0230314),float3(0.22543,-0.960301,0.164329),float3(0.104322,-0.994543,-0.000808547),float3(0.754303,-0.462473,0.465989)},
		{float3(-0.571488,-0.729278,-0.376238),float3(-0.103646,-0.512374,0.852485),float3(-0.699515,-0.381097,0.60452),float3(0.0982092,-0.810516,-0.577424),float3(0.189278,-0.390078,0.901118),float3(-0.781636,-0.620469,0.0637477),float3(-0.0444662,-0.939627,0.339299),float3(0.364766,-0.225647,-0.903343),float3(-0.39857,-0.883022,-0.247817),float3(0.945833,-0.324109,-0.0187878),float3(0.598013,-0.655779,0.460797),float3(-0.957158,-0.241697,0.159474),float3(-0.174365,-0.869456,-0.462215),float3(0.0181754,-0.999809,0.00719049),float3(0.00365152,-0.999981,-0.00487471),float3(-0.0309194,-0.999451,-0.0118694)},
		{float3(-0.558701,-0.827268,0.059001),float3(-0.31473,-0.943192,-0.106456),float3(0.790676,-0.0185497,-0.611954),float3(0.878293,-0.311624,-0.362619),float3(-0.911707,-0.410313,-0.0208045),float3(-0.705356,-0.708841,-0.00409277),float3(0.226121,-0.931003,-0.286537),float3(-0.152878,-0.985536,-0.0731183),float3(-0.906784,-0.131181,0.400667),float3(0.784845,-0.527911,0.324543),float3(0.656999,-0.402785,-0.637273),float3(-0.92735,-0.143653,0.345524),float3(0.858509,-0.0875474,0.50527),float3(0.889754,-0.17518,-0.421486),float3(0.417574,-0.402785,-0.814492),float3(-0.870041,-0.407243,0.277816)},
		{float3(-0.282054,-0.763545,0.5809),float3(0.38745,-0.0462163,0.920732),float3(-0.570065,-0.790753,0.223013),float3(0.159672,-0.632848,0.757634),float3(0.795487,-0.50727,-0.331478),float3(0.40264,-0.879079,0.255149),float3(0.350203,-0.91586,-0.19636),float3(-0.461583,-0.576515,0.674219),float3(-0.0820001,-0.996172,0.0302926),float3(-0.243163,-0.587302,0.771977),float3(0.82196,-0.440432,-0.361111),float3(0.214109,-0.954331,0.20835),float3(0.393867,-0.4169,-0.819184),float3(0.954051,-0.206879,0.216767),float3(-0.189654,-0.972521,-0.135037),float3(-0.100866,-0.987967,-0.117253)},
		
		{float3(0.19001,-0.830829,0.523086),float3(-0.340521,-0.561895,-0.753869),float3(0.383123,-0.780361,-0.494219),float3(-0.577563,-0.316527,0.752483),float3(-0.651017,-0.716662,0.250147),float3(0.180241,-0.90753,-0.379345),float3(0.307849,-0.659111,-0.68615),float3(0.530682,-0.661349,0.530089),float3(-0.00546291,-0.990561,0.136968),float3(0.250008,-0.168646,0.953443),float3(-0.0794892,-0.996667,-0.0183258),float3(0.695671,-0.29376,0.655551),float3(-0.632258,-0.327538,-0.702117),float3(0.343705,-0.445344,0.826762),float3(-0.244292,-0.205914,-0.947587),float3(0.18186,-0.147767,0.972158)},
		{float3(0.126141,-0.872115,-0.472761),float3(0.303526,-0.950673,0.0639794),float3(-0.477399,-0.160942,0.863822),float3(-0.0524248,-0.993762,0.0984267),float3(-0.546806,-0.817646,-0.180164),float3(0.0093487,-0.728572,0.684906),float3(-0.0113193,-0.996428,0.0836859),float3(0.213361,-0.950762,0.224785),float3(-0.194371,-0.962853,-0.187441),float3(-0.0901857,-0.834541,0.543515),float3(0.0655844,-0.980227,-0.186693),float3(0.177002,-0.0225858,-0.983951),float3(-0.196471,-0.948038,0.250248),float3(-0.105383,-0.970028,-0.218952),float3(0.714457,-0.644386,0.272613),float3(0.967688,-0.0134561,0.251793)},
		{float3(0.844729,-0.256151,-0.469915),float3(-0.182777,-0.433579,-0.882384),float3(-0.724785,-0.553999,0.409601),float3(-0.328421,-0.821337,0.466418),float3(-0.0820988,-0.995116,0.0548024),float3(0.742894,-0.434642,-0.509112),float3(-0.421623,-0.750674,0.508649),float3(0.392764,-0.0976921,-0.914436),float3(-0.351574,-0.896443,0.269789),float3(0.95048,-0.265893,-0.160898),float3(-0.181828,-0.155532,-0.970952),float3(0.286009,-0.761759,0.581311),float3(-0.926844,-0.0494552,0.372174),float3(0.141708,-0.79586,-0.588665),float3(0.327619,-0.915822,-0.232241),float3(0.364117,-0.635449,0.680899)},
		{float3(0.724631,-0.687182,-0.0518824),float3(-0.314246,-0.295389,-0.902216),float3(-0.940347,-0.294287,-0.17071),float3(0.607661,-0.249641,-0.753942),float3(0.0622918,-0.515035,-0.854903),float3(-0.298565,-0.642674,0.70557),float3(0.780974,-0.615805,0.104231),float3(0.701432,-0.0118222,-0.712638),float3(0.895419,-0.351519,0.273237),float3(0.0085528,-0.830455,-0.55702),float3(0.707861,-0.121195,0.695877),float3(0.582032,-0.561439,0.588239),float3(0.821271,-0.464858,-0.33079),float3(-0.171624,-0.34582,0.922472),float3(0.51698,-0.577949,-0.631432),float3(-0.391977,-0.894989,0.21295)}
	};

	float fNumSamples = 0.0f;
	float fResults = 0.0f;
	
	float4 fPixelData = DepthNormalBuffer.Load( texturelocation );
	float fPixelDepth = fPixelData.w * Frustum.y;
	float3 fPixelNormal = fPixelData.xyz * 2.0f - 1.0f;

	// Find the view space 3D position of the pixel
	float3 PixelPosVS = ScreenPosToViewPos( DispatchThreadID, fPixelDepth );

	// Use the lower four bits of the dispatch thread ID for
	// selecting the desired rotation vector
	int rotx = DispatchThreadID.x & 0xF;
	int roty = DispatchThreadID.y & 0xF;

	// Kernel scale defines the size of the volume around each texel to sample
	float fKernelScale = 0.01251751f;

	for ( int x = 0; x < 2; x++ )
	{
		for ( int y = 0; y < 8; y++ )
		{
			// Calculate an offset into the reflection vector array
			rotx = (rotx + x) & 0xF;
			roty = (roty + y) & 0xF;

			// Find the new texture coordinates to load
			float3 vRotatedOffset = (reflect( avKernel[y], rotation[rotx][roty] )) * avScales[y];
			
			float3 vFlippedOffset = vRotatedOffset;

			float fSign = dot( fPixelNormal, vRotatedOffset );
			if ( fSign < 0.0f )
				vFlippedOffset = -vFlippedOffset;

			float3 Sample3D = PixelPosVS + vFlippedOffset * fKernelScale;
			
			int3 iNewOffset = ViewPosToScreenPos( Sample3D );
	
#ifndef USE_GSM
			float fSample = DepthNormalBuffer.Load( iNewOffset ).w * Frustum.y;
#else
			float fSample = LoadDepth( iNewOffset - OffsetLocation );
#endif // USE_GSM

			float fDelta = Sample3D.z - fSample;
			float fOcclusion = 0.0f;  // Initialize to zero occlusion
			
			const float fMinDeltaEpsilon = 0.051f;
			const float fFullOcclusionThreshold = 0.051f;
			const float fNoOcclusionThreshold = 0.5354f;
			const float fOcclusionPower = 2.0f;

			if ( fDelta > fMinDeltaEpsilon ) 
			{
				float fNoOcclusionRange = fNoOcclusionThreshold - fFullOcclusionThreshold;      
				if ( fDelta < fFullOcclusionThreshold )
					fOcclusion = 1.0f;
				else
					fOcclusion = max( 1.0f - pow( ( fDelta - fFullOcclusionThreshold ) / fNoOcclusionRange, fOcclusionPower ), 0.0f );
			} 

			// Accumulate the resulting occlusion scaled by the distance from
			// this pixel's depth sample.
			fResults += fOcclusion * (avScales[y]* avScales[y]);
			fNumSamples += 1.0f;
		}
	}

	AmbientOcclusionTarget[DispatchThreadID.xy] = 1.0f - min( ( fResults / fNumSamples ), 1.0f );
}
