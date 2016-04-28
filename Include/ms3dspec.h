
// MS3D File Format (as of v1.7.0)
//
// First comes the header (MS3DHeader)
// Then the number of vertices (WORD)
// Then the actual vertices (MS3DVertex)
//
// Then the number of triangles (WORD)
// Then the actual triangles (MS3DTriangle)
//
// Then the number of groups (WORD)
// Then the actual groups (MS3DGroup)
//
// Then the number of materials (WORD)
// Then the actual materials (MS3DMaterial)
//
// Now comes the keyframer data
//
// fAnimationFPS (float)
// fCurrentTime (float)
// iTotalFrames (int)
//
// Then the number of joints (WORD)
// Then the actual joints (MS3DJoint)
//
// Now comes the sub-version (int)
//
// Then the number of group comments (uint)
// Then all the actual group comments (MS3DComment) - variable length!
//
// Then the number of material comments (uint)
// Then all the actual material comments (MS3DComment) - variable length!
//
// Then the number of joint comments (uint)
// Then all the actual joint comments (MS3DComment) - variable length!
//
// Then the number of model comments (uint) - always 0 or 1
// Then all the actual model comments (MS3DComment) - variable length!
//
// 
// 
// *NOTE* See www.milkshape3d.com for latest format info

#ifndef ms3dspec_h
#define ms3dspec_h

namespace Glyph3
{
	struct MS3DHeader
	{
		char			id[10];							// always "MS3D000000"
		unsigned int	version;						// 3
	};

	struct MS3DVertex
	{
		unsigned char	flags;                                      // SELECTED | SELECTED2 | HIDDEN
		float			vertex[3];                                  //
		char			boneId;                                     // -1 = no bone
		unsigned char	referenceCount;
	};

	struct MS3DTriangle
	{
		unsigned short	flags;                                      // SELECTED | SELECTED2 | HIDDEN
		unsigned short	vertexIndices[3];                           //
		float			vertexNormals[3][3];                        //
		float			s[3];                                       //
		float			t[3];                                       //
		unsigned char	smoothingGroup;                             // 1 - 32
		unsigned char	groupIndex;                                 //
	};

	struct MS3DGroup
	{
		unsigned char	flags;                              // SELECTED | HIDDEN
		char            name[32];                           //
		unsigned short	numtriangles;                       //
		unsigned short  *triangleIndices;      // the groups group the triangles
		char            materialIndex;                      // -1 = no material
	};

	struct MS3DMaterial
	{
		char            name[32];                           //
		float           ambient[4];                         //
		float           diffuse[4];                         //
		float           specular[4];                        //
		float           emissive[4];                        //
		float           shininess;                          // 0.0f - 128.0f
		float           transparency;                       // 0.0f - 1.0f
		char            mode;                               // 0, 1, 2 is unused now
		char            texture[128];                       // texture.bmp
		char            alphamap[128];                      // alpha.bmp
	};

	struct MS3DKeyframeRotation
	{
		float           time;                               // time in seconds
		float           rotation[3];                        // x, y, z angles
	};

	struct MS3DKeyframePosition
	{
		float           time;                               // time in seconds
		float           position[3];                        // local position
	};

	struct MS3DKeyframeJoint
	{
		unsigned char	flags;                              // SELECTED | DIRTY
		char            name[32];                           //
		char            parentName[32];                     //
		float           rotation[3];                        // local reference matrix
		float           position[3];
		unsigned short  numKeyFramesRot;                    //
		unsigned short	numKeyFramesTrans;                  //
		MS3DKeyframeRotation *keyFramesRot;
		MS3DKeyframePosition *keyFramesTrans;
	};
};
#endif